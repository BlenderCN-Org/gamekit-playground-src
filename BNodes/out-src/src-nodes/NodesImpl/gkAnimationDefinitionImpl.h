/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef ANIMATIONDEFINITION_H_
#define ANIMATIONDEFINITION_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkAnimationDefinitionImpl

class gkAnimationDefinitionImpl
		: public gkAnimationDefinitionAbstract
{
public:

	friend class gkAnimationPlayerImpl;

	class AnimationListener : public akAnimationPlayer::akAnimationPlayerEvent
	{
		friend class gkAnimationDefinitionImpl;

		AnimationListener(gkAnimationDefinitionImpl* anim) : akAnimationPlayer::akAnimationPlayerEvent(anim->getPlayer()),m_anim(anim){}
		virtual void onStart(akAnimationPlayer* m_player){}
		virtual void onEnd(akAnimationPlayer* m_player)
		{
			if (m_anim->m_currentState == gkAnimationDefinitionImpl::ST_playing)
			{
				m_anim->m_triggerDone=true;
				m_anim->stop();
			}
		}
		virtual void onLoopEnd(akAnimationPlayer* m_player)
		{
			m_anim->m_triggerLoopDone = true;
		}
		virtual void onTrigger(akAnimationPlayer* m_player, int type){}
		virtual bool onDestruction(akAnimationPlayer* m_player){ return true;}
	private:
		gkAnimationDefinitionImpl* m_anim;
	};

	enum State {
		ST_playing, ST_stopped, ST_paused
	};

	gkAnimationDefinitionImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkAnimationDefinitionImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _preUpdate(); // update your stuff
	virtual void _update(gkScalar tick); // update your stuff
	virtual void _postUpdate(); // update your stuff

	GK_INLINE gkAnimationPlayer* getPlayer() { return m_player; }

	void start();
	void stop();
	void pause();
	void resume();
	void reset();
private:
	gkScalar m_startTime;
	gkScalar m_endTime;
	gkScalar m_animFps;
	gkGameObject* m_target;
	gkAnimationPlayer* m_player;
	gkAnimation* m_anim;
	gkString m_animName;

	bool m_triggerDone;
	bool m_triggerLoopDone;

	MODE m_mode;
	State m_currentState;

	AnimationListener* m_listener;
};

#endif
