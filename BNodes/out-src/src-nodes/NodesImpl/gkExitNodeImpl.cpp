/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkExitNodeImpl.h>
#include "Logic/gkLogicNode.h"

gkExitNodeImpl::gkExitNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)  : gkExitNodeAbstract(parent,id,name)
{}

gkExitNodeImpl::~gkExitNodeImpl()
{}

void gkExitNodeImpl::_initialize()
{

}

bool gkExitNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkExitNodeImpl::_update(gkScalar tick)
{
}

