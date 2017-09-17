/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkObjectNodeImpl.h>
#include "Logic/gkLogicNode.h"

#include "gkScene.h"
#include "gkGameObject.h"
#include "gkEngine.h"

#include "gkGameObjectInstance.h"
gkObjectNodeImpl::gkObjectNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkObjectNodeAbstract(parent,id,name), m_scene(0)
{}

gkObjectNodeImpl::~gkObjectNodeImpl()
{}

void gkObjectNodeImpl::_initialize()
{
	m_scene = m_object?
						m_object->getOwner()
					:   gkEngine::getSingleton().getActiveScene();

	gkString objName(getPropObj());
	if (!objName.empty())
	{
		gkGameObject* sceneObj = 0;

		// if not global try to locate the objName in the group first
		if (!getPropGLOBAL()
				&& getAttachedObject()->isGroupInstance())
		{
			sceneObj= getAttachedObject()->getGroupInstance()->getObject(objName);
		}

		if (!sceneObj)
		{
			sceneObj = m_scene->getObject(objName);
		}

		if (sceneObj)
		{
			attachObject(sceneObj);
		}
		else
		{
			gkLogger::write("Unknown Object:"+objName);
			m_isValid = false;
			return;
		}
		SET_SOCKET_VALUE(OUT_OBJECT,sceneObj->getName());
	}
	else
	{
		m_isValid = false;
	}


}

bool gkObjectNodeImpl::_evaluate(gkScalar tick)
{
//	if (getIN_NAME()->isDirty())
//	{
//
//	}
//	gkString input = getIN_NAME()->getValue();
//	if (input!=m_currentObjectName)
//	{
//		gkLogger::write("gkObjectNodeImpl "+getName()+": tries to to locate objectname:"+input);
//
//		gkGameObject* obj = m_scene->getObject(input);
//		if (obj)
//		{
//			attachObject(obj);
//			m_currentObjectName=input;
//			getOUT_OBJECT()->setValue(obj->getName());
//			gkLogger::write("FOUND!");
//		}
//		else
//		{
//			gkLogger::write("FAILED!");
//		}
//	}
	return false;
}
void gkObjectNodeImpl::_update(gkScalar tick)
{
}

