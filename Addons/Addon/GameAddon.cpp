/*
 * GameAddon.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#include "GameAddon.h"
#include "gkLogger.h"
#include "Network/gkDownloadManager.h"
#include "utStreams.h"
#include "test.pb.h"
#include "Script/Lua/gkLuaManager.h"


GameAddon::GameAddon() {
}

GameAddon::~GameAddon() {
}

extern "C" int luaopen_ts(lua_State* L);

extern "C" int _Script_install(lua_State* L)
{
	return luaopen_ts(L);
}

void GameAddon::setup(){
	gkNetClientManager* mgr = gkNetClientManager::getSingletonPtr();
	if (!mgr) {
		mgr = new gkNetClientManager;
	}
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
};
void GameAddon::tick(gkScalar rate){
	//gkLogger::write("ADDON TICK",true);
}
