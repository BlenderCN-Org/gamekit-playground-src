/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef STATE_H_
#define STATE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include "gkFSM.h"
// gkStateImpl

class gkStateMachineImpl;

class gkStateImpl
		: public gkStateAbstract
{
public:
	class gkStateTrigger : public gkFSM::ITrigger
	{
	public:
		virtual void execute(int fromState, int toState);
		gkStateTrigger(bool startTrigger,gkStateImpl* state) : m_isStartTrigger(startTrigger),m_state(state) {}
	private:
		bool m_isStartTrigger;
		gkStateImpl* m_state;

		friend class gkStateImpl;
	};

	gkStateImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkStateImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff

	void _setParentFSM(gkStateMachineImpl* fsmNode);
	void _addStartTriggersFromSocket(gkILogicSocket* sock);
	void _addEndTriggersFromSocket(gkILogicSocket* sock);
	void _addActiveFromSocket(gkILogicSocket* sock);

	GK_INLINE gkStateMachineImpl* getStateMachine() { return m_fsmNode;}
	bool debug(rapidjson::Value& jsonVal);

	void activateStateInStatemachine();
private:


	gkStateMachineImpl* m_fsmNode;

	gkStateTrigger* m_startTrigger;
	gkStateTrigger* m_endTrigger;

};

#endif
