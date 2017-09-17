/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef OBJECTNODE_H_
#define OBJECTNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkObjectNodeImpl

class gkObjectNodeImpl
		: public gkObjectNodeAbstract
{
public:
	gkObjectNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	~gkObjectNodeImpl();

	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
private:
	gkString m_currentObjectName;
	gkScene* m_scene;
};

#endif
