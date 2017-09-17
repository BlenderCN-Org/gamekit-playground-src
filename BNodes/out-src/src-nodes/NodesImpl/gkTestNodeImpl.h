/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef TESTNODE_H_
#define TESTNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkTestNodeImpl

class gkTestNodeImpl
		: public gkTestNodeAbstract
{
public:
	gkTestNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkTestNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
};

#endif
