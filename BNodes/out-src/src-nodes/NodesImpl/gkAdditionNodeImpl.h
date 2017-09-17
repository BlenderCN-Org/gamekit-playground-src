/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef ADDITIONNODE_H_
#define ADDITIONNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

class gkLuaEvent;
// gkAdditionNodeImpl

class gkAdditionNodeImpl
		: public gkAdditionNodeAbstract
{
public:
	gkAdditionNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkAdditionNodeImpl();




};

#endif
