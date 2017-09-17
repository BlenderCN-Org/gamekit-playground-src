/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef COLLISIONNODE_H_
#define COLLISIONNODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkCollisionNodeImpl
class gkPhysicsController;

class gkCollisionNodeImpl
		: public gkCollisionNodeAbstract
{
public:
	gkCollisionNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkCollisionNodeImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
	virtual void _update(gkScalar tick); // update your stuff
private:
	gkPhysicsController* m_object;

	typedef utArray<gkGameObject*> CollisionObjects;
	CollisionObjects m_colObjList;

};

#endif
