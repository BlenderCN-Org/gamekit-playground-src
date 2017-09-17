/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>
#include <rapidjson/document.h>

// gkPropertyImpl

class gkPropertyImpl
		: public gkPropertyAbstract
{
public:
	gkPropertyImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkPropertyImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(); // update your stuff
	virtual void _update(gkScalar tick); // update your stuff

private:
	void setValueFromINPUT(const gkString& value);
	bool debug(rapidjson::Value& jsonVal);

	gkGameObject* m_targetObject;

	MODE m_mode;
	TYPE m_varType;
	gkString m_propName;
	gkString m_currentValueAsString;
	gkVariable* m_var;

};

#endif
