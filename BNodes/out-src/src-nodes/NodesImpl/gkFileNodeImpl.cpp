/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkFileNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkFileNodeImpl::gkFileNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkFileNodeAbstract(parent,id,name)
{}

gkFileNodeImpl::~gkFileNodeImpl()
{}

void gkFileNodeImpl::_initialize()
{
}

bool gkFileNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkFileNodeImpl::_update(gkScalar tick)
{
}

