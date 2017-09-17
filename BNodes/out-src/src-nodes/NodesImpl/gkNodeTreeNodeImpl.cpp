/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkNodeTreeNodeImpl.h>
#include "Loaders/Blender2/gkNodeTreeConverter.h"
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"
#include "Logic/gkNodeManager.h"

#include "gkLogger.h"

#include "Loaders/Blender2/gkBlenderSceneConverter.h"
gkNodeTreeNodeImpl::gkNodeTreeNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkNodeTreeNodeAbstract(parent,id,name), m_tree(0), m_targetObject(0),m_initialized(false)
{
}

gkNodeTreeNodeImpl::~gkNodeTreeNodeImpl()
{}

void gkNodeTreeNodeImpl::_initialize()
{

}
void gkNodeTreeNodeImpl::_afterInit()
{
	if (getIN_TARGET_OBJ()->isConnected())
	{
		m_targetObject = getIN_TARGET_OBJ()->getGameObject();

		if (!m_targetObject)
		{
			gkLogger::write("WARNING: "+getName()+" : couldn't retrieve targetobject from socket! Using attached object");
			m_targetObject = getAttachedObject();
		}
		m_tree->attachObject(m_targetObject);
	}
	else
	{
		m_targetObject = getObject(getIN_TARGET_OBJ()->getValue());
		if (!m_targetObject)
			m_targetObject = getAttachedObject();
		m_tree->attachObject(m_targetObject);
	}

}

bool gkNodeTreeNodeImpl::_evaluate(gkScalar tick)
{
	if (!m_initialized)
	{
		// add the tree to init-queue. We need to do this here, if doing this
		// inside of the
		m_initialized = true;
		gkNodeManager::getSingleton().addTreeToInitQueue(m_tree);
	}
	return getIN_ENABLED()->getValue();
}
void gkNodeTreeNodeImpl::_update(gkScalar tick)
{
	if (m_tree->isInitialized())
	{
		m_tree->execute(tick);
	}
}


gkNodeTreeNodeCustom::gkNodeTreeNodeCustom(gkLogicTree* parent, size_t id, const gkString& name)
	: gkNodeTreeNodeImpl(parent,id,name)
{
	m_innertreeInputs.type=gkBlenderSceneConverter::cpt_none;
	m_innertreeOutputs.type=gkBlenderSceneConverter::cpt_none;
};


void gkNodeTreeNodeCustom::_initialize()
{
	gkNodeTreeNodeImpl::_initialize();

//	for (int i=0;i<m_innertreeInputs.list.size();i++)
//	{
//		const gkBlenderSceneConverter::CPValue& val = m_innertreeInputs.list.at(i);
//
//		gkString mapping = gkBlenderSceneConverter::getCustomPropertyMapElement(&val,"name")->stringValue;
//		gkString socketAliasName = gkBlenderSceneConverter::getCustomPropertyMapElement(&val,"extName")->stringValue;
//
//		Ogre::StringVector splits = Ogre::StringUtil::split(mapping,"|");
//		gkString spNodeName = splits[0];
//		gkString spSocketName = splits[1];
//
//		// get the node holding the real socket inside the inner logictree
//		gkLogicNode* innerNode = m_tree->getNodeByName(spNodeName);
//		gkILogicSocket* innerSock = innerNode->getInSocketByName(spSocketName);
//
//		// the alias-socket
//		gkILogicSocket* outerSocket = getInSocketByName(socketAliasName);
//
//		// since this is an input-socket we have to connect the incoming socket with the coresponding socket in the inner-node (and vise-versa)
//		outerSocket->getFrom()->link(innerSock);
//		innerSock->link(outerSocket->getFrom());
//
//	}


//	Blender::bNodeSocket* socket = (Blender::bNodeSocket*)bnode->inputs.first;
//	while (socket) {
//		// if current socket is a dynamic socket, create it here
//		gkString socketName(socket->name);
//		const gkBlenderSceneConverter::CPValue* val = gkBlenderSceneConverter::getCustomPropertyMapFromList(innerTreeInputs,"extName",socketName);
//		if (val)
//		{
//			const gkBlenderSceneConverter::CPValue* mapping = gkBlenderSceneConverter::getCustomPropertyMapElement(val,"name");
//			if (mapping)
//			{
//				Ogre::StringVector splits = Ogre::StringUtil::split(mapping->stringValue,"|");
//				gkString spNodeName = splits[0];
//				gkString spSocketName = splits[1];
//
//				int a=0;
//
//			}
////			gkBlenderSceneConverter::CPValue* nodePath = val->map.get("name");
////			if (nodePath)
////			{
////				gkString here = nodePath->stringValue;
////				int a=0;
////			}
//			result->addDynSock(socketName,true);
//		}
//		socket = socket->next;
//	}

}

gkLogicNode* gkNodeTreeNodeCustomInstancer::create(gkLogicTree* tree,Blender::bNode* bnode,gkString objectName)
{
	gkNodeTreeNodeCustom* result = static_cast<gkNodeTreeNodeCustom*>(gkNodeTreeNodeInstancer::create(tree,bnode,objectName));

	gkString treeName = objectName+"_"+result->getPropnodetree()+"_"+result->getName();
	gkLogicTree* innerTree = gkNodeTreeConverter::getSingleton().createLogicTree(result->getPropnodetree(),treeName,false);
	result->_setLogicTree(innerTree);

	gkNodeTreeNodeCustom* node = static_cast<gkNodeTreeNodeCustom*>(result);
	// store data regarding ioSockets for later use
	gkBlenderSceneConverter::CPValue& innerTreeInputs = node->getInnertreeInputs();
	Blender::IDProperty* inSockProp = gkBlenderSceneConverter::findProperty("extInputs",bnode->prop);
	gkBlenderSceneConverter::retrieveCustomProperties(inSockProp,innerTreeInputs);

	gkBlenderSceneConverter::CPValue& innerTreeOutputs = node->getInnertreeOutput();
	Blender::IDProperty* outSockProp = gkBlenderSceneConverter::findProperty("extOutputs",bnode->prop);
	gkBlenderSceneConverter::retrieveCustomProperties(outSockProp,innerTreeOutputs);


	for (int i=0;i<innerTreeInputs.list.size();i++)
	{
		const gkBlenderSceneConverter::CPValue& val = innerTreeInputs.list.at(i);

		gkString mapping = gkBlenderSceneConverter::getCustomPropertyMapElement(&val,"name")->stringValue;
		gkString socketAliasName = gkBlenderSceneConverter::getCustomPropertyMapElement(&val,"extName")->stringValue;

		Ogre::StringVector splits = Ogre::StringUtil::split(mapping,"|");
		gkString spNodeName = splits[0];
		gkString spSocketName = splits[1];

		// get the node holding the real socket inside the inner logictree
		gkLogicNode* innerNode = innerTree->getNodeByName(spNodeName);
		gkILogicSocket* innerSock = innerNode->getInSocketByName(spSocketName);

		// the alias-socket
		result->addSocketData(innerSock,socketAliasName,true);
	}


	for (int i=0;i<innerTreeOutputs.list.size();i++)
	{
		const gkBlenderSceneConverter::CPValue& val = innerTreeOutputs.list.at(i);

		gkString mapping = gkBlenderSceneConverter::getCustomPropertyMapElement(&val,"name")->stringValue;
		gkString socketAliasName = gkBlenderSceneConverter::getCustomPropertyMapElement(&val,"extName")->stringValue;

		Ogre::StringVector splits = Ogre::StringUtil::split(mapping,"|");
		gkString spNodeName = splits[0];
		gkString spSocketName = splits[1];

		// get the node holding the real socket inside the inner logictree
		gkLogicNode* innerNode = innerTree->getNodeByName(spNodeName);
		gkILogicSocket* innerSock = innerNode->getOutSocketByName(spSocketName);

		// the alias-socket
		result->addSocketData(innerSock,socketAliasName,false);
	}

	// finally set values of non-connected sockets
	Blender::bNodeSocket* socket = (Blender::bNodeSocket*)bnode->inputs.first;
	while (socket) {

		gkString socketIdentifier(socket->identifier);
		if (!socket->link && gkBlenderSceneConverter::customPropertyListContains(innerTreeInputs,"extName",socketIdentifier)) // no link? set value
		{
			gkVariable var = gkBlenderSceneConverter::getCustomVariable("value",socket->prop);

			gkILogicSocket* sock = result->getInSocketByName(socketIdentifier);

			// the when we got from blender a vec4 but need it to be a gkColor, here a special case
			if (sock->getVar()->getType() == gkVariable::VAR_COLOR && var.getType() == gkVariable::VAR_VEC4)
			{
				gkVector4 vec4(var.getValueVector4());
				gkColor col(vec4.x,vec4.y,vec4.z,vec4.w);
				var.setValue(col);
			}
			else if (sock->getVar()->getType() == gkVariable::VAR_BOOL && var.getType() == gkVariable::VAR_INT)
			{
				// boolean is recognized as int from the getCustomVariable-method (TODO: fix that!)
				var.setValue(var.getValueInt()==1);
			}
			sock->getVar()->assign(var);

			int a=0;

		}
		socket = socket->next;
	}

	return result;
}





gkLogicNode* gkNodeTreeNodeCustomInstancer::createNode(const gkString& name)
{
	gkLogicNode* result = new gkNodeTreeNodeCustom(0,0,name);
	return result;
}
