/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleWheelImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkVehicleWheelImpl::gkVehicleWheelImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkVehicleWheelAbstract(parent,id,name)
{}

gkVehicleWheelImpl::~gkVehicleWheelImpl()
{}

void gkVehicleWheelImpl::_initialize()
{
}

bool gkVehicleWheelImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkVehicleWheelImpl::_update(gkScalar tick)
{
}

