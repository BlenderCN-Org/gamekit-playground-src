/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSSetImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkPBSSetImpl::gkPBSSetImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSSetAbstract(parent,id,name)
{}

gkPBSSetImpl::~gkPBSSetImpl()
{}

void gkPBSSetImpl::_initialize()
{
}

bool gkPBSSetImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkPBSSetImpl::_update(gkScalar tick)
{
}

