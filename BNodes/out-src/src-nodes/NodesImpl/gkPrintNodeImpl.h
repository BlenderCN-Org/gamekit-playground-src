/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PRINTNODE_H_
#define PRINTNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkPrintNodeImpl

class gkPrintNodeImpl
		: public gkPrintNodeAbstract
{
public:
	gkPrintNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkPrintNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
private:
	gkLogicNode* m_outputNode;
};

#endif
