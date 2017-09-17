/*
 * ttScript.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: ttrocha
 */

#include "ttScript.h"
#include "../ThriftAddon.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "../../../Engine/Script/Api/gsCore.h"
#include "../../../Engine/gkCrypt.h"
#include "../generated/gen-cpp/gameserver_constants.h"


using namespace gs;
using namespace boost;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace apache::thrift::server;



Thrift::Thrift(const gkString& server,int port) :
		socket(new TSocket(server.c_str(), port)),
		transport( new TBufferedTransport(socket)),
		protocol(new TBinaryProtocol(transport)),
		connected(false),
		type(SOCKET),
		httpConnection(0),
		m_errorCallback(0)
{
	client = new ThriftService(protocol);
	transport->open();
	connected = true;

}

Thrift::Thrift(const gkString& server, const gkString& path, int port) : type(HTTP), connected(true), client(0),m_errorCallback(0)
{
	httpConnection = new HTTPClient<gs::TGameServerServiceClient>(server,port,path);
	httpConnection->getClient();
}

Thrift::Thrift(const gkString& url) : type(HTTP), connected(true), client(0),m_errorCallback(0)
{
	httpConnection = new HTTPClient<gs::TGameServerServiceClient>(url);
	httpConnection->getClient();
}

Thrift::~Thrift() {
	if (type==SOCKET) {
		disconnect();
		if (client)
			delete client;
	}
	else if (type==HTTP) {
		httpConnection->closeTransport();
		//TODO check if that is enough and if the client is disposed as well in the conneciton
		delete httpConnection;
		client = 0;
		httpConnection = 0;
	}

	if (m_errorCallback) {
		delete m_errorCallback;
		m_errorCallback = 0;
	}
}

void Thrift::disconnect() {
	if (connected) {
		socket->close();
		transport->close();
		connected = false;
	}
}


ThriftService* Thrift::getService(){
	if (type==HTTP) {
		ThriftService* service = 0;
		try {
			service = (ThriftService*)&httpConnection->getClient();
		} catch(...) {
			gkLogger::write("BangBangBang :(",true);
		}

		return service;
	} else {
		if (connected) {
			return client;
		}
	}

	gkDebugScreen::printTo("Tried to get service, but it is disconnected already!");
	return 0;

}

void Thrift::refreshConnection() {
	if (type==HTTP) {
		httpConnection->refreshTransport();
	}
}

using namespace gs;



ThriftService* getThriftService()  {
	return (ThriftService*)&ThriftAddon::thriftclient->getClient();
}

#define TO_INT(x)     *((int*)x)
#define TO_DOUBLE(x)    *(double*)x
#define TO_BOOL(x)    *(bool*)x
#define TO_STRING(x)  *((std::string*)x )
#define TO_USER(x)    *(gs::TUser*)x

#define CATCHBLOCK(NAME)\
catch (TErrorException& et) {\
	   gkPreparedLuaEvent* evt = m_thrift->getExceptionCallback();\
	   if (evt) {\
		   evt = evt->clone(); \
		   evt->addArgument(gkString(NAME));\
		   evt->addArgument((int)et.error_code);\
		   evt->addArgument(new gkString(et.error_msg));\
		   gkLuaManager::getSingleton().addPreparedLuaEvent(evt);\
	   }\
}\
catch (TException& t) {\
	   gkPreparedLuaEvent* evt = m_thrift->getErrorCallback();\
	   if (evt) {\
		   evt = evt->clone(); \
		   evt->addArgument(gkString(NAME));\
		   evt->addArgument(gkString(t.what()));\
		   gkLuaManager::getSingleton().addPreparedLuaEvent(evt);\
	   }\
	   gkLogger::write(gkString(NAME)+"-Exception! "+gkString(t.what()),true);\
}


void AsyncCaller::run(){
		m_thrift->refreshConnection();

	   if (m_function == PING) {
		   try {
			   m_service->ping();
	//		   gsEngine::getSingleton().__blockTick();
			   m_luaEvent->addArgument(gkString("PING"));
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   catch (TErrorException& et) {
			   gkPreparedLuaEvent* evt = m_thrift->getExceptionCallback();
			   if (evt) {
				   evt = evt->clone(); // replace evt with its clone
				   evt->addArgument(gkString("PING"));
				   evt->addArgument((int)et.error_code);
				   evt->addArgument(new gkString(et.error_msg));
				   gkLuaManager::getSingleton().addPreparedLuaEvent(evt);
			   }
		   }
		   catch (TException& t) {
			   gkPreparedLuaEvent* evt = m_thrift->getErrorCallback();
			   if (evt) {
				   evt = evt->clone(); // replace evt with its clone
				   evt->addArgument(gkString("PING"));
				   evt->addArgument(gkString(t.what()));
				   gkLuaManager::getSingleton().addPreparedLuaEvent(evt);
			   }
			   gkLogger::write("Ping-Exception! "+gkString(t.what()),true);
		   }
	   }
	   else if(m_function == CREATE_GAME) {
		   try{
			   m_service->create_game(TO_STRING(var1),TO_INT(var2),TO_BOOL(var3));

	//		   gsEngine::getSingleton().__blockTick();
			   m_luaEvent->addArgument(true);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("CREATE_GAME")

		   delete (std::string*)var1;
		   delete (int*)var2;
		   delete (bool*)var3;
	   }
// supported for thrift at the moment, but leave it in for some unrational reason
	   else if(m_function == ADD_USER) {
		   try {
			   int result = m_service->add_user(TO_USER(var1));
	//		   gsEngine::getSingleton().__blockTick();
			   // TODO: Check if swig/lua should really be responsible for that
			   m_luaEvent->addArgument(result);
			   m_luaEvent->addArgument("gs::TUser*",var1,1);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);

		   }
		   CATCHBLOCK("ADD_USER")

	   }
	   else if(m_function == ADD_SCORE) {
		   try {
			   std::vector<gs::TAddScoreResponse> response;
			   double score = TO_DOUBLE(var2);
			   int modeNr = TO_INT(var1);
			   m_service->add_score(response,modeNr,score);
	//		   gsEngine::getSingleton().__blockTick();

			   m_luaEvent->addArgument(modeNr);
			   m_luaEvent->addArgument((float)score);
			   for (int i=0;i<response.size();i++) {
				   gs::TAddScoreResponse& resp = response[i];
				   m_luaEvent->addArgument(resp.os);
				   m_luaEvent->addArgument(resp.pos);
				   m_luaEvent->addArgument(resp.result_code);
			   }
//			   m_luaEvent->addArgument("_p_std__vectorT_gs__TAddScoreResponse_t",response,1);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("ADD_SCORE")

		   delete (int*)var1;
		   delete (double*)var2;

	   }
	   else if (m_function == LOGIN_USER) {
		   try {
			   gkLogger::write("login_user start",true);
			   gs::TLoginResponse* result = new gs::TLoginResponse();
			   gkLogger::write("login_user before",true);

			   // request salt
			   std::string salt;

			   m_service->request_logintoken(salt);

			   gkLogger::write("GOT Salt:"+salt,true);

			   gkString plainPassword(TO_STRING(var2));
			   gkString md5Password(gkCrypt::md5(plainPassword));
			   gkString md5PasswdPlusSalt(gkCrypt::md5(md5Password + salt));

			   gkLogger::write("GOT Salt:"+plainPassword+" => "+md5Password+" =("+salt+")=> "+md5PasswdPlusSalt,true);

			   m_service->login_user(*result,TO_STRING(var1),md5PasswdPlusSalt);
			   gkLogger::write("login_user after",true);

			   m_luaEvent->addArgument("gs::TLoginResponse*",result,1);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("LOGIN_USER")

		   delete (std::string*) var1;
		   delete (std::string*) var2;


	   }
	   else if (m_function == GET_HIGHSCORES) {
		   try {
			   std::vector<gs::TScore>* result = new std::vector<gs::TScore>();
			   int mode = TO_INT(var1);
			   gkLogger::write("GET HIGHSCORE MODE:"+gkToString(mode));
			   int amount = TO_INT(var2);
			   int offset = TO_INT(var3);
			   std::string datasourceTable = TO_STRING(var4);

			   m_service->get_highscores(*result,mode,amount,offset);

			   m_luaEvent->addArgument("ScoresVec*",result,1);
			   m_luaEvent->addArgument(mode);
			   m_luaEvent->addArgument(offset);
			   m_luaEvent->addArgument(amount);
			   m_luaEvent->addArgument(datasourceTable);

			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);

		   }
		   CATCHBLOCK("GET_HIGHSCORES")

		   delete (int*) var1;
		   delete (int*) var2;
		   delete (int*) var3;
		   delete (std::string*) var4;
	   }
	   else if (m_function == GET_PLAYER_SCORES) {
		   try {
			   int mode = TO_INT(var1);
			   int amount = TO_INT(var2);
			   int offset = TO_INT(var3);
			   std::string datasourceTable = TO_STRING(var4);

			   std::vector<gs::TScore>* result = new std::vector<gs::TScore>();
			   m_service->get_player_scores(*result,mode,amount,offset);

			   m_luaEvent->addArgument("ScoresVec*",result,1);
			   m_luaEvent->addArgument(mode);
			   m_luaEvent->addArgument(offset);
			   m_luaEvent->addArgument(amount);
			   m_luaEvent->addArgument(datasourceTable);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("GET_PLAYER_SCORES")

		   delete (int*) var1;
		   delete (int*) var2;
		   delete (int*) var3;
		   delete (std::string*) var4;
	   }
	   else if (m_function == SET_GAME) {
		   try {
			   gkLogger::write("SET_GAME START",true);
			   gkLogger::write("SET_GAME AFTER CALL",true);

#if GK_PLATFORM == GK_PLATFORM_ANDROID
			   gs::TSetGameResult::type result = m_service->set_game(TO_STRING(var1),gs::g_gameserver_constants.os_android,CLIENT_VERSION);
#elif GK_PLATFORM == GK_PLATFORM_BLACKBERRY
			   gs::TSetGameResult::type result = m_service->set_game(TO_STRING(var1),gs::g_gameserver_constants.os_bb,CLIENT_VERSION);
#elif GK_PLATFORM == GK_PLATFORM_APPLE_IOS
			   gs::TSetGameResult::type result = m_service->set_game(TO_STRING(var1),gs::g_gameserver_constants.os_ios,CLIENT_VERSION);
#elif GK_PLATFORM == GK_PLATFORM_WIN32
			   gs::TSetGameResult::type result = m_service->set_game(TO_STRING(var1),gs::g_gameserver_constants.os_windows,CLIENT_VERSION);
#elif GK_PLATFORM == GK_PLATFORM_APPLE
			   gs::TSetGameResult::type result = m_service->set_game(TO_STRING(var1),gs::g_gameserver_constants.os_mac,CLIENT_VERSION);
#elif GK_PLATFORM == GK_PLATFORM_LINUX
	   gs::TSetGameResult::type result = m_service->set_game(TO_STRING(var1),gs::g_gameserver_constants.os_linux,CLIENT_VERSION);
#endif
			   m_luaEvent->addArgument((int)result);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);

		   }
		   CATCHBLOCK("SET_GAME")

		   delete (std::string*) var1;

	   }
	   else if (m_function == CHECK_SESSION) {
		   try {
			   gs::TSessionCheck check;
			   m_service->check_session(check);

			   m_luaEvent->addArgument(check.hasGame);
			   m_luaEvent->addArgument(check.hasPlayer);
			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);

		   }
		   CATCHBLOCK("CHECK_SESSION")

		   delete (std::string*) var1;
	   }
	   else if (m_function == CHECK_USERDATA) {
		   try {
			   gs::TUserDataResponse::type resp = m_service->check_userdata(TO_STRING(var1),TO_STRING(var2));

			   m_luaEvent->addArgument((int)resp);
			   m_luaEvent->addArgument(gkString(TO_STRING(var1)));
			   m_luaEvent->addArgument(gkString(TO_STRING(var2)));

			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("CHECK_USERDATA")

		   delete (std::string*) var1;
		   delete (std::string*) var2;
	   }
	   else if (m_function == GET_PLAYER_POS) {
		   try {
			   int modeNr = TO_INT(var1);
			   int pos = m_service->get_player_pos(modeNr);

			   m_luaEvent->addArgument(pos);

			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("GET_PLAYER_POS")

		   delete (int*) var1;
	   }
	   else if (m_function == GET_POS_BY_SCORE) {
		   try {
			   int modeNr = TO_INT(var1);
			   double score = TO_DOUBLE(var2);
			   int pos = m_service->get_pos_by_score(modeNr,score);

			   m_luaEvent->addArgument(modeNr);
			   m_luaEvent->addArgument((float)score);
			   m_luaEvent->addArgument(pos);

			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("GET_POS_BY_SCORE")

		   delete (int*) var1;
		   delete (double*) var2;
	   }
	   else if (m_function == REQUEST_SIGNINTOKEN) {
		   try {
			   TSigninToken token;
			   m_service->request_signintoken(token,TO_STRING(var1));

			   m_luaEvent->addArgument(token.tokenName);
			   m_luaEvent->addArgument(token.tokenHash);

			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("REQUEST_SIGNINTOKEN")

		   delete (std::string*) var1;
	   }
	   else if (m_function == LOGIN_BY_TOKEN) {
		   try {
			   TAutoLoginResponse respAutologin;
			   m_service->login_by_token(respAutologin,TO_STRING(var1),TO_STRING(var2),TO_STRING(var3));

			   if (respAutologin.resultCode==TAutoLoginResultCode::ok) {
				   m_luaEvent->addArgument(respAutologin.tokenName);
				   m_luaEvent->addArgument(respAutologin.tokenHash);
				   m_luaEvent->addArgument("gs::TUser*",new gs::TUser(respAutologin.user),1);

				   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
			   } else {
				   m_luaEvent->addArgument("fail");
				   m_luaEvent->addArgument("fail");
				   m_luaEvent->addArgument("fail");

				   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
			   }
		   }
		   CATCHBLOCK("LOGIN_BY_TOKEN")

		   delete (std::string*) var1;
		   delete (std::string*) var2;
		   delete (std::string*) var3;
	   }
	   else if (m_function == GET_BEFORELEVEL_DATA) {
		   try {
			   TBeforeLevelData* respBeforeLevelData = new TBeforeLevelData;
			   int mode = TO_INT(var1);
			   int topAmount = TO_INT(var2);
			   int meAmount = TO_INT(var3);

			   m_service->get_before_level_data(*respBeforeLevelData,mode,topAmount,meAmount);

			   m_luaEvent->addArgument(mode);
			   m_luaEvent->addArgument(topAmount);
			   m_luaEvent->addArgument(meAmount);
			   m_luaEvent->addArgument("gs::TBeforeLevelData*",respBeforeLevelData,1);

			   gkLuaManager::getSingleton().addPreparedLuaEvent(m_luaEvent);
		   }
		   CATCHBLOCK("GET_BEFORELEVEL_DATA")

		   delete (int*) var1;
		   delete (int*) var2;
		   delete (int*) var3;
	   }

}

bool gs::TMode::operator<(gs::TMode const&) const{
	return true;
}

bool gs::TAddScoreResponse::operator<(gs::TAddScoreResponse const&) const{
	return true;
}

bool gs::TLoginResponse::operator<(gs::TLoginResponse const&) const {
	return true;
}

bool gs::TGetUserResponse::operator<(gs::TGetUserResponse const&) const {
	return true;
}

bool gs::TSessionCheck::operator<(gs::TSessionCheck const&) const {
	return true;
}

bool gs::TErrorException::operator<(gs::TErrorException const&) const {
	return true;
}

bool gs::TSigninToken::operator<(gs::TSigninToken const&) const {
	return true;
}

bool gs::TAutoLoginResponse::operator<(gs::TAutoLoginResponse const&) const {
	return true;
}

bool gs::TBeforeLevelData::operator<(gs::TBeforeLevelData const&) const {
	return true;
}
