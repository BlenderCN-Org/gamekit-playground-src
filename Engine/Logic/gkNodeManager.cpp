/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
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
#include <rapidjson/encodings.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "gkNodeTreeConverter.h"
#include "gkNodeManager.h"
#include "gkLogicTree.h"

#include "gsCore.h"



gkNodeContext::gkNodeContext(const gkString& ctxName)
{
	if (m_ctxName.empty() )
	{
		m_ctxName=gkUtils::getUniqueName("node_context");
	}
	else
	{
		m_ctxName = ctxName;
	}
}

gkNodeContext::~gkNodeContext()
{
	Context::Iterator iter(m_context);
	while (iter.hasMoreElements())
	{
		// delete all categories
		delete iter.getNext().second;
	}
	m_context.clear();
}


void gkNodeContext::addNode(const gkString& category,const gkString& name,gkLogicNode* node)
{
	Category** catCheck = m_context.get(category.c_str());
	Category* cat = 0;

	if (catCheck)
	{
		cat = *catCheck;
	}
	else
	{
		cat = new Category();
		m_context.insert(category.c_str(),cat);
	}

	cat->insert(name.c_str(),node);
}

gkLogicNode* gkNodeContext::getNode(const gkString& categoryName,const gkString& uuid)
{
	Category* cat = getCategory(categoryName);
	if (cat)
	{
		gkLogicNode** result = cat->get(uuid.c_str());
		if (result)
			return *result;
		else
			return 0;
	}
	return 0;
}

gkNodeContext::Category* gkNodeContext::getCategory(const gkString& categoryName)
{
	Category** result = m_context.get(categoryName.c_str());
	if (result)
		return *result;
	else
		return 0;
}


gkNodeManager::gkNodeManager()
	: gkResourceManager("NodeManager", "LogicNode"), m_currentExecutionNode(0),m_currentExecutionTree(0),m_isCurrentCreationMainTree(false),m_debugNodes(false)
{
	m_currentDebugJSON.SetArray();
	addDefaultNodes();
}

gkNodeManager::~gkNodeManager()
{
	destroyAll();
	if (gkNodeTreeConverter::getSingletonPtr())
		delete gkNodeTreeConverter::getSingletonPtr();
}

gkResource* gkNodeManager::createImpl(const gkResourceName& name, const gkResourceHandle& handle)
{
	return new gkLogicTree(this, name, handle);
}

void gkNodeManager::notifyResourceCreatedImpl(gkResource* res)
{
	gkLogicTree* tree = static_cast<gkLogicTree*>(res);
//	if (tree->getName().empty())
	if (m_isCurrentCreationMainTree)
	{
		m_locals.push_back(tree);
		m_initTrees.push_back(tree);
	}
}

void gkNodeManager::notifyResourceDestroyedImpl(gkResource* res)
{
	gkLogicTree* tree = static_cast<gkLogicTree*>(res);
//	if (tree->getName().empty())
	m_locals.erase(tree);
}

void gkNodeManager::notifyDestroyAllImpl()
{
	m_locals.clear();
}

void gkNodeManager::notifyDestroyGroupImpl(const gkResourceNameString &group)
{
	TreeList::Iterator iter(m_locals);

	while (iter.hasMoreElements())
	{
		gkLogicTree* tree = iter.getNext();
		if (group == tree->getResourceName().group)
			m_locals.erase(tree);
	}
}

gkLogicTree* gkNodeManager::createLogicTree(const gkString& groupName,bool mainTree) //create normal tree
{
	return createLogicTree(gkResourceName("", groupName),mainTree);
}

gkLogicTree* gkNodeManager::createLogicTree(gkResourceName resName,bool mainTree)
{
	m_isCurrentCreationMainTree = mainTree;
	gkLogicTree* tree = create<gkLogicTree>(resName);
	tree->_setIsMainTree(m_isCurrentCreationMainTree);
	return tree;
}

void gkNodeManager::update(gkScalar tick)
{
	if (!m_initTrees.empty())
	{
		// go through the different init stages
		{
			utListIterator<TreeList> iter(m_initTrees);
			while (iter.hasMoreElements())
			{
				gkLogicTree* tree = iter.getNext();
				tree->init(gkLogicTree::ip_init);
			}
		}
		{
			utListIterator<TreeList> iter(m_initTrees);
			while (iter.hasMoreElements())
			{
				gkLogicTree* tree = iter.getNext();
				tree->init(gkLogicTree::ip_afterInit);
			}
		}
		{
			utListIterator<TreeList> iter(m_initTrees);
			while (iter.hasMoreElements())
			{
				gkLogicTree* tree = iter.getNext();
				tree->init(gkLogicTree::ip_afterInit2);
				tree->_setInitialized();
			}
		}
		m_initTrees.clear();
	}
	// update all non group trees
	if (!m_locals.empty())
	{
		Ogre::Timer timer;
		utListIterator<TreeList> iter(m_locals);
		int sum = 0;
		int nodeCount=0;
		if (m_debugNodes)
		{
			m_currentDebugString="";
//			m_currentDebugJSON.Clear();
		}

		while (iter.hasMoreElements())
		{
			timer.reset();
			gkLogicTree* tree = iter.getNext();
			if (!tree->isInitialized() || !tree->getAttachedObject()->isInstanced())
				continue;

			tree->execute(tick);

			int time = (int)timer.getMicroseconds();
			sum += time;
			nodeCount++;
//			gkLogger::write("Execute "+tree->getName()+" took "+gkToString(time),true);

		}

		ClearSockets::Iterator clearIter(m_clearSockets);
		while (clearIter.hasMoreElements())
		{
			clearIter.getNext()->forceClear();
		}
		m_clearSockets.clear();


		if (m_debugNodes)
		{
			m_currentDebugString = json2string<rapidjson::Document>(m_currentDebugJSON);
			m_currentDebugStringCompressed = compressString(m_currentDebugString,COMPRESSION_FAST);

//			gkLogger::write(m_currentDebugString);

			gkString re;
			re = uncompressString(m_currentDebugStringCompressed);

			if (re.size() != m_currentDebugString.size())
			{
				// compression-error
				int a=0;
			}
		}

//		gkLogger::write("TreeSum "+gkToString(nodeCount)+" nodes: took "+gkToString(sum)+"\n",true);
	}
}

void gkNodeManager::addSocketToClearList(gkILogicSocket* socket)
{
	m_clearSockets.push_back(socket);
}

void gkNodeManager::removeSocketFromClearList(gkILogicSocket* socket)
{
	m_clearSockets.erase(socket);
}

UT_IMPLEMENT_SINGLETON(gkNodeManager);
