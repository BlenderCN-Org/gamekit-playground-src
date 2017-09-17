/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkSeqFinishedImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkSeqFinishedImpl::gkSeqFinishedImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkSeqFinishedAbstract(parent,id,name),gkSequenceBrick(),m_type(TYPE_ONCE)
{}

gkSeqFinishedImpl::~gkSeqFinishedImpl()
{}

void gkSeqFinishedImpl::_initialize()
{
	m_type = (TYPE)getPropTYPE();
}

bool gkSeqFinishedImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkSeqFinishedImpl::_update(gkScalar tick)
{
	if (getIN_finished()->isDirty())
	{
		if (getIN_finished(true)->getValue())
		{
			setFinished(true);
		}
		else
		{
			// TYPE_ONCE: don not unfinish brick once it was set true

			// TYPE_ALWAYS: you set the current state
			if (getPropTYPE() == TYPE_ALWAYS)
			{
				setFinished(false);
			}
		}
	}
}

bool gkSeqFinishedImpl::debug(rapidjson::Value& jsonNode)
{
	addDebugSocket(jsonNode,gkSeqFinishedImpl::SOCKET_IN_finished,"1",gkToString(isFinished()),true);
	addDebugSocket(jsonNode,gkSeqFinishedImpl::SOCKET_IN_connect,"1",gkToString(isFinished()),true);
	return true;
}
