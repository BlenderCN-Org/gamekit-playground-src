/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTypeFloatImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTypeFloatImpl::gkTypeFloatImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTypeFloatAbstract(parent,id,name)
{}

gkTypeFloatImpl::~gkTypeFloatImpl()
{}

void gkTypeFloatImpl::_initialize()
{
	getOUT_VAL()->setValue(getIN_VAL(true)->getValue());
}

bool gkTypeFloatImpl::_evaluate(gkScalar tick)
{
	return getIN_VAL()->isDirty();
}
void gkTypeFloatImpl::_update(gkScalar tick)
{
	getOUT_VAL()->setValue(getIN_VAL(true)->getValue());
}
