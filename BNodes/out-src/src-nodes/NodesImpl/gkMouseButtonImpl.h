/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef MOUSEBUTTON_H_
#define MOUSEBUTTON_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "gkWindowSystem.h"
// gkMouseButtonImpl

class gkMouseButtonImpl
		: public gkMouseButtonAbstract
{
public:
	gkMouseButtonImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkMouseButtonImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

	bool isButtonDown();
private:
	int m_counter;
	bool m_pressed;
	bool m_listenerBound;
};

#endif
