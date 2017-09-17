/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSMaterialImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkPBSSlotImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"


gkPBSSlotImpl::gkPBSSlotImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPBSSlotAbstract(parent,id,name), m_parentPBSMaterial(0), m_mapSlot(Ogre::PbsMaterial::MS_MAIN)
{}

gkPBSSlotImpl::~gkPBSSlotImpl()
{}

void gkPBSSlotImpl::_afterInit()
{
	bool valid = checkValidity();

	if (valid)
	{
		m_parentPBSMaterial = static_cast<gkPBSMaterialImpl*>(getIN_PBS_MAT()->getFrom()->getParent());
	}

}

Ogre::PbsMaterial* gkPBSSlotImpl::getMaterial()
{
	 return m_parentPBSMaterial->getMaterial();
}

bool gkPBSSlotImpl::_evaluate(gkScalar tick)
{
	return m_isValid;
}
void gkPBSSlotImpl::_update(gkScalar tick)
{
	if (!m_parentPBSMaterial->isValid())
	{
		m_isValid = false;
	}

	Ogre::PbsMaterial* pbsMat = m_parentPBSMaterial->getMaterial();
	if (getIN_OFFSET_X()->isDirty() ||getIN_OFFSET_Y()->isDirty() ||getIN_SCALE_X()->isDirty() ||getIN_SCALE_X()->isDirty())
	{
		pbsMat->setOffsetAndScale(m_mapSlot,gkVector2(getIN_OFFSET_X(true)->getValue(),getIN_OFFSET_Y(true)->getValue()),gkVector2(getIN_SCALE_X(true)->getValue(),getIN_SCALE_Y(true)->getValue()));
	}
	if (getIN_UV_MAP()->isDirty())
	{
		pbsMat->setUvSetIndex(m_mapSlot,getIN_UV_MAP(true)->getValue());
	}
}

