/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTypeColorImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkTypeColorImpl::gkTypeColorImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTypeColorAbstract(parent,id,name)
{}

gkTypeColorImpl::~gkTypeColorImpl()
{}

void gkTypeColorImpl::setColor()
{
	gkColor newCol(getIN_R(true)->getValue(),getIN_G(true)->getValue(),getIN_B(true)->getValue(),getIN_A(true)->getValue());
	if (newCol!=m_col)
	{
		m_col = newCol;
		getOUT_COLOR()->setValue(m_col);
	}
}

void gkTypeColorImpl::_initialize()
{
	setColor();
}

bool gkTypeColorImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkTypeColorImpl::_update(gkScalar tick)
{
	if (getIN_R()->isDirty() ||getIN_G()->isDirty() ||getIN_B()->isDirty() ||getIN_A()->isDirty())
	{
		setColor();
	}
}

