/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Nestor Silveira.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include <rapidjson/document.h>
#include "gkLogicTree.h"
#include "gkNodeManager.h"

using namespace Ogre;

gkLogicTree::gkLogicTree(gkResourceManager *creator, const gkResourceName &name, const gkResourceHandle &handle)
	:	gkResource(creator, name, handle),
		m_object(0),
		m_initialized(false), 
		m_sorted(false),
		m_uniqueHandle(0),
		m_isMainTree(true),
		m_isDebugging(false),
		m_isDebuggingInitalized(false),
		m_jsonTreeInfo(0),
		m_jsonNodeArray(0)
{
}


gkLogicTree::~gkLogicTree()
{
	destroyNodes();
}



void gkLogicTree::attachObject(gkGameObject* ob)
{
	m_object = ob;

	// notify all
	if (!m_allNodes.empty())
	{
		NodeIterator iter(m_allNodes);
		while (iter.hasMoreElements())
			iter.getNext()->attachObject(m_object);
	}
}


void gkLogicTree::destroyNodes(void)
{
	if (!m_allNodes.empty())
	{
		NodeIterator iter(m_allNodes);
		while (iter.hasMoreElements())
			delete iter.getNext();
	}
	m_allNodes.clear();
	m_nodeMap.clear();
	m_uniqueHandle = 0;
}


void gkLogicTree::freeUnused(void)
{
	// delete nodes that have no links
}

gkLogicNode* gkLogicTree::getNode(UTsize idx)
{
	if (idx >= 0 && idx < m_allNodes.size())
		return m_allNodes.at(idx);
	return 0;
}

gkLogicNode* gkLogicTree::getNodeByName(const gkString& name)
{
	gkLogicNode** result = m_nodeMap.get(name.c_str());
	if (result) {
		return *result;
	}
	return 0;
}

// Sort trees for optimal execution
class gkLogicSolver
{
public:
	typedef gkLogicNode* gkLogicNodeT;

	static bool sort(const gkLogicNodeT& a, const gkLogicNodeT& b)
	{
		return a->getPriority() < b->getPriority();
	}

	// sort the nodes so that nodes with lower bucketNrs get executed first to ensure
	// a certain order. like first the nodes that set data(BUCKET_SETTER), then nodes that get data(BUCKET_GETTER) and then
	// nodes the use the data (BUCKET_EXECUTOR).
	static bool sortBuckets(const gkLogicNodeT& a, const gkLogicNodeT& b)
	{
		return a->getBucketNr() > b->getBucketNr();
	}

public:

	typedef std::deque<gkLogicNode*> QUEUE;

	void solve(gkLogicTree* tree)
	{
		QUEUE l;
		gkLogicTree::NodeIterator iter = tree->getNodeIterator();
		while (iter.hasMoreElements())
			setPriority(tree, iter.getNext(), l);
	}

	void setPriority(gkLogicTree* tree, gkLogicNode* node, QUEUE& l)
	{
		// setting priority + 1 for each connection

		GK_ASSERT(node);

		QUEUE::const_iterator it = std::find(l.begin(), l.end(), node);
		if (it != l.end()) return;

		l.push_back(node);

		gkLogicNode::Sockets& sockets = node->getInputs();
		if (!sockets.empty())
		{
			gkLogicNode::SocketIterator sockit(sockets);
			while (sockit.hasMoreElements())
			{
				gkILogicSocket* sock = sockit.getNext();
				if (sock->isLinked())
				{
					gkILogicSocket* fsock = sock->getFrom();

					GK_ASSERT(fsock);

					gkLogicTree::NodeIterator iter = tree->getNodeIterator();
					while (iter.hasMoreElements())
					{
						gkLogicNode* onode = iter.getNext();
						if (onode != node && onode == fsock->getParent())
						{
							if (onode->getPriority() <= node->getPriority())
							{
								onode->setPriority(node->getPriority() + 1);
								setPriority(tree, onode, l);
							}
						}
					}
				}
			}
		}

		l.pop_back();
	}
};


#ifdef OGREKIT_BUILD_MOBILE
# define NT_DUMP_ORDER 1
#else
# define NT_DUMP_ORDER 0
#endif

void gkLogicTree::solveOrder(bool forceSolve)
{
	if (m_sorted && !forceSolve)
		return;

	if (m_sorted)
	{
		NodeIterator iter(m_allNodes);
		while (iter.hasMoreElements())
			iter.getNext()->setPriority(0);
	}
	m_sorted = true;

	gkLogicSolver s;
	s.solve(this);

#if NT_DUMP_ORDER == 1
	m_allNodes.sort(gkLogicSolver::sort);
	m_allNodes.sort(gkLogicSolver::sortBuckets);
#else
	FILE* fp = fopen("NodeTree_dump.txt", "wb");

	fprintf(fp, "--- node order before sort ---\n");
	NodeIterator iter(m_allNodes);
	while (iter.hasMoreElements())
	{
		gkLogicNode* lnode = iter.getNext();
		fprintf(fp, "%s:%i\n", (typeid(*lnode).name()), lnode->getPriority());
	}

	m_allNodes.sort(gkLogicSolver::sort);
	m_allNodes.sort(gkLogicSolver::sortBuckets);

	fprintf(fp, "--- node order after sort ---\n");
	NodeIterator iter2(m_allNodes);
	while (iter2.hasMoreElements())
	{
		gkLogicNode* lnode = iter2.getNext();
		fprintf(fp, "%s:%i %i\n", (typeid(*lnode).name()), lnode->getPriority(),lnode->getBucketNr());
	}
	fclose(fp);
#endif

}

void gkLogicTree::init(InitPass pass)
{
	gkNodeManager* nodeManager = gkNodeManager::getSingletonPtr();
	nodeManager->__setCurrentExecutionTree(this);

	NodeIterator iter(m_allNodes);
	while (iter.hasMoreElements())
	{
		gkLogicNode* node = iter.getNext();
		nodeManager->__setCurrentExecutionNode(node);
		if (pass == ip_init)
		{
			// check if the connections valid
			// this is meant to be overridden so any node can tell, if so or not
			// default:true
			node->checkValidity();
			node->initialize();
		}
		else if (pass == ip_afterInit)
		{
			node->_afterInit();
		}
		else if (pass == ip_afterInit2)
		{
			node->_afterInit2();
		}
		nodeManager->__setCurrentExecutionNode(0);
	}
	nodeManager->__setCurrentExecutionTree(0);

}

#define JSON_STRING(NAME,STRING_VALUE)\
rapidjson::Value NAME;\
NAME.SetString(STRING_VALUE.c_str(),STRING_VALUE.length(),jsonDoc.GetAllocator());


void gkLogicTree::execute(gkScalar tick)
{
	m_isDebugging = gkNodeManager::getSingleton().isDebugging();
	rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();

	if (m_isDebugging)
	{
		if (!m_isDebuggingInitalized)
		{
			rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();

			rapidjson::Value treeInfo(rapidjson::kObjectType);


			if (m_isDebugging)
			{
				JSON_STRING(valOwner,getAttachedObject()->getName())
				treeInfo.AddMember("owner",valOwner,jsonDoc.GetAllocator());
				JSON_STRING(valTreename,getOriginalName());
				treeInfo.AddMember("treename",valTreename,jsonDoc.GetAllocator());

				rapidjson::Value nodeArray(rapidjson::kArrayType);

				treeInfo.AddMember("nodes",nodeArray,jsonDoc.GetAllocator());
				m_jsonNodeArray = &treeInfo["nodes"];

				jsonDoc.PushBack(treeInfo,jsonDoc.GetAllocator());
				int idx = jsonDoc.Size()-1;
				m_jsonTreeInfo = &jsonDoc[jsonDoc.Size()-1];
			}
			m_isDebuggingInitalized=true;
		}

		m_jsonNodeArray->Clear();
	}

	if (m_allNodes.empty())
	{
		// undefined
		return;
	}

	if (!m_sorted)
		solveOrder();

	Ogre::Timer clock;

	gkNodeManager* nodeManager = gkNodeManager::getSingletonPtr();
	nodeManager->__setCurrentExecutionTree(this);

	{
		NodeIterator iter(m_preUpdateNodes);
		while (iter.hasMoreElements())
		{
			gkLogicNode* node = iter.getNext();
			nodeManager->__setCurrentExecutionNode(node);

			// can continue
			if (node->evaluate(tick))
			{
				node->_preUpdate();
			}

			nodeManager->__setCurrentExecutionNode(0);
		}

	}
	{
		NodeIterator iter(m_updateNodes);
		while (iter.hasMoreElements())
		{
			gkLogicNode* node = iter.getNext();
			nodeManager->__setCurrentExecutionNode(node);

			// can continue
			if (node->evaluate(tick))
			{
				node->update(tick);
			}

			nodeManager->__setCurrentExecutionNode(0);
		}

	}
	{
		NodeIterator iter(m_postUpdateNodes);
		while (iter.hasMoreElements())
		{
			gkLogicNode* node = iter.getNext();
			nodeManager->__setCurrentExecutionNode(node);

			// can continue
			if (node->evaluate(tick))
			{
				node->_postUpdate();
			}


			if (m_isDebugging)
			{
				rapidjson::Value jsonNode(rapidjson::kObjectType);

				JSON_STRING(valNodeType,node->getType())
				jsonNode.AddMember("nodeType",valNodeType,jsonDoc.GetAllocator());
				JSON_STRING(valNodeName,node->getName())
				jsonNode.AddMember("nodeName",valNodeName,jsonDoc.GetAllocator());
				gkGameObject* attachedObject = node->getAttachedObject();
				JSON_STRING(valAttachedObj,attachedObject->getName())
				jsonNode.AddMember("attachedObject",valAttachedObj,jsonDoc.GetAllocator());

				if (attachedObject->isGroupInstance())
				{
					jsonNode.AddMember("isGroupInstance",true,jsonDoc.GetAllocator());
					JSON_STRING(valGIRoot,attachedObject->getGroupInstance()->getRoot()->getName())
					jsonNode.AddMember("giRootName",valGIRoot,jsonDoc.GetAllocator());
				}

				rapidjson::Value inputs(rapidjson::kArrayType);
				rapidjson::Value outputs(rapidjson::kArrayType);
				rapidjson::Value props(rapidjson::kArrayType);
				jsonNode.AddMember("inputs",inputs,jsonDoc.GetAllocator());
				jsonNode.AddMember("outputs",outputs,jsonDoc.GetAllocator());
				jsonNode.AddMember("props",props,jsonDoc.GetAllocator());
				if (node->debug(jsonNode))
				{
					if (!node->isValid())
					{
						jsonNode.AddMember("color","(1.0,0.0,0.0)",jsonDoc.GetAllocator());
					}

					m_jsonNodeArray->PushBack(jsonNode,jsonDoc.GetAllocator());

				}



			}

			nodeManager->__setCurrentExecutionNode(0);
		}
	}

}

gkLogicTree* gkLogicTree::clone(gkGameObject* destinationObject)
{
	// TODO: This is kind of hacky since I rely on the loader to create the logictree
	//       You will still need to have the blend-file in RAM, manually create logictrees won't be
	//		 cloned.
	// 		 A logicnode you be able to recreate itself on its own. But for now that should work.
	gkLogicTree* clone = gkNodeTreeConverter::getSingleton().createLogicTree(getOriginalName(),destinationObject->getName());
	clone->attachObject(destinationObject);
	return clone;
}
