/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkStateImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateMachineImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkFSM.h"
gkStateMachineImpl::gkStateMachineImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkStateMachineAbstract(parent,id,name), m_fsm(0), m_initialized(false),m_currentStateNr(-1),m_active(false)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkStateMachineImpl::~gkStateMachineImpl()
{
	if (m_fsm)
		delete m_fsm;
}

void gkStateMachineImpl::_initialize()
{
	gkString stmName = getPropGLOBAL()
							?getPropNAME()
							:getAttachedObject()->getName()+"_"+getPropNAME();

	machinesByName.insert(stmName.c_str(),this);
	gkLogger::write("INIT STM:"+stmName);

	m_fsm = new gkFSM();

	// process connected states and register itself as state-node.
	int amountStates = getOUT_STATES()->getOutSocketAmount();
	for (int i=0;i<amountStates;i++)
	{
		gkLogicNode* node = getOUT_STATES()->getOutSocketAt(i)->getParent();
		if (node->getType() != "State")
		{
			gkLogger::write("Illegal state-connection to node of type:" + node->getType());
			continue;
		}
		gkStateImpl* state = static_cast<gkStateImpl*>(node);
		int stateId = state->getPropID();
		gkString stateName = state->getPropNAME();
		m_statesByName.insert(stateName.c_str(),state);
		m_statesByID.insert(state->getPropID(),state);
		// set a ref to this statemachine-node where the state-node will add its triggers to this FSM
		state->_setParentFSM(this);
	}

}

bool gkStateMachineImpl::isActive()
{
	bool active = getIN_UPDATE()->getValue();

	if (!m_active && active)
	{
		m_active = true;
		if (m_currentStateNr!=-1)
			m_fsm->setState(m_currentStateNr);
	}
	else if (m_active && !active)
	{
		m_currentStateNr = m_fsm->getState();
		m_active = false;
		m_fsm->setState(-1);
	}
	return active;
}

bool gkStateMachineImpl::_evaluate(gkScalar tick)
{
	return this->isActive();
}
void gkStateMachineImpl::_preUpdate()
{
	gkString name(getPropNAME());
	if (!m_initialized)
	{
		gkStateImpl** initialState = m_statesByName.get(getPropINITIAL_STATE().c_str());

		if (!initialState)
		{
			gkLogger::write("ERROR: unknown initial state:"+getPropINITIAL_STATE());
			return;
		}

		m_currentStateNr=(*initialState)->getPropID();
		m_fsm->setState( (*initialState)->getPropID());
		m_initialized=true;
	}
	// check for state-transitions
	m_fsm->update();

	getOUT_CURRENT_STATE()->setValue(m_fsm->getState());
}

void gkStateMachineImpl::_postUpdate()
{
	// first reset all sockets that were set by start/end-trigger
	if (m_resetTriggerList.size()>0)
	{
		ResetTriggerList::Iterator iter(m_resetTriggerList);
		while (iter.hasMoreElements())
		{
			gkLogicSocket<bool>* sock = iter.getNext();
			sock->setValue(false);
		}
		m_resetTriggerList.clear();
	}
}

gkStateImpl* gkStateMachineImpl::getStateByName(const gkString& name)
{
	gkStateImpl** state = m_statesByName.get(name.c_str());
	if (!state)
		return 0;
	return *state;
}

gkStateImpl* gkStateMachineImpl::getStateByID(int id)
{
	gkStateImpl** state = m_statesByID.get(id);
	if (!state)
		return 0;
	return *state;
}

gkStateImpl* gkStateMachineImpl::getCurrentState()
{
	gkStateImpl* currentState = getStateByID(m_fsm->getState());
	return currentState;
}

void gkStateMachineImpl::setState(gkStateImpl* newState)
{
	int stateID = newState->getPropID();
	if (!newState
			|| stateID==m_fsm->getState())
		return;

	m_fsm->setState(stateID,true);
}
void gkStateMachineImpl::setState(int ID)
{
	if (ID==m_fsm->getState())
		return;

	m_fsm->setState(ID,true);
}


gkStateMachineImpl::StateMachinesByName gkStateMachineImpl::machinesByName;

gkStateMachineImpl* gkStateMachineImpl::getStateMaschine(const gkString& name)
{
	gkStateMachineImpl** statemachine = machinesByName.get(name.c_str());
	if (!statemachine)
		return 0;
	return *statemachine;
}


bool gkStateMachineImpl::debug(rapidjson::Value& jsonNode)
{
	bool result = gkStateMachineAbstract::debug(jsonNode);
//	gkLogger::write(gkNodeManager::getSingleton().json2string(jsonNode));
	return result;
}
