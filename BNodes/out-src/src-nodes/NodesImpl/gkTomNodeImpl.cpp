/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include "gkTomNodeImpl.h"
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTomNodeImpl::gkTomNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTomNodeAbstract(parent,id,name)
{}

gkTomNodeImpl::~gkTomNodeImpl()
{}

void gkTomNodeImpl::_initialize()
{
}

bool gkTomNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkTomNodeImpl::_update(gkScalar tick)
{
}

