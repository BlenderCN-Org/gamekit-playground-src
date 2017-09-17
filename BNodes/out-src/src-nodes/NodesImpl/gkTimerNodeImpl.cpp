/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkTimerNodeImpl.h>
#include "Logic/gkLogicNode.h"

gkTimerNodeImpl::gkTimerNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkTimerNodeAbstract(parent,id,name), m_value(0),m_initValue(0),m_endValue(0),m_dir(0), m_active(false)
{}

gkTimerNodeImpl::~gkTimerNodeImpl()
{}

void gkTimerNodeImpl::_initialize()
{
	m_initValue = getIN_initTime()->getValue();
	setTimer();
}

void gkTimerNodeImpl::setTimer()
{
	if (getPropORDER()==ORDER_asc) {
		m_value = 0;
		m_endValue = m_initValue;
		m_dir = 1;
	} else {
		m_value = m_initValue;
		m_endValue = 0;
		m_dir = -1;
	}
	m_active = true;
}

bool gkTimerNodeImpl::_evaluate(gkScalar tick)
{
	if (getIN_RESET()->getValue()) {
		setTimer();
	}
	return m_active && getIN_UPDATE()->getValue();
}

void gkTimerNodeImpl::_update(gkScalar tick)
{
	bool finished = false;

	if (m_dir == 1)
	{
		m_value = m_value + tick;
		finished = m_value >= m_endValue;
	}
	else
	{
		m_value = m_value - tick;
		finished = m_value <= m_endValue; // 0
	}

	if (finished)
	{
		// turn it off, till the time gets resetted
		m_active = false;
		m_value = m_endValue;
	}

	getOUT_CURRENT_TIME()->setValue(m_value);
	getOUT_IS_DONE()->setValue(finished);
	getOUT_IS_RUNNING()->setValue(!finished);


}

