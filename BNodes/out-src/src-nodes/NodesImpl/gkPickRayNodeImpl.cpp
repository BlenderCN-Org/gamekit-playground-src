/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <gkCam2ViewportRay.h>
#include <gkGameObject.h>
#include <gkLogger.h>
#include <gkMathUtils.h>
#include <Physics/gkRayTest.h>
#include <gkResource.h>
#include <gkScene.h>
#include <gkSceneManager.h>
#include "gkPickRayNodeImpl.h"
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkPickRayNodeImpl::gkPickRayNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkPickRayNodeAbstract(parent,id,name)
{}

gkPickRayNodeImpl::~gkPickRayNodeImpl()
{}

void gkPickRayNodeImpl::_initialize()
{
}

bool gkPickRayNodeImpl::_evaluate(gkScalar tick)
{
	bool evaluate = getIN_UPDATE(true)->getValue();

	if (!evaluate)
	{
		getOUT_HIT()->setValue(false);
		getOUT_NOT_HIT()->setValue(true);
		return false;
	}

	return true;
}
void gkPickRayNodeImpl::_update(gkScalar tick)
{
	gkScene* scene = getIN_sceneName(true)->getValue().empty()?
						getAttachedObject()->getOwner()
					:   static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(getIN_sceneName(true)->getValue()));

	if (!scene) {
		gkLogger::write(getName()+": Couldn't find scene! "+getIN_sceneName(true)->getValue());
		return;
	}


	Ogre::Ray ray = gkCam2ViewportRay(getIN_X(true)->getValue(),getIN_Y(true)->getValue());

	bool didHit = false;

	if (!getProphitProperty().empty()){
		xrayFilter xray(0, getProphitProperty(), "");
		gkVector3 from(ray.getOrigin());
		gkVector3 to(ray.getOrigin()+ray.getDirection()*1000);

		didHit = m_rayTest.collides(from, to, xray);
	}
	else
	{
		didHit = m_rayTest.collides(ray);
	}

	if (didHit)
	{
		gkGameObject* hitObj = m_rayTest.getHitObject();
		getOUT_HIT_OBJ()->setValue(hitObj->getName());
		getOUT_HIT_OBJ()->setSocketObject(hitObj);
		getOUT_HIT_NORMAL()->setValue(m_rayTest.getHitNormal());
		getOUT_HIT_POINT()->setValue(m_rayTest.getHitPoint());
		getOUT_HIT()->setValue(true);
	}
	else
	{
		getOUT_HIT()->setValue(false);
	}

}

