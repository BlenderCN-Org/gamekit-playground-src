/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkStateImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateMachineImpl.h>
#include <rapidjson/document.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkFSM.h"

#include "Logic/gkNodeManager.h"
gkStateImpl::gkStateImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkStateAbstract(parent,id,name), m_fsmNode(0), m_startTrigger(0), m_endTrigger(0)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkStateImpl::~gkStateImpl()
{}

void gkStateImpl::_initialize()
{
	_addActiveFromSocket(getOUT_ACTIVE());
	_addStartTriggersFromSocket(getOUT_START_TRIGGER());
	_addEndTriggersFromSocket(getOUT_END_TRIGGER());
}

void gkStateImpl::_addStartTriggersFromSocket(gkILogicSocket* sock)
{
	int amount = sock->getOutSocketAmount();
	for (int i=0;i<amount;i++)
	{
		// get the input-sockets of the nodes that are connected to this socket and
		// wire it to my start_trigger-socket
		gkILogicSocket* inSock = sock->getOutSocketAt(i);
		getOUT_START_TRIGGER()->link(inSock);
		inSock->link(getOUT_START_TRIGGER());
	}

}
void gkStateImpl::_addEndTriggersFromSocket(gkILogicSocket* sock)
{
	int amount = sock->getOutSocketAmount();
	for (int i=0;i<amount;i++)
	{
		// get the input-sockets of the nodes that are connected to this socket and
		// wire it to my start_trigger-socket
		gkILogicSocket* inSock = sock->getOutSocketAt(i);
		getOUT_END_TRIGGER()->link(inSock);
		inSock->link(getOUT_END_TRIGGER());
	}
}
void gkStateImpl::_addActiveFromSocket(gkILogicSocket* sock)
{
	int amount = sock->getOutSocketAmount();
	for (int i=0;i<amount;i++)
	{
		// get the input-sockets of the nodes that are connected to this socket and
		// wire it to my start_trigger-socket
		gkILogicSocket* inSock = sock->getOutSocketAt(i);
		getOUT_ACTIVE()->link(inSock);
		inSock->link(getOUT_ACTIVE());
	}
}


bool gkStateImpl::_evaluate(gkScalar tick)
{
	// nothing to do here. all logic is done from within the triggers
	return false;
}
void gkStateImpl::_update(gkScalar tick)
{
}

void gkStateImpl::_setParentFSM(gkStateMachineImpl* fsmNode)
{
	if (m_fsmNode)
	{
		gkLogger::write("StateImpl has already a statemachine! Ignoring");
		return;
	}
	this->m_fsmNode = fsmNode;
	m_startTrigger = new gkStateTrigger(true,this);
	m_endTrigger = new gkStateTrigger(false,this);

	const int stateId = getPropID();
	m_fsmNode->getFSM()->addStartTrigger(stateId,m_startTrigger);
	m_fsmNode->getFSM()->addEndTrigger(stateId,m_endTrigger);
}

void gkStateImpl::gkStateTrigger::execute(int from, int to)
{
	if (m_isStartTrigger)
	{
		m_state->getOUT_ACTIVE()->setValue(true);
		m_state->getOUT_START_TRIGGER()->setValue(true);
		// register this socket in order to be set to false on next tick
		m_state->m_fsmNode->addResetTrigger(m_state->getOUT_START_TRIGGER());
	}
	else
	{
		m_state->getOUT_ACTIVE()->setValue(false);
		m_state->getOUT_END_TRIGGER()->setValue(true);
		// register this socket in order to be set to false on next tick
		m_state->m_fsmNode->addResetTrigger(m_state->getOUT_END_TRIGGER());
	}
}


bool gkStateImpl::debug(rapidjson::Value& jsonVal)
{
	gkLogicNode::debug(jsonVal);
	if (getOUT_ACTIVE()->getValue())
	{
		rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();
		// if fromState is active then set a color
		jsonVal.AddMember("color","(0.14,0.63,0.02)",jsonDoc.GetAllocator());
	}

//	gkLogger::write(gkNodeManager::getSingleton().json2string(jsonVal));

	return true;
}

void gkStateImpl::activateStateInStatemachine()
{
	m_fsmNode->setState(this);
}
