/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPropertyImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Logic/gkNodeManager.h"

gkPropertyImpl::gkPropertyImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPropertyAbstract(parent,id,name), m_mode((MODE)0), m_targetObject(0),m_varType((TYPE)0), m_var(0),m_currentValueAsString("__NOVAL__")
{
	setBucketNr((int)BUCKET_SETTER);
}

gkPropertyImpl::~gkPropertyImpl()
{}

void gkPropertyImpl::_initialize()
{
	if (getIN_OVERRIDE_NAME()->isConnected())
	{
		int a=0;
	}



	m_mode = (MODE)getPropMODE();
	m_varType = (TYPE)getPropTYPE();
	m_propName = getIN_OVERRIDE_NAME()->isConnected()
					?""
					:getPropNAME();


	gkGameObject* targetObject = getIN_TARGET_OBJ()->isConnected()
									? getIN_TARGET_OBJ()->getGameObject()
									: (m_targetObject = getObject(getIN_TARGET_OBJ()->getValue()));

	if (!targetObject)
		return;

	if (m_mode == MODE_init_property) // if not create it and use the values from the node as initial values
	{
		m_var = targetObject->getVariable(m_propName);

		if (m_var)
		{
			// this property is already created??
			gkLogger::write(getName()+": THERE IS ALREADY AN INITALIZATION for property:"+m_propName);

			m_isValid = false;
			return;
		}
		m_var = targetObject->createVariable(m_propName,true);
		const gkString& value = getIN_VALUE()->isConnected()
										? getIN_VALUE()->getFrom()->getVar()->toString()
										: getIN_VALUE()->getValue();
		setValueFromINPUT(value);

	}
}


bool gkPropertyImpl::_evaluate(gkScalar tick)
{
	bool dirty = getIN_VALUE()->isDirty();
	bool enabled = getIN_ENABLED()->getValue();
	return getIN_ENABLED()->getValue()
			&&getIN_ENABLED()->isDirty();
}
void gkPropertyImpl::_update(gkScalar tick)
{
	gkGameObject* targetObject = m_targetObject
									? m_targetObject
									: getIN_TARGET_OBJ()->getGameObject();

	if (!targetObject)
		return;

	gkString propName = !m_propName.empty()
							?m_propName
							:getIN_OVERRIDE_NAME()->getVar(true)->toString();

	if (!m_var) // first check if this value is already created
	{
		m_var = targetObject->getVariable(propName);

		if (!m_var) // this var is not initialized? create it
		{
			m_var = targetObject->createVariable(propName,true);
			// no need to set the value since this should be done in the following sections
		}
	}



	if (m_mode == MODE_set_property)
	{
		if (getIN_OVERRIDE_NAME()->isConnected())
			{
				int a=0;
			}

		bool connected = getIN_VALUE()->isConnected();
		if (connected)
		{
			gkVariable* var = getIN_VALUE(true)->getFrom()->getVar();
			gkString st = var->toString();
			int a=0;
		}

		const gkString& value = getIN_VALUE()->isConnected()
										? getIN_VALUE(true)->getFrom()->getVar()->toString()
										: getIN_VALUE(true)->getValue();

		gkString curVal = m_var->toString();
		if (value == curVal)
			return;

		setValueFromINPUT(value);

		getOUT_VALUE()->setVar(m_var);
	}
	else if (m_mode == MODE_remove_property)
	{
		targetObject->removeVariable(propName);
		getOUT_VALUE()->setValue("");
	}
}


void gkPropertyImpl::setValueFromINPUT(const gkString& value)
{
//	if (getIN_VALUE()->isConnected())
//	{
//		gkVariable* var = getIN_VALUE()->getFrom()->getVar();
//		m_var->assign(*var);
//		return;
//	}
//	const gkString& value = getIN_VALUE()->getValue();

	switch (m_varType)
	{
	case TYPE_bool:
				bool b;
				gkFromString(value,b);
				m_var->setValue(b);
				break;
	case TYPE_float:
				float f;
				gkFromString(value,f);
				m_var->setValue(f);
				break;
	case TYPE_string:
				m_var->setValue(value);
				break;
	default:
			gkLogger::write(getName()+" : UNKNOWN VALUE-TYPE:" + gkToString((int)m_varType));
			m_var = 0;
	}
}

bool gkPropertyImpl::debug(rapidjson::Value& jsonVal)
{
	gkString IN_ENABLED =  getIN_ENABLED()->getVar(true)->toString();
	gkString IN_VALUE =  getIN_VALUE()->getVar(true)->toString();
	gkString IN_TARGET =  getIN_TARGET_OBJ()->getVar(true)->toString();

	gkString OUT_VALUE =  getOUT_VALUE()->getVar()->toString();

	addDebugSocket(jsonVal,"ENABLED","1",IN_ENABLED,true);
	addDebugSocket(jsonVal,"TARGET_OBJ","10",IN_TARGET,true);

	// those are the cause: we need to force string for debugging reason, otherwise the type
	// would have been whatever valueType is currently attached which will not work on the blender-side
	addDebugSocket(jsonVal,"VALUE","10",IN_VALUE,true);
	addDebugSocket(jsonVal,"VALUE","10",OUT_VALUE,false);

//	gkString result = gkNodeManager::getSingleton().json2string<rapidjson::Value>(jsonVal);
//	gkLogger::write(result,true);

	return true;
}
