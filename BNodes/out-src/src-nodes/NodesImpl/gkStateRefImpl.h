/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef STATEREF_H_
#define STATEREF_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkStateRefImpl

class gkStateImpl;

class gkStateRefImpl
		: public gkStateRefAbstract
{
public:
	gkStateRefImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkStateRefImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
	virtual bool debug(rapidjson::Value& jsonNode);

	GK_INLINE gkStateImpl* getState() { return m_state; }
private:
	gkStateImpl* m_state;

};

#endif
