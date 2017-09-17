/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkNoOpImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkNoOpImpl::gkNoOpImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkNoOpAbstract(parent,id,name)
{}

gkNoOpImpl::~gkNoOpImpl()
{}

void gkNoOpImpl::_initialize()
{
}

bool gkNoOpImpl::_evaluate(gkScalar tick)
{
	return false;
}
void gkNoOpImpl::_update(gkScalar tick)
{
}

