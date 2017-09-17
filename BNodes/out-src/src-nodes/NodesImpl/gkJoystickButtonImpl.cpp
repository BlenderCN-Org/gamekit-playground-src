/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkJoystickButtonImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkWindowSystem.h"

#include "gkInput.h"
gkJoystickButtonImpl::gkJoystickButtonImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkJoystickButtonAbstract(parent,id,name), m_joystick(0),m_btnNr(0),m_pressed(false)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkJoystickButtonImpl::~gkJoystickButtonImpl()
{}

void gkJoystickButtonImpl::_afterInit2()
{
	m_btnNr = getPropbuttonNr();
	initJoystick();
	if (!m_joystick)  {
		m_isValid = false;
	}
}

bool gkJoystickButtonImpl::initJoystick()
{
	int joystickNr = getIN_joystickNr(true)->getValue();
	m_joystick = gkWindowSystem::getSingleton().getJoystick(joystickNr);
	if (!m_joystick) {
		m_isValid = false;
		return false;
	}
	return true;
}

bool gkJoystickButtonImpl::_evaluate(gkScalar tick)
{
	if (getIN_joystickNr(true)->isDirty()) {
		initJoystick();
	}
	return m_isValid && getIN_UPDATE()->getValue();
}
void gkJoystickButtonImpl::_update(gkScalar tick)
{
	bool isDown = m_joystick->isButtonDown(m_btnNr);

	getOUT_IS_DOWN()->setValue(isDown);
	getOUT_IS_NOT_DOWN()->setValue(!isDown);
	if (isDown && !m_pressed) {
		getOUT_IS_PRESSED()->setValue(true);
		getOUT_IS_RELEASED()->setValue(false);
		m_pressed = true;
	}
	else if (!isDown && m_pressed) {
		getOUT_IS_PRESSED()->setValue(false);
		getOUT_IS_RELEASED()->setValue(true);
		m_pressed = false;
	}
	else {
		getOUT_IS_PRESSED()->setValue(false);
		getOUT_IS_RELEASED()->setValue(false);
	}
}

