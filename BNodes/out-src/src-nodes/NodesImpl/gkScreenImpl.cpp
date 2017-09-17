/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkScreenImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkScreenImpl::gkScreenImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkScreenAbstract(parent,id,name)
{}

gkScreenImpl::~gkScreenImpl()
{}

void gkScreenImpl::_initialize()
{
}

bool gkScreenImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkScreenImpl::_update(gkScalar tick)
{
}

