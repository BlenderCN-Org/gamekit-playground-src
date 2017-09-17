/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PROPERTYGET_H_
#define PROPERTYGET_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <rapidjson/document.h>

class gkVariable;
// gkPropertyGetImpl

class gkPropertyGetImpl
		: public gkPropertyGetAbstract
{
public:
	gkPropertyGetImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPropertyGetImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _afterInit(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
	virtual bool debug(rapidjson::Value& jsonVal);

private:
	void getVar();

	gkVariable* m_var;
	gkGameObject* m_target;
};

#endif
