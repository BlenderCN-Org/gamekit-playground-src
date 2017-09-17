/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkAdditionNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"
#include "Script/Lua/gkLuaUtils.h"
#include "Script/Api/gsNodes.h"

gkAdditionNodeImpl::gkAdditionNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkAdditionNodeAbstract(parent,id,name)
{}

gkAdditionNodeImpl::~gkAdditionNodeImpl()
{
}



