/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTemplaterNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTemplaterNodeImpl::gkTemplaterNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTemplaterNodeAbstract(parent,id,name)
{}

gkTemplaterNodeImpl::~gkTemplaterNodeImpl()
{}

void gkTemplaterNodeImpl::_initialize()
{
}

bool gkTemplaterNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkTemplaterNodeImpl::_update(gkScalar tick)
{
}

