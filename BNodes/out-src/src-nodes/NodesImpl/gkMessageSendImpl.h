/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef MESSAGESEND_H_
#define MESSAGESEND_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <gkEventManager.h>
#include "Thread/gkPtrRef.h"
// gkMessageSendImpl

class gkMessageSendImpl
		: public gkMessageSendAbstract
{
public:
	gkMessageSendImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkMessageSendImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
//	virtual void _afterInit();
//	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(gkScalar tick); // preUpdate
	virtual void _update(gkScalar tick); // update your stuff
//	virtual void _postUpdate(gkScalar tick); // afterUpdate
//	virtual bool debug(rapidjson::Value& jsonNode);

private:
	IEventDataPtr m_event;

};

#endif
