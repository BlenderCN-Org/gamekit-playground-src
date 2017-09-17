/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira & Charlie C.

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
#ifndef _gkSource_h_
#define _gkSource_h_


#include "Thread/gkCriticalSection.h"
#include "gkCommon.h"
#include "gkSoundUtil.h"
#include "gkMathUtils.h"
#include "gkSerialize.h"

class gkBuffer;
class gkSound;
class gkGameObject;
class gkSoundStream;

class gkSource
{
private:
	gkBuffer*            m_playback;    // active playback buffer
	gkSoundProperties    m_props;       // properties to attach to the sound stream
	gkSound*             m_reference;   // reference sound object

	// Force internal usage.

	friend class gkBuffer;
	friend class gkStreamer;

	// Bind / unbind a buffer to this object.
	// This object is only playable when a buffer is attached.
	void bind(gkBuffer* buf);

	// Playback buffer.
	gkBuffer* getBuffer(void) {return m_playback;}

	// Playback stream.
	gkSoundStream* getStream(void);



	mutable gkCriticalSection m_cs;
public:

	gkSource(gkSound* sound);
	virtual ~gkSource();


	GK_INLINE bool                  isStopped(void)   const  {return !isPlaying() || isPaused(); }
	GK_INLINE bool                  isPlaying(void)   const  {return isBound();}
	GK_INLINE bool                  isBound(void)     const  {return m_playback != 0;}
	GK_INLINE bool                  isLooped(void)    const  {return m_props.m_loop;}
	GK_INLINE gkSoundProperties&    getProperties(void)      {return m_props;}
	GK_INLINE gkSound*              getCreator(void)         {return m_reference;}

	GK_INLINE void setProperties(gkSoundProperties& props)  {m_props = props;}

	// Object state for 3D sounds
	void updatePropsForObject(gkGameObject* obj);
	// e.g. volume or pitch has been changed
	void requestUpdateProperties(void);

	bool isPaused(void) const;

	// playing states
	void play(void);
	void pause(bool pause=true);
	void stop(void);
	void loop(bool v);

	void resetPlayable(void);
};


#endif//_gkSource_h_
