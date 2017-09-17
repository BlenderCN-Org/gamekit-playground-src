/*
 * gkNodeTreeConverter.h
 *
 *  Created on: Nov 11, 2015
 *      Author: ttrocha
 */

#ifndef ENGINE_LOADERS_BLENDER2_GKNODETREECONVERTER_H_
#define ENGINE_LOADERS_BLENDER2_GKNODETREECONVERTER_H_
#include "gkString.h"
#include "Blender.h"
#include "gkMathUtils.h"

#include "gkBlenderSceneConverter.h"

#include "utSingleton.h"
class gkBlendFile;
class gkLogicTree;
class gkLogicNode;

class gkNodeInstancer {
public:
	gkNodeInstancer(){};
	virtual ~gkNodeInstancer(){};
	virtual gkLogicNode* create(gkLogicTree* tree,Blender::bNode*,gkString objectName="")=0;
	virtual gkString getName()=0;
	virtual gkString getTreeID()=0;
	void processDynamicNodeInput(Blender::bNodeSocket* socket,gkLogicNode* newNode);

protected:
	// overwrite this if you want to just load the node-data from the file but
	// want to process it in another way, e.g. reuse a already existent node
	virtual gkLogicNode* addNode(gkLogicNode* node,gkLogicTree* tree);
};

//class MotionNodeInstancer : public gkNodeInstancer
//{
//public:
//	MotionNodeInstancer(){};
//	~MotionNodeInstancer(){};
//	gkLogicNode* create(gkLogicTree* tree,Blender::bNode* bnode);
//
//	gkString getName()
//	{
//		return "MotionNode";
//	}
//
//	gkString getTreeID() {
//		return "Gamekit";
//	}
//};
//
//class MouseNodeInstancer : public gkNodeInstancer
//{
//public:
//	MouseNodeInstancer(){};
//	~MouseNodeInstancer(){};
//	gkLogicNode* create(gkLogicTree* tree,Blender::bNode* bnode);
//
//	gkString getName()
//	{
//		return "MouseNode";
//	}
//
//	gkString getTreeID() {
//		return "Gamekit";
//	}
//};



class gkNodeTreeConverter : public utSingleton<gkNodeTreeConverter>{
public:
	gkNodeTreeConverter();
	virtual ~gkNodeTreeConverter();

	void process(gkBlendFile* fp);

	gkLogicTree* createLogicTree(const gkString& nodeTreeName,const gkString& objectName, bool isMainTree=true);

	gkLogicNode* createNode(gkLogicTree* tree,Blender::bNode* bnode,gkString objectName="");

	static void addNodeInstancer(gkNodeInstancer* instancer);

	static float getBNodeSocketFloat(Blender::bNodeSocket* socket);
	static int getBNodeSocketInt(Blender::bNodeSocket* socket);
	static gkString getBNodeSocketString(Blender::bNodeSocket* socket);
	static gkColor getBNodeSocketColor(Blender::bNodeSocket* socket);
	static gkVector3 getBNodeSocketVector(Blender::bNodeSocket* socket);
	static bool getBNodeSocketBoolean(Blender::bNodeSocket* socket);
	// object socket is basically a boolean socket, which doesn't show checkbox on the blender-ui
	// nothing more,yet
	static bool getBNodeSocketObject(Blender::bNodeSocket* socket);

	static void receiveIOSocketData(bool& isIOSocket,gkString& ioSocketName,Blender::IDProperty* prop);

	typedef utHashTable<utCharHashKey,gkNodeInstancer*> NodeInstancerMap;
private:


	typedef utHashTable<utCharHashKey,Blender::bNodeTree*> BNodeTreeMap;
	BNodeTreeMap m_treeMap;

	static NodeInstancerMap INSTANCERMAP;

	typedef utHashTable<utCharHashKey,gkLogicNode*> BNode2LogicNodeMap;
	BNode2LogicNodeMap m_logicNodeMap;

	UT_DECLARE_SINGLETON(gkNodeTreeConverter);
};

void addDefaultNodes();
#endif /* ENGINE_LOADERS_BLENDER2_GKNODETREECONVERTER_H_ */
