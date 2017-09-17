/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha (dertom).

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

#ifndef _gkNetworkManager_h_
#define _gkNetworkManager_h_

#include "OgreKit.h"
#include "gkSettings.h"
#include "utSingleton.h"

class gkNetworkServerThread;

struct gkNetworkListener
{
    gkNetworkListener(){};
	virtual ~gkNetworkListener(){};
//	virtual void onStarted();
//	virtual void onClosed();
	virtual void onData(void* data,int dataSize)=0;
};


#if (OGREKIT_NETWORK_ENET_SUPPORT)
#include "enet/enet.h"
#include "gkNetworkServerThread.h"
#endif

typedef utList<gkNetworkListener*> NetworkListenerList;


class gkNetworkServer : public utSingleton<gkNetworkServer>{
public:
	UT_DECLARE_SINGLETON(gkNetworkServer);
	gkNetworkServer();
	~gkNetworkServer();
	void start(UTuint16 port);
	void broadcast(char channel,bool reliable,void* data, int dataSize);
	void poll(UTuint16 pollTime);
	void stop();
	void addListener(gkNetworkListener* listener);
	bool isRunning();
private:
	NetworkListenerList m_serverListener;

#if (OGREKIT_NETWORK_ENET_SUPPORT)
	ENetAddress m_serverConnectionAddress;
	ENetHost* m_serverHost;
#endif
};

class gkNetworkClient : public utSingleton<gkNetworkClient>{
public:
	UT_DECLARE_SINGLETON(gkNetworkClient);
	gkNetworkClient();
	~gkNetworkClient();
	void start(gkString address, UTuint16 port);
	void sendToServer(char channel,bool reliable,void* data, int dataSize);
	void poll(UTuint16 pollTime);
	void stop();
	void addListener(gkNetworkListener* listener);
	bool isRunning();
private:
	NetworkListenerList m_clientListener;
#if (OGREKIT_NETWORK_ENET_SUPPORT)
	ENetAddress m_clientConnectionAddress;
	ENetEvent m_event;
	ENetHost* m_clientHost;
	ENetPeer* m_connectedServerPeer;
#endif
};

class gkNetworkManager : public utSingleton<gkNetworkManager>
{
	UT_DECLARE_SINGLETON(gkNetworkManager);
public:
	gkNetworkManager();
	~gkNetworkManager();
	void startServerPollThread(UTint16 port,UTint16 pollTime);
	void stopServerPollThread(void);
	bool hasServerPollThread(void);
	void createServer();
	void createClient();
private:

	gkNetworkServerThread* m_serverPollThread;


};


#endif

