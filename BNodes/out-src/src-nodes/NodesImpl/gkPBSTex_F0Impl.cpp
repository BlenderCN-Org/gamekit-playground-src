/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSSlotImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSTex_F0Impl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkPath.h"

#include "gkPBSManager.h"
gkPBSTex_F0Impl::gkPBSTex_F0Impl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSTex_F0Abstract(parent,id,name), m_initialized(false), m_parentSlot(0)
{}

gkPBSTex_F0Impl::~gkPBSTex_F0Impl()
{}

void gkPBSTex_F0Impl::_initialize()
{
	if (isValid())
	{
		m_parentSlot = static_cast<gkPBSSlotImpl*>(getIN_SLOT()->getFrom()->getParent() );
	}
}

bool gkPBSTex_F0Impl::_evaluate(gkScalar tick)
{
	return !m_initialized && isValid();
}
void gkPBSTex_F0Impl::_update(gkScalar tick)
{
	if (!m_parentSlot->isValid())
	{
		m_isValid = false;
		return;
	}

	m_texPtr = gkPBSManager::loadTexture(getPropPATH(),Ogre::TEX_TYPE_2D);

	if (m_texPtr.isNull())
	{
		m_isValid = false;
		gkPath path(getPropPATH());
		gkLogger::write(getName()+" : couldn't locate F0-Texture: "+path.base()+" in ogre-resources");
		return;
	}

	m_parentSlot->getMaterial()->setF0Texture(
			m_parentSlot->getMapSlot()
			,m_texPtr,Ogre::PbsMaterial::TextureAddressing()
			,(Ogre::PbsMaterial::BlendFunction)getPropBLEND_FUNC()
			,getIN_BLEND_FAC(true)->getValue());

	m_initialized=true;
}

