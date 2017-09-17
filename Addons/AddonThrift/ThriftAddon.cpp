/*
 * GameAddon.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#include "ThriftAddon.h"
#include "gkLogger.h"
#include "Network/gkDownloadManager.h"
#include "utStreams.h"
#include "Script/Lua/gkLuaManager.h"

#include "gkValue.h"

#include "gkString.h"
#include "gkThriftManager.h"



ThriftAddon::ThriftAddon() {
}

ThriftAddon::~ThriftAddon() {
//	delete thriftclient;
//	thriftclient = 0;
}

extern "C" int luaopen_ta(lua_State* L);

extern "C" int _Script_install(lua_State* L)
{
	return luaopen_ta(L);
}

void ThriftAddon::setup(){
//	gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
//	if (!mgr) {
//		mgr = new gkNetClientManager;
//	}
//	utMemoryStream* call = mgr->getToStream("http://localhost:8080/Restful/rest/user");
//
//	tutorial::Person person;
//	person.ParseFromArray(call->ptr(),call->size());
//
//
//	gkLogger::write("Yeah email:"+person.email(),true);
//	gkLogger::write("ADDON SETUP!",true);
//
//	lua_protobuf_tutorial_open(gkLuaManager::getSingleton().getLua());
	_Script_install(gkLuaManager::getSingleton().getLua());




// Sample dedicated server:
/*	  boost::shared_ptr<TSocket> socket(new TSocket("192.168.0.177", 9090));
	  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
*/

//	  thriftclient = new HTTPClient<gs::TGameServerServiceClient>("127.0.0.1",8080,"/ThriftServlet/gamedb");
//	  thriftclient->getClient().ping();
//	  gs::TUser user;
//	  thriftclient->getClient().login_user(user,"dertom","dertom","triquacir");
//	  thriftclient->getClient().add_score(0,10.0);
//	  gs::TUser user;
//	  thriftclient->getClient().login_user(user,"dertom","dertom");
//	  thriftclient->getClient().login_user(user,"dertom","dertom");
//	  gkLogger::write("RESULT:"+gkToString(result),true);





};
void ThriftAddon::tick(gkScalar rate){
	//gkLogger::write("ADDON TICK",true);
}

bool gs::TUser::operator<(gs::TUser const&) const {
	return true;
}

bool gs::TScore::operator<(gs::TScore const&) const {
	return true;
}

bool gs::TGame::operator<(gs::TGame const&) const {
	return true;
}

bool gs::TRegInfo::operator<(gs::TRegInfo const&) const {
	return true;
}


HTTPClient<gs::TGameServerServiceClient>* ThriftAddon::thriftclient = 0;
