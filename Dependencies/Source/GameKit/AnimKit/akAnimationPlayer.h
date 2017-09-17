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

#ifndef AKANIMATIONPLAYER_H
#define AKANIMATIONPLAYER_H

#include "akAnimation.h"
#include "akMathUtils.h"

class akAnimationPlayer
{
public:
	struct akAnimationPlayerTrigger {
		akAnimationPlayerTrigger(akScalar time,int type)
			: m_timePos(time),m_type(type)
		{}
		akScalar m_timePos;
		int m_type; // some int-value that is given to the event-listener
	};

	class akAnimationPlayerEvent
	{
	public:
		akAnimationPlayerEvent(akAnimationPlayer* player):m_player(player){}
		virtual ~akAnimationPlayerEvent() {}
		virtual void onStart(akAnimationPlayer* m_player){}
		virtual void onEnd(akAnimationPlayer* m_player){}
		virtual void onLoopEnd(akAnimationPlayer* m_player){}
		virtual void onTrigger(akAnimationPlayer* m_player, int type){}
		virtual bool onDestruction(akAnimationPlayer* m_player){ return true;}
	protected:
		akAnimationPlayer* m_player;
	};
protected:
	akAnimation*         m_action;
	akScalar             m_evalTime, m_weight;
	bool                 m_enabled;
	int                  m_mode;
	akScalar             m_speedfactor;

	bool				 m_overrideStop; // override a stop-request (e.g. if some logic needs the same action that is going to be stopped)
	bool 				 m_requestStop;

	typedef utList<akAnimationPlayerTrigger> AnimationTriggers;
	typedef utList<akAnimationPlayerEvent*> AnimationEvents;
	AnimationEvents m_events;
	AnimationTriggers m_triggers;
	akAnimationPlayerTrigger* m_currentTrigger;
	int m_currentTriggerIdx;

public:
	akAnimationPlayer();
	akAnimationPlayer(akAnimation* resource);
	virtual ~akAnimationPlayer();

	UT_INLINE akScalar         getTimePosition(void) const { return m_evalTime; }
	UT_INLINE akScalar         getWeight(void) const       { return m_weight; }
	UT_INLINE int              getMode(void) const         { return m_mode; }
	UT_INLINE akScalar         getSpeedFactor(void) const  { return m_speedfactor; }
	UT_INLINE akScalar         getLength(void) const       { return m_action? m_action->getLength() : 0;}

	UT_INLINE void             setMode(int v)              { m_mode = v; }
	UT_INLINE akAnimation*     getAnimation()			   { return m_action; }
	UT_INLINE void             setAnimation(akAnimation* v){ m_action = v; }
	UT_INLINE void             setSpeedFactor(akScalar v)  { m_speedfactor = v; }

	void			           enable(bool v);
	UT_INLINE bool             isEnabled(void) const       { return m_enabled; }
	UT_INLINE bool             isDone(void) const          { return !m_enabled || m_evalTime >= getLength(); }

	void fireAnimationEnd();
	void fireLoopEnd();

	void addAnimationEvent(akAnimationPlayerEvent* evt) {
		m_events.push_back(evt);
	}
	void removeAnimationEvent(akAnimationPlayerEvent* evt) {
		m_events.erase(evt);
	}

	void clearEvents();

	void addTrigger(akScalar time,int type) {
		m_triggers.push_back(akAnimationPlayerTrigger(time,type));
		m_triggers.sort(akAnimationPlayer::sort);
		refreshCurrentTrigger();
	}

	static bool sort(const akAnimationPlayerTrigger& a, const akAnimationPlayerTrigger& b);

	void setTimePosition(akScalar v);
	void setWeight(akScalar w);

	void evaluate(akScalar tick);
	void reset(void);
	void requestStop() { m_requestStop = true;}
	bool hasStopRequest() { return m_requestStop;}
	void overrideStop() { m_overrideStop = true;}
	bool hasOverrideStop() { return m_overrideStop;}
	void clearRequests() { m_requestStop = false; m_overrideStop=false;}

private:
	virtual void evaluateImpl(akScalar time) = 0;
	void refreshCurrentTrigger();
};



#endif // AKANIMATIONPLAYER_H
