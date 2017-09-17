/*
-------------------------------------------------------------------------------
    This file is part of gkGUI.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Alberto Torres Ruiz

    Contributor(s): Thomas Trocha(dertom).
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

#ifndef _gkGUI_h_
#define _gkGUI_h_

#define DEFAULT_ROCKET_RESOURCE_GROUP "Rocket"

#include <Rocket/Core.h>
#include "utSingleton.h"
#include "gkCommon.h"
#include "Script/Lua/gkLuaScript.h"
#include "gkLogger.h"
#include "Rocket/Controls/DataSource.h"
#include <Rocket/Controls/DataFormatter.h>

class RocketRenderListener;
class RocketEventListener;
class RenderInterfaceOgre3D;

typedef Rocket::Core::Element gkGuiElement;

class gkGUI
{
protected:

	RocketRenderListener*	m_rkRenderListener;
	RocketEventListener*	m_rkEventListener;	
	Rocket::Core::Context*	m_rkContext;
	RenderInterfaceOgre3D* rkOgreRenderer;
	gkWindow*		m_window;
	Rocket::Core::ElementDocument* m_document;
public:
	gkGUI(gkWindow* window);
	~gkGUI();
	
	GK_INLINE Rocket::Core::Context* getContext() {return m_rkContext;}

	void onWindowResize();
	static gkGuiElement* getCurrentElement() { return CURRENT_GUIEVENT_ELEMENT; }
	static gkString getStringEventParameter(const gkString& name);
	static int getIntEventParameter(const gkString& name);

	static Rocket::Core::Event* CURRENT_GUIEVENT;
	static gkGuiElement* CURRENT_GUIEVENT_ELEMENT;
	static gkGuiElement* CURRENT_GUIEVENT_TARGET;
	static gkGuiElement* CURRENT_RCSSEVENT_ELEMENT;
	static gkGuiElement* CURRENT_DRAG_ELEMENT;
	static gkString CURRENT_RCSSEVENT_NAME;
	static gkString CURRENT_RCSSEVENT_PROPERTY;


	// reset the mouseposition to reset hover/effects
	void _touchMouse();
};

class gkGuiDocumentEventInstancer : public Rocket::Core::EventListenerInstancer
{
public:
	virtual Rocket::Core::EventListener* InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element);

	/// Releases this event listener instancer.
	void Release() {
		gkLogger::write("Release gkGuiDocumentEventInstancer!",true);
	};

	void ReleaseEvent(Rocket::Core::Event* event)
	{
		gkLogger::write("Release Event:"+gkString(event->GetType().CString()),true);
	}

};


class gkGuiDocumentEventListener : public Rocket::Core::EventListener
{
public:
	gkGuiDocumentEventListener(const gkString& scriptText,Rocket::Core::Element* element);

	void ProcessEvent(Rocket::Core::Event& event);
private:
	enum Status {
		NOT_INIT,VALID,INVALID
	};

	gkString m_attribute;
	gkLuaScript* m_script;
	Status m_status;
	Rocket::Core::Element* m_element;
};

class gkGuiDataSource : public Rocket::Controls::DataSource
{
public:
	gkGuiDataSource(const gkString& dsName) : Rocket::Controls::DataSource(Rocket::Core::String(dsName.c_str()))
	{}
	~gkGuiDataSource(){}

};

class gkGuiDataFormatter : public Rocket::Controls::DataFormatter
{
public:

	gkGuiDataFormatter(const gkString& formatterName) : Rocket::Controls::DataFormatter(formatterName.c_str())
	{}

	~gkGuiDataFormatter(){}
};

#endif // _gkGUI_h_

