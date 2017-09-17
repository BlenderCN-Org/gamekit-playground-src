/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkScreenActionImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkScreenActionImpl::gkScreenActionImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkScreenActionAbstract(parent,id,name)
{}

gkScreenActionImpl::~gkScreenActionImpl()
{}

void gkScreenActionImpl::_initialize()
{
}

bool gkScreenActionImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkScreenActionImpl::_update(gkScalar tick)
{
}

