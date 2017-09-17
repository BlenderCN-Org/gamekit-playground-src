/*
 * ttScript.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: ttrocha
 */

#include "script/ttScript.h"
#include "gkLogger.h"

gkString ttSuperTest::tester(gamedb::PUser* person) {
	gkLogger::write("The name is:"+person->username(),true);
	return person->username();
}

ttUser* ttServer::createUser(const gkString& username, const gkString& password, const gkString& email)
{
	return m_userDB.createUser(username,password,email);
}

ttUser* ttServer::getUser(const gkString& username)
{
	return m_userDB.getUser(username);
}

ttUser* ttServer::login(const gkString& username, const gkString& password)
{
	ttUser* user = m_userDB.login(username,password);
	if (user)
		m_loginUser = user;
	return user;
}

ttUser* ttServer::getLoginUser() {
	return m_loginUser;
}

void ttServer::registerLoginPlayerToGame(const gkString& gameName)
{
	m_userDB.registerLoginPlayerToGame(gameName);
}
