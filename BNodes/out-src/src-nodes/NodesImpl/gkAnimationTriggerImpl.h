/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef ANIMATIONTRIGGER_H_
#define ANIMATIONTRIGGER_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkAnimationTriggerImpl

class gkAnimationDefinitionImpl;

class gkAnimationTriggerImpl
		: public gkAnimationTriggerAbstract, public akAnimationPlayer::akAnimationPlayerEvent
{
public:
	gkAnimationTriggerImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkAnimationTriggerImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _afterInit2(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _postUpdate(); // update your stuff

	virtual void onTrigger(akAnimationPlayer* m_player, int type);

private:
	gkAnimationDefinitionImpl* m_anim;
	int m_triggerType;
	static int TRIGGER_COUNT;
};

#endif
