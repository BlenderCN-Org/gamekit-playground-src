/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef ANIMATIONPLAYER_H_
#define ANIMATIONPLAYER_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkAnimationPlayerImpl

class gkAnimationDefinitionImpl;

class gkAnimationPlayerImpl
		: public gkAnimationPlayerAbstract
{
public:

	gkAnimationPlayerImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkAnimationPlayerImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _preUpdate();
	virtual void _update(gkScalar tick); // update your stuff
	virtual void _postUpdate();
private:
	gkAnimationDefinitionImpl* m_anim;

};

#endif
