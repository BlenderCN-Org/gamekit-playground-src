/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkSeqTimeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkSeqTimeImpl::gkSeqTimeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkSeqTimeAbstract(parent,id,name), m_time(0.0f), m_parentSequence(0)
{}

gkSeqTimeImpl::~gkSeqTimeImpl()
{}

void gkSeqTimeImpl::_initialize()
{
	if (isValid())
	{
		m_parentSequence = static_cast<gkSequenceImpl*>(getIN_connect()->getFrom()->getParent());

		if (!m_parentSequence)
		{
			gkLogger::write(getName()+": THERE IS NO Connected Sequence!");
			m_isValid = false;
		}

		reset();
	}
}

bool gkSeqTimeImpl::_evaluate(gkScalar tick)
{
	return m_isValid && m_parentSequence->getOUT_ACTIVE()->getValue() && !isFinished();
}
void gkSeqTimeImpl::_update(gkScalar tick)
{
	m_time -= tick;
	if (m_time<=0)
	{
		m_time = 0;
		setFinished(true);
	}
}

bool gkSeqTimeImpl::debug(rapidjson::Value& jsonNode)
{

	addDebugSocket(jsonNode,SOCKET_IN_connect,"1",gkToString(isFinished()),true);

	setDebugStatus(jsonNode,"Time:"+gkToString(m_time));
	return true;
}

void gkSeqTimeImpl::reset()
{
	 gkSequenceImpl::gkSequenceBrick::reset();
	 m_time = getPropWAIT();
}
