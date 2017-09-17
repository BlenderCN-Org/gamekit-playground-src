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
#ifndef _gkNodeManager_h_
#define _gkNodeManager_h_


#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "gkLogicCommon.h"
#include "gkResourceManager.h"
#include "gkMathUtils.h"
#include "utSingleton.h"

#include "utTypes.h"

class gkILogicSocket;
class gkGameObject;

class gkNodeContext
{
public:
	typedef utHashTable<utCharHashKey,gkLogicNode*> Category;
	typedef utHashTable<utCharHashKey,Category*> Context;

	gkNodeContext(const gkString& ctxName="");
	~gkNodeContext();

	void addNode(const gkString& category,const gkString& name,gkLogicNode* node);

	Category* getCategory(const gkString& categoryName);
	gkLogicNode* getNode(const gkString& catergoryName,const gkString& uuid);
private:
	gkString m_ctxName;


	Context m_context;
};

class gkNodeManager : public gkResourceManager, public utSingleton<gkNodeManager>
{
public:
	
public:
	gkNodeManager();
	~gkNodeManager();

	gkResource* createImpl(const gkResourceName& name, const gkResourceHandle& handle);

	gkLogicTree* createLogicTree(const gkString& groupName="", bool mainTree=true); //create normal tree
	gkLogicTree* createLogicTree(gkResourceName name, bool mainTree=true); //create normal tree

	void update(gkScalar tick);

	// those values get set during execution of a logictree, otherwise they should be NULL
	GK_INLINE gkLogicTree* __getCurrentExecutionTree() { return m_currentExecutionTree; }
	GK_INLINE gkLogicNode* __getCurrentExecutionNode() { return m_currentExecutionNode; }
	GK_INLINE void __setCurrentExecutionTree(gkLogicTree* tree) { m_currentExecutionTree = tree;}
	GK_INLINE void __setCurrentExecutionNode(gkLogicNode* node) { m_currentExecutionNode = node;}

	GK_INLINE void addTreeToInitQueue(gkLogicTree* tree) { m_initTrees.push_back(tree); }
	void addSocketToClearList(gkILogicSocket* socket);
	void removeSocketFromClearList(gkILogicSocket* socket);

	GK_INLINE gkNodeContext& getContext() { return m_nodeContext; }

	// TODO: Get rid of the debug-stuff. maybe outsource to a debug-class
	GK_INLINE void enableDebugging(bool enable) { m_debugNodes = enable;}
	GK_INLINE bool isDebugging() { return m_debugNodes;}
	GK_INLINE gkString getCurrentDebugString() { return m_currentDebugString;}
	GK_INLINE gkString getCurrentDebugStringCompressed() { return m_currentDebugStringCompressed;}
	GK_INLINE rapidjson::Document& getDebugJSONDoc() { return m_currentDebugJSON; }

	template<typename T>
	gkString json2string(T& val)
	{
	    rapidjson::GenericStringBuffer< rapidjson::UTF8<> > buffer;
	    rapidjson::Writer< rapidjson::GenericStringBuffer< rapidjson::UTF8<> > > writer(buffer);

	    val.Accept(writer);
	    const char* str = buffer.GetString();
		return gkString(str);
	}
	//--------------------------------------
protected:
	virtual void notifyDestroyAllImpl(void);
	virtual void notifyDestroyGroupImpl (const gkResourceNameString &group);

	virtual void notifyResourceCreatedImpl(gkResource* res);
	virtual void notifyResourceDestroyedImpl(gkResource* res);

	typedef utList<gkLogicTree*>	TreeList;
	TreeList    m_locals;
	TreeList    m_initTrees;
	
	UT_DECLARE_SINGLETON(gkNodeManager);
private:
	gkLogicTree* m_currentExecutionTree;
	gkLogicNode* m_currentExecutionNode;

	bool m_isCurrentCreationMainTree;
	bool m_debugNodes; // build string representation as json
	gkString m_currentDebugString; // the string holding the debug-data
	gkString m_currentDebugStringCompressed;
	rapidjson::Document m_currentDebugJSON;

	// clear dirty sockets at a stable moment when all nodes are processed
	typedef utList<gkILogicSocket*> ClearSockets;
	ClearSockets m_clearSockets;

	gkNodeContext m_nodeContext;
};


#endif//_gkNodeManager_h_
