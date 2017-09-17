/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PBSMATERIAL_H_
#define PBSMATERIAL_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkPBSMaterialImpl

namespace Ogre
{
	class PbsMaterial;
}

class gkPBSMaterialImpl
		: public gkPBSMaterialAbstract
{
public:
	gkPBSMaterialImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPBSMaterialImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
	virtual void _postUpdate();
	GK_INLINE Ogre::PbsMaterial* getMaterial() { return m_pbsMaterial; };
	GK_INLINE bool isValid() { return m_isValid; }

private:
	bool m_initialized;
	Ogre::PbsMaterial* m_pbsMaterial;
	bool m_materialAttached;
};

#endif
