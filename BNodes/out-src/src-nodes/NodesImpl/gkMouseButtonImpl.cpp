/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMouseButtonImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkWindowSystem.h"

#include "gkInput.h"
gkMouseButtonImpl::gkMouseButtonImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkMouseButtonAbstract(parent,id,name),m_counter(0),m_pressed(false), m_listenerBound(false)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkMouseButtonImpl::~gkMouseButtonImpl()
{}

void gkMouseButtonImpl::_initialize()
{

}

bool gkMouseButtonImpl::_evaluate(gkScalar tick)
{
	if (!getIN_UPDATE()->getValue())
	{
		if (getIN_UPDATE(true)->isDirty())
		{
			getOUT_IS_DOWN()->setValue(false);
			getOUT_NOT_DOWN()->setValue(true);
			getOUT_PRESSED()->setValue(false);
			getOUT_RELEASED()->setValue(false);
		}
		return false;
	}

	bool ok = GET_SOCKET_VALUE(IN_UPDATE);

	float delay = getPropDelay();
	if (ok && ++m_counter > getPropDelay())
	{
		m_counter = 0;
	}

	return ok && !m_counter;
}
void gkMouseButtonImpl::_update(gkScalar tick)
{
	bool isPressed = isButtonDown();

	SET_SOCKET_VALUE(OUT_IS_DOWN, isPressed);
	SET_SOCKET_VALUE(OUT_NOT_DOWN, !isPressed);

	if (isPressed && !m_pressed)
	{
		m_pressed = true;
		SET_SOCKET_VALUE(OUT_PRESSED, true);
		SET_SOCKET_VALUE(OUT_RELEASED, false);
	}
	else if (!isPressed && m_pressed)
	{
		m_pressed = false;
		SET_SOCKET_VALUE(OUT_PRESSED, false);
		SET_SOCKET_VALUE(OUT_RELEASED, true);
	}
	else
	{
		SET_SOCKET_VALUE(OUT_PRESSED, false);
		SET_SOCKET_VALUE(OUT_RELEASED, false);
	}
}

bool gkMouseButtonImpl::isButtonDown()
{
	if (gkWindowSystem::getSingleton().getMouse()->isButtonDown(getPropButton())) {
		gkLogger::write("node pressed!",true);
	}
	//	return gkWindowSystem::getSingleton().getMouse()->isButtonDown(getPropButton());
	return gkWindowSystem::getSingleton().getMouse()->isButtonDown(getPropButton());
}

