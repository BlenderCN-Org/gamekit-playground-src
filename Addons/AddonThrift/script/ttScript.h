/*
 * ttScript.h
 *
 *  Created on: Apr 3, 2013
 *      Author: ttrocha
 */

#ifndef TTSCRIPT_H_
#define TTSCRIPT_H_

#include "utStreams.h"
#include "gkString.h"
#include "ThriftAddon.h"
#include "../generated/gen-cpp/gameserver_types.h"
#include "Thread/gkActiveObject.h"
#include "Thread/gkPtrRef.h"
#include "gkLogger.h"
#include <vector>

#include "openssl/md5.h"

#include "../generated/gen-cpp/TGameServerService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/Thrift.h>
#include "../../../Engine/Thread/gkThread.h"
#include "../../../Engine/Script/Api/gsCommon.h"
#include "../../../Engine/Script/Lua/gkLuaUtils.h"
#include "../../../Engine/Thread/gkPtrRef.h"
#include "../../../Engine/Thread/gkActiveObject.h"
#include "../../../Engine/Network/gkDownloadManager.h"
#include "../../../Engine/gkCrypt.h"

#define CLIENT_VERSION 1

using gs::TGameServerServiceClient;


#ifndef SWIG
class ThriftService : public gs::TGameServerServiceClient {
public:
	ThriftService(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) : gs::TGameServerServiceClient(prot) {}
};
#else
#define int32_t int


class ThriftService {
public:
	ThriftService(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot)  {}

	  virtual void ping() = 0;
	  virtual gs::TSetGameResult::type set_game(const std::string& gameid, const int32_t os, const int client_version) = 0;
	  virtual void create_game(const std::string& name, const int modeAmount, const bool ascOrdering) = 0;
	  virtual void create_modes(const std::string& gameid, const int amount, const bool ascOrdering) = 0;
	  virtual void get_all_games(std::set<gs::TGame> & _return) = 0;
	  virtual gs::TAddUserResultCode::type add_user(const gs::TUser& u) = 0;
	  virtual void get_user_by_username(gs::TGetUserResponse& _return, const std::string& username) = 0;
	  virtual void get_user_list(std::set<gs::TUser> & _return) = 0;
	  virtual void request_logintoken(std::string& _return) = 0;
	  virtual void login_user(gs::TLoginResponse& _return, const std::string& username, const std::string& password) = 0;
	  virtual gs::TUserDataResponse::type check_userdata(const std::string& username, const std::string& email) = 0;
	  virtual void get_login_user(gs::TGetUserResponse& _return) = 0;
	  virtual void check_session(gs::TSessionCheck& _return) = 0;
	  virtual void add_score(std::vector<gs::TAddScoreResponse> & _return, const int mode, const double score) = 0;
	  virtual void get_player_scores(std::vector<gs::TScore> & _return, const int mode, const int amount, const int offset) = 0;
	  virtual void get_highscores(std::vector<gs::TScore> & _return, const int mode, const int amount, const int offset) = 0;
	  virtual int get_player_pos(const int mode) = 0;
	  virtual int get_pos_by_score(const int mode, const double score) = 0;
	  virtual void request_signintoken(gs::TSigninToken& _return, const std::string& meta) = 0;
	  virtual void login_by_token(gs::TAutoLoginResponse& _return, const std::string& tokenUser, const std::string& tokenHash, const std::string& meta) = 0;
	  virtual void get_before_level_data(gs::TBeforeLevelData& _return, const int32_t mode, const int32_t top_amount, const int32_t me_amount) = 0;
	  virtual void get_player_highscores(std::vector<gs::TScore> & _return) = 0;
	  virtual void get_player_highscore(gs::TScore& _return, const int32_t mode) = 0;

};
#endif

typedef std::vector<gs::TScore> ScoresVec;
typedef std::vector<gs::TAddScoreResponse> AddScoreResponse;
class Thrift;

class AsyncCaller
#ifndef SWIG
: public gkCall
#endif
{
public:
	   enum FuncCall {
			PING, CREATE_GAME, ADD_USER, ADD_SCORE, LOGIN_USER, GET_PLAYER_SCORES,
			GET_HIGHSCORES,SET_GAME, CHECK_SESSION, CHECK_USERDATA, GET_PLAYER_POS,
			GET_POS_BY_SCORE,REQUEST_SIGNINTOKEN,LOGIN_BY_TOKEN,GET_PLAYER_HIGHSCORE,
			GET_PLAYER_HIGHSCORES,GET_BEFORELEVEL_DATA
	   };

	   AsyncCaller(Thrift* thrift,ThriftService* service,FuncCall thriftfunc,gsSelf self,gsFunction func)
			: m_service(service), m_function(thriftfunc),m_thrift(thrift),
			  var1(0),var2(0),var3(0),var4(0),var5(0)
	   {
		   // this event needs to be passed to gkLuaManager::getSingleton().addPreparedLuaEvent(..) or disposed on your own
		   // TODO: implement sort of resource-manager for this kind
		   m_luaEvent = new gkPreparedLuaEvent(self,func);
	   }

	   void dispose() {
		   // dispose the prepared element because you don't want to pass it anywhere anymore
		   delete m_luaEvent;
		   m_luaEvent = 0;
	   }


	   void setArgs(void* arg1,void* arg2=0,void* arg3=0,void* arg4=0,void* arg5=0) {
		   var1 = arg1,var2=arg2,var3=arg3,var4=arg4,var5=arg5;
	   }
	   void setArg1(void* arg) {
		   var1 = arg;
	   }
	   void setArg2(void* arg) {
		   var2 = arg;
	   }
	   void setArg3(void* arg) {
		   var3 = arg;
	   }
	   void setArg4(void* arg) {
		   var4 = arg;
	   }
	   void setArg5(void* arg) {
		   var5 = arg;
	   }

	   void run();
private:
	   void* var1;
	   void* var2;
	   void* var3;
	   void* var4;
	   void* var5;
	   Thrift* m_thrift;
	   ThriftService* m_service;
	   gkPreparedLuaEvent* m_luaEvent;
	   FuncCall m_function;
};


class AsyncThriftService
{
public:


	AsyncThriftService(Thrift* thrift,ThriftService* service) :
		m_service(service),m_thrift(thrift){}


	   void ping(gsSelf self, gsFunction func)
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::PING,self,func);
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }
	   void createGame(const std::string& name, const int modeAmount,const bool ascOrdering,gsSelf self, gsFunction func)
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::CREATE_GAME,self,func);
		   ac->setArgs((void*)new std::string(name),(void*)new int(modeAmount),new bool(ascOrdering));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }
	   int add_user(gs::TUser& u,gsSelf self, gsFunction func)
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::ADD_USER,self,func);
		   u.password = gkCrypt::md5(u.password);
		   ac->setArgs((void*)new gs::TUser(u));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   int add_score(const int mode, const double score,gsSelf self, gsFunction func)
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::ADD_SCORE,self,func);
		   ac->setArgs((void*)new int(mode), (void*)new double(score));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void login_user(const std::string& username, const std::string& password
			   	   	   	   ,gsSelf self, gsFunction func)
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::LOGIN_USER,self,func);

		   ac->setArgs(new std::string(username),new std::string(password));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void get_player_scores( const int mode, const int amount,const int offset,gsSelf self, gsFunction func, const gkString& datasourceTable="")
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::GET_PLAYER_SCORES,self,func);
		   ac->setArgs(new int(mode),new int(amount), new int(offset),new std::string(datasourceTable));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void get_highscores(const int mode, const int amount,const int offset,gsSelf self, gsFunction func, const gkString& datasourceTable="")
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::GET_HIGHSCORES,self,func);
		   ac->setArgs(new int(mode),new int(amount),new int(offset),new std::string(datasourceTable));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void set_game(const std::string& gameid,gsSelf self, gsFunction func)
	   {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::SET_GAME,self,func);
		   ac->setArgs(new std::string(gameid));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void check_session(gsSelf self,gsFunction func){
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::CHECK_SESSION,self,func);
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void check_userdata(const std::string& username, const std::string& email,gsSelf self,gsFunction func){
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::CHECK_USERDATA,self,func);
		   ac->setArgs(new std::string(username),new std::string(email));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void get_player_pos(const int modeNr, gsSelf self,gsFunction func) {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::GET_PLAYER_POS,self,func);
		   ac->setArgs(new int(modeNr));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void get_pos_by_score(const int modeNr, const double score,gsSelf self,gsFunction func) {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::GET_POS_BY_SCORE,self,func);
		   ac->setArgs(new int(modeNr),new double(score));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void request_signintoken(const std::string& meta,gsSelf self,gsFunction func) {
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::REQUEST_SIGNINTOKEN, self,func);
		   ac->setArgs(new std::string(meta));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }
	   void login_by_token(const std::string& tokenUser, const std::string& tokenHash, const std::string& meta,gsSelf self,gsFunction func){
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::LOGIN_BY_TOKEN, self,func);
		   ac->setArgs(new std::string(tokenUser),new std::string(tokenHash),new std::string(meta));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }

	   void get_before_level_data(const int mode, const int top_amount, const int me_amount,gsSelf self,gsFunction func){
		   AsyncCaller* ac = new AsyncCaller(m_thrift,m_service,AsyncCaller::GET_BEFORELEVEL_DATA, self,func);
		   ac->setArgs(new int(mode),new int(top_amount),new int(me_amount));
		   gkNetClientManager::getSingleton().getNetworkHelper().enqueue(gkPtrRef<gkCall>(ac));
	   }
//		  virtual void get_player_highscores(std::vector<gs::TScore> & _return) = 0;
//		  virtual void get_player_highscore(gs::TScore& _return, const int32_t mode) = 0;
	   /*
	   void get_user_by_username(gs::TUser& _return, const std::string& username) = 0;
	   void login_user(gs::TUser& _return, const std::string& username, const std::string& password, const std::string& gameid) = 0;
	   void get_user_list(std::set<gs::TUser> & _return) = 0;
	   void get_player_scores(std::vector<gs::TScore> & _return, const int mode, const int amount) = 0;
	   void get_highscores_scores(std::vector<gs::TScore> & _return, const int mode, const int amount) = 0;
	   int add_score(const int mode, const double score) = 0;
	   void getPassword(std::string& _return, const std::string& name) = 0;
	   void get_all_games(std::set<gs::TGame> & _return) = 0;
	   void createModes(const std::string& gameid, const int amount) = 0;
	   void clear_list() = 0;*/


private:
	   ThriftService* m_service;
	   Thrift* m_thrift;
};

class Thrift {
	public:
		// libcurl
		Thrift(const gkString& url);
		// socket-based
		Thrift(const gkString& server, int port);
		// Thrift http-client
		Thrift(const gkString& server, const gkString& path, int port);
		~Thrift();
		ThriftService* getService();
		void refreshConnection();
		void disconnect();

		gkPreparedLuaEvent* getErrorCallback() {
			return m_errorCallback;
		}

		void setErrorCallback(gsSelf self, gsFunction func)
	    {
			m_errorCallback = new gkPreparedLuaEvent(self,func);
	    }

		gkPreparedLuaEvent* getExceptionCallback() {
			return m_exceptionCallback;
		}

		void setExceptionCallback(gsSelf self, gsFunction func) {
			m_exceptionCallback = new gkPreparedLuaEvent(self,func);
		}

		bool isOpen() {
			if (type==HTTP) {
				return httpConnection->isOpen();
			} else {
				return transport->isOpen();
			}
		}


	private:
		HTTPClient<gs::TGameServerServiceClient>* httpConnection;
		boost::shared_ptr<apache::thrift::transport::TTransport> socket;
		boost::shared_ptr<apache::thrift::transport::TTransport> transport;
		boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol;
		ThriftService* client;
		bool connected;
		enum ConnectionType {
			SOCKET, HTTP
		};
		ConnectionType type;
		gkPreparedLuaEvent* m_errorCallback;
		gkPreparedLuaEvent* m_exceptionCallback;

};

extern ThriftService* getThriftService();






#endif /* TTSCRIPT_H_ */
