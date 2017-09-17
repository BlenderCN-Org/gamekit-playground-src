/*
 * GameServer.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: ttrocha
 */

#include "GameServer.h"



PUser* UserDB::createUser(const gkString& username, const gkString& password, const gkString& email){
	utMemoryStream* stream = (utMemoryStream*)m_session->getURL(m_restURL+"/user/create/"+username+"/"+password+"/"+email);
	PUser* user = new PUser;
	if (user->ParseFromArray(stream->ptr(),stream->size()))
		return user;
	delete user;
	delete stream;
	return 0;
}

PUser* UserDB::getUser(const gkString& username) {
	utMemoryStream* stream = (utMemoryStream*)m_session->getURL(m_restURL+"/user/"+username);
	PUser* user = new PUser;
	if (user->ParseFromArray(stream->ptr(),stream->size()))
		return user;
	delete user;
	delete stream;
	return 0;
}

PUser* UserDB::login(const gkString& username, const gkString& password) {
	utMemoryStream* stream = (utMemoryStream*)m_session->getURL(m_restURL+"/user/login/"+username+"/"+password);
	PUser* user = new PUser;
	if (user->ParseFromArray(stream->ptr(),stream->size()))
		return user;
	delete user;
	delete stream;
	return 0;
}

void UserDB::registerLoginPlayerToGame(const gkString& gameName) {
	utMemoryStream* stream = (utMemoryStream*)m_session->getURL(m_restURL+"/game/"+gameName+"/register");
	delete stream;
}
