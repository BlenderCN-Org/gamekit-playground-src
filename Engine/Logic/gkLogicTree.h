/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): silveira.nestor.
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
#ifndef _gkLogicTree_h_
#define _gkLogicTree_h_

#include <rapidjson/document.h>
#include "gkLogicCommon.h"
#include "gkLogicNode.h"
#include "gkResource.h"


class gkGameObject;
class gkNodeManager;

class gkLogicTree : public gkResource
{
public:

	typedef utList<gkLogicNode*>        NodeList;
	typedef utListIterator<NodeList>    NodeIterator;
	typedef utHashTable<utCharHashKey,gkLogicNode*> NodeMap;
	enum InitPass {
		ip_init,
		ip_afterInit,
		ip_afterInit2
	};
public:


	gkLogicTree(gkResourceManager *creator, const gkResourceName &name, const gkResourceHandle &handle);


	gkLogicTree(gkNodeManager* creator, UTsize id, const gkString& name);
	gkLogicTree(gkNodeManager* creator, UTsize id);
	~gkLogicTree();

	// call the corresponding initpass
	void init(InitPass pass);
	// execute all nodes
	void execute(gkScalar tick);


	GK_INLINE gkGameObject* getAttachedObject(void) {return m_object;}
	GK_INLINE bool hasNodes(void)                   {return !m_allNodes.empty();}
	GK_INLINE int  getNodeAmount(void) 			    {return m_allNodes.size();}
	GK_INLINE bool isGroup(void)                    {return !m_name.getName().empty();}
	GK_INLINE NodeIterator getNodeIterator(void)    {return NodeIterator(m_allNodes);}
	GK_INLINE bool isMainTree()						{return m_isMainTree;} // if not it is created to be used from within another logic-tree
	GK_INLINE void _setIsMainTree(bool b)			{m_isMainTree = b;}
	GK_INLINE bool isInitialized()					{return m_initialized;}
	GK_INLINE void _setInitialized()					{m_initialized=true;}

	void addUpdateNode(gkLogicNode* node)
	{
		m_updateNodes.push_back(node);
	}
	void addPreUpdateNode(gkLogicNode* node)
	{
		m_preUpdateNodes.push_back(node);
	}
	void addPostUpdateNode(gkLogicNode* node)
	{
		m_postUpdateNodes.push_back(node);
	}


	void attachObject(gkGameObject* ob);

	// node access

	template<class T>
	T* createNode(const gkString& name="")
	{
		T* pNode = new T(this, m_uniqueHandle, name);
		GK_ASSERT(pNode);
		addNode(pNode);
		return pNode;
	}

	void addNode(gkLogicNode* node,bool inPreUpdate=true,bool inUpdate=true,bool inPostUpdate=true)
	{
		if (m_object) node->attachObject(m_object);
		m_allNodes.push_back(node);
		node->setHandle(m_uniqueHandle);
		m_uniqueHandle ++;

		// is there a name provided? add it to NodeMap
		if (!node->getName().empty())
		{
			m_nodeMap.insert(node->getName().c_str(),node);
		}

		if (inPreUpdate)
			m_preUpdateNodes.push_back(node);
		if (inUpdate)
			m_updateNodes.push_back(node);
		if (inPostUpdate)
			m_postUpdateNodes.push_back(node);
	}

	GK_INLINE void _setOriginalName(const gkString& name) { m_originalName = name; }
	GK_INLINE gkString getOriginalName(void)			 { return m_originalName; }

	gkLogicNode* getNodeByName(const gkString& name);

	gkLogicNode*            getNode(UTsize idx);
	void                    destroyNodes(void);
	void                    freeUnused(void);
	void                    solveOrder(bool forceSolve = false);

	// clone this logictree into this gameobject
	gkLogicTree*			clone(gkGameObject* destinationObject);
protected:
	bool                m_initialized, m_sorted;
	size_t              m_uniqueHandle;
	gkGameObject*       m_object;

	NodeList            m_allNodes;

	NodeList            m_preUpdateNodes;
	NodeList            m_updateNodes;
	NodeList            m_postUpdateNodes;



	NodeMap				m_nodeMap;
	gkString			m_originalName;
	bool				m_isMainTree;
	bool 				m_isDebuggingInitalized;
	bool				m_isDebugging;

	rapidjson::Value*   m_jsonTreeInfo;
	rapidjson::Value*   m_jsonNodeArray;
};


#endif//_gkLogicTree_h_
