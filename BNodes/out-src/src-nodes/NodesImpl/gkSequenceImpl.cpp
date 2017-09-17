/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkSequenceImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkSequenceImpl::gkSequenceImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkSequenceAbstract(parent,id,name), m_state(ST_WAITING)
{}

gkSequenceImpl::~gkSequenceImpl()
{}

void gkSequenceImpl::_initialize()
{
	reset();
}

bool gkSequenceImpl::_evaluate(gkScalar tick)
{
	return getIN_RESET()->isDirty()
			|| getIN_ENABLE()->isDirty()
			|| m_state == ST_RUNNING
			|| getOUT_ON_END()->getValue();
}

void gkSequenceImpl::_preUpdate()
{


}

void gkSequenceImpl::_update(gkScalar tick)
{
	if (getIN_RESET()->isDirty() && getIN_RESET(true)->getValue())
	{
		resetAll();
	}

	if (m_state == ST_RUNNING && getOUT_ON_START()->getValue()) // reset the on_start-flag
	{
		getOUT_ON_START()->setValue(false);
	}

	if (getIN_ENABLE()->isDirty())
	{
		if (getIN_ENABLE(true)->getValue() && m_state != ST_RUNNING)
		{
			m_state = ST_RUNNING;
			getOUT_ACTIVE()->setValue(true);
			getOUT_ON_START()->setValue(true);
			getOUT_ON_END()->setValue(false);
			getOUT_FINISHED()->setValue(false);
		}
//		else if (!getIN_ENABLE(true)->getValue() && m_state == ST_RUNNING)
//		{
//			m_state = ST_WAITING;
//			getOUT_ACTIVE()->setValue(false);
//		}
	}

	if (m_state == ST_RUNNING)
	{
		// check connected sequence-bricks
		int outSocketAmount = getOUT_LOGIC()->getOutSocketAmount();
		bool isActive = getOUT_ACTIVE()->getValue();

		for (int i=0;i<outSocketAmount;i++)
		{
			gkLogicNode* node = getOUT_LOGIC()->getOutSocketAt(i)->getParent();

			gkSequenceBrick* seqFin = dynamic_cast<gkSequenceBrick*>(node);

			// we can stop if we find one single brick that is not finished yet
			bool finished = seqFin->isFinished();
			if (!finished)
			{
				m_state = ST_RUNNING;
				getOUT_FINISHED()->setValue(false);
				return;
			}
		}
		// if we get here all bricks are finished so the whole sequence is finished
		m_state = ST_FINISHED;
		getOUT_FINISHED()->setValue(true);
		getOUT_ON_END()->setValue(true);
		getOUT_ACTIVE()->setValue(false);
	}
	else if (m_state == ST_FINISHED)
	{
		// evaluate will only let us here once (to unset the on_end-trigger)
		getOUT_ON_END()->setValue(false);
	}
}

void gkSequenceImpl::reset()
{
	getOUT_ACTIVE()->setValue(false);
	getOUT_FINISHED()->setValue(false);
	m_state = ST_WAITING;
	const int outSocketAmount = getOUT_LOGIC()->getOutSocketAmount();
	for (int i=0;i<outSocketAmount;i++)
	{
		gkLogicNode* node = getOUT_LOGIC()->getOutSocketAt(i)->getParent();

		gkSequenceBrick* seqFin = dynamic_cast<gkSequenceBrick*>(node);
		if (seqFin)
			seqFin->reset();
		else
			gkLogger::write("CAST PROBLEM gkSequenceIMPL");
	}
}

void gkSequenceImpl::resetAll()
{
	reset();
	// also reset the sequences that are connected on the finished-outsocket
	const int connectionAtFinishedSocket = getOUT_FINISHED()->getOutSocketAmount();
	for (int i=0;i<connectionAtFinishedSocket;i++)
	{
		gkLogicNode* node = getOUT_FINISHED()->getOutSocketAt(i)->getParent();

		if (node->getType() == gkSequenceImpl::NODE_NAME)
		{
			gkSequenceImpl* seqImpl = static_cast<gkSequenceImpl*>(node);
			seqImpl->resetAll();
		}
	}
}

bool gkSequenceImpl::debug(rapidjson::Value& jsonNode)
{
	bool result = gkLogicNode::debug(jsonNode);

	if (getOUT_ACTIVE()->getValue())
	{
		rapidjson::Document& jsonDoc = gkNodeManager::getSingleton().getDebugJSONDoc();
		// if fromState is active then set a color
		jsonNode.AddMember("color","(0.14,0.63,0.02)",jsonDoc.GetAllocator());
	}
	return result;
}
