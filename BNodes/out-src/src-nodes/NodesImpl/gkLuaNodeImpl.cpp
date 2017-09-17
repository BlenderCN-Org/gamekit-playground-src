/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkLuaNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"
#include "Script/Lua/gkLuaManager.h"
#include "gkResourceName.h"

gkLuaNodeImpl::gkLuaNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkLuaNodeAbstract(parent,id,name)
{
	// let lua nodes be executed at last
	setBucketNr(((int)BUCKET_EXECUTOR)+1);
}

gkLuaNodeImpl::~gkLuaNodeImpl()
{
}

void gkLuaNodeImpl::_initialize()
{
}

bool gkLuaNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkLuaNodeImpl::_update(gkScalar tick)
{
}

