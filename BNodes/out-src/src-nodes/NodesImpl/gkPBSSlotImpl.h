/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PBSSLOT_H_
#define PBSSLOT_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "OgreHlmsPbsMaterial.h"
// gkPBSSlotImpl

class gkPBSMaterialImpl;
namespace Ogre{	class PbsMaterial;}

class gkPBSSlotImpl
		: public gkPBSSlotAbstract
{
public:
	gkPBSSlotImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPBSSlotImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
	Ogre::PbsMaterial* getMaterial();
	GK_INLINE Ogre::PbsMaterial::MapSlot getMapSlot() { return m_mapSlot; }
private:
	gkPBSMaterialImpl* m_parentPBSMaterial;

	Ogre::PbsMaterial::MapSlot m_mapSlot;
};

#endif
