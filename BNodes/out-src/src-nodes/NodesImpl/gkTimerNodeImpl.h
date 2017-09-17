/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef TIMERNODE_H_
#define TIMERNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "Logic/gkTimerNode.h"
// gkTimerNodeImpl

class gkTimerNodeImpl
		: public gkTimerNodeAbstract
{
public:
	gkTimerNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkTimerNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

private:
	float m_value;
	float m_initValue;
	float m_endValue;
	float m_dir;

	bool m_active;

	void setTimer();
};



#endif
