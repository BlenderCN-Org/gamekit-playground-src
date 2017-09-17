/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkVectorDecompImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkVectorDecompImpl::gkVectorDecompImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkVectorDecompAbstract(parent,id,name)
{}

gkVectorDecompImpl::~gkVectorDecompImpl()
{}

void gkVectorDecompImpl::_initialize()
{
	decomp();
}

bool gkVectorDecompImpl::_evaluate(gkScalar tick)
{
//	return getIN_vec()->isDirty();
	return true;
}
void gkVectorDecompImpl::_update(gkScalar tick)
{
	decomp();
}

void gkVectorDecompImpl::decomp()
{
	gkVector3 in = getIN_vec(true)->getValue();
	getOUT_x()->setValue(in.x);
	getOUT_y()->setValue(in.y);
	getOUT_z()->setValue(in.z);
}
