/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkAnimationNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Animation/gkAnimationManager.h"

#include "Animation/gkAnimation.h"

#include "gkGameObject.h"
#include "gkSkeleton.h"

#include "gkEngine.h"
#include "gkUserDefs.h"
#include "gkMathUtils.h"
gkAnimationNodeImpl::gkAnimationNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkAnimationNodeAbstract(parent,id,name), m_animPlayer(0), m_target(0), m_state(stopped), m_playMode(AK_ACT_LOOP), m_listener(0)
	  ,m_startTime(0),m_endTime(0),m_lastTime(-1)
{}

gkAnimationNodeImpl::~gkAnimationNodeImpl()
{
	if (m_listener)
	{
		delete m_listener;
	}
}

void gkAnimationNodeImpl::_initialize()
{
	if (!m_isValid)
		return;

	if (getPropANIM_NAME()=="")
	{
		m_isValid = false;
		return;
	}

	m_target = getIN_TARGET()->isConnected()
					?getIN_TARGET()->getGameObject()
					:getAttachedObject();

	m_animPlayer = m_target->getAnimationPlayer(getPropANIM_NAME());

	if (!m_animPlayer)
	{
		if (m_target->getSkeleton())
		{
			m_target = static_cast<gkGameObject*>(m_target->getSkeleton());
			m_animPlayer = m_target->getSkeleton()->getAnimationPlayer(getPropANIM_NAME());
		}
	}

	if (!m_animPlayer)
	{
		gkAnimation* res = gkAnimationManager::getSingleton().getAnimation(gkResourceName(getPropANIM_NAME(), m_target->getGroupName()));
		if(res)
		{
			m_target->addAnimation(res, getPropANIM_NAME());
			m_animPlayer = m_target->getAnimationPlayer(getPropANIM_NAME());
		}

		if (!m_animPlayer)
		{
			gkLogger::write(getName()+": unknown animation:"+getPropANIM_NAME());
			m_isValid = false;
			return;
		}
	}

	if (getPropFRAME_CUSTOM())
	{
		gkScalar fps = gkEngine::getSingleton().getUserDefs().animFps;
		m_animPlayer->addTrigger(getPropFRAME_START() / fps,(int)tt_startframe);
		m_animPlayer->addTrigger(getPropFRAME_END() / fps,(int)tt_endframe);

		m_startTime = getPropFRAME_START() / fps;
		m_endTime = getPropFRAME_END() / fps;
	}
	else
	{
		m_startTime = 0;
		m_endTime = m_animPlayer->getLength();
	}

	m_playMode = (akAnimationEvalMode)(1 >> getPropMODE());

	getOUT_DONE_TRIGGER()->setValue(false);
	getOUT_RUNNING()->setValue(false);
	getOUT_PAUSED()->setValue(false);
	getOUT_STOPPED()->setValue(true);
}

bool gkAnimationNodeImpl::_evaluate(gkScalar tick)
{
	if (!m_isValid)
		return false;

	if (getIN_START(true)->isDirty() && getIN_START()->getValue())
	{
		play();
	}

	if (getIN_STOP(true)->isDirty() && getIN_STOP()->getValue())
	{
		gkLogger::write("STOP");
		stop();
	}

	if (m_state==playing && getIN_PAUSE(true)->isDirty() && getIN_PAUSE()->getValue())
	{
		m_animPlayer->enable(false);
		pause();
	}

	if (getIN_RESUME(true)->isDirty() && getIN_RESUME()->getValue())
	{
		// play automatically handles the pause-state
		play();
	}

	if (getIN_RESET(true)->isDirty())
	{
		m_animPlayer->reset();
	}

	return getIN_ENABLED()->getValue();
}

void gkAnimationNodeImpl::_update(gkScalar tick)
{

	if (m_animPlayer->getTimePosition() >= m_endTime)
	{
		if (m_playMode == AK_ACT_LOOP)
		{
			m_animPlayer->setTimePosition(m_startTime);
		}
		else if (m_playMode == AK_ACT_END)
		{
			stop();
		}
		else if (m_playMode == AK_ACT_INVERSE)
		{
			// this is not really supported atm
			// need to work with direction...
		}

	}
}

void gkAnimationNodeImpl::play()
{
	if (m_state==playing)
		return;

	m_target->playAnimation(m_animPlayer,(float)getPropBLEND(),m_playMode,1);
	getOUT_RUNNING()->setValue(true);
	getOUT_STOPPED()->setValue(false);
	getOUT_PAUSED()->setValue(false);

	if (m_state!=paused && getPropRESET_ONSTART())
	{
		m_animPlayer->setTimePosition(m_startTime);
	}
	m_state = playing;
}



void gkAnimationNodeImpl::stop()
{
	if (m_state == stopped)
		return;

	// only request a stop, so that I can
	m_animPlayer->requestStop();
//	m_target->stopAnimation(m_animPlayer);
	m_state = stopped;
	getOUT_STOPPED()->setValue(true);
	getOUT_RUNNING()->setValue(false);
	getOUT_PAUSED()->setValue(false);
	if (getPropRESET_ONSTOP())
	{
		m_animPlayer->setTimePosition(m_startTime);
		m_animPlayer->evaluate(0);
	}
}

void gkAnimationNodeImpl::pause()
{
	if (m_state != playing)
		return;

	m_state = paused;
	getOUT_RUNNING()->setValue(false);
	getOUT_PAUSED()->setValue(true);
	getOUT_STOPPED()->setValue(false); // is paused != stopped??????
}
