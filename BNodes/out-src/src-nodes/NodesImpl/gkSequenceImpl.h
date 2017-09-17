/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkSequenceImpl

class gkSequenceImpl
		: public gkSequenceAbstract
{
public:

	class gkSequenceBrick
	{
	public:
		gkSequenceBrick() : m_finished(0) {}
		virtual ~gkSequenceBrick(){}
		bool isFinished() { return m_finished;}
		virtual void reset() { m_finished = false;}
		void setFinished(bool finished) { m_finished = finished;}
	protected:
		bool m_finished;
	};

	enum State {
		ST_FINISHED,
		ST_RUNNING,
		ST_WAITING
	};

	gkSequenceImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkSequenceImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _preUpdate();
	virtual void _update(gkScalar tick); // update your stuff
	virtual bool debug(rapidjson::Value& jsonNode);

	void reset();
	void resetAll();
private:
	State m_state;
};

#endif
