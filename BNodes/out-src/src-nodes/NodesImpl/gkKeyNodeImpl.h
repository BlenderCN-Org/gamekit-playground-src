/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef KEYNODE_H_
#define KEYNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkKeyNodeImpl

class gkKeyNodeImpl
		: public gkKeyNodeAbstract
{
public:
	gkKeyNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkKeyNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
	bool isButtonDown();
private:
	int m_counter;
	bool m_pressed;

	bool m_curIsDown;
};

#endif
