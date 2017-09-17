/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMyNewNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkMyNewNodeImpl::gkMyNewNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkMyNewNodeAbstract(parent,id,name)
{}

gkMyNewNodeImpl::~gkMyNewNodeImpl()
{}

void gkMyNewNodeImpl::_initialize()
{
}

bool gkMyNewNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkMyNewNodeImpl::_update(gkScalar tick)
{
}

