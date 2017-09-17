/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMessageGetImpl.h>
#include <Events/gkEvents.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkVariable.h"
gkMessageGetImpl::gkMessageGetImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkMessageGetAbstract(parent,id,name), m_fired(false)
{
	setBucketNr((int)gkLogicNode::BUCKET_GETTER);
}

gkMessageGetImpl::~gkMessageGetImpl()
{}

void gkMessageGetImpl::_initialize()
{
	m_type = gkMessageEvent::sk_EventTypePrefix.str()+"_"+getPropTYPE();
	safeAddListener(this,m_type);
}

bool gkMessageGetImpl::_evaluate(gkScalar tick)
{
	return true;
}
void gkMessageGetImpl::_preUpdate()
{
	if (m_fired)
	{
		getOUT_FIRED()->setValue(true);
		getOUT_NOT_FIRED()->setValue(false);
	}
	else
	{
		getOUT_FIRED()->setValue(false);
		getOUT_NOT_FIRED()->setValue(true);
		getOUT_VALUE()->setValue("");
		getOUT_EXT()->setValue("");
	}
}

void gkMessageGetImpl::_postUpdate()
{
	m_fired = false;
}


bool gkMessageGetImpl::handleEvent( IEventData const & event)
{
	m_fired = true;
	const gkMessageEvent& msgEvent = static_cast<const gkMessageEvent&>(event);

	gkVariable* valueData = msgEvent.getData("value");
	getOUT_VALUE()->setVar( valueData );

	gkVariable* extData = msgEvent.getData("ext");
	getOUT_EXT()->setVar( extData );
	return true;
}

