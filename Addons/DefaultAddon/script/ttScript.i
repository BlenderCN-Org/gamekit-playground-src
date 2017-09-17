%module ts

/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Benjamin Tolputt.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/


%{
#define GS_TYPEOF(a, b)   ((a) && typeid(*a) == typeid(b))
#define GS_TYPE_RET(GK, GS, T) if (GS_TYPEOF(GK, gk##T)) { SWIG_NewPointerObj(L, GS, SWIGTYPE_p_##gs##T, 1); return 1; }

// ------------------------------------------------------------------------------
// Convert & store gk<Class> objects as gs<Class> with consistent userdata values
// ------------------------------------------------------------------------------
template <class GKTYPE, class GSTYPE>
static int gsLuaStoreAndReturnObject(lua_State *L, GKTYPE* obj, const char* className, swig_type_info* swigTypeInfo)
{
    // Early exit on nil
    if (!obj)
    {
        lua_pushnil(L);
        return 1;
    }
    
    // Create the storage name
    char tableName[256];
    sprintf(tableName, ".gsCache{%s}", className);
    
    // Try to get the table in which the objects would be stored
    lua_pushstring(L, tableName);
    lua_rawget(L,     LUA_REGISTRYINDEX);
    
    // If that did not work - create it and put it on top
    if (!lua_istable(L, -1))
    {
        lua_pushstring(L, tableName);
        lua_newtable(L);
        
        lua_createtable(L,  0, 1);
        lua_pushstring(L,   "v");
        lua_setfield(L,     -2, "__mode");
        lua_setmetatable(L, -2);
        
        lua_rawset(L, LUA_REGISTRYINDEX);
        
        // Try to get the table
        lua_pushstring(L, tableName);
        lua_rawget(L,     LUA_REGISTRYINDEX);
    }
    
    // We now have a table to get the user data from - so check if it's there
    lua_pushlightuserdata(L, obj);
    lua_gettable(L, -2);
    
    if (lua_isnil(L, -1))
    {
        // We no longer need the nil value
        lua_pop(L, 1);
        
        // Push the pointer value onto the stack (as the key)
        lua_pushlightuserdata(L, obj);
        
        // Create and push the value
        GSTYPE* wrappedObject = new GSTYPE(obj);
        SWIG_NewPointerObj(L, wrappedObject, swigTypeInfo, 1);
        
        // Store it in the table
        lua_settable(L, -3);
        
        // Reget the value from the table
        lua_pushlightuserdata(L, obj);
        lua_gettable(L, -2);
    }
    return 1;
}
#define GS_LUA_OBJECT_WRAP_STORE_RAW(OBJ, T, WT) gsLuaStoreAndReturnObject<T, WT>(L, (T*)OBJ, #WT, SWIGTYPE_p_##WT)
#define GS_LUA_OBJECT_WRAP_STORE(OBJ, T, WT) gsLuaStoreAndReturnObject<gk##T, gs##WT>(L, (gk##T*)OBJ, #WT, SWIGTYPE_p_##gs##WT)
#define GS_LUA_OBJECT_STORE(OBJ, T) GS_LUA_OBJECT_WRAP_STORE(OBJ, T, T)
#define GS_LUA_OBJECT_RET(OBJ, T) if (GS_TYPEOF(OBJ, gk##T)) { return GS_LUA_OBJECT_STORE(OBJ, T); }
#define GS_LUA_OBJECT_WRAP_RET(OBJ, T, WT) if (GS_TYPEOF(OBJ, gk##T)) { return GS_LUA_OBJECT_WRAP_STORE(OBJ, T, WT); }



#include "script/ttScript.h"
%}

%typemap(out) ttUser* 
%{ if ($1) { SWIG_arg += GS_LUA_OBJECT_WRAP_STORE_RAW($1, ttUser, tsUser); } %}

%include "../../../Engine/Script/Api/gsTypes.i"

#%include "../../../Engine/Script/Api/gsCommon.h"

//static int gsLuaStoreAndReturnObject(lua_State *L, GKTYPE* obj, const char* className, swig_type_info* swigTypeInfo)
 
// mapping
//%typemap(out) gkScene* 
//%{ if ($1) { SWIG_arg += GS_LUA_OBJECT_STORE($1, Scene); } %}

//%typemap(out) ttWorld* 
//%{ if ($1) { SWIG_arg += gsLuaStoreAndReturnObject<ttWorld, tsWorld>(L, (ttWorld*)$1, "World", SWIGTYPE_p_tsWorld); } %}

%include "ttScript.h"
