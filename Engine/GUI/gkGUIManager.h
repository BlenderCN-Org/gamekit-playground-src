/*
-------------------------------------------------------------------------------
    This file is part of gkGUIManager.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Alberto Torres Ruiz

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

#ifndef _gkGUIManager_h_
#define _gkGUIManager_h_

#include "utSingleton.h"
#include <gkString.h>

#include "gkGUI.h"
#include "gkInstancedManager.h"
#include "gkTweenManager.h"
#include "gkMathUtils.h"
#include "gkEngine.h"

#include "Script/Lua/gkLuaUtils.h"

class FileInterfaceOgre3D;
class SystemInterfaceOgre3D;

enum GuiElementType {
	GKGUI_DOCUMENT,
	GKGUI_DATASOURCE
};



class gkGUIManager :  public gkInstancedManager,public utSingleton<gkGUIManager>
{
private:
	FileInterfaceOgre3D*	m_rkFileInterface;
	SystemInterfaceOgre3D*	m_rkOgreSystem;

	class gkGUIEngineListener : public gkEngine::Listener {
	public:
		gkGUIEngineListener(gkGUIManager* guiMgr) : m_guiMgr(guiMgr),m_tweenMgr(guiMgr->getTweenManager()) {}
		virtual ~gkGUIEngineListener() {}
		virtual void tick(gkScalar rate)
		{
			m_tweenMgr->update(rate);
		}
		private:
		gkGUIManager* m_guiMgr;
		gkTweenManager* m_tweenMgr;
	};

	enum Mode {
		fromBlenderText, fromFile, none
	};

public:

	gkGUIManager();
	~gkGUIManager();
	
	void loadFont(const gkString& name);
	gkInstancedObject* getDocument(const gkString& name);
	void loadCursor(const gkString& name);
	void showCursor(bool show);
	bool isCursorVisible();
	void setCursor(const gkString& name);
	void setLuaTranslation(gkLuaEvent* evt);

	gkGUI* getGUI();
	void unloadGUI();
	void showDebugger(bool showit);
	bool debuggerVisible();
	gkTweenManager* getTweenManager() { return m_tweenManager;}
	
	inline gkString getPathPrefix() { return m_pathPrefix; }
	inline void setPathPrefix(const gkString& pathPrefix) { m_pathPrefix = pathPrefix; }

private:
	int m_currentType;
	bool m_cursorVisible;
	Mode m_grabMode;

	gkString m_tmpText;

	gkGUI* m_gui;
	gkTweenManager* m_tweenManager;
	gkGUIEngineListener* m_engineListener;
	gkGuiDocumentEventInstancer* eventListenerInstancer;

	void notifyResourceDestroyedImpl(gkResource* res);
	gkResource* createImpl(const gkResourceName& name, const gkResourceHandle& handle);

	gkString m_pathPrefix;

	UT_DECLARE_SINGLETON(gkGUIManager);
};

#endif // _gkGUIManager_h_
