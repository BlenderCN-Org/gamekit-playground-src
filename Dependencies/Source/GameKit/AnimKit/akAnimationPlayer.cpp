/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Xavier T.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#include "akAnimationPlayer.h"
#include "akAnimationSequence.h"


akAnimationPlayer::akAnimationPlayer()
	:	m_action(0),
	    m_evalTime(0.f),
	     m_weight(1.0),
	     m_enabled(true),
	     m_mode(AK_ACT_LOOP),
	     m_speedfactor(1.0f),
	     m_currentTrigger(0),
	     m_currentTriggerIdx(0),
		 m_requestStop(false),
		 m_overrideStop(false)
{
}

akAnimationPlayer::akAnimationPlayer(akAnimation* resource)
	:	m_action(resource),
	    m_evalTime(0.f),
	     m_weight(1.0),
	     m_enabled(true),
	     m_mode(AK_ACT_LOOP),
	     m_speedfactor(1.0f),
	     m_currentTrigger(0),
	     m_currentTriggerIdx(0),
		 m_requestStop(false),
		 m_overrideStop(false)
{
}

akAnimationPlayer::~akAnimationPlayer()
{
	clearEvents();
}

void akAnimationPlayer::clearEvents()
{
	AnimationEvents::Iterator iter(m_events);
	while(iter.hasMoreElements()) {
		akAnimationPlayerEvent* evt = iter.getNext();

		// ask the event if it is ok that it will be killed
		bool deleteEvent = evt->onDestruction(this);

		if (deleteEvent)
			delete evt;
	}
}

void akAnimationPlayer::setTimePosition(akScalar v)
{
	if (m_enabled && m_action)
	{
		m_evalTime = akClampf(v, 0.f, m_action->getLength());
	}
}



void akAnimationPlayer::setWeight(akScalar w)
{
	if (m_enabled)
	{
		m_weight = akClampf(w, 0, 1);
	}
}



void akAnimationPlayer::evaluate(akScalar tick)
{
	if (!m_enabled || !m_action)
		return;
	
	akScalar end = m_action->getLength();
	akScalar dt = m_speedfactor * tick;
	
	if (m_mode & AK_ACT_LOOP)
	{
		if (m_evalTime < 0.f || m_evalTime >= end)
		{
			m_evalTime = 0.f;
			refreshCurrentTrigger();
			// tell the listeners
			AnimationEvents::Iterator iter(m_events);
			fireLoopEnd();
		}
		else
		{
			m_evalTime += dt;
		}

	}
	else
	{
		bool foundEnd = false;
		if (m_evalTime < 0.f)
		{
			m_evalTime = 0.f;
			foundEnd = true;
		}

		if (m_evalTime  > end)
		{
			m_evalTime = end - dt;
			foundEnd = true;
		}


		// tell the listeners
		if (!foundEnd)
		{
			m_evalTime += dt;
		}

	}

	akScalar time = m_evalTime;

	if (m_mode & AK_ACT_INVERSE)
	{
		time = end - m_evalTime;
	}
	else
	{	// check if the current trigger fires! and check if possible following triggers are
		// also firing
		while (m_currentTrigger && m_evalTime > m_currentTrigger->m_timePos)
		{
			AnimationEvents::Iterator iter(m_events);
			while (iter.hasMoreElements()){
				iter.getNext()->onTrigger(this,m_currentTrigger->m_type);
			}

			if ( (m_currentTriggerIdx+1) < m_triggers.size())
			{
				m_currentTrigger = &m_triggers.at(++m_currentTriggerIdx);
			}
			else
			{
				m_currentTrigger = 0;
				m_currentTriggerIdx = 0;
			}

		}
	}

	evaluateImpl(time);
	
	
}

void akAnimationPlayer::fireAnimationEnd()
{
	AnimationEvents::Iterator iter(m_events);
	while (iter.hasMoreElements()){
		iter.getNext()->onEnd(this);
	}
}


void akAnimationPlayer::fireLoopEnd()
{
	AnimationEvents::Iterator iter(m_events);
	while (iter.hasMoreElements()){
		iter.getNext()->onLoopEnd(this);
	}
}

void akAnimationPlayer::reset(void)
{
	m_evalTime = 0.f;
	refreshCurrentTrigger();
	evaluate(0);
//	m_weight = 1.f;
//	m_enabled = false;
}

bool akAnimationPlayer::sort(const akAnimationPlayerTrigger& a, const akAnimationPlayerTrigger& b)
{
	return a.m_timePos > b.m_timePos;
}

void akAnimationPlayer::enable(bool v)
{
	// don't do anything if the state doesn't change
	if (m_enabled == v)
		return;

	// tell the listeners
	AnimationEvents::Iterator iter(m_events);

	while (iter.hasMoreElements()){
		if (v)
			iter.getNext()->onStart(this);
		else
			iter.getNext()->onEnd(this);
	}

	m_enabled = v;
	refreshCurrentTrigger();
}

void akAnimationPlayer::refreshCurrentTrigger()
{
	if (m_triggers.size()==0){
		m_currentTrigger=0;
		return;
	}

	float currentTimePos = getTimePosition();

	AnimationTriggers::Iterator iter(m_triggers);

	if (m_mode & AK_ACT_INVERSE) // is the animation player reverse?
	{
		for (int i=m_triggers.size();i>=0;i--)
		{
			akAnimationPlayerTrigger& trigger = m_triggers.at(i);
			if (trigger.m_timePos < currentTimePos) {
				m_currentTrigger = &trigger;
				m_currentTriggerIdx = i;
				break;
			}
		}
	}
	else // is animation playing the normal direction?
	{
		for (int i=0;i<m_triggers.size();i++)
		{
			akAnimationPlayerTrigger& trigger = m_triggers.at(i);
			if (trigger.m_timePos > currentTimePos) {
				m_currentTrigger = &trigger;
				m_currentTriggerIdx = i;
				break;
			}
		}
	}


}
