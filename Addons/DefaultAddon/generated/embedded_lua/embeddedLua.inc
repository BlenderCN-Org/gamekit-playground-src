void install_Templates_DefaultAddon(lua_State* L)
{
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_dostring(L, _SOUND); 
    luaL_dostring(L, _PLAYER); 
    luaL_dostring(L, NGUI); 
    luaL_dostring(L, _LEVEL); 
    luaL_dostring(L, _GAME); 
    luaL_dostring(L, _ELEMENTS); 
    luaL_dostring(L, _STARTER); 
    luaL_dostring(L, _UI); 
    lua_pop(L, 1); 
}
