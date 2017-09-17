/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha(dertom)

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

#ifndef _gsSound_h_
#define _gsSound_h_
#include "gkString.h"
#include "gkSerialize.h"


extern void setGlobalVolume(float volume);

extern float getGlobalVolume(void);

class gsSource;
class gkSource;

class gsSoundManager
{
public:
	gsSoundManager(){};
	~gsSoundManager(){};
	// TODO: Check if this should be set as %newobject in gsObject.i (swig related)
	bool loadSound(const gkString& soundFile,const gkString& soundName="",const gkString& groupName="");
	gkSource* createSource(const gkString& soundName);
	void destroySource(gsSource* source);
};


class gsSource
{
	friend class gsSoundManager;

public:
	gsSource(gkSource* source);
	~gsSource();

	void play();
	bool isPlaying();
	void pause();
	bool isPaused();
	void stop();

	float getPitch();
	void setPitch(float pitch);
	float changePitch(float delta);

	float getVolume();
	void setVolume(float volume);
	float changeVolume(float delta);

	void destroy();
	void recreate();

	// receive internal properties. use updateProperties to apply them
	gkSoundProperties& _getProperties();
	// if changing sound-properties apply them with that call
	void _updateProperties();

private:
	gkSource* m_source;
	gkString m_name;
#ifndef OGREKIT_OPENAL_SOUND
	gkSoundProperties m_fakeProps;
#endif
};

#endif
