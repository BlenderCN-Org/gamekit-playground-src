/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkStateMachineRefImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkStateMachineRefImpl::gkStateMachineRefImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkStateMachineRefAbstract(parent,id,name)
{}

gkStateMachineRefImpl::~gkStateMachineRefImpl()
{}

void gkStateMachineRefImpl::_initialize()
{

}

bool gkStateMachineRefImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkStateMachineRefImpl::_update(gkScalar tick)
{
}

