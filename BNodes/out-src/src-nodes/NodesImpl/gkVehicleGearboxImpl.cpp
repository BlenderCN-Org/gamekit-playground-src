/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkVehicleGearboxImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkVehicleGearboxImpl::gkVehicleGearboxImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkVehicleGearboxAbstract(parent,id,name)
{}

gkVehicleGearboxImpl::~gkVehicleGearboxImpl()
{}

void gkVehicleGearboxImpl::_initialize()
{
}

bool gkVehicleGearboxImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkVehicleGearboxImpl::_update(gkScalar tick)
{
}

