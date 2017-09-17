/*
-------------------------------------------------------------------------------
    This file is part of gkGUIManager.
    http://gamekit.googlecode.com/

    Copyright (c) 2012 Alberto Torres Ruiz

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

#include "gkGUIManager.h"

#include <gkFont.h>
#include <gkFontManager.h>
#include <rocket/DecoratorAtlasImage.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

#include "rocket/SystemInterfaceOgre3D.h"
#include "rocket/FileInterfaceOgre3D.h"
#include "rocket/gkGUIDocumentRocket.h"
#include "gkGameObject.h"
#include "gkWindow.h"
#include "gkWindowSystem.h"
#include "gkPath.h"
#include "rocket/DecoratorTweenInstancer.h"
#include "gkTextFile.h"
#include "gkTextManager.h"

UT_IMPLEMENT_SINGLETON(gkGUIManager)

gkGUIManager::gkGUIManager()
	:	gkInstancedManager("GUI","GUIELEMENT"), m_currentType(GKGUI_DOCUMENT),
	 	m_gui(0), m_rkFileInterface(0), m_rkOgreSystem(0), m_grabMode(fromFile)
{
	m_tweenManager = new gkTweenManager;
	// Rocket initialisation.

	Ogre::ResourceGroupManager::getSingleton().createResourceGroup(DEFAULT_ROCKET_RESOURCE_GROUP);

	m_rkOgreSystem = new SystemInterfaceOgre3D();
	Rocket::Core::SetSystemInterface(m_rkOgreSystem);

	Rocket::Core::Initialise();
	Rocket::Controls::Initialise();

	m_rkFileInterface = new FileInterfaceOgre3D();
	Rocket::Core::SetFileInterface(m_rkFileInterface);

	// register eventlistener-instance that tries to create event-listeners for each on...-Event
	eventListenerInstancer = new gkGuiDocumentEventInstancer();
	Rocket::Core::Factory::RegisterEventListenerInstancer(eventListenerInstancer);
	eventListenerInstancer->RemoveReference();

	// register tween-decorator
	DecoratorTweenInstancer* tweenDecoratorInstancer = new DecoratorTweenInstancer();
	Rocket::Core::Factory::RegisterDecoratorInstancer("tween", tweenDecoratorInstancer);
	tweenDecoratorInstancer->RemoveReference();

	DecorationAtlasImageInstancer* atlasImageInstancer = new DecorationAtlasImageInstancer();
	Rocket::Core::Factory::RegisterDecoratorInstancer("atlasimage",atlasImageInstancer);
	atlasImageInstancer->RemoveReference();

	m_engineListener = new gkGUIEngineListener(this);
	gkEngine::getSingleton().addListener(m_engineListener);
}


gkGUIManager::~gkGUIManager()
{
	destroyAll();

	gkEngine::getSingleton().removeListener(m_engineListener);
	delete m_engineListener;

	gkWindowSystem::getSingleton().getMainWindow()->destroyGUI();
	Rocket::Core::Shutdown();

	delete m_rkOgreSystem;
	delete m_rkFileInterface;

	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(DEFAULT_ROCKET_RESOURCE_GROUP);

	delete m_tweenManager;
}

void gkGUIManager::unloadGUI()
{
	destroyAllInstances();
	gkInstancedManager::ResourceIterator resIter = getResourceIterator();
//	while (resIter.hasMoreElements())
//	{
//		gkGuiDocumentRocket* doc = static_cast<gkGuiDocumentRocket*>(resIter.getNext().second);
//		doc->_getDocument()->RemoveReference();
//	}
//	getGUI()->getContext()->RemoveReference();
}

void gkGUIManager::loadFont(const gkString& name){
	gkFont *fnt = gkFontManager::getSingleton().getByName<gkFont>(name);
	
	if (fnt)
	{
		Rocket::Core::FontDatabase::LoadFontFace((const unsigned char*)fnt->getData(), fnt->getSize());
	}
	else
	{
		gkPath fntPath(name);

		bool res = false;

		if (fntPath.exists())
		{
			res = Rocket::Core::FontDatabase::LoadFontFace(name.c_str());
		}

		if (!res)
		{
			gkLogger::write("Couldn't load font:"+name,true);
		}

	}
}

gkInstancedObject* gkGUIManager::getDocument(const gkString& name)
{
	m_grabMode = none;
	gkInstancedObject* doc = getByName<gkInstancedObject>(name);

	if (doc)
		return doc;

	gkTextFile* txt = (gkTextFile*)gkTextManager::getSingleton().getByName(name);

	m_currentType = GKGUI_DOCUMENT;

	if (txt) {
		m_grabMode = fromBlenderText;
		m_tmpText = txt->getText();
		return static_cast<gkInstancedObject*>(create(name));
	}


	gkPath path(name);

	if (path.exists())
	{
		gkLogger::write("load document:"+name,true);
		m_grabMode = fromFile;
		return static_cast<gkInstancedObject*>(create(name));
	}
	else
	{
		gkLogger::write("Unknown document:"+name,true);
	}


	return 0;
}

void gkGUIManager::showDebugger(bool showit)
{
		if (showit)
		{
			Rocket::Debugger::Initialise(getGUI()->getContext());
			Rocket::Debugger::SetVisible(true);

		} else
		{
			Rocket::Debugger::SetVisible(false);
		}
}

bool gkGUIManager::debuggerVisible()
{
	return Rocket::Debugger::IsVisible();
}

void gkGUIManager::loadCursor(const gkString& name)
{
//	Rocket::Core::ElementDocument* cursor = gkGUIManager::getSingleton().getGUI()->getContext()->LoadMouseCursor(name.c_str());
	Rocket::Core::ElementDocument* cursor = gkGUIManager::getSingleton().getGUI()->getContext()->LoadMouseCursor(name.c_str());
	if (cursor){
		cursor->RemoveReference();
		m_cursorVisible = true;
	}

}

void gkGUIManager::showCursor(bool show)
{
	gkGUIManager::getSingleton().getGUI()->getContext()->ShowMouseCursor(show);
	m_cursorVisible = show;
}
bool gkGUIManager::isCursorVisible()
{
	return m_cursorVisible;
}

void gkGUIManager::setCursor(const gkString& name)
{
	gkGUIManager::getSingleton().getGUI()->getContext()->SetMouseCursor(name.c_str());
}

gkResource* gkGUIManager::createImpl(const gkResourceName& name, const gkResourceHandle& handle)
{
	switch (m_currentType)
	{
		case GKGUI_DOCUMENT:     if (m_grabMode==fromBlenderText)
									return new gkGuiDocumentRocket(m_tmpText,this, name, handle);
								 else
									return new gkGuiDocumentRocket(this, name, handle);
	}
	return 0;
}



void gkGUIManager::notifyResourceDestroyedImpl(gkResource* res)
{
	gkGameObject* gobj = static_cast<gkGameObject*>(res);
	if (gobj)
		gobj->destroyInstance();
}

gkGUI* gkGUIManager::getGUI()
{
	if (!m_gui)
		m_gui = gkWindowSystem::getSingleton().getMainWindow()->getGUI();
	return m_gui;
}

void gkGUIManager::setLuaTranslation(gkLuaEvent* evt)
{
	m_rkOgreSystem->setLuaTranslation(evt);
}
