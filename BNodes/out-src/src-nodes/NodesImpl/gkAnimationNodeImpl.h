/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef ANIMATIONNODE_H_
#define ANIMATIONNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "akCommon.h"
// gkAnimationNodeImpl

// using the lua-version for the time being

class gkAnimationNodeImpl
		: public gkAnimationNodeAbstract
{
public:
	class AnimationListener : gkAnimationPlayer::akAnimationPlayerEvent
	{
	public:
		AnimationListener(akAnimationPlayer* player):gkAnimationPlayer::akAnimationPlayerEvent(player){}
		virtual ~AnimationListener() {}
		virtual void onStart(akAnimationPlayer* m_player){}
		virtual void onEnd(akAnimationPlayer* m_player){}
		virtual void onLoopEnd(akAnimationPlayer* m_player){}
		virtual void onTrigger(akAnimationPlayer* m_player, int type)
		{
			if (type == gkAnimationNodeImpl::tt_endframe)
			{

			}
		}
		virtual bool onDestruction(akAnimationPlayer* m_player){ return true;}
	};

	enum State {
		stopped,playing,paused
	};

	enum TriggerType {
		tt_startframe=0,tt_endframe
	};

	gkAnimationNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkAnimationNodeImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

private:
	void play();
	void stop();
	void pause();

	gkGameObject* m_target;
	gkAnimationPlayer* m_animPlayer;
	AnimationListener* m_listener;
	State m_state;
	akAnimationEvalMode m_playMode;

	gkScalar m_startTime;
	gkScalar m_endTime;

	gkScalar m_lastTime;
};

#endif
