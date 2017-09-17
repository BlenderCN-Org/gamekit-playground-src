/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkAnimationDefinitionImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkAnimationPlayerImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"


gkAnimationPlayerImpl::gkAnimationPlayerImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkAnimationPlayerAbstract(parent,id,name), m_anim(0)
{}

gkAnimationPlayerImpl::~gkAnimationPlayerImpl()
{}

void gkAnimationPlayerImpl::_afterInit()
{
	gkNodeContext& ctx = gkNodeManager::getSingleton().getContext();
	gkString findAnimationDef = getAttachedObject()->getName()+"_"+getPropNAME();
	m_anim = static_cast<gkAnimationDefinitionImpl*>(ctx.getNode(gkAnimationDefinitionImpl::getStaticType(),findAnimationDef.c_str()));
	if (!m_anim)
	{
		gkLogger::write(getName()+": Unknown animation "+findAnimationDef);
		m_isValid = false;
	}
	else
	{
		gkString rawAnimName = m_anim->getPropRAW_ANIM_NAME();
		setPropRAW_ANIM_NAME(rawAnimName);
	}
}

bool gkAnimationPlayerImpl::_evaluate(gkScalar tick)
{
	return m_isValid;
}

void gkAnimationPlayerImpl::_preUpdate()
{
	if (m_anim->m_triggerDone)
		int a=0;

	getOUT_DONE_TRIGGER()->setValue(m_anim->m_triggerDone);
	getOUT_LOOPEND_TRIGGER()->setValue(m_anim->m_triggerLoopDone);
}

void gkAnimationPlayerImpl::_postUpdate()
{
}

void gkAnimationPlayerImpl::_update(gkScalar tick)
{
	getOUT_ENABLED()->setValue(getIN_ENABLED(true)->getValue());

	if (getIN_RESET() && getIN_RESET(true)->getValue())
	{
		m_anim->reset();
	}

	if (!getIN_ENABLED()->getValue())
	{
		getOUT_DONE_TRIGGER()->setValue(false);
		getOUT_LOOPEND_TRIGGER()->setValue(false);
		getOUT_ENABLED()->setValue(false);
		getOUT_PAUSED()->setValue(false);
		getOUT_STOPPED()->setValue(true);
		return;
	}

	if (getIN_START()->isDirty() && getIN_START(true)->getValue())
	{
		gkLogger::write("START!"+getName(),true);
		m_anim->start();
	}
	if (getIN_STOP()->isDirty() && getIN_STOP(true)->getValue())
	{
		gkLogger::write("STOP!",true);
		m_anim->stop();
	}

	if (getIN_PAUSE()->isDirty() && getIN_PAUSE(true)->getValue())
	{
		gkLogger::write("PAUSE!",true);
		m_anim->pause();
	}

	if (getIN_RESUME()->isDirty() && getIN_RESUME(true)->getValue())
	{
		m_anim->resume();
	}






	getOUT_ENABLED()->setValue(true);
	getOUT_PAUSED()->setValue(m_anim->m_currentState == gkAnimationDefinitionImpl::ST_paused);
	getOUT_RUNNING()->setValue(m_anim->m_currentState == gkAnimationDefinitionImpl::ST_playing);
	getOUT_STOPPED()->setValue(m_anim->m_currentState == gkAnimationDefinitionImpl::ST_stopped);
}

