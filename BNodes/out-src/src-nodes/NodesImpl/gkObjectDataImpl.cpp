/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkObjectDataImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

gkObjectDataImpl::gkObjectDataImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkObjectDataAbstract(parent,id,name), m_obj(0)
{
	setBucketNr((int)BUCKET_SETTER);
}

gkObjectDataImpl::~gkObjectDataImpl()
{}

void gkObjectDataImpl::_initialize()
{
	m_obj = getIN_OBJECT()->isConnected()
			?getIN_OBJECT(true)->getGameObject()
			:getAttachedObject();
}

bool gkObjectDataImpl::_evaluate(gkScalar tick)
{
	return m_isValid && m_obj;
}
void gkObjectDataImpl::_update(gkScalar tick)
{
	if (getIN_OBJECT(true)->isDirty()) {
		m_obj = getIN_OBJECT()->isConnected()
				?getIN_OBJECT(true)->getGameObject()
				:getAttachedObject();
	}

	getOUT_LINVEL()->setValue(m_obj->getLinearVelocity());
	getOUT_WPOS()->setValue(m_obj->getWorldPosition());
	getOUT_POS()->setValue(m_obj->getPosition());
	getOUT_WROT()->setValue(m_obj->getWorldRotation().toVector3());
	getOUT_ROT()->setValue(m_obj->getRotation().toVector3());
	getOUT_SCALE()->setValue(m_obj->getScale());
	getOUT_WSCALE()->setValue(m_obj->getWorldScale());
	getOUT_LINVEL()->setValue(m_obj->getLinearVelocity());
	getOUT_ANGVEL()->setValue(m_obj->getAngularVelocity());
}

bool gkObjectDataImpl::debug(rapidjson::Value& val)
{
	bool res = gkLogicNode::debug(val);

	gkString result = gkNodeManager::getSingleton().json2string(val);
//	gkLogger::write("ObjectDataImpl:" + result);
	return res;
}
