/*
-------------------------------------------------------------------------------
    This file is part of gkGUI.
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

#include "gkWindow.h"
#include "gkGUI.h"

#include <gkFont.h>
#include <gkFontManager.h>
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

#include "rocket/NewRenderInterfaceOgre3D.h"
#include "rocket/RocketRenderListener.h"
#include "rocket/RocketEventListener.h"

gkGuiElement* gkGUI::CURRENT_GUIEVENT_ELEMENT = 0;
gkGuiElement* gkGUI::CURRENT_GUIEVENT_TARGET = 0;
gkGuiElement* gkGUI::CURRENT_DRAG_ELEMENT = 0;
Rocket::Core::Event* gkGUI::CURRENT_GUIEVENT = 0;

gkGuiElement* gkGUI::CURRENT_RCSSEVENT_ELEMENT = 0;
gkString gkGUI::CURRENT_RCSSEVENT_NAME("");
gkString gkGUI::CURRENT_RCSSEVENT_PROPERTY("");

gkGUI::gkGUI(gkWindow *window):
	m_window(window),
	m_document(0)
{
	
	rkOgreRenderer = new RenderInterfaceOgre3D(window->getWidth(), window->getHeight());

	m_rkContext = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(window->getWidth(), window->getHeight()), rkOgreRenderer);

	m_rkEventListener  = new RocketEventListener(window, m_rkContext);
	m_rkRenderListener = new RocketRenderListener(window->getRenderWindow(), m_rkContext);

}

gkGUI::~gkGUI()
{
	Rocket::Core::RenderInterface *ri;

	if (m_rkContext)
	{
		ri = m_rkContext->GetRenderInterface();
		m_rkContext->RemoveReference();
		delete ri;
	}
	delete m_rkEventListener;
	delete m_rkRenderListener;

}


Rocket::Core::EventListener* gkGuiDocumentEventInstancer::InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element)
{
	Rocket::Core::EventListener* newListener = new gkGuiDocumentEventListener(value.CString(),element);
	return newListener;
}

gkGuiDocumentEventListener::gkGuiDocumentEventListener(const gkString& scriptText,Rocket::Core::Element* element)
	: m_element(element)
{
	if (!scriptText.empty())
	{
		gkString elementId(gkUtils::getUniqueName("gui_event"));

		// get the script from the markup and create a lua script
		m_script = gkLuaManager::getSingleton().createFromText(elementId,scriptText+" ");

		if (m_script)
			m_status = VALID;
		else
			m_status = INVALID;
	}
}

void gkGuiDocumentEventListener::ProcessEvent(Rocket::Core::Event& event)
{
	// did creating a script from the rml-markus fail-skip the event
	if (m_status==INVALID)
		return;


	if (m_status == VALID)
	{
		gkGUI::CURRENT_GUIEVENT_ELEMENT = m_element;
		gkGUI::CURRENT_GUIEVENT_TARGET = event.GetTargetElement();
		gkGUI::CURRENT_GUIEVENT = &event;

		gkGUI::CURRENT_DRAG_ELEMENT = static_cast< Rocket::Core::Element* >(event.GetParameter< void* >("drag_element", NULL));
		m_script->execute();
		gkGUI::CURRENT_GUIEVENT_TARGET = 0;
		gkGUI::CURRENT_DRAG_ELEMENT = 0;
		gkGUI::CURRENT_GUIEVENT_ELEMENT = 0;
		gkGUI::CURRENT_GUIEVENT = 0;
	}
}

// FIXME: Not working as thought, yet
gkString gkGUI::getStringEventParameter(const gkString& name) {
	if (gkGUI::CURRENT_GUIEVENT) {
		return gkGUI::CURRENT_GUIEVENT->GetParameter<Rocket::Core::String>(name.c_str(),Rocket::Core::String("")).CString();
	}

	return "";
}

int gkGUI::getIntEventParameter(const gkString& name) {
	if (gkGUI::CURRENT_GUIEVENT) {
		return gkGUI::CURRENT_GUIEVENT->GetParameter<int>(name.c_str(),0);
	}

	return 0;
}

void gkGUI::onWindowResize()
{
	rkOgreRenderer->onWindowResize();
}

void gkGUI::_touchMouse()
{
	m_rkEventListener->mouseMoved(*m_window->getMouse());
}
