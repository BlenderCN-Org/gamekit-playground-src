/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSSlotImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSTex_AlbedoImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkPath.h"

#include "gkPBSManager.h"
gkPBSTex_AlbedoImpl::gkPBSTex_AlbedoImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSTex_AlbedoAbstract(parent,id,name), m_parentSlot(0), m_initialized(false)
{}

gkPBSTex_AlbedoImpl::~gkPBSTex_AlbedoImpl()
{}

void gkPBSTex_AlbedoImpl::_initialize()
{
	checkValidity();
	if (isValid())
	{
		m_parentSlot = static_cast<gkPBSSlotImpl*>(getIN_SLOT()->getFrom()->getParent());
	}

}



bool gkPBSTex_AlbedoImpl::_evaluate(gkScalar tick)
{
	return m_isValid && !m_initialized;
}
void gkPBSTex_AlbedoImpl::_update(gkScalar tick)
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
		gkLogger::write(getName()+" : couldn't locate Albedo-Texture: "+path.base()+" in ogre-resources");
		return;
	}

	m_parentSlot->getMaterial()->setAlbedoTexture(
			m_parentSlot->getMapSlot()
			,m_texPtr,Ogre::PbsMaterial::TextureAddressing()
			,(Ogre::PbsMaterial::BlendFunction)getPropBLEND_FUNC()
			,getIN_BLEND_FAC(true)->getValue());
	m_initialized=true;
}

