-- This file is executed after the engine started up and before all other scripts (even before -e injected script that can be passed via the runtime's cli)
Gk.log("booting gk-lua-runtime")

-- setting some environment variables
LUA_LIB_DIR="lua/lua-libs" -- the root of our lua runtime environemnt (those files are located in Engine/User/codescript/lua/lua-libs)

-- init lua-
Gk.import(LUA_LIB_DIR.."/misc/_utils.lua")
initPackageLoader()

-- Default Loop with once- and always-queue
Gk.import(LUA_LIB_DIR.."/misc/_game.lua")

Gk.import(LUA_LIB_DIR.."/nodes/_nodes.lua")


-- HOOKS:

-- this function is called right after a new scene was instantiated
function __onSceneCreated()
  Gk.log("HOOK __onSceneCreated")
  wrapAllTreesWithLuaNodes()
end

Gk.log("boot-process finished") 