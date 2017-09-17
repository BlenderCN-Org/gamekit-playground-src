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
#include "Network/gkNetworkManager.h"

UT_IMPLEMENT_SINGLETON(gkNetworkServer);

	gkNetworkServer::gkNetworkServer() : m_serverHost(0){};

	gkNetworkServer::~gkNetworkServer()
	{
		stop();
		m_singleton=0;
	}

	void gkNetworkServer::start(UTuint16 port)
	{
		  if (enet_initialize () != 0)
		    {
		        fprintf (stderr, "An error occurred while initializing ENet.\n");
		        return;
		    }
		    atexit (enet_deinitialize);

		m_serverConnectionAddress.host=ENET_HOST_ANY;

		m_serverConnectionAddress.port = port;
		m_serverHost = enet_host_create (& m_serverConnectionAddress /* the address to bind the server host to */,
		                                 32      /* allow up to 32 clients and/or outgoing connections */,
		                                  2      /* allow up to 2 channels to be used, 0 and 1 */,
		                                  0      /* assume any amount of incoming bandwidth */,
		                                  0      /* assume any amount of outgoing bandwidth */);

		if (m_serverHost == NULL)
		{
			printf("mist!\n");
			fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
			exit (EXIT_FAILURE);
		} else {
			printf("Server ONLINE!");
		}
	}

	void gkNetworkServer::broadcast(char channel,bool reliable,void* data, int dataSize)
	{
	    ENetPacket* packet = enet_packet_create (data,dataSize,
	                                                reliable?ENET_PACKET_FLAG_RELIABLE:0);
		/* Send the packet to the peer over channel id 0. */
	    /* One could also broadcast the packet by         */
	    enet_host_broadcast (m_serverHost, channel, packet);
	    enet_host_flush (m_serverHost);
	}

	void gkNetworkServer::poll(UTuint16 pollTime)
	{
		ENetEvent event;
//		printf("poll-server...\n");
		fflush(stdout);
	    if (enet_host_service (m_serverHost, &event, pollTime) > 0)
	    {
	    	switch (event.type)
	        {
	        case ENET_EVENT_TYPE_CONNECT:
//	            printf ("A new client connected from %x:%u.\n",
//	                    event.peer -> address.host,
//	                    event.peer -> address.port);
	            /* Store any relevant client information here. */
	            event.peer -> data = (void*)"Client information";


	            break;

	        case ENET_EVENT_TYPE_RECEIVE:
//	            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
//	                    event.packet -> dataLength,
//	                    event.packet -> data,
//	                    event.peer -> data,
//	                    event.channelID);
	            fflush(stdout);

	    		UTsize i;
	    		for (i = 0; i < m_serverListener.size(); ++i){
	    			m_serverListener.at(i)->onData(event.packet->data,event.packet->dataLength);
	    		}
	    		enet_host_broadcast(m_serverHost,0,event.packet);
	            break;

	        case ENET_EVENT_TYPE_DISCONNECT:
	            printf ("%s disconected.\n", event.peer -> data);

	            /* Reset the peer's client information. */

	            event.peer -> data = NULL;
	            break;
	        }
	    }
	}
	void gkNetworkServer::stop()
	{
		if (m_serverHost){
			enet_host_destroy(m_serverHost);
			m_serverHost=0;
		}
	}
	void gkNetworkServer::addListener(gkNetworkListener* listener)
	{
		m_serverListener.push_back(listener);
	}
	bool gkNetworkServer::isRunning()
	{
		return m_serverHost!=NULL;
	}


//---------------------------------------------------------
	UT_IMPLEMENT_SINGLETON(gkNetworkClient);

	gkNetworkClient::gkNetworkClient() : m_clientHost(0),m_connectedServerPeer(0){};
	gkNetworkClient::~gkNetworkClient()
	{
		stop();
		m_singleton=0;
	};

	void gkNetworkClient::start(gkString bindAddress, UTuint16 port)
	{
		  if (enet_initialize () != 0)
		    {
		        fprintf (stderr, "An error occurred while initializing ENet.\n");
		        return;
		    }
		    atexit (enet_deinitialize);
		ENetEvent event;

		m_clientHost = enet_host_create (NULL /* create a client host */,
	                1 /* only allow 1 outgoing connection */,
	                2 /* allow up 2 channels to be used, 0 and 1 */,
	                57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
	                14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

	    if (m_clientHost == NULL)
	    {
	        fprintf (stderr,
	                 "An error occurred while trying to create an ENet client host.\n");
	        exit (EXIT_FAILURE);
	    }

		enet_address_set_host(&m_clientConnectionAddress, bindAddress.c_str());
		m_clientConnectionAddress.port = port;
	    /* Initiate the connection, allocating the two channels 0 and 1. */
	    m_connectedServerPeer = enet_host_connect (m_clientHost, & m_clientConnectionAddress, 2, 0);

	    if (m_connectedServerPeer == NULL)
	    {
	       fprintf (stderr,
	                "No available peers for initiating an ENet connection.\n");
	       exit (EXIT_FAILURE);
	    }

	    if (enet_host_service (m_clientHost, &event, 5000) > 0 &&
	    		event.type == ENET_EVENT_TYPE_CONNECT)
	     {
	     	printf("Connection to some.server.net:1234 succeeded.");
	     	fflush(stdout);
	 //    	ostringstream out;
	 //    	char type = 1;
	 //    	out << "tact";
	 //
	 //    	string st = out.str();
	 //    	send(0,&st.c_str(),st.size(),true);
	 //    	send(0,&t,size,true);



	//     	MSGGameTime gt;
	//     	gt.type=NMSG_GAMETIME;
	//     	m_networkStart = time_ms();
	//     	send(0,&gt,sizeof(gt),true);

	//     	fflush(stdout);
	     }
	     else
	     {
	         /* Either the 5 seconds are up or a disconnect event was */
	         /* received. Reset the peer in the event the 5 seconds   */
	         /* had run out without any significant event.            */
	         enet_peer_reset (m_connectedServerPeer);

	         puts ("Connection to some.server.net:1234 failed.");
	     }

	}
	void gkNetworkClient::sendToServer(char channel,bool reliable,void* data, int dataSize)
	{
		if (m_connectedServerPeer)
		{
			ENetPacket* packet = enet_packet_create (data,dataSize,
		                                                reliable?ENET_PACKET_FLAG_RELIABLE:0);
		    enet_peer_send(m_connectedServerPeer,channel,packet);
		    enet_host_flush (m_clientHost);
			printf("Sent data!");
		} else {
			printf("Tried to send data to server, but you are not connected!");
		}
	}
	void gkNetworkClient::poll(UTuint16 pollTime)
	{
		ENetEvent event;
	//	printf("poll-client...\n");
		fflush(stdout);
	    if (enet_host_service (m_clientHost, &event, pollTime) > 0)
	    {
	    	switch (event.type)
	        {
	        case ENET_EVENT_TYPE_CONNECT:
	            printf ("A new client connected from %x:%u.\n",
	                    event.peer -> address.host,
	                    event.peer -> address.port);
	            /* Store any relevant client information here. */
	            event.peer -> data = (void*)"Client information";


	            break;

	        case ENET_EVENT_TYPE_RECEIVE:
	            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
	                    event.packet -> dataLength,
	                    event.packet -> data,
	                    event.peer -> data,
	                    event.channelID);


	    		UTsize i;
	    		for (i = 0; i < m_clientListener.size(); ++i){
	    			m_clientListener.at(i)->onData(event.packet->data,event.packet->dataLength);
	    		}

	            break;

	        case ENET_EVENT_TYPE_DISCONNECT:
	            printf ("%s disconected.\n", event.peer -> data);

	            /* Reset the peer's client information. */

	            event.peer -> data = NULL;
	            break;
	        }
	    }
	}
	void gkNetworkClient::stop()
	{
		if (m_clientHost){
			enet_host_destroy(m_clientHost);
			m_clientHost=0;
		}
	}
	void gkNetworkClient::addListener(gkNetworkListener* listener)
	{
		m_clientListener.push_back(listener);
	}
	bool gkNetworkClient::isRunning()
	{
		return m_clientHost!=0;
	}


//----------------------------------------------------------





	UT_IMPLEMENT_SINGLETON(gkNetworkManager);

gkNetworkManager::gkNetworkManager():m_serverPollThread(0)
{
}

gkNetworkManager::~gkNetworkManager() {
	stopServerPollThread();
	if (gkNetworkServer::getSingletonPtr())
	{
		delete gkNetworkServer::getSingletonPtr();
	}
	if (gkNetworkClient::getSingletonPtr())
	{
		delete gkNetworkClient::getSingletonPtr();
	}
}

void gkNetworkManager::startServerPollThread(UTint16 port,UTint16 pollTime)
{
	if (gkNetworkServer::getSingletonPtr()){
		gkLogger::write("SERVER-ERROR: There is already a server! Abort...");
		return;
	}

	if (!m_serverPollThread)
	{
		m_serverPollThread = new gkNetworkServerThread;
		m_serverPollThread->start(pollTime,port);
	}
	else
	{
		gkLogger::write("SERVER-ERROR: Tried to start another server-poll-thread, but only one at a time is allowed...!");
	}
}

void gkNetworkManager::stopServerPollThread(void)
{
	if (m_serverPollThread)
	{
		delete m_serverPollThread;
		m_serverPollThread=NULL;
	}
}

bool gkNetworkManager::hasServerPollThread()
{
	return m_serverPollThread!=0;
}



void gkNetworkManager::createServer(){
	if (!gkNetworkServer::getSingletonPtr()){
		new gkNetworkServer;
	} else {
		gkLogger::write("Warning: there is already a network-server! Ignoring....");
		return;
	}
}
void gkNetworkManager::createClient() {
	if (!gkNetworkClient::getSingletonPtr()){
		new gkNetworkClient;
	} else {
		gkLogger::write("Warning: there is already a network-server! Ignoring....");
		return;
	}
}



