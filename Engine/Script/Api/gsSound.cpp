/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "gsSound.h"
#include "gkDebugScreen.h"

#ifdef OGREKIT_OPENAL_SOUND
# include "gkSoundManager.h"
# include "Sound/gkSource.h"
# include "gkSound.h"
#endif

#include "gkLogger.h"

void setGlobalVolume(float volume){
	#ifdef OGREKIT_OPENAL_SOUND
		if (gkSoundManager::getSingletonPtr()){
			gkSoundManager::getSingleton().setGlobalVolume(volume);
		}
	#endif
}

float getGlobalVolume(void){
#ifdef OGREKIT_OPENAL_SOUND
	if (gkSoundManager::getSingletonPtr()){
		return gkSoundManager::getSingleton().getGlobalVolume();
	}
	else {
		return -1.0f;
	}
#else
	return -1.0f;
#endif
}

bool gsSoundManager::loadSound(const gkString& soundFile,const gkString& soundName,const gkString& groupName)
{
#ifdef OGREKIT_OPENAL_SOUND
	return gkSoundManager::getSingleton().loadSound(soundFile,soundName,groupName);
#else
	return false;
#endif
}


gkSource* gsSoundManager::createSource(const gkString& soundName)
{
#ifdef OGREKIT_OPENAL_SOUND
	// check if we got a sound with that name in our SoundManager
	gkSound* snd = gkSoundManager::getSingleton().getByName<gkSound>(soundName);
	if(!snd) {
		gkDebugScreen::printTo("Unknown sound:"+soundName+"! Couldn't create source!");
		return 0;
	}

	// create a source from it and give it to the lua-layer
	gkSource* newSource = snd->createSource();
	return newSource;
#else
	return 0;
#endif
}

void gsSoundManager::destroySource(gsSource* source)
{
#ifdef OGREKIT_OPENAL_SOUND
	if (source)
	{
		source->m_source->getCreator()->destroySource(source->m_source);
		source->m_source=0;
	}
#endif
}

//#define CHECK_SOURCE\

gsSource::gsSource(gkSource* source) : m_source(source),
#ifdef OGREKIT_OPENAL_SOUND
		m_name(source->getCreator()->getName())
#else
		m_name("noname")
#endif
{}

gsSource::~gsSource()
{
#ifdef OGREKIT_OPENAL_SOUND
	if (!m_source)
		gkDebugScreen::printTo("Source:"+m_name+" already destroyed!");

	if (m_source)
	{
		gkLogger::write("destroyed source:"+m_source->getCreator()->getName(),true);
		m_source->getCreator()->destroySource(m_source);
		m_source = 0;
	}
#endif
}

#ifdef OGREKIT_OPENAL_SOUND
gkSoundProperties& gsSource::_getProperties()
{
	return m_source->getProperties();
}

void gsSource::_updateProperties()
{
	m_source->requestUpdateProperties();
}

void gsSource::play()
{
	m_source->play();
}

bool gsSource::isPlaying()
{
	return m_source->isPlaying() && !m_source->isPaused();
}

void gsSource::pause()
{
	m_source->pause();
}

bool gsSource::isPaused()
{
	return m_source->isPaused();
}

void gsSource::stop()
{
	m_source->stop();
}

float gsSource::getPitch()
{
	return m_source->getProperties().m_pitch;
}

void gsSource::setPitch(float pitch)
{
	m_source->getProperties().m_pitch=pitch;
	_updateProperties();
}

float gsSource::changePitch(float dtPitch)
{
	float newPitch = getPitch() + dtPitch;

	if (newPitch < 0)
		newPitch = 0;
	setPitch(newPitch);

	return newPitch;
}

float gsSource::getVolume()
{
	return m_source->getProperties().m_volume;
}

void gsSource::setVolume(float volume)
{
	m_source->getProperties().m_volume=volume;
	_updateProperties();
}

float gsSource::changeVolume(float dtVolume)
{
	float newVolume = gkClamp<float>(getVolume() + dtVolume,0,1);

	setVolume(newVolume);

	return newVolume;
}

void gsSource::destroy() {
	if (m_source)
	{
		m_source->getCreator()->destroySource(m_source);
		m_source=0;
	} else {
		gkDebugScreen::printTo(m_name+" already destroyed!");
	}
}

void gsSource::recreate() {
	if (!m_source)
	{
		gkSound* snd = gkSoundManager::getSingleton().getByName<gkSound>(m_name);

		if(!snd) {
			gkDebugScreen::printTo("Unknown sound:"+m_name+"! Couldn't create source!");
			return;
		}

		// create a source from it and give it to the lua-layer
		m_source = snd->createSource();
	}
}
#else
gkSoundProperties& gsSource::_getProperties()
{
	return m_fakeProps;
}

void gsSource::_updateProperties()
{
}

void gsSource::play()
{
}

bool gsSource::isPlaying()
{
	return 0;
}

void gsSource::pause()
{
}

bool gsSource::isPaused()
{
	return 0;
}

void gsSource::stop()
{
}

float gsSource::getPitch()
{
	return 0;
}

void gsSource::setPitch(float pitch)
{
}

float gsSource::changePitch(float dtPitch)
{
	return 0;
}

float gsSource::getVolume()
{
	return 0;
}

void gsSource::setVolume(float volume)
{
}

float gsSource::changeVolume(float dtVolume)
{
	return 0;
}

void gsSource::destroy() {
}

void gsSource::recreate() {

}
#endif
