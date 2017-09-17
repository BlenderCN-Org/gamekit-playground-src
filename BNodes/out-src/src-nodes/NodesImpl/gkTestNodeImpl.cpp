/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTestNodeImpl.h>
#include "Logic/gkLogicNode.h"

gkTestNodeImpl::gkTestNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)  : gkTestNodeAbstract(parent,id,name)
{}

gkTestNodeImpl::~gkTestNodeImpl()
{}

void gkTestNodeImpl::_initialize()
{
}

bool gkTestNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkTestNodeImpl::_update(gkScalar tick)
{
	float fac = getPropFac();
	getAttachedObject()->rotate(gkEuler(fac,fac,fac));
}

