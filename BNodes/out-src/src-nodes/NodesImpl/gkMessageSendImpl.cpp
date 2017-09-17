/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkMessageSendImpl.h>
#include <Events/gkEvents.h>
#include <gkEventManager.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkValue.h"
gkMessageSendImpl::gkMessageSendImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkMessageSendAbstract(parent,id,name)
{
	setBucketNr((int)gkLogicNode::BUCKET_SETTER);
}

gkMessageSendImpl::~gkMessageSendImpl()
{}

void gkMessageSendImpl::_initialize()
{

	m_event = IEventDataPtr(new gkMessageEvent(getPropTYPE()));
}

bool gkMessageSendImpl::_evaluate(gkScalar tick)
{
	return getIN_SEND()->getValue();
}
void gkMessageSendImpl::_update(gkScalar tick)
{
//	if (getIN_VALUE(true)->isDirty())
	bool connected = getIN_VALUE()->isConnected();
	{

		gkVariable* valueData = connected
							?	getIN_VALUE()->getVar(true)
							:	getIN_VALUE()->getVar();

		static_cast<gkMessageEvent*>(m_event.get())->setData("value",valueData);
	}
//	if (getIN_EXT(true)->isDirty())
	{
		gkVariable* valueData = connected
							?	getIN_EXT()->getVar(true)
							:	getIN_EXT()->getVar();

		static_cast<gkMessageEvent*>(m_event.get())->setData("ext",valueData);
	}
	gkLogger::write("SEND EVENT:"+gkToString(getIN_SEND()->isDirty()));
	safeQueueEvent(m_event);
}

