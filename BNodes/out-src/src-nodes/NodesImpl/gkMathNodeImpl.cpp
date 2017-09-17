/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMathNodeImpl.h>
#include "Logic/gkLogicNode.h"

gkMathNodeImpl::gkMathNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)  : gkMathNodeAbstract(parent,id,name)
{}

gkMathNodeImpl::~gkMathNodeImpl()
{}

void gkMathNodeImpl::_initialize()
{}

bool gkMathNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkMathNodeImpl::_update(gkScalar tick)
{
}

