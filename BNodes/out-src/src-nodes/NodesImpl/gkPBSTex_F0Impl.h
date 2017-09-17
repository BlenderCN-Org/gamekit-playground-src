/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PBSTEX_F0_H_
#define PBSTEX_F0_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkPBSTex_F0Impl

class gkPBSSlotImpl;

class gkPBSTex_F0Impl
		: public gkPBSTex_F0Abstract
{
public:
	gkPBSTex_F0Impl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPBSTex_F0Impl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
private:
	bool m_initialized;
	gkPBSSlotImpl* m_parentSlot;
	Ogre::TexturePtr m_texPtr;
};

#endif
