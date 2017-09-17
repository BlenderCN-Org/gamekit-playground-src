/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkAnimationDefinitionImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "Animation/gkAnimationManager.h"

#include "gkResourceName.h"

#include "gkEngine.h"
#include "gkUserDefs.h"

#include "akCommon.h"
gkAnimationDefinitionImpl::gkAnimationDefinitionImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkAnimationDefinitionAbstract(parent,id,name)
	, m_target(0), m_player(0), m_anim(0), m_currentState(ST_stopped), m_animFps(24), m_startTime(0),m_endTime(0),m_mode(MODE_end)
	, m_listener(0), m_triggerDone(false), m_triggerLoopDone(false)
{}

gkAnimationDefinitionImpl::~gkAnimationDefinitionImpl()
{
	// the listener gets deleted by the animationPlayer
	m_listener = 0;
//	if (m_listener)
//	{
//		delete m_listener;
//	}
}

void gkAnimationDefinitionImpl::_initialize()
{
	m_target = getIN_TARGET()->isConnected()
				? getIN_TARGET(true)->getGameObject()
				: getAttachedObject();

	gkString animName(getPropANIM_NAME());
	m_anim = gkAnimationManager::getSingleton().getAnimation(gkResourceName(getPropANIM_NAME(), m_target->getGroupName()));

	if(!m_anim)
	{
		gkLogger::write(getName()+": unknown animation:"+getPropANIM_NAME());
		m_isValid = false;
		return;
	}

	if (getPropNAME()=="")
	{
		gkLogger::write(getName()+": Noname specified for animation!",true);
		m_isValid = false;
		return;
	}

	m_animName = m_target->getName()+"_"+getPropNAME();
	// make the raw animation name available via property, so you can manipulate it directly from code
	setPropRAW_ANIM_NAME(m_animName);

	m_player = m_target->addAnimation(m_anim,m_animName);

	m_player->setSpeedFactor(getPropSPEED());

	m_animFps = gkEngine::getSingleton().getUserDefs().animFps;
	if (getPropFRAME_CUSTOM())
	{
		m_startTime = getPropFRAME_START() / m_animFps;
		m_endTime = getPropFRAME_END() / m_animFps;
	}
	else
	{
		m_startTime = 0;
		m_endTime = m_anim->getInternal()->getLength();
	}

	m_mode = (MODE)(getPropMODE());

	m_listener = new AnimationListener(this);
	m_player->addAnimationEvent(m_listener);
}



void gkAnimationDefinitionImpl::start()
{
	if (getPropRESET_ONSTART())
	{
		reset();
	}

	gkLogger::write("START ANIMATION:"+m_animName);

	if (m_currentState == ST_paused)
	{
		resume();
		return;
	}

	if (m_currentState != ST_playing)
	{
		m_currentState = ST_playing;
		int pmode = getPropMODE();
		akAnimationEvalMode mode = (akAnimationEvalMode) (1 << getPropMODE());
		int outmode = (int)mode;
		m_player->setMode(mode);
		m_target->playAnimation(m_player,getPropBLEND(),(int)mode,1);
	}
}
void gkAnimationDefinitionImpl::stop()
{
	if (m_currentState == ST_playing)
	{
		m_currentState = ST_stopped;
		m_triggerDone = true;
		m_target->stopAnimation(m_player);

		if (getPropRESET_ONSTOP())
		{
			reset();
		}
	}
}
void gkAnimationDefinitionImpl::pause()
{
	if (m_currentState == ST_playing)
	{
		m_currentState = ST_paused;
		m_player->enable(false);
	}
}
void gkAnimationDefinitionImpl::resume()
{
	if (m_currentState == ST_paused)
	{
		m_currentState = ST_playing;
		m_player->enable(true);
	}
}
void gkAnimationDefinitionImpl::reset()
{
	if (getPropFRAME_CUSTOM())
	{
		m_player->setTimePosition( m_startTime );
		m_player->evaluate(0);
	}
	else
	{
		m_player->reset();
		m_player->evaluate(0);
	}
}



bool gkAnimationDefinitionImpl::_evaluate(gkScalar tick)
{
	return m_currentState == ST_playing || m_triggerDone || m_triggerLoopDone;
}


void gkAnimationDefinitionImpl::_preUpdate()
{
	if ( m_player->getTimePosition() >= m_endTime )
	{
		if ( m_mode == MODE_end )
		{
			stop();
		}
		else if ( m_mode == MODE_loop)
		{
			m_player->reset();
		}
	}
}

void gkAnimationDefinitionImpl::_update(gkScalar tick)
{

}


void gkAnimationDefinitionImpl::_postUpdate()
{
	m_triggerDone = false;
	m_triggerLoopDone = false;
}
