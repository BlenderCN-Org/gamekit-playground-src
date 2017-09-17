/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkScreenNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkScreenNodeImpl::gkScreenNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkScreenNodeAbstract(parent,id,name)
{}

gkScreenNodeImpl::~gkScreenNodeImpl()
{}

void gkScreenNodeImpl::_initialize()
{
}

bool gkScreenNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkScreenNodeImpl::_update(gkScalar tick)
{
}

