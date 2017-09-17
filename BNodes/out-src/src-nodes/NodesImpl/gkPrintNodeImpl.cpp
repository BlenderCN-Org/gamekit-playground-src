/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkPrintNodeImpl.h>
#include "Logic/gkLogicNode.h"

#include "gkDebugScreen.h"
gkPrintNodeImpl::gkPrintNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPrintNodeAbstract(parent,id,name),m_outputNode(0)
{}

gkPrintNodeImpl::~gkPrintNodeImpl()
{}

void gkPrintNodeImpl::_initialize()
{
	if (getIN_TEXT()->isConnected())
	{
		m_outputNode = getIN_TEXT()->getFrom()->getParent();
	}
}

void gkPrintNodeImpl::_afterInit()
{
	// init value
	_update(0);
}

bool gkPrintNodeImpl::_evaluate(gkScalar tick)
{
	return getIN_DOPRINT()->getValue() && (!getPropON_CHANGE() || getIN_TEXT()->isDirty());
}
void gkPrintNodeImpl::_update(gkScalar tick)
{
	gkString output;

	if (m_outputNode)
	{
		output = m_outputNode->getName()+": "+getPropPREFIX()+" "+getIN_TEXT(true)->getVar(true)->toString();
	}
	else
	{
		output = getName()+": "+getPropPREFIX()+" "+getIN_TEXT(true)->getValue();
	}
	int mode = getPropMODE();
	MODE m = (MODE) mode;

	if (m == MODE_log_screen || m==MODE_only_screen)
	{
		gkDebugScreen::printTo(output);
	}
	if (m == MODE_log_screen || m==MODE_only_log)
	{
		gkLogger::write(output,true);
	}

}

