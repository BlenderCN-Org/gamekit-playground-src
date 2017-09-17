/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkCharacterNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkCharacterNodeImpl::gkCharacterNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkCharacterNodeAbstract(parent,id,name)
{}

gkCharacterNodeImpl::~gkCharacterNodeImpl()
{}

void gkCharacterNodeImpl::_initialize()
{
}

bool gkCharacterNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkCharacterNodeImpl::_update(gkScalar tick)
{
}

