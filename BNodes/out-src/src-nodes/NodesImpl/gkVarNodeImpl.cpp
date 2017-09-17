/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkVarNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkVarNodeImpl::gkVarNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkVarNodeAbstract(parent,id,name)
{}

gkVarNodeImpl::~gkVarNodeImpl()
{}

void gkVarNodeImpl::_initialize()
{
}

bool gkVarNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkVarNodeImpl::_update(gkScalar tick)
{
}

