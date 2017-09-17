/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PBSTEX_ENV_H_
#define PBSTEX_ENV_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkPBSTex_EnvImpl
class gkPBSMaterialImpl;

class gkPBSTex_EnvImpl
		: public gkPBSTex_EnvAbstract
{
public:
	gkPBSTex_EnvImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPBSTex_EnvImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

private:
	bool m_initialized;
	gkPBSMaterialImpl* m_parentMaterial;
	Ogre::TexturePtr m_texPtr;
};

#endif
