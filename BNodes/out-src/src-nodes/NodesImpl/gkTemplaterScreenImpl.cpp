/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTemplaterScreenImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTemplaterScreenImpl::gkTemplaterScreenImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTemplaterScreenAbstract(parent,id,name)
{}

gkTemplaterScreenImpl::~gkTemplaterScreenImpl()
{}

void gkTemplaterScreenImpl::_initialize()
{
}

bool gkTemplaterScreenImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkTemplaterScreenImpl::_update(gkScalar tick)
{
}

