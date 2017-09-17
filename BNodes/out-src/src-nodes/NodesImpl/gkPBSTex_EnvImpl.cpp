/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSMaterialImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSTex_EnvImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"
#include "gkPath.h"

#include "OgreResourceGroupManager.h"

#include "OgreTexture.h"

#include "OgreTextureManager.h"
#include "OgreHlmsPbsMaterial.h"

#include "gkPBSManager.h"
gkPBSTex_EnvImpl::gkPBSTex_EnvImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSTex_EnvAbstract(parent,id,name), m_parentMaterial(0), m_initialized(false)
{}

gkPBSTex_EnvImpl::~gkPBSTex_EnvImpl()
{}

void gkPBSTex_EnvImpl::_initialize()
{
	if (isValid())
		m_parentMaterial = static_cast<gkPBSMaterialImpl*>(getIN_SLOT()->getFrom()->getParent());
}

bool gkPBSTex_EnvImpl::_evaluate(gkScalar tick)
{
	return !m_initialized && m_isValid;
}
void gkPBSTex_EnvImpl::_update(gkScalar tick)
{
	if (!m_parentMaterial->isValid())
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

	m_parentMaterial->getMaterial()->setEnvironmentMap(m_texPtr,getIN_INTENSITY_FAC()->getValue());
	m_initialized=true;
}

