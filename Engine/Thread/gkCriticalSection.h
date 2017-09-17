/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) Nestor Silveira.

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
#ifndef _gkCriticalSection_h_
#define _gkCriticalSection_h_

#include "gkNonCopyable.h"

#if (defined WIN32 && !__MINGW32__)
#include <windows.h>
#elif EMSCRIPTEN
	// TODO
#else
#include <pthread.h>
#include <semaphore.h>
#endif

class gkCriticalSection : gkNonCopyable
{
public:

	gkCriticalSection();

	~gkCriticalSection();

	class Lock
	{
	public:
		Lock(gkCriticalSection& obj);
		~Lock();

	private:
		gkCriticalSection& m_obj;
	};

	friend class Lock;

	void BeginLock();

	void EndLock();

private:

#if (defined WIN32 && !__MINGW32__)
	CRITICAL_SECTION m_cs;
#elif EMSCRIPTEN
	// TODO
#else
	pthread_mutex_t m_cs;
#endif
};

#endif//_gkCriticalSection_h_
