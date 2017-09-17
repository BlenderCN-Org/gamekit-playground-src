/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTypeBoolImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTypeBoolImpl::gkTypeBoolImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTypeBoolAbstract(parent,id,name)
{}

gkTypeBoolImpl::~gkTypeBoolImpl()
{}

void gkTypeBoolImpl::_initialize()
{
	getOUT_VAL()->setValue(getIN_VAL(true)->getValue());
}

bool gkTypeBoolImpl::_evaluate(gkScalar tick)
{
	return getIN_VAL()->isDirty();
}
void gkTypeBoolImpl::_update(gkScalar tick)
{
	getOUT_VAL()->setValue(getIN_VAL(true)->getValue());
}

