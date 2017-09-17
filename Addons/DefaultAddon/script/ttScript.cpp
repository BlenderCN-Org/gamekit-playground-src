/*
 * ttScript.cpp
 *
 *  Created on: Apr 3, 2013
 *      Author: ttrocha
 */

#include "script/ttScript.h"
#include "gkLogger.h"
#include "../../../Engine/Script/Lua/gkLuaManager.h"
#include "DefaultGameAddon.h"


void activateEmbeddedScripts() {
	install_Templates_DefaultAddon(gkLuaManager::getSingleton().getLua());
}
