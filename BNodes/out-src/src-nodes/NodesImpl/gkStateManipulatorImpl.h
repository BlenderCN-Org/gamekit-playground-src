/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef STATEMANIPULATOR_H_
#define STATEMANIPULATOR_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

class gkStateMachineImpl;
class gkStateRefImpl;

// gkStateManipulatorImpl

class gkStateManipulatorImpl
		: public gkStateManipulatorAbstract
{
public:
	gkStateManipulatorImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkStateManipulatorImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

private:
	gkStateRefImpl* m_stateRef;
	TYPE m_type;

};

#endif
