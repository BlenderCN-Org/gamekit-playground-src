/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkStateImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateMachineImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateManipulatorImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkStateRefImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"


gkStateManipulatorImpl::gkStateManipulatorImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkStateManipulatorAbstract(parent,id,name), m_stateRef(0),m_type((TYPE)0)
{}

gkStateManipulatorImpl::~gkStateManipulatorImpl()
{}

void gkStateManipulatorImpl::_afterInit()
{

	m_type = (TYPE)getPropTYPE();

	if (m_isValid) // this guartenees that I have excactly one manipulation StateRef
	{
		m_stateRef = static_cast<gkStateRefImpl*>(getOUT_MANIP_STATE()->getOutSocketAt(0)->getParent());
	}
}

bool gkStateManipulatorImpl::_evaluate(gkScalar tick)
{
	return m_isValid
			&& m_stateRef
			&& getIN_UPDATE()->getValue();
}
void gkStateManipulatorImpl::_update(gkScalar tick)
{
	if (m_type == TYPE_setstate)
	{
		if (!m_stateRef)
		{
			gkLogger::write("ERROR "+getName()+": No state in set-state-type:"+gkToString(m_type));
		}
		m_stateRef->getState()->activateStateInStatemachine();
	}
	else
	{
		gkLogger::write("ERROR "+getName()+": Unknown Type in StateManipulator:"+gkToString(m_type));
		m_isValid=false;
	}
}

