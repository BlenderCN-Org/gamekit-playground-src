/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha (dertom)

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

#ifndef GKNETWORKSERVERTHREAD_H_
#define GKNETWORKSERVERTHREAD_H_

#include "OgreKit.h"
#include "Thread/gkThread.h"
#include "Network/gkNetworkManager.h"

class gkNetworkServer;

class gkNetworkServerThread : public gkCall {
public:
	gkNetworkServerThread();
	virtual ~gkNetworkServerThread();

	// start server-poll-thread. if no server is started, yet
	// a server is created listening on the specified port
	void start(UTuint16 pollTime,UTuint16 port=0);
	void stop();
	bool isRunning() { return m_isRunning;}
	void pause() { m_requestPause = true;	}
	void resume();

private:
	// the loop
	virtual void run();
	void finalize();

	UTuint16	        m_pollTime;
	UTuint16			m_port;

	bool m_isRunning, m_requestPause;
	gkSyncObj m_pauseSync,m_finishSync;
	gkCriticalSection   m_cs;

	gkThread*           m_thread;
	gkNetworkServer*	m_server;

};

#endif /* GKNETWORKSERVERTHREAD_H_ */
