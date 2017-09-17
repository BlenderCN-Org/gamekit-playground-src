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
#include "gkLogicNode.h"
#include "gkLogicTree.h"

#include "Script/Api/gsNodes.h"

#include "OgreString.h"
using namespace Ogre;



gkLogicNode::gkLogicNode(gkLogicTree* parent, UTsize handle, const gkString& name) :
	m_handle(handle), m_object(0), m_other(0), m_parent(parent),
	m_hasLinks(false), m_priority(0), m_name(name),
	m_luaInit(0),m_luaEval(0), m_luaUpdate(0),m_ignoreLUA(false),
	m_isValid(true), // keep it initialized true, the use must know the best time to check for validity
	m_printDebug(false),m_bucketNr((int)BUCKET_EXECUTOR)
{
}

gkLogicNode::~gkLogicNode()
{
	if (m_luaInit)
		delete m_luaInit;
	if (m_luaEval)
		delete m_luaEval;
	if (m_luaUpdate)
		delete m_luaUpdate;

	{
		SocketIterator iter(m_inputs);

		while (iter.hasMoreElements())
		{
			gkILogicSocket* pSocket = iter.getNext();
			if (pSocket->removeRef() == 0)
				delete pSocket;
		}
		m_inputs.clear();
	}

	{
		SocketIterator iter(m_outputs);

		while (iter.hasMoreElements())
		{
			gkILogicSocket* pSocket = iter.getNext();
			if (pSocket->removeRef() == 0)
				delete pSocket;
		}
		m_outputs.clear();
	}

	{
		PropertyByNameMap::Iterator iter(m_propertyNameMap);
		while (iter.hasMoreElements())
		{
			gkVariable* var = iter.getNext().second;
			delete var;
		}
		m_propertyNameMap.clear();
	}
}



gkILogicSocket* gkLogicNode::getInputSocket(UTsize index)
{
	if (index >= 0 && index < m_inputs.size())
		return m_inputs.at(index);
	return 0;
}


gkILogicSocket* gkLogicNode::getOutputSocket(UTsize index)
{
	if (index >= 0 && index < m_outputs.size())
		return m_outputs.at(index);
	return 0;
}

gkVariable* gkLogicNode::getProperty(const gkString& name)
{
	gkVariable** var = m_propertyNameMap.get(name.c_str());
	if (!var) {
		return 0;
	}
	return *var;
}

void gkLogicNode::initialize()
{
	if (!m_ignoreLUA && m_luaInit)
	{
		m_luaInit->beginCall();
//		m_luaInit->addArgument("gsLogicNode *",new gsLogicNode(this),0);
//		m_luaInit->addArgument("gsTest *",new gsTest(),0);
		m_luaInit->call();
	}
	else
	{
		_initialize();
	}
}

bool gkLogicNode::evaluate(gkScalar tick)
{
	if (!m_ignoreLUA && m_luaEval)
	{
		m_luaEval->beginCall();
//		m_luaEval->addArgument("gsLogicNode *",new gsLogicNode(this),0);
//		m_luaEval->addArgument("gsTest *",new gsTest(),0);
		m_luaEval->addArgument(tick);
		bool result;
		m_luaEval->call(result);
		return result;
	}
	else
	{
		return _evaluate(tick);
	}
}
void gkLogicNode::update(gkScalar tick)
{
	if (!m_ignoreLUA && m_luaUpdate)
	{
		m_luaUpdate->beginCall();
//		m_luaUpdate->addArgument("gsLogicNode *",new gsLogicNode(this),0);
//		m_luaUpdate->addArgument("gsTest *",new gsTest(),0);
		m_luaUpdate->addArgument(tick);
		m_luaUpdate->call();
	}
	else
	{
		_update(tick);
	}
}

void gkLogicNode::setDebugStatus(rapidjson::Value& jsonNode,const gkString& statusmsg)
{
	rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();
	rapidjson::Value statusValue;
	statusValue.SetString(statusmsg.c_str(),jsonDoc.GetAllocator());
	jsonNode.AddMember("STATUS",statusValue,jsonDoc.GetAllocator());
}

void gkLogicNode::addDebugSocket(rapidjson::Value& jsonNode,const gkString& socketName, const gkString& valueType,const gkString& value, bool isInput)
{
	rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();

	rapidjson::Value& jsonSocketArray = isInput
									?jsonNode["inputs"]
									:jsonNode["outputs"];

	gkString newSockName;
	if (isInput)
	{
		newSockName = Ogre::StringUtil::startsWith(socketName,"IN_")
					? socketName
					: "IN_"+socketName;
	}
	else
	{
		newSockName = Ogre::StringUtil::startsWith(socketName,"OUT_")
					? socketName
					: "OUT_"+socketName;
	}

	rapidjson::Value jsonSocket(rapidjson::kObjectType);

	rapidjson::Value val;
	val.SetString(value.c_str(),jsonDoc.GetAllocator());

	rapidjson::Value valType;
	valType.SetString(valueType.c_str(),jsonDoc.GetAllocator());

	rapidjson::Value valSocketName;
	valSocketName.SetString(newSockName.c_str(),jsonDoc.GetAllocator());

	jsonSocket.AddMember("socketName",valSocketName,jsonDoc.GetAllocator());
	jsonSocket.AddMember("value",val,jsonDoc.GetAllocator());
	jsonSocket.AddMember("valueType",valType,jsonDoc.GetAllocator());

	jsonSocketArray.PushBack(jsonSocket,jsonDoc.GetAllocator());
}

bool gkLogicNode::debugCheckVariable(gkVariable* var)
{
	return (var->getType()>0 && var->getType()<7) || var->getType()==10;
}

bool gkLogicNode::debug(rapidjson::Value& jsonNode)
{
	gkString name(getName());
	rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();

	{

		gkLogicNode::Sockets::Iterator iter(getInputs());
		while (iter.hasMoreElements())
		{
			gkILogicSocket* sock = iter.getNext();

			gkString socketName = sock->getName();
			gkString value = "noval";
			gkString valueType = "notype";
			if (sock->isUsingVar())
			{
				gkVariable* var = sock->isConnected()
										?sock->getFrom()->getVar()
										:sock->getVar();
				gkString val = var->toString();

				if (var->getType()==5)
				{
					int a=0;
				}

				if (debugCheckVariable(var))
				{
					value = var->toString();
					valueType = gkToString(var->getType());
				}
//							gkLogger::write("IN Value "+getAttachedObject()->getName()+" "+node->getName()+" "+sock->getName()+": "+var->toString());
			}

			addDebugSocket(jsonNode,socketName,valueType,value,true);

		}
	}

	{
		gkLogicNode::Sockets::Iterator iter(getOutputs());
		while (iter.hasMoreElements())
		{
			gkILogicSocket* sock = iter.getNext();
			gkString sockName = sock->getName();
			gkString value = "noval";
			gkString valueType = "notype";

			if (sock->isUsingVar())
			{
				gkVariable* var = sock->getVar();
				gkString val = var->toString();
//							gkLogger::write("OUT Value "+getAttachedObject()->getName()+" "+node->getName()+" "+sock->getName()+": "+var->toString());
				if (debugCheckVariable(var))
				{
					if (var->getType() == gkVariable::VAR_VEC2)
					{
						gkVector2 vec = var->getValueVector2();
						value = "(" + gkToString(vec.x)+ ","+gkToString(vec.y)+")";
					}
					else if (var->getType() == gkVariable::VAR_VEC3)
					{
						gkVector3 vec = var->getValueVector3();
						value = "(" + gkToString(vec.x)+ ","+gkToString(vec.y)+","+gkToString(vec.z)+")";
					}
					else if (var->getType() == gkVariable::VAR_VEC4)
					{
						gkVector4 vec = var->getValueVector4();
						value = "(" + gkToString(vec.x)+ ","+gkToString(vec.y)+","+gkToString(vec.z)+","+gkToString(vec.w)+")";
					}
					else
					{
						value = var->toString();
					}
					valueType = gkToString(var->getType());
				}
			}

			addDebugSocket(jsonNode,sockName,valueType,value,false);

		}

		if (m_printDebug)
		{
			gkLogger::write("DEBUG "+getName()+": "+gkNodeManager::getSingleton().json2string(jsonNode) );
		}
	}

//				{
//					rapidjson::Value jsonProperties(rapidjson::kArrayType);
//
//					gkLogicNode::PropertyByNameMap::Iterator iter(node->getPropertyMap());
//
//					while (iter.hasMoreElements())
//					{
//						rapidjson::Value jsonProp(rapidjson::kObjectType);
//
//						jsonProp.AddMember("propName",iter.peekNext().first.key(),jsonDoc.GetAllocator());
//						gkVariable* var = iter.peekNextValue();
//						jsonProp.AddMember("value",var->toString().c_str(),jsonDoc.GetAllocator());
//
//						jsonProperties.PushBack(jsonProp,jsonDoc.GetAllocator());
//						iter.next();
//					}
//					jsonNode.AddMember("props",jsonProperties,jsonDoc.GetAllocator());


//				}
	return true;
}

gkGameObject* gkLogicNode::getObject(const gkString& name,GetObjectMode mode)
{
	if (name.empty())
		return getAttachedObject();

	if (mode == only_global)
	{
		return getAttachedObject()->getOwner()->getObject(name);
	}
	else if (mode == only_local)
	{
		gkGameObject* myObj = getAttachedObject();
		if (myObj->isGroupInstance())
		{
			return myObj->getGroupInstance()->getObject(name);
		}
		else
		{
			return getAttachedObject()->getOwner()->getObject(name);
		}
	}
	else if (mode == local_then_global)
	{
		gkGameObject* result = getObject(name,only_local);

		if (result)
			return result;

		return getObject(name,only_global);
	}
	else
	{
		gkLogger::write(getName()+" : UNKNOWN MODE! "+gkToString((int)mode));
		return 0;
	}
}

bool gkLogicNode::checkValidity()
{
	bool valid = true;
	int socketAmount = m_inputs.size() + m_outputs.size();
	for (int i=0; i<socketAmount; i++)
	{
		valid = valid
				&& m_sockets[i]->_checkValidity();
	}
	return m_isValid = valid;
}
