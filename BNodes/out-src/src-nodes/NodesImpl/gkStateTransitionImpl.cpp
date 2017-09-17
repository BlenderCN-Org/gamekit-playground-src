/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkStateImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateMachineImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateRefImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateTransitionImpl.h>
#include <rapidjson/document.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Logic/gkNodeManager.h"
gkStateTransitionImpl::gkStateTransitionImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkStateTransitionAbstract(parent,id,name),m_fromState(0),m_toState(0)
{}

gkStateTransitionImpl::~gkStateTransitionImpl()
{}

void gkStateTransitionImpl::_initialize()
{
}

bool gkStateTransitionImpl::_evaluate(gkScalar tick)
{
	return false;
}
void gkStateTransitionImpl::_update(gkScalar tick)
{
}

void gkStateTransitionImpl::_afterInit2()
{
	checkValidity();
	if (!getIN_FROM()->isConnected() || !getOUT_TO()->isConnected())
	{
		gkLogger::write("ERROR "+getName()+": statetransition needs to have FROM and TO-sockets connected! ");
		return;
	}

	if (getOUT_TO()->getOutSocketAmount()>1)
	{
		gkLogger::write("ERROR "+getName()+": to-sockets is only allowed to have one connection!");
		return;
	}
	gkLogicNode* fromNode = getIN_FROM()->getFrom()->getParent();
	gkLogicNode* toNode = getOUT_TO()->getOutSocketAt(0)->getParent();

	if  (fromNode->getType() == "StateRef")
	{
		m_fromState = static_cast<gkStateRefImpl*>(fromNode)->getState();
	}
	else if (fromNode->getType() == "State")
	{
		m_fromState = static_cast<gkStateImpl*>(fromNode);
		if (!m_fromState)
		{
			gkLogger::write("ERROR "+getName()+" couldn't get state from "+fromNode->getName());
			m_isValid = false;
			return;
		}
	}
	else
	{
		gkLogger::write("ERROR "+getName()+": fromNode needs to be stateref or state!");
		return;
	}
	if (!m_fromState)
	{
		gkLogger::write("ERROR "+getName()+" couldn't get state from "+fromNode->getName());
		m_isValid = false;
		return;
	}

	if  (toNode->getType() == "StateRef")
	{
		m_toState = static_cast<gkStateRefImpl*>(toNode)->getState();
	}
	else if (toNode->getType() == "State")
	{
		m_toState = static_cast<gkStateImpl*>(toNode);
	}
	else
	{
		gkLogger::write("ERROR "+getName()+": toNode needs to be stateref or state!");
		return;
	}
	if (!m_toState)
	{
		gkLogger::write("ERROR "+getName()+" couldn't get state from "+toNode->getName());
		m_isValid = false;
		return;
	}

	gkStateMachineImpl* fsmNode = m_fromState->getStateMachine();

	if (fsmNode != m_toState->getStateMachine())
	{
		gkLogger::write("ERROR "+getName()+": fromNode and toNode have different statemachines!");
		return;
	}

	fsmNode->getFSM()->addTransition(m_fromState->getPropID(),m_toState->getPropID(),getPropWAIT())->when(new gkTransitionEvent(getIN_CONDITION()));

}


bool gkStateTransitionImpl::debug(rapidjson::Value& jsonVal)
{
	if (!m_fromState)
		return false;

	if (m_fromState->getOUT_ACTIVE()->getValue())
	{
		rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();
		// if fromState is active then set a color
		jsonVal.AddMember("color","(0.14,0.63,0.02)",jsonDoc.GetAllocator());
	}

	gkString fromStateActive = m_fromState->getOUT_ACTIVE()->getVar()->toString();
	gkString toStateActive = m_toState->getOUT_ACTIVE()->getVar()->toString();
	gkString inConditionState = gkToString(getIN_CONDITION()->getValue());

	addDebugSocket(jsonVal,"FROM",fromStateActive,"1",true);
	addDebugSocket(jsonVal,"TO",toStateActive,"1",false);
	addDebugSocket(jsonVal,"CONDITION",inConditionState,"1",true);

	return true;
}
