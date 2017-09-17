/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkKeyNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkInput.h"

#include "gkWindowSystem.h"
gkKeyNodeImpl::gkKeyNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
: gkKeyNodeAbstract(parent,id,name),m_counter(0),m_pressed(false), m_curIsDown(false)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkKeyNodeImpl::~gkKeyNodeImpl()
{}

void gkKeyNodeImpl::_initialize()
{
	_update(0);
}

bool gkKeyNodeImpl::_evaluate(gkScalar tick)
{
	bool ok = GET_SOCKET_VALUE(IN_UPDATE);

	if (ok && ++m_counter > getPropDelay())
	{
		m_counter = 0;
	}

	return ok && !m_counter;
}
void gkKeyNodeImpl::_update(gkScalar tick)
{
	bool isPressed = isButtonDown();

	SET_SOCKET_VALUE(OUT_IS_DOWN, isPressed);
	SET_SOCKET_VALUE(OUT_NOT_DOWN, !isPressed);

	m_curIsDown = isPressed;

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

bool gkKeyNodeImpl::isButtonDown()
{
	return gkWindowSystem::getSingleton().getKeyboard()->isKeyDown((gkScanCode)getPropKey());
}
