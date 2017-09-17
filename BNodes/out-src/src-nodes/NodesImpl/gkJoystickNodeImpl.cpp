/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkJoystickNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkWindowSystem.h"
#include "gkInput.h"

gkJoystickNodeImpl::gkJoystickNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkJoystickNodeAbstract(parent,id,name), m_joystick(0), m_axisAmount(0), m_deathMax(0), m_deathMin(0)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkJoystickNodeImpl::~gkJoystickNodeImpl()
{}

void gkJoystickNodeImpl::_afterInit2()
{
	if (!initJoystick())
		return;

	getOUT_IS_ACTIVE()->setValue(true);
	m_axisAmount = m_joystick->getAxesNumber();
	m_deathMax = getPropdeathZoneMax();
	m_deathMin = getPropdeathZoneMin();
}

bool gkJoystickNodeImpl::initJoystick()
{
	int joystickNr = getIN_joystickNr(true)->getValue();
	m_joystick = gkWindowSystem::getSingleton().getJoystick(joystickNr);
	if (!m_joystick) {
		m_isValid = false;
		getOUT_IS_ACTIVE()->setValue(false);
		return false;
	}
	return true;
}


bool gkJoystickNodeImpl::_evaluate(gkScalar tick)
{
	if (getIN_joystickNr(true)->isDirty()) {
		initJoystick();
	}
	return m_isValid && getIN_UPDATE(true)->getValue();
}
void gkJoystickNodeImpl::_update(gkScalar tick)
{
	if (m_axisAmount > 0) {
		float val = m_joystick->getAxisValue(0);

		if (val < m_deathMin) {
			val = val + m_deathMin;
		}
		else if (val > m_deathMax) {
			val = val - m_deathMax;
		}
		else {
			val = 0;
		}
		getOUT_AXIS1()->setValue((float)val);
		getOUT_AXIS1_REL()->setValue(m_joystick->getRelAxisValue(0));

		getOUT_LEFT()->setValue(val < 0);
		getOUT_RIGHT()->setValue(val > 0);
	}

	if (m_axisAmount > 1){
		float val = m_joystick->getAxisValue(1);

		if (val < m_deathMin) {
			val = val + m_deathMin;
		}
		else if (val > m_deathMax) {
			val = val - m_deathMax;
		}
		else {
			val = 0;
		}
		getOUT_AXIS2()->setValue((float)val);
		getOUT_AXIS2_REL()->setValue(m_joystick->getRelAxisValue(1));

		getOUT_UP()->setValue(val < 0);
		getOUT_DOWN()->setValue(val > 0);
	}

	if (m_axisAmount > 2){
		float val = m_joystick->getAxisValue(2);
		getOUT_AXIS3()->setValue((float)val);
		getOUT_AXIS3_REL()->setValue(m_joystick->getRelAxisValue(2));
	}

	if (m_axisAmount > 3){
		float val = m_joystick->getAxisValue(3);
		getOUT_AXIS4()->setValue((float)val);
		getOUT_AXIS4_REL()->setValue(m_joystick->getRelAxisValue(3));
	}
}

