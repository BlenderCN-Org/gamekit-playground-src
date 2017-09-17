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
#include "gkLuaManager.h"
#include "gkLuaScript.h"
#include "gkLuaUtils.h"
#include "gkUtils.h"
#include "gkTextManager.h"
#include "gkTextFile.h"
#include "gkLogger.h"
#include "gkPath.h"
#include "utStreams.h"



extern "C" int _OgreKitLua_install(lua_State* L);



gkLuaManager::gkLuaManager()
	:   gkResourceManager("LuaManager", "Lua"),
		L(0),
		m_preparedLuaEvents("lua-queue")
{
	//gkLogger::write("lm1",true);

	L = lua_open();
	//gkLogger::write("lm2",true);
	luaL_openlibs(L);
	//gkLogger::write("lm3",true);

	_OgreKitLua_install(L);
	//gkLogger::write("lm4",true);
	//luaopen_pb(L);
}


gkLuaManager::~gkLuaManager()
{
	destroyAll();
	if (L) lua_close(L);
}

gkResource* gkLuaManager::createImpl(const gkResourceName& name, const gkResourceHandle& handle)
{
	return new gkLuaScript(this, name, handle);
}


void gkLuaManager::notifyResourceCreatedImpl(gkResource* res)
{
	gkTextFile* intern = (gkTextFile*)gkTextManager::getSingleton().getByName(res->getName());

	if (intern)
	{
		gkLuaScript* script = (gkLuaScript*)res;
		script->setScript(intern->getText());		
	}
}

void gkLuaManager::notifyResourceDestroyedImpl(gkResource* res)
{
}


void gkLuaManager::decompileAll(void)
{
	Resources::Iterator iter = m_resources.iterator();
	while (iter.hasMoreElements())
	{
		gkLuaScript* script = (gkLuaScript*)iter.peekNextValue();
		script->decompile();
		iter.next();
	}

}


void gkLuaManager::decompileGroup(const gkString& group)
{
	Resources::Iterator iter = m_resources.iterator();
	while (iter.hasMoreElements())
	{
		gkLuaScript* script = (gkLuaScript*)iter.peekNextValue();
		if (script->getGroupName() == group)
			script->decompile();
		iter.next();
	}
}

gkLuaScript* gkLuaManager::createFromText(const gkResourceName& name, const gkString& text)
{
	if (exists(name))
		return 0;

	gkLuaScript* script = create<gkLuaScript>(name);

	script->setScript(text+" ");

	return script;
}


gkLuaScript* gkLuaManager::createFromTextBlock(const gkResourceName& name)
{
	if (exists(name))
		return 0;

	gkTextFile* intern = (gkTextFile*)gkTextManager::getSingleton().getByName(name);

	if (intern == 0)
	{
		gkPrintf("Invalid internal text file %s\n", name.getName().c_str());
		return 0;
	}

	return createFromText(name, intern->getText());
}

gkLuaScript* gkLuaManager::createFromFile(const gkString& filename, const gkResourceName& resourceName)
{
	gkPath file(filename);


	if (file.exists())
	{
		gkString scriptText(file.getAsString());

		gkLuaScript* script = createFromText(resourceName,scriptText+" ");
		script->setFilePath(file.directory());

		return script;
	}

	//gkLogger::write("Couldn't find file:"+filename);
	return 0;
}

bool gkLuaManager::registerEvent(const gkString& name, gkLuaEvent* evt)
{
	// first check if the name is already used
	utCharHashKey funcHash(name.c_str());
	if (m_luaFuncMap.find(funcHash)!=UT_NPOS)
	{
		gkLogger::logScreenConsole("ERROR LuaManager: Function with name:'"+name+"' is already registered!");
		return false;
	}
	m_luaFuncMap.insert(funcHash,evt);
	return true;
}


gkLuaEvent* gkLuaManager::getFunction(const gkString& name)
{
	int pos;
	if ((pos = m_luaFuncMap.find(utCharHashKey(name.c_str())))!=UT_NPOS)
	{
		gkLuaEvent* evt = m_luaFuncMap.at(pos);
		return evt;
	}
	return 0;
}

gkLuaScript* gkLuaManager::CURRENTSCRIPT = 0;

gkString gkLuaManager::locateFileRelativeToCurrentScript(const gkString& stPath)
{
		// no script in codeblock? try to find it in the filesystem
		gkString resultPath("");
		gkPath path(stPath);

		gkLuaScript* currentScript = gkLuaManager::CURRENTSCRIPT;

		if (currentScript && currentScript->isCreatedFromFile())
		{
			gkString tryTwo(currentScript->getFilePath()+gkPath::SEPERATOR+path.base());

			gkPath p2(tryTwo);

			if (p2.exists())
			{
				resultPath = tryTwo;
			}
			else
			{
				gkString tryThree(currentScript->getFilePath()+gkPath::SEPERATOR+path.getPath());
				gkPath p3(tryThree);

				if (p3.exists())
				{
					resultPath = tryThree;
				}
			}

		}



		return resultPath;
}

void gkLuaManager::addPreparedLuaEvent(gkPreparedLuaEvent* prepEvent){
	m_preparedLuaEvents.push(prepEvent);
	//gkLogger::write("Added prep-event",true);
}

void gkLuaManager::executePreparedLuaEvents() {
	//gkLogger::write("l1", true);
	int size = m_preparedLuaEvents.size();
	//gkLogger::write("l2", true);
	for (int i=0;i<size;i++) {
		//gkLogger::write("l3", true);
		gkPreparedLuaEvent* evt;
		m_preparedLuaEvents.pop(evt);
		evt->call();

		// is the event marked to be deleted after execution?
		if (evt->isDeleteAfterExecution())
			delete evt;
	}
	//gkLogger::write("l4", true);
	m_preparedLuaEvents.reset();
	//gkLogger::write("l5", true);
}

UT_IMPLEMENT_SINGLETON(gkLuaManager);
