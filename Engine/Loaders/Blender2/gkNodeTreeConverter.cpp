/*
 * gkNodeTreeConverter.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: ttrocha
 */

#include <Blender.h>

#include <gkNodeTreeConverter.h>
#include "gkBlendFile.h"
#include "gkBlendInternalFile.h"

#include "gkLogicTree.h"

#include "gkLogicNode.h"

#include "gkMotionNode.h"

#include "gkLogicCommon.h"

#include "gkNodeManager.h"

#include "OgreString.h"

#include "gkMouseNode.h"

#include "gkValue.h"

#include "gkVariable.h"
gkNodeTreeConverter::NodeInstancerMap gkNodeTreeConverter::INSTANCERMAP;

void addDefaultNodes()
{
//	gkNodeTreeConverter::addNodeInstancer(new MotionNodeInstancer());
//	gkNodeTreeConverter::addNodeInstancer(new MouseNodeInstancer());
}


gkNodeTreeConverter::gkNodeTreeConverter() {
	if (INSTANCERMAP.find("MotionNode")==UT_NPOS)
	{
		addDefaultNodes();

	}


}

gkNodeTreeConverter::~gkNodeTreeConverter()
{
	NodeInstancerMap::Iterator iter(INSTANCERMAP);
	while (iter.hasMoreElements()) {
		gkNodeInstancer* instancer = iter.getNext().second;
		delete instancer;
	}
	INSTANCERMAP.clear();
	// TODO Auto-generated destructor stub
}

void gkNodeTreeConverter::process(gkBlendFile* bF)
{
	gkBlendListIterator iter = bF->_getInternalFile()->getNodeTreeList();

	while (iter.hasMoreElements()) {
		// for faster access read all blender nodetrees and keep them in a hashtable
		Blender::bNodeTree* tree = (Blender::bNodeTree*)iter.getNext();
		gkString treeName = GKB_IDNAME(tree);
		if (Ogre::StringUtil::startsWith(treeName,"debug_"))
		{
			gkLogger::write("Ignoring DEBUG-Nodetree:"+treeName);
			continue;
		}
		gkLogger::write("gkNodeTreeConverter: cache tree:"+treeName);
		m_treeMap.insert(treeName.c_str(),tree);
	}
}

float gkNodeTreeConverter::getBNodeSocketFloat(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocketValueFloat* floatValue = (Blender::bNodeSocketValueFloat*)socket->default_value;
//	float value = floatValue->value;

	float value = gkBlenderSceneConverter::retrieveCustomPropertyFloat("value",socket->prop,0.f);
	return value;
}

int gkNodeTreeConverter::getBNodeSocketInt(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocketValueInt* intValue = (Blender::bNodeSocketValueInt*)socket->default_value;
//	int value = intValue->value;
	int value = gkBlenderSceneConverter::retrieveCustomPropertyInt("value",socket->prop,0);
	return value;
}

gkString gkNodeTreeConverter::getBNodeSocketString(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocketValueString* stringValue = (Blender::bNodeSocketValueString*)socket->default_value;
//	gkString result = gkString(stringValue->value);
	gkString result = gkBlenderSceneConverter::retrieveCustomPropertyString("value",socket->prop);
	return result;
}

gkColor gkNodeTreeConverter::getBNodeSocketColor(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocketValueRGBA* colorValue = (Blender::bNodeSocketValueRGBA*)socket->default_value;
//	gkColor col(colorValue->value[0],colorValue->value[1],colorValue->value[2],colorValue->value[3]);
	gkVector4 vec;
	gkBlenderSceneConverter::retrieveCustomPropertyFloatVec("value",socket->prop,vec);
	gkColor col(vec.x,vec.y,vec.z,vec.w);
	return col;
}

gkVector3 gkNodeTreeConverter::getBNodeSocketVector(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocketValueVector* vecValue = (Blender::bNodeSocketValueVector*)socket->default_value;
//	gkVector3 vec(vecValue->value[0],vecValue->value[1],vecValue->value[2]);

	gkVector3 vec;
	gkBlenderSceneConverter::retrieveCustomPropertyFloatVec("value",socket->prop,vec);
	return vec;
}

bool gkNodeTreeConverter::getBNodeSocketBoolean(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocket* boolSock = (Blender::bNodeSocket*)socket->default_value;
	bool result = gkBlenderSceneConverter::retrieveCustomPropertyInt("value",socket->prop);
	return result;
}

bool gkNodeTreeConverter::getBNodeSocketObject(Blender::bNodeSocket* socket)
{
//	Blender::bNodeSocket* boolSock = (Blender::bNodeSocket*)socket->default_value;
	bool result = gkBlenderSceneConverter::retrieveCustomPropertyInt("value",socket->prop);
	return result;
}


void gkNodeTreeConverter::receiveIOSocketData(bool& isIOSocket,gkString& ioSocketName,Blender::IDProperty* prop)
{
	isIOSocket = gkBlenderSceneConverter::retrieveCustomPropertyInt("isIOSocket",prop,0);
	ioSocketName = gkBlenderSceneConverter::retrieveCustomPropertyString("ioSocketName",prop,"");
}



gkLogicTree* gkNodeTreeConverter::createLogicTree(const gkString& nodeTreeName,const gkString& objectName, bool isMainTree)
{
	Blender::bNodeTree** btree = m_treeMap.get(nodeTreeName.c_str());

	if (!btree) {
		gkLogger::write( "Unknown logictree with name:" + nodeTreeName,true);
		return 0;
	}

	gkString idname((*btree)->idname);
	// create the object's nodetree with name: objectname_nodetreename (so it is possible to have different instances of the tree)
	gkString treeName(objectName+"_"+nodeTreeName);
	gkLogicTree* tree = static_cast<gkLogicTree*>(gkNodeManager::getSingleton().createLogicTree(gkResourceName(treeName,idname),isMainTree));
	tree->_setOriginalName(nodeTreeName);
	// run the
	Blender::bNode* bnode = (Blender::bNode*)(*btree)->nodes.first;

	// first pass, create the logic-nodes
	while (bnode) {
		bool enabled = (bool)gkBlenderSceneConverter::retrieveCustomPropertyInt("enabled__",bnode->prop,1);

		if (!enabled || gkString(bnode->idname)=="NodeFrame" || gkString(bnode->idname)=="NodeReroute" )
		{
			int a=0;
			bnode = bnode->next;

			continue;
		}
		gkLogicNode* newNode = createNode(tree,bnode,objectName);

		gkString nodeHashName = treeName + gkString(bnode->name);

		m_logicNodeMap.insert(utCharHashKey(nodeHashName.c_str()),newNode);

		gkString propName = nodeHashName;

		gkString customUUIDProp = newNode->getUUIDPropname();
		// if there is a custom (string)prop use this one
		if (!customUUIDProp.empty())
		{
			gkVariable* var = newNode->getProperty(newNode->getUUIDPropname());

			if (var->getType() == gkVariable::VAR_STRING)
			{
				propName = objectName + "_"+ var->getValueString();
			}
		}

		gkNodeManager::getSingleton().getContext().addNode(newNode->getType(),propName,newNode);

		bnode = bnode->next;
	}

	// second pass:
	// now that we have all nodes created we can wire it's sockets together
	bnode = (Blender::bNode*)(*btree)->nodes.first;
	while(bnode)
	{
		gkString toNodeHashName = treeName + gkString(bnode->name);

		if (!m_logicNodeMap.get(utCharHashKey(toNodeHashName.c_str())) // check if this node exists in mapping (maybe it is disabled)
				|| gkString(bnode->idname)=="NodeReroute" || gkString(bnode->idname)=="NodeFrame") // ignore reroute-node. those gets handled if they are on the from-socket
		{
			bnode=bnode->next;
			continue;
		}
		gkLogicNode* node = *m_logicNodeMap.get(utCharHashKey(toNodeHashName.c_str()));

		// wire incoming-sockets
		Blender::bNodeSocket* socket = (Blender::bNodeSocket*)bnode->inputs.first;
		while (socket) {
			if (socket->link) // is this socket linked to another socket?
			{
				Blender::bNodeSocket* tempSock = socket;
				// that is me
				Blender::bNode* toBNode = (Blender::bNode*)tempSock->link->tonode;
				Blender::bNodeSocket* toBSocket = (Blender::bNodeSocket*)tempSock->link->tosock;
				// the node I want to connect my incoming-socket to
				Blender::bNode* fromBNode = (Blender::bNode*)tempSock->link->fromnode;
				while (fromBNode!=0 && gkString(fromBNode->idname)=="NodeReroute") { // traverse rerouter-nodes that have only one input and one output socket
					tempSock = (Blender::bNodeSocket*)(fromBNode->inputs.first);
					if (tempSock->link)
						fromBNode = tempSock->link->fromnode;
					else
					{
						fromBNode = 0;
						break;
					}
				}

				if (!fromBNode)
				{
					socket = socket->next;
					continue;
				}


				Blender::bNodeSocket* fromBSocket = (Blender::bNodeSocket*)tempSock->link->fromsock;

				gkString fromNodeHashName = treeName + gkString(fromBNode->name);
				gkLogicNode** fromNode = m_logicNodeMap.get(utCharHashKey(fromNodeHashName.c_str()));

				if (fromNode) {
					gkLogger::write("Got node:" + gkString(fromBNode->name) + "=>" + gkString(toBNode->name),true);

					gkILogicSocket* toSocket = node->getInSocketByName(toBSocket->identifier);

					gkILogicSocket* fromSocket = (*fromNode)->getOutSocketByName(fromBSocket->identifier);

					if (!fromSocket || !toSocket) {
						gkLogger::write("ERROR: Node socket-mismatch! ");
					} else {
						toSocket->link(fromSocket);
						fromSocket->link(toSocket);
					}
// TODO:
//					if (!fromSocket) {
//						gkLogger::write("ERROR: Node Socket-Mismatch!! Couldn't find from-socket with name:"+fromBSocket->identifier+" in Node:"+(fromBNode->idname),true);
//					}
//					if (!toSocket) {
//						gkLogger::write("ERROR: Node Socket-Mismatch!! Couldn't find to-socket with name:"+socket->identifier+" in Node:"+(toBNode->idname),true);
//					}


				}


			}
			socket = socket->next;
		}
		bnode=bnode->next;
	}

	return tree;
}

void gkNodeTreeConverter::addNodeInstancer(gkNodeInstancer* instancer)
{
	if (!instancer) {
		gkLogger::write("Got null-instancer!?",true);
		return;
	}
	gkString instancerName = instancer->getName();
	gkString treeID = instancer->getTreeID();
	INSTANCERMAP.insert((treeID+"_"+instancerName).c_str(),instancer);
	if (gkEngine::getSingleton().getUserDefs().verbose)
	{
		gkLogger::write("Add instancer:"+treeID+"_"+instancerName);
	}
}

gkLogicNode* gkNodeTreeConverter::createNode(gkLogicTree* tree,Blender::bNode* bnode,gkString objectName)
{
	gkString nodeID(bnode->idname);
	const gkResourceName& treeName = tree->getResourceName();
	gkNodeInstancer** instancer = INSTANCERMAP.get( (treeName.getGroup()+"_"+nodeID).c_str());
	if (instancer)
	{
		gkLogicNode* result = (*instancer)->create(tree,bnode,objectName);
		// TODO: Create INPUT/OUTPUT-Sockets
		gkLogger::write("NODE-CREATED: "+nodeID,true);

		// create dynamic sockets
		// create dynamic nodes
		gkBlenderSceneConverter::CPValue dynamicInSockets;
		Blender::IDProperty* inSockProp = gkBlenderSceneConverter::findProperty("NEW_IN_SOCKETS",bnode->prop);
		gkBlenderSceneConverter::retrieveCustomProperties(inSockProp,dynamicInSockets);

		gkBlenderSceneConverter::CPValue dynamicOutSockets;
		Blender::IDProperty* outSockProp = gkBlenderSceneConverter::findProperty("NEW_OUT_SOCKETS",bnode->prop);
		gkBlenderSceneConverter::retrieveCustomProperties(outSockProp,dynamicOutSockets);

		Blender::bNodeSocket* socket = (Blender::bNodeSocket*)bnode->inputs.first;
		while (socket) {
			// if current socket is a dynamic socket, create it here
			gkString socketName(socket->name);
			if (gkBlenderSceneConverter::customPropertyListContains(dynamicInSockets,"name",socketName))
			{
				result->addDynSock(socketName,true);
				if (!socket->link)
				{
					(*instancer)->processDynamicNodeInput(socket,result);
				}
			}
			socket = socket->next;
		}
		// create dynamics output-sockets
		socket = (Blender::bNodeSocket*)bnode->outputs.first;
		while (socket)
		{
			gkString socketName(socket->name);
			if (gkBlenderSceneConverter::customPropertyListContains(dynamicOutSockets,"name",socketName))
			{
				result->addDynSock(socketName,false);
			}
			socket = socket->next;
		}

		bool debug = (bool)gkBlenderSceneConverter::retrieveCustomPropertyInt("printDebug",bnode->prop,0);
		result->setPrintDebug(debug);

		return result;
	}

	gkLogger::write("WARNING: No Instancer for NodeID:"+nodeID,true);
}

gkLogicNode* gkNodeInstancer::addNode(gkLogicNode* node,gkLogicTree* tree)
{
	tree->addNode(node);
	return node;
}



void gkNodeInstancer::processDynamicNodeInput(Blender::bNodeSocket* socket,gkLogicNode* newNode)
{
	// dynamic socket
	if ( gkString(socket->idname) == "NodeTreeSTRING")
	{
		gkString stringValue = gkNodeTreeConverter::getBNodeSocketString(socket);
		gkILogicSocket* sock = newNode->getInSocketByName(socket->name);
		gkVariable var(stringValue);
		sock->setVar(&var);
	}
	else if ( gkString(socket->idname) == "NodeTreeINT")
	{
		int intValue = gkNodeTreeConverter::getBNodeSocketInt(socket);
		gkILogicSocket* sock = newNode->getInSocketByName(socket->name);
		gkVariable var(intValue);
		sock->setVar(&var);
	}
	else if ( gkString(socket->idname) == "NodeTreeBOOL")
	{
		bool boolVal = gkNodeTreeConverter::getBNodeSocketBoolean(socket);
		gkILogicSocket* sock = newNode->getInSocketByName(socket->name);
		gkVariable var(boolVal);
		sock->setVar(&var);
	}
	else if ( gkString(socket->idname) == "NodeTreeFLOAT")
	{
		float floatValue = gkNodeTreeConverter::getBNodeSocketFloat(socket);
		gkILogicSocket* sock = newNode->getInSocketByName(socket->name);
		gkVariable var(floatValue);
		sock->setVar(&var);
	}
	else if ( gkString(socket->idname) == "NodeTreeVECTOR")
	{
		gkVector3 vecVal = gkNodeTreeConverter::getBNodeSocketVector(socket);
		gkILogicSocket* sock = newNode->getInSocketByName(socket->name);
		gkVariable var(vecVal);
		sock->setVar(&var);
	}
	else if ( gkString(socket->idname) == "NodeTreeCOLOR")
	{
		gkColor colVal = gkNodeTreeConverter::getBNodeSocketColor(socket);
		gkILogicSocket* sock = newNode->getInSocketByName(socket->name);
		gkVariable var(colVal);
		sock->setVar(&var);
	}
}

UT_IMPLEMENT_SINGLETON(gkNodeTreeConverter);
