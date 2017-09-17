/*
 * GameServer.h
 *
 *  Created on: Apr 11, 2013
 *      Author: ttrocha
 */

#ifndef GAMESERVER_H_
#define GAMESERVER_H_
#include "test.pb.h"
#include "gkString.h"
#include "Network/gkDownloadManager.h"

using namespace gamedb;

class GameServer {
public:
	GameServer(){};
	virtual ~GameServer(){};
};

class UserDB {
public:
	UserDB(const gkString& restURL = gkString("http://localhost:8080/GameServer/rest"))
		: m_session(0), m_restURL(restURL)
	{
		m_session = gkNetClientManager::getSingleton().createHttpSession("__gameserver__");
	};
	virtual ~UserDB(){};

	PUser* createUser(const gkString& username, const gkString& password, const gkString& email);
	PUser* getUser(const gkString& username);
	PUser* login(const gkString& username, const gkString& password);
	void   registerLoginPlayerToGame(const gkString& gameName);
private:
	gkHttpSession* m_session;
	gkString m_restURL;
};

class GameDB {
public:
	GameDB(){};
	virtual ~GameDB(){};
};

#endif /* GAMESERVER_H_ */
