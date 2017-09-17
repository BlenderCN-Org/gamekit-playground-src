/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef STATETRANSITION_H_
#define STATETRANSITION_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <rapidjson/document.h>
#include "gkFSM.h"

// gkStateTransitionImpl

class gkStateImpl;

class gkStateTransitionImpl
		: public gkStateTransitionAbstract
{
public:

	class gkTransitionEvent : public gkFSM::IEvent
	{
	public:
		gkTransitionEvent(gkILogicSocket* socket) : m_socket(socket){}
		virtual bool evaluate() { return m_socket->getVar()->getValueBool();}
	private:
		gkILogicSocket* m_socket;
	};

	gkStateTransitionImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkStateTransitionImpl();

	virtual void initalize() {
		gkStateTransitionAbstract::initialize();
	}

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

	virtual bool debug(rapidjson::Value& jsonVal);

private:
	gkStateImpl* m_fromState;
	gkStateImpl* m_toState;

};

#endif
