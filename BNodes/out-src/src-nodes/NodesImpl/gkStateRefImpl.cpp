/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkStateImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateMachineImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateRefImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Logic/gkNodeManager.h"

#include "Logic/gkLogicTree.h"
gkStateRefImpl::gkStateRefImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkStateRefAbstract(parent,id,name),m_state(0)
{}

gkStateRefImpl::~gkStateRefImpl()
{}

void gkStateRefImpl::_afterInit()
{
	// TODO: There needs to be an insocket tellin what object to use. for now you can only take the attached one
	gkString stmNAME = getPropGLOBAL()
							?getPropSTM()
							:getAttachedObject()->getName()+"_"+getPropSTM();



	gkStateMachineImpl* stm = gkStateMachineImpl::getStateMaschine(stmNAME);
	if (!stm) {
		gkString internalName = getAttachedObject()->getName()+"_"+getName();

		gkNodeManager* nodeManager = gkNodeManager::getSingletonPtr();
		gkLogicTree* tree = nodeManager->__getCurrentExecutionTree();

		gkLogger::write("ERROR "+getName()+"/"+tree->getName()+": Unknown statemachine with name:"+stmNAME);
		return;
	}

	m_state = stm->getStateByName(getPropSTATE());
	if (!m_state)
	{
		gkLogger::write("ERROR Unknown state:"+getPropSTATE()+" in Statemachine:"+stm->getName());
		return;
	}

	m_state->_addStartTriggersFromSocket(getOUT_START_TRIGGER());
	m_state->_addEndTriggersFromSocket(getOUT_END_TRIGGER());
	m_state->_addActiveFromSocket(getOUT_ACTIVE());
}

bool gkStateRefImpl::_evaluate(gkScalar tick)
{
	return false;
}
void gkStateRefImpl::_update(gkScalar tick)
{
}

bool gkStateRefImpl::debug(rapidjson::Value& jsonNode)
{
	// since this is only a wrapper use the values from the specific state and
	// set the name new
	if (!m_state)
		return false;

	bool result = m_state->debug(jsonNode);
	rapidjson::Document& doc = gkNodeManager::getSingleton().getDebugJSONDoc();
	jsonNode["nodeName"].SetString(getName().c_str(),doc.GetAllocator());

//	getOUT_START_TRIGGER()->getValue()
//	gkLogger::write(gkNodeManager::getSingleton().json2string(jsonNode));
	return result;
}
