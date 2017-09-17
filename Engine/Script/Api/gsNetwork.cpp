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

#include "gsNetwork.h"

#ifdef EMSCRIPTEN
# include <emscripten.h>
#endif

#include "Network/gkMessageNetForwarder.h"

void startNetServer(unsigned short port){
	if (!gkNetworkManager::getSingleton().hasServerPollThread())
	{
		gkNetworkManager::getSingleton().startServerPollThread(port,100);
	} else {
		gkLogger::write("Tried to start server(via lua), but there is already a server running!!",true);
	}
}
void stopNetServer(){
	if (gkNetworkManager::getSingleton().hasServerPollThread())
	{
		gkNetworkManager::getSingleton().stopServerPollThread();
	} else {
		gkLogger::write("Tried to stop server(via lua), but there is no server running!!",true);
	}
}

bool isServerRunning(){
	return gkNetworkManager::getSingleton().hasServerPollThread();
}

void startNetClient(gkString serverAddress,unsigned short port){
	if (!gkNetworkClient::getSingletonPtr())
	{
		new gkNetworkClient; // I make it like this to underline that the client is accessable via singleton
		gkNetworkClient::getSingleton().start(serverAddress,port);
		gkMessageNetForwarder* netf = new gkMessageNetForwarder;
		gkMessageManager::getSingleton().addListener(netf);
		gkNetworkClient::getSingleton().addListener(netf);

	} else {
		gkLogger::write("Tried to start client(via lua), but there is already a client running!!",true);
	}

}
void stopNetClient(){
	if (gkNetworkClient::getSingletonPtr())
	{
		gkNetworkClient::getSingleton().stop();
	} else {
		gkLogger::write("Tried to stop client(via lua), but there is no client running!!",true);
	}
}
bool isClientRunning(){
	return gkNetworkClient::getSingletonPtr();
}

void onSocketOpen(int fd, void* userData)
{
	if (socketCallback)
	{
		socketCallback->beginCall();
		socketCallback->addArgument(gkString("open"));
		socketCallback->call();
	}
}
void onSocketMessage(int fd, void* userData)
{
	if (socketCallback)
	{
		socketCallback->beginCall();
		socketCallback->addArgument(gkString("message"));
		socketCallback->addArgument(gkString((char*)userData));
		socketCallback->call();
	}
}

void onSocketError(int fd, int err, const char* msg, void* userData)
{
	if (socketCallback)
	{
		socketCallback->beginCall();
		socketCallback->addArgument(gkString("error"));
		socketCallback->addArgument(gkString(msg));
		socketCallback->addArgument(gkString((char*)userData));
		socketCallback->call();
	}
}

void initCallbacks()
{
#ifdef EMSCRIPTEN
   emscripten_set_socket_open_callback(0, onSocketOpen);
   emscripten_set_socket_error_callback(0, onSocketError);
   emscripten_set_socket_message_callback(0, onSocketMessage);
#endif
}



// TODO: THIS IS JUST FOR TESTING! LEAK!!!
extern void setSocketCallback(gsFunction func)
{
	if (socketCallback) {
		delete socketCallback;
	}

	socketCallback = new gkLuaEvent(func);
}



