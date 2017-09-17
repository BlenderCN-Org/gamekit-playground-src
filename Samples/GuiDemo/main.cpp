/*
-------------------------------------------------------------------------------
    This file is part of GuiDemo.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 harkon.kr

    Contributor(s): Alberto Torres Ruiz
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product m_documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#include "OgreKit.h"

#include "GUI/gkGUI.h"
#include "GUI/gkGUIManager.h"
#include "Rocket/Controls.h"
#include "../../Engine/GUI/rocket/gkGUIDocumentRocket.h"

#define GUIDEMO_GROUP_NAME	"Rocket"
#define DEMO_BLEND_FILE		"logo_text.blend"
#define ASSETS_BLEND_FILE	"assets.blend"
#define ROCKET_CURSOR_PAGE	"cursor.rml"
#define ROCKET_DEMO_PAGE	"demo.rml"
#define DEMO_PAGE_BUTTON_ID "submit1"
#define DEMO_PAGE_INPUT_ID	"input1"

class GuiDemo : 
	public gkCoreApplication, 
	public gkWindowSystem::Listener,
	public Rocket::Core::EventListener
{
protected:
	gkString    m_blend;
	gkScene*    m_scene;
	gkGuiDocumentRocket* m_doc;
	void loadGUI();

public:
	GuiDemo();
	virtual ~GuiDemo();

	int setup(int argc, char** argv);

	virtual void keyReleased(const gkKeyboard& key, const gkScanCode& sc);

	virtual void ProcessEvent(Rocket::Core::Event& event);

private:

	virtual bool setup(void);
};



GuiDemo::GuiDemo()
	:	m_blend(DEMO_BLEND_FILE), 
		m_scene(0), m_doc(0)
{
}


GuiDemo::~GuiDemo()
{
}

int GuiDemo::setup(int argc, char** argv)
{
	int winsize_x		= 800;
	int winsize_y		= 600;
	m_prefs.wintitle	= gkString("OgreKit GuiDemo (Press Escape to exit)[") + m_blend + gkString("]");
 	m_prefs.grabInput = false;

	return 0;
}



bool GuiDemo::setup(void)
{
	gkBlendFile* blend = gkBlendLoader::getSingleton().loadFile(gkUtils::getFile(m_blend), "", GUIDEMO_GROUP_NAME);
	if (!blend)
	{
		gkPrintf("File loading failed.\n");
		return false;
	}

	m_scene = blend->getMainScene();
	if (!m_scene)
	{
		gkPrintf("No usable scenes found in blend.\n");
		return false;
	}



	m_scene->createInstance();

	// add input hooks
	gkWindowSystem::getSingleton().addListener(this);

	gkBlendLoader::getSingleton().loadFile(gkUtils::getFile(ASSETS_BLEND_FILE), "", GUIDEMO_GROUP_NAME);

	//--

	loadGUI();


	return true;
}


void GuiDemo::loadGUI()
{
	GK_ASSERT(m_scene);

	// Install fonts
	gkGUIManager *gm = gkGUIManager::getSingletonPtr();

	gm->loadFont("Delicious-Roman");
	gm->loadFont("Delicious-Bold");
	gm->loadFont("Delicious-Italic");
	gm->loadFont("Delicious-BoldItalic");

	m_doc = static_cast<gkGuiDocumentRocket*>(gm->getDocument(ROCKET_DEMO_PAGE));
	m_doc->createInstance();

	gm->loadCursor(ROCKET_CURSOR_PAGE);
	gm->getGUI()->getContext()->AddEventListener("click",this);

//	gm->setDebug(gm->getGUI());
}


void GuiDemo::ProcessEvent(Rocket::Core::Event& event)
{
	GK_ASSERT(m_document);



	Rocket::Core::Element* elm = m_doc->_getDocument()->GetElementById(DEMO_PAGE_INPUT_ID);

	if (elm)
	{
		printf("button clicked: %s\n", static_cast<Rocket::Controls::ElementFormControl*>(elm)->GetValue().CString());
	}
}



void GuiDemo::keyReleased(const gkKeyboard& key, const gkScanCode& sc)
{
	if (sc == KC_ESCKEY)
	{
		m_engine->requestExit();
	}
}


int main(int argc, char** argv)
{
	GuiDemo okit;
	if (okit.setup(argc, argv) != 0)
	{
		// error
		return -1;
	}

	// Launch runtime
	okit.run();
	return 0;
}
