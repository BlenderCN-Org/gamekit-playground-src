/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkDeltaNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkDeltaNodeImpl::gkDeltaNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkDeltaNodeAbstract(parent,id,name)
{}

gkDeltaNodeImpl::~gkDeltaNodeImpl()
{}

void gkDeltaNodeImpl::_initialize()
{
}

bool gkDeltaNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkDeltaNodeImpl::_update(gkScalar tick)
{
}

