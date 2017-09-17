/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkEngineNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkEngineNodeImpl::gkEngineNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkEngineNodeAbstract(parent,id,name)
{}

gkEngineNodeImpl::~gkEngineNodeImpl()
{}

void gkEngineNodeImpl::_initialize()
{
}

bool gkEngineNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkEngineNodeImpl::_update(gkScalar tick)
{
}

