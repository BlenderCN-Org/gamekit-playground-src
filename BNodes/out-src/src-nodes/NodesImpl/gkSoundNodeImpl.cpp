/*
 * gkTemplateNode.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: ttrocha
 */

#include <BNodes/out-src/src-nodes/NodesImpl/gkSoundNodeImpl.h>
#include "Logic/gkLogicNode.h"
#include "Logic/gkLogicSocket.h"

#include "gkLogger.h"

#include "Sound/gkSoundManager.h"
#include "Sound/gkSource.h"
#include "Sound/gkSound.h"

gkSoundNodeImpl::gkSoundNodeImpl(gkLogicTree* parent, size_t id, const gkString& name)
	: gkSoundNodeAbstract(parent,id,name), m_source(0)
{}

gkSoundNodeImpl::~gkSoundNodeImpl()
{
}

void gkSoundNodeImpl::_initialize()
{
	gkSound* snd = gkSoundManager::getSingleton().getByName<gkSound>(getPropFILEP());
	if (!snd) {
		bool result = gkSoundManager::getSingleton().loadSound(getPropFILEP(),getPropFILEP());
		if (!result) {
			gkLogger::write("couldn't find sound:"+getPropFILEP());
			return;
		}
		snd = gkSoundManager::getSingleton().getByName<gkSound>(getPropFILEP());
	}
	m_source = snd->createSource();
}

bool gkSoundNodeImpl::_evaluate(gkScalar tick)
{
	return getIN_ENABLED(false);
}
void gkSoundNodeImpl::_update(gkScalar tick)
{
	m_source->play();
}

