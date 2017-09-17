/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef MESSAGEGET_H_
#define MESSAGEGET_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <gkEventManager.h>

// gkMessageGetImpl

class gkMessageGetImpl
		: public gkMessageGetAbstract, public IEventListener
{
public:
	gkMessageGetImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkMessageGetImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
//	virtual void _afterInit();
//	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(); // preUpdate
	virtual void _preUpdate(); // update your stuff
	virtual void _postUpdate(); // afterUpdate
//	virtual bool debug(rapidjson::Value& jsonNode);

	virtual const gkString & getListenerName(void) { static const gkString name="gkMessageGetImplListener"; return name;};

	virtual bool handleEvent( IEventData const & event);
private:
	gkHashedString m_type;
	bool m_fired;
};

#endif
