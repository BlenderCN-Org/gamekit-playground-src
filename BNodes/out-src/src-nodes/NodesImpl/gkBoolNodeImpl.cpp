/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkBoolNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkBoolNodeImpl::gkBoolNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkBoolNodeAbstract(parent,id,name)
{}

gkBoolNodeImpl::~gkBoolNodeImpl()
{}

void gkBoolNodeImpl::_initialize()
{
}

bool gkBoolNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkBoolNodeImpl::_update(gkScalar tick)
{
	bool a = getIN_A()->getValue();
	bool b = getIN_B()->getValue();
	int opInt = getPropOperation();
	Operation op = (Operation)opInt;
	switch (op)
	{
	bool result;
	case Operation_and:
		result = a && b;
		getOUT_RESULT()->setValue(result);
		getOUT_NOT()->setValue(!result);
		break;
	case Operation_or:
		result = a || b;
		getOUT_RESULT()->setValue(result);
		getOUT_NOT()->setValue(!result);
		break;
	case Operation_not:
		result = !a;
		getOUT_RESULT()->setValue(result);
		getOUT_NOT()->setValue(!result);
		break;
	case Operation_if:
		getOUT_RESULT()->setValue(a);
		getOUT_NOT()->setValue(!a);
		break;
	default:
		gkLogger::write("ERROR: Unknown Operation:"+gkToString(getPropOperation()));
		break;
	}
}

