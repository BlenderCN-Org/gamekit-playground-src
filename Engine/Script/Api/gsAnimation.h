#ifndef TTANIMATIONSCRIPT_H_
#define TTANIMATIONSCRIPT_H_

#include <External/Ogre/gkOgreVideoBackground.h>
#include "../../gkLogger.h"
#include "../../Animation/gkAnimation.h"
#include "gsCommon.h"
#include "../Lua/gkLuaUtils.h"


/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2013 Thomas Trocha

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

#ifdef OGREKIT_USE_THEORA
# include "TheoraVideoClip.h"
# include "OgreVideoManager.h"

typedef TheoraVideoClip gkVideoTexture;


class gsVideoTexture
{
private:
	gkVideoTexture* m_vtex;
public:
	gsVideoTexture(gkVideoTexture* vtex) : m_vtex(vtex){};

	void seekToFrame(int frame) { m_vtex->seekToFrame(frame);}
	void update(float dt) { m_vtex->update(dt);}
	float updateToNextFrame(bool withAudio=false)
	{
		if (withAudio)
			m_vtex->decodedAudioCheck();
		return m_vtex->updateToNextFrame();
	}
	float getDuration() { return m_vtex->getDuration();}
	void setLoop(bool loop) { m_vtex->setAutoRestart(loop);}
	int getWidth() { return m_vtex->getWidth();}
	int getHeight() { return m_vtex->getHeight();}
	float getTimePosition() { return m_vtex->getTimePosition();}
	float getSpeed() { return m_vtex->getPlaybackSpeed();}
	void setSpeed(float speed) { m_vtex->setPlaybackSpeed(speed);}
	int getNumFrames() { return m_vtex->getNumFrames();}
	int getFPS() { return m_vtex->getFPS();}
	void pause() { m_vtex->pause();}
	void play() { m_vtex->play();}
	void restart() {m_vtex->restart();}
	bool isDone() { return m_vtex->isDone();}
	bool isPaused() { return m_vtex->isPaused();}
	void setAutoupdate(bool b) { m_vtex->setAutoUpdate(b);}

};


class gsVideoManager
{
public:
	gkVideoTexture* getVideoTexture(const gkString& name)
	{
		Ogre::OgreVideoManager* vM = static_cast<Ogre::OgreVideoManager*>(Ogre::OgreVideoManager::getSingletonPtr());
		if (vM)
		{
			gkVideoTexture* vtex = vM->getVideoClipByName(name);
			return vtex;
		}
		return 0;
	}

	void setBackgroundVideo(const gkString& material)
	{
		gkVideoBackground::getSingleton().setMaterial(material);
	}
};


#endif

class gsAnimationPlayer
{
public:

	gsAnimationPlayer(gkAnimationPlayer* player)
		: m_player(player)
	{
		m_animFps = 1 / gkEngine::getSingleton().getUserDefs().animFps;
	}


	int getFrame(void) {
		return (int)(m_player->getTimePosition() / m_animFps);
	}

	void setFrame(int frame)
	{
		m_player->setTimePosition(frame * m_animFps);
	}

	float getTimePosition(void) {
		return m_player->getTimePosition();
	}

	void setTimePosition(float time) {
		m_player->setTimePosition(time);
	}

	int getMode(void) {
		return m_player->getMode();
	}

	void setMode(int mode) {
		m_player->setMode(mode);
	}

	float getSpeedFactor() {
		return m_player->getSpeedFactor();
	}

	void setSpeedFactor(float speed) {
		m_player->setSpeedFactor(speed);
	}

	bool isDone() {
		return m_player->isDone();
	}

	bool isEnabled() {
		return m_player->isEnabled();
	}

	void setEnabled(bool enable) {
		return m_player->enable(enable);
	}

	void evaluate(float tick) {
		m_player->evaluate(tick);
	}

	void setWeight(float weight)
	{
		m_player->setWeight(weight);
	}

	float getWeight()
	{
		return m_player->getWeight();
	}

	void reset() {
		m_player->reset();
	}

	gkAnimationPlayer* get() { return m_player; }

	enum gsAnimationPlayerEventType {
		onStart, onEnd, onLoopEnd, onTrigger, onDestruction
	};

	class gsAnimationEvent : public gkAnimationPlayer::akAnimationPlayerEvent {
	public:
		gsAnimationEvent(akAnimationPlayer* player, gsSelf self,gsFunction func)
			: gkAnimationPlayer::akAnimationPlayerEvent(player), evt(self,func) {}

		virtual void onStart(akAnimationPlayer* m_player);
		virtual void onEnd(akAnimationPlayer* m_player);
		virtual void onLoopEnd(akAnimationPlayer* m_player);
		virtual void onTrigger(akAnimationPlayer* m_player, int type);
		virtual bool onDestruction(akAnimationPlayer* m_player);
	private:
		gkLuaEvent evt;
	};

	void addEventListener(gsSelf self,gsFunction func)
	{
		m_player->addAnimationEvent(new gsAnimationEvent(m_player,self,func));
	}

	void clearEvents()
	{
		m_player->clearEvents();
	}

	void addTrigger(float time, int type) {
		m_player->addTrigger(time,type);
	}

private:
	gkAnimationPlayer* m_player;
	float m_animFps;
};
#endif
