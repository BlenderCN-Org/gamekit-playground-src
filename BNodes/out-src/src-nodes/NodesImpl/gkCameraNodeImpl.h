/*
 * gkTemplateNode.h
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#ifndef CAMERANODE_H_
#define CAMERANODE_H_

#include <BNodes/out-src/src-gen/Generated/gkGamekit.h>

// gkCameraNodeImpl

class gkCameraNodeImpl
		: public gkCameraNodeAbstract
{
public:
	gkCameraNodeImpl(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkCameraNodeImpl();

	// overwrite delegator(?) methods that get called by the node if no lua-logic is there (or the strategy tells to do it)
	virtual void _initialize(); // called on node-creation
//	virtual void _afterInit();
//	virtual void _afterInit2();
	virtual bool _evaluate(gkScalar tick); // called everytick to check if the update-method should be called
//	virtual void _preUpdate(); // preUpdate
	virtual void _update(gkScalar tick); // update your stuff
//	virtual void _postUpdate(); // afterUpdate
//	virtual bool debug(rapidjson::Value& jsonNode);

private:
	void calculateNewPosition(const gkVector3& currentPosition, gkScalar rayLength, gkScalar tick);

private:

	gkVector3 m_center;
	gkVector3 m_oldCenter;

	gkGameObject* m_target;
	gkGameObject* m_centerObj;

	gkQuaternion m_rollNode;
	gkQuaternion m_pitchNode;

	gkScalar m_idealRadius;
	bool m_radiusIdealIsSet;

	gkScalar m_oldRadius;
	bool m_oldRadiusIsSet;
};

#endif
