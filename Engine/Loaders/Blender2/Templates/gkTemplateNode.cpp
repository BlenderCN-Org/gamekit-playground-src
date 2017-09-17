/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include /*name|dq,fu,pre#gk,post#Impl.h:nodeName*/"gkTemplateNode.h"/*endname*/
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/::/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/(gkLogicTree* parent, size_t id, const gkString& name)
	: /*name:abstractName*/gkTemplateNodeAbstract/*endname*/(parent,id,name)
{}

/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/::~/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/()
{}

void /*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/::_initialize()
{
}

bool /*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/::_evaluate(gkScalar tick)
{
	return true;
}
void /*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/::_update(gkScalar tick)
{
}


