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
TODO, implement networking, experiment with enet and various other utilities
*/
#ifndef THRIFTMANAGER_H_
#define THRIFTMANAGER_H_

#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/transport/TSocket.h"
#include "thrift/transport/TTransport.h"
#include "thrift/protocol/TProtocol.h"
#include "thrift/transport/THttpClient.h"
#include "gkMessageManager.h"
#include "../../../gamekit-pendencies/thrift/lib/cpp/src/thrift/transport/TCurlClient.h"
//#include "thrift/protocol/TCompactProtocol.h"


using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;


//TODO: Check memory-leack
template <class T>
class HTTPClient : gkMessageManager::GenericMessageListener {
	public:
		HTTPClient(gkString host,int port,gkString path)
			: gkMessageManager::GenericMessageListener("",GK_MSG_GROUP_INTERNAL,GK_MSG_INTERNAL_GAME_PAUSE)
		      ,transport(new THttpClient(host,port,path)),
			  m_host(host),m_port(port),m_path(path),
			  protocol(new TBinaryProtocol(transport)),
			  client(protocol)
	{
			if (gkMessageManager::getSingletonPtr()) {
				gkMessageManager::getSingleton().addListener(this);
			}
	}

	HTTPClient(gkString url)
			: gkMessageManager::GenericMessageListener("",GK_MSG_GROUP_INTERNAL,GK_MSG_INTERNAL_GAME_PAUSE)
		      ,transport(new TCurlClient(url)),
			  m_host(""),m_port(0),m_path(""),
			  protocol(new TBinaryProtocol(transport)),
			  client(protocol)
	{
			if (gkMessageManager::getSingletonPtr()) {
				gkMessageManager::getSingleton().addListener(this);
			}
	}

	~HTTPClient() {
		transport->close();
	 }

	void connect() {

	}

	T& getClient() {
		if (!transport->isOpen())
			transport->open();
		return client;
	}

	void handleMessage(gkMessageManager::Message* message) {
		if (message->m_subject==GK_MSG_INTERNAL_GAME_PAUSE) {
			transport->close();
		}
	}

	void closeTransport() {
		if (transport)
			transport->close();
	}

	void refreshTransport() {
		if (!transport->isOpen())
			transport->open();
	}

	bool isOpen() {
		return transport->isOpen();
	}

	private:
		  gkString m_host;
		  gkString m_path;
		  int      m_port;

		  boost::shared_ptr<TTransport> transport;
		  boost::shared_ptr<TProtocol> protocol;
		  T client;
};
#endif
