/*
 * gkNetworkServerThread.cpp
 *
 *  Created on: 12.04.2012
 *      Author: ttrocha
 */

#include "gkNetworkServerThread.h"
#include "gkLogger.h"

gkNetworkServerThread::gkNetworkServerThread() : m_isRunning(false),m_thread(0),m_server(0),m_requestPause(false),m_port(0),m_pollTime(0)
{
}

gkNetworkServerThread::~gkNetworkServerThread() {
	finalize();
}

void gkNetworkServerThread::finalize()
{
	if (m_thread){
		stop();
	}
}

void gkNetworkServerThread::start(UTuint16 pollTime,UTuint16 port)
{
	m_pollTime = pollTime;
	m_port=port;
	if (gkNetworkServer::getSingletonPtr())
	{
		gkLogger::write("Server already running... you have to start the server via this method...abort!");
		return;
	}

	gkCriticalSection::Lock lock(m_cs);
	if (!isRunning())
	{
		if (!m_thread)
		{
			m_thread = new gkThread(this);
		}
	}
}
void gkNetworkServerThread::stop(){
	gkCriticalSection::Lock lock(m_cs);
	if (isRunning())
	{
		m_isRunning = false;
		// wait until we left the loop
//		m_finishSync.wait();
		delete m_thread;
		m_thread=0;
		if (m_server)
		{
			delete m_server;
			m_server=0;
		}
	}
}


void gkNetworkServerThread::resume()
{
	m_pauseSync.signal();
}

void gkNetworkServerThread::run(){
	m_isRunning = true;
	m_requestPause=false;
	while (m_isRunning)
	{
		if (!m_server)
		{
			m_server = new gkNetworkServer;
			m_server->start(m_port);
		}
		m_server->poll(m_pollTime);
		//gkLogger::write("poll server....",true);
		if (m_requestPause){
			gkLogger::write("pause server...");
			m_pauseSync.wait();
			gkLogger::write("resume server...");
		}
	}
	// tell that we left the loop
	m_finishSync.signal();
}

