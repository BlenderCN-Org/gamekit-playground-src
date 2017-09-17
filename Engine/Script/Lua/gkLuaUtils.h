/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): none yet.
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
#ifndef _gkLuaUtils_h_
#define _gkLuaUtils_h_

extern "C" {
#include "../../../Dependencies/Source/Lua/lua/lua.h"
#include <lualib.h>
#include "../../../Dependencies/Source/Lua/lua/lauxlib.h"
}

#include "utTypes.h"
#include "gkString.h"
#include "../../../Dependencies/Source/GameKit/Utils/utTypes.h"


#define GK_UPVALUETYPE      1
#define GK_UPVALUEMETHOD    2
#define GK_NOPARAM         -1
#define GK_PARAM            1



typedef int (*gkLuaMethod)(lua_State* L);


struct gkLuaMethodDef
{
	const char*      m_name;
	gkLuaMethod      m_meth;
	int              m_flag;
	const char*      m_params;
	// internal
	unsigned int     m_hash;
};


struct gkLuaTypeDef
{
	const char*              m_name;
	gkLuaTypeDef*            m_parent;
	gkLuaMethodDef*          m_methods;
};


enum gkLuaDataTypes
{
	LU_NIL      = '\0',
	LU_OPT      = '|',
	LU_BOOL     = 'b',
	LU_NUMBER   = 'n',
	LU_INT      = 'i',
	LU_ENUM     = 'e',
	LU_FLOAT    = 'f',
	LU_DOUBLE   = 'd',
	LU_STRING   = 's',
	LU_FUNCTION = 'F',
	LU_INSTANCE = '.',
};

typedef struct gkLuaCurState
{
	lua_State* L;
	int m_id;
} gkLuaCurState;


class gkLuaObject
{
private:
	lua_State* L;
	int m_ref, m_own;
public:

	gkLuaObject() : L(0), m_ref(-1), m_own(0) {}
	gkLuaObject(lua_State* inL, int input) : L(inL), m_ref(-1), m_own(1) {ref(input);}
	gkLuaObject(const gkLuaObject& ob) : L(ob.L), m_ref(ob.m_ref), m_own(0) {}

	~gkLuaObject() { unref(); }

	void ref(int input)
	{
		if (m_ref != -1)
			unref();

		if (L)
		{
			lua_pushvalue(L, input);
			m_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		}
	}

	void unref(void)
	{
		if (m_ref != -1 && L && m_own)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, m_ref);
			m_ref = -1;
		}
	}

	int         get(void)   const {return m_ref;}
	lua_State*  getL(void)        {return L;}
};

class swig_type_info;

class gkLuaEvent
{
protected:

	lua_State*   L;
	gkLuaObject* m_self;
	gkLuaObject* m_callback;
	int          m_callArgs, m_trace;
	bool         m_error;
	bool		 m_isHelperThread;
public:

	// global function
	gkLuaEvent(gkLuaCurState fnc);

	// member function
	gkLuaEvent(gkLuaCurState self, gkLuaCurState fnc, bool isHelperThread=false);
	virtual ~gkLuaEvent();


	gkLuaEvent* clone(void);


	// push callback and self if present,
	void beginCall(void);
	// add args,
	void addArgument(bool val);
	void addArgument(int val);
	void addArgument(float val);
	void addArgument(const char* str);
	void addArgument(const gkString& val);
    // call this like this:
    // addArgument("tsScriptClass *",pointer to instance,0=you are responsible for deletion 1=swig will kill it for you)
    void addArgument(const gkString& swigType,void* pntr,int swigOwner=0);


    // increase argument-counter so you can do your own argument lua_push-whatever and call this afterwards
    void incArgumentCount();

	// push extra args,
	// do the call
	bool call();


	// return bool
	bool call(bool& result);
	const char* callString();
	int callInt();
};

/** this will store all arguments and push to the lua-stack only when the call is executed
 *  this is needed for preparing the call in e.g. one thread and be passed to the lua-queue and be executed on the
 *  lua-thread (main)
 */
class gkPreparedLuaEvent : gkLuaEvent
{
public:
	gkPreparedLuaEvent(gkLuaCurState func) : gkLuaEvent(func), m_deleteAfterExecution(true) {}
	gkPreparedLuaEvent(gkLuaCurState self,gkLuaCurState func) : gkLuaEvent(self,func), m_deleteAfterExecution(true) {}

	~gkPreparedLuaEvent() {
		ArgumentList::Iterator iter(m_preparedArguments);
		while (iter.hasMoreElements()) {
			PreparedArgument* arg = iter.getNext();
			arg->dispose();
			delete arg;
		}
	}
		// add args,
		void addArgument(bool val);
		void addArgument(int val);
		void addArgument(float val);
		void addArgument(const gkString& val);
	    // call this like this:
	    // addArgument("tsScriptClass *",pointer to instance,0=you are responsible for deletion 1=swig will kill it for you)
	    void addArgument(const gkString& swigType,void* pntr,int swigOwner=0);

		bool call();

		gkPreparedLuaEvent* clone(void) {
			gkPreparedLuaEvent* evt = new gkPreparedLuaEvent(*this);
			evt->m_self = m_self ? new gkLuaObject(*m_self) : 0;
			evt->m_callback = m_callback ? new gkLuaObject(*m_callback) : 0;
			return evt;
		};

		bool isDeleteAfterExecution() { return m_deleteAfterExecution; }
		void setDeleteAfterExecution(bool b) { m_deleteAfterExecution = b; }
private:
		enum ArgumentType {
			typeBool, typeInt, typeFloat, typeCharStar, typeGkString, typeSwigTypeString
		};

		class PreparedArgument {
		public:


			PreparedArgument(ArgumentType type, void* value)
				:m_type(type), m_value(value),m_value2(0),m_value3(0)
			{}

			void setValue2(void* val2) { m_value2 = val2;}
			void setValue3(void* val3) { m_value3 = val3;}
			ArgumentType getType() { return m_type; }
			void* getValue1() { return m_value;}
			void* getValue2() { return m_value2;}
			void* getValue3() { return m_value3;}
			void dispose();
		private:
			ArgumentType m_type;
			void* m_value;
			void* m_value2;
			void* m_value3;
		};

		typedef utArray<PreparedArgument*> ArgumentList;
		ArgumentList m_preparedArguments;
		bool m_deleteAfterExecution;
//		utArray
};

class gkLuaState
{
private:
	lua_State* L;
	int m_top;
public:
	gkLuaState(lua_State* inL) : L(inL) { m_top = lua_gettop(L); }
	~gkLuaState() {lua_settop(L, m_top);}
};


#define lua_savestate(L) gkLuaState ___lock___(L);


extern void lua_popall(lua_State* L);
extern void lua_dumpstack(lua_State* L);
extern int  lua_pushtraceback(lua_State* L);
extern bool lua_isclass(lua_State* L, int idx);

extern gkLuaTypeDef* lua_getclasstype(lua_State* L);
extern gkLuaMethodDef* lua_getmethodtype(lua_State* L);
extern int lua_findclassmethod(lua_State* L, const char* name, gkLuaTypeDef* cls);


// binding
extern void lua_beginnamespace(lua_State* L, const char* nsp);
extern void lua_endnamespace(lua_State* L);
extern void lua_addclasstype(lua_State* L, gkLuaTypeDef* type);
extern void lua_addmethods(lua_State* L, gkLuaMethodDef* type);
extern void lua_addconstant(lua_State* L, const char* name, double v);
extern void lua_addconstant(lua_State* L, const char* name, float v);
extern void lua_addconstant(lua_State* L, const char* name, int v);
extern void lua_addconstant(lua_State* L, const char* name, const char* v);

#endif//_gkLuaUtils_h_
