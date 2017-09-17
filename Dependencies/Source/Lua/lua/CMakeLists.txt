set(lua_src
	lapi.c
	lauxlib.c
	lbaselib.c
	lcode.c
	ldblib.c
	ldebug.c
	ldo.c
	ldump.c
	lfunc.c
	lgc.c
	linit.c
	liolib.c
	llex.c
	lmathlib.c
	lmem.c
	loadlib.c
	lobject.c
	lopcodes.c
	loslib.c
	lparser.c
	lstate.c
	lstring.c
	lstrlib.c
	ltable.c
	ltablib.c
	ltm.c
	lundump.c
	lvm.c
	lzio.c
	print.c
)

set(lua_hdr
	lapi.h
	lauxlib.h
	lcode.h
	ldebug.h
	ldo.h
	lfunc.h
	lgc.h
	llex.h
	llimits.h
	lmem.h
	lobject.h
	lopcodes.h
	lparser.h
	lstate.h
	lstring.h
	ltable.h
	ltm.h
	lua.h
	luaconf.h
	lualib.h
	lundump.h
	lvm.h
	lzio.h
)

if (NOT RELEASE_MODE AND NOT WIN32) 
	message(STATUS "LUA: Using DYNAMIC LOADING (since not in RELEASE-MODE")
	add_definitions(-DLUA_USE_DLOPEN)
endif()

set(lua_standalone		lua.c)
set(lua_bc_compiler		luac.c)

include_directories(.)
add_library(Lua STATIC ${lua_src} ${lua_hdr})

config_target_if_iphone_build(Lua TRUE)
