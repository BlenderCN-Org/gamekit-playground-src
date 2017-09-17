/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef EXITNODE_H_
#define EXITNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkExitNodeImpl

class gkExitNodeImpl
		: public gkExitNodeAbstract
{
public:
	gkExitNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkExitNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
};

#endif
