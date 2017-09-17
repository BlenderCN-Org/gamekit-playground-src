/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkObjectManipulatorImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkGameObject.h"

#include "gkGameObjectInstance.h"

#include "gkScene.h"
gkObjectManipulatorImpl::gkObjectManipulatorImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkObjectManipulatorAbstract(parent,id,name), m_target(0)
{
}

gkObjectManipulatorImpl::~gkObjectManipulatorImpl()
{}

void gkObjectManipulatorImpl::_initialize()
{

}

bool gkObjectManipulatorImpl::_evaluate(gkScalar tick)
{
	return getIN_ENABLED()->getValue();
}
void gkObjectManipulatorImpl::_update(gkScalar tick)
{
	m_target = getIN_TARGET()->isConnected()
				? getIN_TARGET()->getGameObject()
				: getAttachedObject();

	if (getPropTYPE()==gkObjectManipulatorImpl::TYPE_DESTROY_INSTANCE)
	{
		if (m_target && m_target->isInstanced())
		{
			if (m_target->isGroupInstance())
			{
				m_target->getGroupInstance()->destroyInstance();
			}
			else
			{
				m_target->destroyInstance();
			}
		}
	}
}

