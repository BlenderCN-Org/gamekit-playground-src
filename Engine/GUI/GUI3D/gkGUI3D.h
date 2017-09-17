/*
 * gkGUI3D.h
 *
 *  Created on: Aug 23, 2012
 *      Author: ttrocha
 */

#ifndef gkGUI3D_H_
#define gkGUI3D_H_

#include <Gui3DScreenPanel.h>
#include "Gui3D.h"
#include "Gui3DPanel.h"
#include "Gui3DCaption.h"
#include "Gui3DPanelColors.h"
#include "OISKeyboard.h"
#include "OISMouse.h"
//#include "Gorilla/OgreConsoleForGorilla.h"
#include "OgreStringVector.h"
#include "OgreViewport.h"
#include "OgreSceneManager.h"
#include "OgreCamera.h"
#include "OISInputManager.h"
#include "utSingleton.h"
#include "utTypes.h"
#include "gkScene.h"
#include "GUI/GUI3D/gkGUI3D.h"

#include "OgreConsoleForGorilla.h"
//#include "Script/ttScript.h"

class gsGUI3DElement;

class gkGUI3D : public OIS::MouseListener,public OIS::KeyListener, public utSingleton<gkGUI3D> {
public:
	typedef utList<Gui3D::Panel*> Panel3DList;
	typedef utList<Gui3D::ScreenPanel*> Panel2DList;

	typedef utHashTable<utPointerHashKey,gsGUI3DElement*> GuiScriptMapping;
	gkGUI3D();
	virtual ~gkGUI3D();

	UT_DECLARE_SINGLETON(gkGUI3D);

	Gorilla::Screen* initGorilla(const gkString& atlas,gkScene* scene=0);
	void initConsole();

	void addPanel3d(Gui3D::Panel* panel);
	void removePanel3d(Gui3D::Panel* panel);
	void addPanel2d(Gui3D::ScreenPanel* panel);
	void removePanel2d(Gui3D::ScreenPanel* panel);

	Gui3D::PanelColors* getPanelColors() {
		return mPanelColors;
	}

	void showConsole(bool showIt);


	Gorilla::Screen* createScreen(const gkString& screenName,const gkString& atlas="dejavu");
	void addScriptMapping(Gui3D::PanelElement* panelElem, gsGUI3DElement* scriptElem);
	gsGUI3DElement* getScriptMapping(Gui3D::PanelElement* elem);
	void removeScriptMapping(Gui3D::PanelElement* panelElem);
	Gui3D::Gui3D* mGui3D;
	Gorilla::Screen* mScreen;
	Gui3D::PanelColors* mPanelColors;
	void setMouseCursor(const gkString& sprite) {
		mCursorName = sprite;
		if (mMousePointer) { mMousePointer->background_image(sprite);}
	}

	void showCursor(bool showCursor);

	OgreConsole* getConsole(bool init=false);

private:
	OgreConsole* mConsole;


	typedef utList<gkLuaEvent*> ConsoleCommands;

	void addConsoleCommmand(const gkString& commandName,gkLuaEvent* evt);

	int mClicksOnButton;

	bool buttonPressed(Gui3D::PanelElement* e);

	bool checkboxChecked(Gui3D::PanelElement* e);
	bool comboboxValueChanged(Gui3D::PanelElement* e);
	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseMoved(const OIS::MouseEvent &arg);
	Ogre::Vector2 getScreenCenterMouseDistance();



	// The main panel
//	Gui3D::Panel* mPanel;

	// Keep track of some captions to modify their contents on callback
	Gui3D::Caption* captionButton;
	Gui3D::Caption* captionChecked;
	Gui3D::Caption* captionCombobox;


	// As Gui3D doesn't fully abstract Gorilla, you still have to deal with it.
	// See http://www.valentinfrechaud.fr/Gui3DWiki/index.php/Gui3D_and_Gorilla for more infos.
	Gorilla::Layer* mMousePointerLayer;
	Gorilla::Rectangle* mMousePointer;
	Ogre::String mCursorName;
	Ogre::Vector2 mNormalizedMousePosition;

	Ogre::Viewport* mViewport;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	OIS::InputManager* mInputManager;
//	OIS::Mouse* mMouse;
//	OIS::Keyboard* mKeyboard;

	// For the smooth movment when moving the mouse
	Ogre::Vector3 cameraDirection;

	Panel3DList m_panel3dList;
	Panel2DList m_panel2dList;
	GuiScriptMapping m_scriptMapping;
};


/*! struct. MySimpleDemoPanelColors
	desc.
		Example of customisation of Gui3D colors
*/
struct MySimpleDemoPanelColors : public Gui3D::PanelColors
{
	MySimpleDemoPanelColors();
};

void whoami(Ogre::StringVector&);


#endif /* gkGUI3D_H_ */
