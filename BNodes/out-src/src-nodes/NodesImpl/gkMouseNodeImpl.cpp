/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMouseNodeImpl.h>
#include "Logic/gkLogicNode.h"

#include "gkInput.h"

#include "gkWindowSystem.h"

#include "Logic/gkLogicSocket.h"
gkMouseNodeImpl::gkMouseNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)  : gkMouseNodeAbstract(parent,id,name)
	,m_curScaleX(1.0f),m_curScaleY(1.0f), m_curAbsX(0.0f),m_curAbsY(0.0f),m_curRelX(0.0f),m_curRelY(0.0f),m_curIsMotion(false),m_curIsWheelMotion(false),m_curWheel(0.0f)
{
	setBucketNr((int)gkLogicNode::BUCKET_GETTER);
}

gkMouseNodeImpl::~gkMouseNodeImpl()
{}

void gkMouseNodeImpl::_initialize()
{}

#define CHECK_AND_SET(VAR,SOCKET) \
if (VAR!=m_cur##VAR) \
{ \
	m_cur##VAR=VAR; \
	SET_SOCKET_VALUE(SOCKET, VAR); \
}


bool gkMouseNodeImpl::_evaluate(gkScalar tick)
{
	return true;
}

void gkMouseNodeImpl::_update(gkScalar tick)
{
	gkMouse* dev = gkWindowSystem::getSingleton().getMouse();

	// breaking naming convention to fit the macro. sry

	if (getIN_SCALE_X()->isDirty())
	{
		m_curScaleX = getIN_SCALE_X(true)->getValue();
	}
	if (getIN_SCALE_Y()->isDirty())
	{
		m_curScaleY = getIN_SCALE_Y(true)->getValue();
	}

	gkScalar RelX = 0;
	gkScalar RelY = 0;

	bool IsMotion = dev->moved;
	if (dev->moved)
	{
		RelX = dev->relative.x * m_curScaleX;
		RelY = dev->relative.y * m_curScaleY;

		gkScalar AbsX = dev->position.x;
		gkScalar AbsY = dev->position.y;
		CHECK_AND_SET(AbsX,OUT_ABS_X);
		CHECK_AND_SET(AbsY,OUT_ABS_Y);
	}

	CHECK_AND_SET(RelX,OUT_REL_X);
	CHECK_AND_SET(RelY,OUT_REL_Y);


	CHECK_AND_SET(IsMotion,OUT_MOTION);

	bool IsWheelMotion = dev->wheelDelta?true:false;
	CHECK_AND_SET(IsWheelMotion,OUT_WHEEL_MOTION);

	gkScalar Wheel = dev->wheelDelta;
	CHECK_AND_SET(Wheel,OUT_WHEEL);

}

