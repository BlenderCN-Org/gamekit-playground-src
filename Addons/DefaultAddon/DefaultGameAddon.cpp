/*
 * GameAddon.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: ttrocha
 */

#include "DefaultGameAddon.h"
#include "gkLogger.h"
#include "Script/Lua/gkLuaUtils.h"
#include "Script/Lua/gkLuaManager.h"
#include "generated/embedded_lua/gsTemplates.h"

DefaultGameAddon::DefaultGameAddon() {
}

DefaultGameAddon::~DefaultGameAddon() {
}



extern "C"{
	#include "generated/embedded_lua/embeddedLua.inc"
	// there might be name-collisions. 1st rename the module name in ttScript.i
	int luaopen_ts(lua_State* L);
	// 2nd rename this method
	int _DefaultGameAddonScript_install(lua_State* L)
	{
		int result = luaopen_ts(L);
		return result;
	}

}

void DefaultGameAddon::setup(){
	//gkLogger::write("SETUP DEFAULTADDON",true);
	_DefaultGameAddonScript_install(gkLuaManager::getSingleton().getLua());
};
void DefaultGameAddon::tick(gkScalar rate){
	//gkLogger::write("TICK DEFAULT",true);
}
