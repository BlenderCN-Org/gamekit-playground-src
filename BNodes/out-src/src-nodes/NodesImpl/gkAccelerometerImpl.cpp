/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkAccelerometerImpl.h>
#include <Events/gkEvents.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "OgreStringVector.h"

#include "gkValue.h"

#include "gkLogger.h"
gkAccelerometerImpl::gkAccelerometerImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkAccelerometerAbstract(parent,id,name), m_isActive(false)
{}

gkAccelerometerImpl::~gkAccelerometerImpl()
{}

void gkAccelerometerImpl::_initialize()
{
	m_eventType = gkMessageEvent::sk_EventTypePrefix.str()+"_mobile-accel";
	safeAddListener(this,m_eventType);
}

bool gkAccelerometerImpl::_evaluate(gkScalar tick)
{
	bool active = getIN_UPDATE()->getValue();
	if (active && !m_isActive) {
		// tell the mobile env to activate accelerometer
		gkMessageManager::getSingleton().sendMessage("","","start_sensor","accel");
		m_isActive = true;
	}
	else if (!active && m_isActive) {
		// turn it of again
		gkMessageManager::getSingleton().sendMessage("","","stop_sensor","accel");
		m_isActive = false;
		getOUT_X()->setValue(0);
		getOUT_Y()->setValue(0);
		getOUT_Z()->setValue(0);
		m_currentState = gkVector3::ZERO;
	}
	return false;
}
void gkAccelerometerImpl::_update(gkScalar tick)
{

}

bool gkAccelerometerImpl::handleEvent( IEventData const & event)
{
	const gkMessageEvent& msgEvent = static_cast<const gkMessageEvent&>(event);

	gkVariable* valueData = msgEvent.getData("value");
	m_currentState = valueData->getValueVector3();
	getOUT_X()->setValue(m_currentState.x);
	getOUT_Y()->setValue(m_currentState.y);
	getOUT_Z()->setValue(m_currentState.z);
	return true;
}
