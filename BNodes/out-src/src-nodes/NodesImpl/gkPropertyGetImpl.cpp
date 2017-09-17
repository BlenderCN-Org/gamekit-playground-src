/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPropertyGetImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkGameObject.h"
#include "gkGameObjectInstance.h"

gkPropertyGetImpl::gkPropertyGetImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPropertyGetAbstract(parent,id,name), m_target(0), m_var(0)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkPropertyGetImpl::~gkPropertyGetImpl()
{}

void gkPropertyGetImpl::_afterInit()
{
	m_target = getIN_TARGET_OBJ()->isConnected()
									? getIN_TARGET_OBJ()->getGameObject()
									: getAttachedObject();

	getVar();
}

bool gkPropertyGetImpl::_evaluate(gkScalar tick)
{
	return true;
}

void gkPropertyGetImpl::_update(gkScalar tick)
{
	getVar();
}

bool gkPropertyGetImpl::debug(rapidjson::Value& jsonVal)
{
	gkString OUT_VALUE =  getOUT_VALUE()->getVar()->toString();
	addDebugSocket(jsonVal,"VALUE","10",OUT_VALUE,false);
	return true;
}

void gkPropertyGetImpl::getVar()
{
	gkString propName = getPropNAME();
	if (!m_var)
	{
		if (m_target->isGroupInstance())
		{
			// if part of group-instance first check if the variable is overriden on the group-instance
			m_var = m_target->getGroupInstance()->getRoot()->getVariable(propName);
		}

		if (!m_var)
		{
			// no variable found-yet, check on the object itselfs
			m_var = m_target->getVariable(propName);
		}
	}

	if (!m_var)
	{
//		gkLogger::write(getName()+": Couldn't find target-property:"+getPropNAME());
		return;
	}
	gkVariable* var = getOUT_VALUE()->getVar();
	if (*var != *m_var)
	{
		getOUT_VALUE()->setVar(m_var);
	}
}
