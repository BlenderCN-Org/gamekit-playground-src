/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkElementImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkElementImpl::gkElementImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkElementAbstract(parent,id,name)
{}

gkElementImpl::~gkElementImpl()
{}

void gkElementImpl::_initialize()
{
}

bool gkElementImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkElementImpl::_update(gkScalar tick)
{
}

