/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleGearImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkVehicleGearImpl::gkVehicleGearImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkVehicleGearAbstract(parent,id,name)
{}

gkVehicleGearImpl::~gkVehicleGearImpl()
{}

void gkVehicleGearImpl::_initialize()
{
}

bool gkVehicleGearImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkVehicleGearImpl::_update(gkScalar tick)
{
}

