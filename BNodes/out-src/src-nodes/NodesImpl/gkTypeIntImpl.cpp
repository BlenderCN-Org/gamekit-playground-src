/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTypeIntImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTypeIntImpl::gkTypeIntImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTypeIntAbstract(parent,id,name)
{}

gkTypeIntImpl::~gkTypeIntImpl()
{}

void gkTypeIntImpl::_initialize()
{
	getOUT_VAL()->setValue(getIN_VAL(true)->getValue());
}

bool gkTypeIntImpl::_evaluate(gkScalar tick)
{
	return getIN_VAL()->isDirty();
}
void gkTypeIntImpl::_update(gkScalar tick)
{
	getOUT_VAL()->setValue(getIN_VAL(true)->getValue());
}
