/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkStateMachineImpl

class gkFSM;
class gkStateImpl;

class gkStateMachineImpl
		: public gkStateMachineAbstract
{
public:
	gkStateMachineImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkStateMachineImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _preUpdate(); // update your stuff
	virtual void _postUpdate();
	GK_INLINE gkFSM* getFSM() {return m_fsm;}

	// register this socket to be set to false again before next update
	void addResetTrigger(gkLogicSocket<bool>* sock) { m_resetTriggerList.push_back(sock); }

	gkStateImpl* getStateByName(const gkString& name);
	gkStateImpl* getStateByID(int id);
	gkStateImpl* getCurrentState();
	void setState(gkStateImpl* newState);
	void setState(int ID);

	GK_INLINE bool isActive();
	virtual bool debug(rapidjson::Value& jsonNode);


	static gkStateMachineImpl* getStateMaschine(const gkString& name);

private:
	typedef utHashTable<utCharHashKey,gkStateMachineImpl*> StateMachinesByName;
	static StateMachinesByName machinesByName;

	typedef utHashTable<utCharHashKey,gkStateImpl*> StatesByName;
	StatesByName m_statesByName;
	typedef utHashTable<utIntHashKey,gkStateImpl*> StatesByID;
	StatesByID m_statesByID;

	typedef utList< gkLogicSocket<bool>* > ResetTriggerList; // Keeps a ref to the sockets that fired start/endTrigger
	ResetTriggerList m_resetTriggerList;
	gkFSM* m_fsm;

	bool m_active;
	int m_currentStateNr;

	bool m_initialized;
};

#endif
