/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkIfNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkIfNodeImpl::gkIfNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkIfNodeAbstract(parent,id,name), m_a(0),m_b(0)
{}

gkIfNodeImpl::~gkIfNodeImpl()
{}

void gkIfNodeImpl::_initialize()
{
	if (getIN_a()->isConnected() && getIN_b()->isConnected())
	{
		if (getIN_a()->getVar()->getType() != getIN_b()->getVar()->getType())
		{
			m_isValid = false;
		}
		else
		{
			m_a = getIN_a()->getVar();
			m_b = getIN_b()->getVar();
		}
	}
}

bool gkIfNodeImpl::_evaluate(gkScalar tick)
{
	return getIN_a()->isDirty() || getIN_b()->isDirty();
}
void gkIfNodeImpl::_update(gkScalar tick)
{
	gkVariable a;
	gkVariable b;

	if (getIN_a()->isConnected())
		a.assign(*getIN_a()->getVar());

	if (getIN_b()->isConnected())
		b.assign(*getIN_b()->getVar());

	if (!getIN_a()->isConnected() && getIN_b()->isConnected())
		a.setValue(getIN_b()->getVar()->getType(),getIN_a()->getValue());

	if (!getIN_b()->isConnected() && getIN_a()->isConnected())
		b.setValue(getIN_a()->getVar()->getType(),getIN_b()->getValue());


	bool result = false;
	switch (getPropFUNC())
	{
		case FUNC_EQ:
						result = a == b;
						break;
		case FUNC_GE:
						result = a >= b;
						break;
		case FUNC_GT:
						result = a > b;
						break;
		case FUNC_LE:
						result = a <= b;
						break;
		case FUNC_LT:
						result = a < b;
						break;
		case FUNC_NOT:
						result = a != b;
						break;
	}

	getOUT_true()->setValue(result);
	getOUT_false()->setValue(!result);
}

