/*
 * ttScript.h
 *
 *  Created on: Apr 3, 2013
 *      Author: ttrocha
 */

#ifndef TTSCRIPT_H_
#define TTSCRIPT_H_

#include "test.pb.h"
#include "utStreams.h"
#include "gkString.h"
#include "GameServer.h"

typedef gamedb::PUser ttUser;

class ttSuperTest {
public:
	int add(int a,int b) { return a+b;}

	gamedb::PUser* toUser(utMemoryStream* stream,bool dispose=false) {
//		utMemoryStream* stream = ((utMemoryStream*)v);

		gamedb::PUser* user = new gamedb::PUser;
		user->ParseFromArray(stream->ptr(),stream->size());

		if (dispose)
			delete stream;
		return user;
	}

	gkString tester(gamedb::PUser* person);

};



class ttServer {
public:
	ttServer(const gkString& restURL)
		: m_userDB(restURL),m_loginUser(0),m_restURL(restURL)
	{}

	ttUser* createUser(const gkString& username, const gkString& password, const gkString& email);
	ttUser* getUser(const gkString& username);
	ttUser* login(const gkString& username, const gkString& password);
	ttUser* getLoginUser();
	void   registerLoginPlayerToGame(const gkString& gameName);
private:
	UserDB   m_userDB;
	GameDB   m_gameDB;
	ttUser*  m_loginUser;
	gkString m_restURL;
};



class tsUser {
public:
	tsUser(ttUser* user) : m_user(user){};

	gkString getUsername() {
		return m_user->username();
	}
	gkString getPassword() {
		return m_user->password();
	}
	gkString getEmail() {
		return m_user->email();
	}
private:
	ttUser* m_user;
};

#endif /* TTSCRIPT_H_ */
