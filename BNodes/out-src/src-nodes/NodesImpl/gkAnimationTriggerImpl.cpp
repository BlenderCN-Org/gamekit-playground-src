/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkAnimationDefinitionImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkAnimationTriggerImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkEngine.h"
#include "gkUserDefs.h"

#include "gkValue.h"
gkAnimationTriggerImpl::gkAnimationTriggerImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkAnimationTriggerAbstract(parent,id,name),akAnimationPlayerEvent(0), m_anim(0),m_triggerType(0)
{}

gkAnimationTriggerImpl::~gkAnimationTriggerImpl()
{}

void gkAnimationTriggerImpl::_afterInit2()
{
	if (m_isValid)
	{
		m_anim = static_cast<gkAnimationDefinitionImpl*>(getIN_ANIMATION()->getFrom()->getParent());
		m_player = m_anim->getPlayer();
		m_player->addAnimationEvent(this);
		gkScalar animFps = gkEngine::getSingleton().getUserDefs().animFps;
		m_triggerType = TRIGGER_COUNT++;
		m_player->addTrigger(getPropframe() / animFps,m_triggerType);
	}
}

void gkAnimationTriggerImpl::onTrigger(akAnimationPlayer* m_player, int type)
{
	if (type == m_triggerType)
	{
		gkLogger::write("TRIGGER!! "+getName()+" :"+gkToString(m_player->getTimePosition()));
		getOUT_TRIGGERED()->setValue(true);
		getOUT_NOT_TRIGGERED()->setValue(false);
	}
}


bool gkAnimationTriggerImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkAnimationTriggerImpl::_postUpdate()
{
	getOUT_TRIGGERED()->setValue(false);
	getOUT_NOT_TRIGGERED()->setValue(true);
}

int gkAnimationTriggerImpl::TRIGGER_COUNT = 0;
