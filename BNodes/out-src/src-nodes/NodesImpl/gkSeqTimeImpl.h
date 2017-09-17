/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef SEQTIME_H_
#define SEQTIME_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkSequenceImpl.h>
// gkSeqTimeImpl

class gkSeqTimeImpl
		: public gkSeqTimeAbstract, public gkSequenceImpl::gkSequenceBrick
{
public:
	gkSeqTimeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkSeqTimeImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
//	virtual void _afterInit();
//	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(gkScalar tick); // preUpdate
	virtual void _update(gkScalar tick); // update your stuff
//	virtual void _postUpdate(gkScalar tick); // afterUpdate
	virtual bool debug(rapidjson::Value& jsonNode);

	void reset();
private:
	float m_time;
	gkSequenceImpl* m_parentSequence;
};

#endif
