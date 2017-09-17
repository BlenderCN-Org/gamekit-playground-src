/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSMaterialImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"
#include "OgreHlmsPbsMaterial.h"

#include "gkEntity.h"
#include "gkScene.h"
#include "OgreEntity.h"
#include "gkGameObject.h"

#include "gkPBSManager.h"
gkPBSMaterialImpl::gkPBSMaterialImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSMaterialAbstract(parent,id,name) , m_pbsMaterial(0), m_materialAttached(false), m_initialized(false)
{}

gkPBSMaterialImpl::~gkPBSMaterialImpl()
{
	if (m_pbsMaterial)
	{
		delete m_pbsMaterial;
	}
}

void gkPBSMaterialImpl::_initialize()
{
	if (isValid())
	{
		m_pbsMaterial = new Ogre::PbsMaterial();
	}
}

bool gkPBSMaterialImpl::_evaluate(gkScalar tick)
{
//	return m_isValid && !m_initialized && getIN_ENABLED()->getValue();
	// keep checking
	return m_isValid && getIN_ENABLED()->getValue();
}



void gkPBSMaterialImpl::_update(gkScalar tick)
{
	if (m_pbsMaterial)
	{

		if (getIN_ALBEDO_COL()->isDirty())
		{
			const gkColor& col = getIN_ALBEDO_COL(true)->getValue();
			m_pbsMaterial->setAlbedo(col);
		}

		if (getIN_F0_COL()->isDirty())
		{
			const gkColor& col = getIN_F0_COL(true)->getValue();
			m_pbsMaterial->setF0(col);
		}

		if (getIN_ROUGH()->isDirty())
		{
			m_pbsMaterial->setRoughness(getIN_ROUGH(true)->getValue());
		}

		if (getIN_LR_OFFSET())
		{
			m_pbsMaterial->setLightRoughnessOffset(getIN_LR_OFFSET(true)->getValue());
		}

	}
}

void gkPBSMaterialImpl::_postUpdate()
{
	if (m_initialized) // we are ready here
		return;

	if (!m_materialAttached && getIN_ENABLED()->getValue())
	{
		if (m_pbsMaterial)
		{
			gkGameObject* gobj = getAttachedObject();

			Ogre::Entity* ent = gobj->getEntity()->getEntity();

//			Ogre::PbsMaterial* pbsMaterial = new Ogre::PbsMaterial();
//			pbsMaterial->setAlbedo(Ogre::ColourValue::Red);
//
//			pbsMaterial->setRoughness(0.5);
//			pbsMaterial->setLightRoughnessOffset(0.5);
//			pbsMaterial->setF0(gkColor(0.5,0.5,0.5));

//			m_pbsMaterial->setAlbedo(Ogre::ColourValue::Red);

//			m_pbsMaterial->setRoughness(0.5);
//			m_pbsMaterial->setLightRoughnessOffset(0.5);
//			m_pbsMaterial->setF0(gkColor(0.5,0.5,0.5));


			gobj->getOwner()->getPBSManager()->createHLMSMaterial(ent,m_pbsMaterial,"pbsmat_"+gobj->getName());


//			gobj->getOwner()->getPBSManager()->createHLMSMaterial(ent,m_pbsMaterial,"pbsmat_"+gobj->getName());
			m_initialized = true;
		}
	}
}
