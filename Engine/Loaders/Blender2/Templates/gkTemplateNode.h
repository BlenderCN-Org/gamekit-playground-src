/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef /*name|u,post#_H_:nodeName*/TEMPLATENODE/*endname*/
#define /*name|u,post#_H_:nodeName*/TEMPLATENODE/*endname*/


#include /*name|dq,fu,pre#gk,post#.h:treename*/"gkTemplateNode.h"/*endname*/

/*block:rip*/
#include "gkAbstractNodesTemplate.h"
/*endblock:rip*/
// /*name:gkNodeNameImpl*/nodename/*endname*/

class /*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/
		: public /*name|fu,pre#gk,post#Abstract,setvar#abstractName:nodeName*/gkTemplateNodeAbstract/*endname*/
{
public:
	/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~/*name:gkNodeNameImpl*/gkTemplateNodeImpl/*endname*/();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
//	virtual void _afterInit();
//	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(); // preUpdate
	virtual void _update(gkScalar tick); // update your stuff
//	virtual void _postUpdate(); // afterUpdate
//	virtual bool debug(rapidjson::Value& jsonNode);

};

#endif
