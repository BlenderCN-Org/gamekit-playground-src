/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSSlotImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSTex_NormalImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkPath.h"
#include "gkPBSManager.h"
gkPBSTex_NormalImpl::gkPBSTex_NormalImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSTex_NormalAbstract(parent,id,name), m_parentSlot(0), m_initialized(false)
{}

gkPBSTex_NormalImpl::~gkPBSTex_NormalImpl()
{}

void gkPBSTex_NormalImpl::_initialize()
{
	bool valid = checkValidity();

	if (valid)
		m_parentSlot = static_cast<gkPBSSlotImpl*>(getIN_SLOT()->getFrom()->getParent());

}

bool gkPBSTex_NormalImpl::_evaluate(gkScalar tick)
{
	return m_isValid && !m_initialized;;
}
void gkPBSTex_NormalImpl::_update(gkScalar tick)
{
	if (!m_parentSlot->isValid())
	{
		m_isValid = false;
		return;
	}

	Ogre::TexturePtr tex = gkPBSManager::loadTexture(getPropPATH(),Ogre::TEX_TYPE_2D);

	if (tex.isNull())
	{
		m_isValid = false;
		gkPath path(getPropPATH());
		gkLogger::write(getName()+" : couldn't locate resource "+path.base()+" in ogre-resources");
		return;
	}

	m_parentSlot->getMaterial()->setNormalrTexture(
			m_parentSlot->getMapSlot()
			,tex,Ogre::PbsMaterial::TextureAddressing()
			,getIN_NORMAL_BLEND(true)->getValue()
			,getIN_R_BLEND(true)->getValue());
	m_initialized=true;
}

