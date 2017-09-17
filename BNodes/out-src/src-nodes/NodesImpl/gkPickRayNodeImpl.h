/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PICKRAYNODE_H_
#define PICKRAYNODE_H_

#include "Physics/gkRayTest.h"
#include "gkGamekit.h"

// gkPickRayNodeImpl

class gkPickRayNodeImpl
		: public gkPickRayNodeAbstract
{
public:
	gkPickRayNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPickRayNodeImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
//	virtual void _afterInit();
//	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(); // preUpdate
	virtual void _update(gkScalar tick); // update your stuff
//	virtual void _postUpdate(); // afterUpdate
//	virtual bool debug(rapidjson::Value& jsonNode);
private:
	gkRayTest m_rayTest;
};

#endif
