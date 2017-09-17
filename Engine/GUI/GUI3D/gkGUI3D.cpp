/*
 * gkGUI3D.cpp
 *
 *  Created on: Aug 23, 2012
 *      Author: ttrocha
 */

#include "gkGUI3D.h"
#include "gkWindowSystem.h"
#include "gkEngine.h"
#include "gkScene.h"
#include "gkWindow.h"
#include "gkViewport.h"
#include "gkCamera.h"
#include <vector>
#include "gkDebugScreen.h"
#include "gkValue.h"
#include "Gui3DPanelColors.h"

using Gui3D::PanelColors;

gkGUI3D::gkGUI3D()
	:		mGui3D(0), mScreen(0), mClicksOnButton(0),captionButton(0),captionChecked(0),captionCombobox(0),
	 		mMousePointerLayer(0), mMousePointer(0),  mViewport(0),mSceneMgr(0),mCamera(0), mInputManager(0),
	 		mPanelColors(0)
	 		,mConsole(0)
{
//	gkLogger::enable("gk",true);
	// TODO Auto-generated constructor stub
}

gkGUI3D::~gkGUI3D() {
	if (mConsole)
		delete mConsole;
}

void gkGUI3D::initConsole() {
    mConsole = new OgreConsole();
    mConsole->init(mScreen);
//    mConsole->addCommand(Ogre::String("whoami"), whoami);
}

Gorilla::Screen* gkGUI3D::createScreen(const gkString& screenName, const gkString& atlas)
{
	Gorilla::Screen* screen = mGui3D->getScreen(screenName);

	if(!screen)
		screen = mGui3D->createScreen(mViewport, atlas, screenName);

	return screen;
}


OgreConsole* gkGUI3D::getConsole(bool init)
{
	if (mConsole)
		return mConsole;

	if (init)
		initConsole();

	return mConsole;
}

void gkGUI3D::showConsole(bool showIt)
{
	if (!mConsole)
	{
		initConsole();
	}
	mConsole->setVisible(showIt);
}





void gkGUI3D::showCursor(bool showCursor)
{
	if (mMousePointerLayer)
	{
		mMousePointerLayer->setVisible(showCursor);
	}
}

Gorilla::Screen* gkGUI3D::initGorilla(const gkString& atlas,gkScene* scene) {

	if (!scene)
	{
		scene = gkEngine::getSingleton().getActiveScene();
		mViewport = scene->getMainCamera()->getCamera()->getViewport();
	}
	else
		mViewport = gkWindowSystem::getSingleton().getMainWindow()->getViewport(0)->getViewport();

	mSceneMgr = scene->getManager();
	mCamera = scene->getMainCamera()->getCamera();
	mInputManager = gkEngine::getSingleton().getActiveScene()->getDisplayWindow()->getInputManager();
	gkEngine::getSingleton().getActiveScene()->getDisplayWindow()->_setOISMouseListener(this);
	gkEngine::getSingleton().getActiveScene()->getDisplayWindow()->_setOISKeyboardListener(this);

	mPanelColors = new MySimpleDemoPanelColors();

	mGui3D = new Gui3D::Gui3D(mPanelColors);

	mScreen = mGui3D->createScreen(mViewport, atlas, "mainScreen");
//	mKeyboard->setEventCallback(this);
//	mMouse->setEventCallback(this);
//	mMouse->getMouseState().width = mViewport->getActualWidth();
//	mMouse->getMouseState().height = mViewport->getActualHeight();

	// Create a layer for the mousePointer
	mNormalizedMousePosition = Ogre::Vector2(0.5, 0.5);

	mMousePointerLayer = mGui3D->getScreen("mainScreen")->createLayer(0);
	mMousePointerLayer->setVisible(false);

	if (!mMousePointer)
	{
		mMousePointer = mMousePointerLayer->createRectangle(mViewport->getActualWidth()/2,
			mViewport->getActualHeight()/2, 12, 18);
		mMousePointer->background_image("mousepointer");
	}

	return mScreen;

//	Gui3D::Panel* panel = new Gui3D::Panel(
//		mGui3D, mSceneMgr, Ogre::Vector2(4, 4), 10, "simpleDemo", "test_panel");
//
//	PanelColors* pcols = panel->getPanelColors();
//
//	panel->makeCaption(5, -30, 390, 30, "Simple Demo Panel");
//
//	panel->makeButton(100, 10, 200, 30, "click me!");
//	panel->makeCaption(10, 60, 100, 30, "check it ! ");
//	panel->makeCheckbox(110, 60, 30, 30)->setSelecteStateChangedCallback(this, &gkGUI3D::checkboxChecked);
//
//	std::vector<Ogre::String> items;
//	items.push_back("item1");
//	items.push_back("item2");
//	items.push_back("item3");
//	items.push_back("item4");
//
//	items.push_back("item5");
//	items.push_back("item6");
//	panel->makeCombobox(10, 110, 380, 100, items, 4)->setValueChangedCallback(this, &gkGUI3D::comboboxValueChanged);
//
//	captionButton = panel->makeCaption(10, 250, 380, 30, "The button hasn't been clicked yet");
//	captionChecked = panel->makeCaption(10, 300, 380, 30, "The checkbox hasn't been checked");
//	captionCombobox = panel->makeCaption(10, 350, 380, 30, "No selected values yet");
//

//	panel->mNode->setPosition(0, 2.1, -8);




}
MySimpleDemoPanelColors::MySimpleDemoPanelColors()
	: Gui3D::PanelColors()
{
	// General
	transparent = Gorilla::rgb(0, 0, 0, 0);

	// Panel
	//panelBackgroundSpriteName = "panelBackground";

	// Button
//	buttonInactiveSpriteName = "buttonInactive";
//	buttonOveredSpriteName = "buttonOvered";
//	buttonNotOveredSpriteName = "buttonNotOvered";
//	buttonClickedSpriteName = "buttonClicked";

	buttonBackgroundClickedGradientType = Gorilla::Gradient_NorthSouth;
	buttonBackgroundClickedGradientStart = Gorilla::rgb(55, 255, 255, 200);
	buttonBackgroundClickedGradientEnd = Gorilla::rgb(55, 255, 255, 170);

	buttonBackgroundOveredGradientType = Gorilla::Gradient_NorthSouth;
	buttonBackgroundOveredGradientStart = Gorilla::rgb(255, 255, 255, 128);
	buttonBackgroundOveredGradientEnd = Gorilla::rgb(255, 255, 255, 100);

	buttonBackgroundNotOveredGradientType = Gorilla::Gradient_NorthSouth;
	buttonBackgroundNotOveredGradientStart = Gorilla::rgb(255, 255, 255, 80);
	buttonBackgroundNotOveredGradientEnd = Gorilla::rgb(255, 255, 255, 50);

	buttonBackgroundInactiveGradientType = Gorilla::Gradient_NorthSouth;
	buttonBackgroundInactiveGradientStart = Gorilla::rgb(255, 255, 255, 15);
	buttonBackgroundInactiveGradientEnd = Gorilla::rgb(255, 255, 255, 5);

	buttonText = Gorilla::rgb(255, 0, 0, 255);
	buttonTextInactive = Gorilla::rgb(255, 255, 255, 70);
	buttonTextSize = 24;

	buttonBorder = Gorilla::rgb(70, 70, 70, 50);
	buttonBorderHighlight = Gorilla::rgb(255, 100, 100, 170);
	buttonBorderInactive = Gorilla::rgb(70, 70, 70, 15);
	buttonBorderSize = 1;

	// Checkbox
	checkboxOveredBackgroundSpriteName = "checkboxOvered";
	checkboxNotOveredBackgroundSpriteName = "checkboxNotOvered";
	checkboxCheckedNotOveredBackgroundSpriteName = "checkboxCheckedNotOvered";
	checkboxCheckedOveredBackgroundSpriteName = "checkboxCheckedOvered";

	// Combobox
	comboboxButtonPreviousOveredSpriteName = "comboboxpreviouselementbuttonovered";
	comboboxButtonPreviousNotOveredSpriteName = "comboboxpreviouselementbuttonnotovered";
	comboboxButtonPreviousInactiveSpriteName = "comboboxpreviouselementbuttoninactive";

	comboboxButtonNextOveredSpriteName = "comboboxnextelementbuttonovered";
	comboboxButtonNextNotOveredSpriteName = "comboboxnextelementbuttonnotovered";
	comboboxButtonNextInactiveSpriteName = "comboboxnextelementbuttoninactive";

	comboboxBackgroundGradientType = Gorilla::Gradient_NorthSouth;
	comboboxBackgroundGradientStart = Gorilla::rgb(55, 255, 255, 0);
	comboboxBackgroundGradientEnd = Gorilla::rgb(55, 255, 255, 0);

	comboboxOveredElement = Gorilla::rgb(55, 255, 255, 100);
	comboboxNotOveredElement = Gorilla::rgb(55, 255, 255, 30);
	comboboxSelectedElement = Gorilla::rgb(20, 20, 120, 200);

	comboboxText = Ogre::ColourValue::Black;
	comboboxTextSize = 14;

	comboboxBorder = Gorilla::rgb(70, 70, 70, 50);
	comboboxBorderHighlight = Gorilla::rgb(255, 100, 100, 170);
	comboboxBorderSize = 0;

	// InlineSelector
	inlineselectorButtonPreviousOveredSpriteName = "inlineselectorPreviousButtonOvered";
	inlineselectorButtonPreviousNotOveredSpriteName = "inlineselectorPreviousButtonNotOvered";
	inlineselectorButtonPreviousInactiveSpriteName = "inlineselectorPreviousButtonInactive";

	inlineselectorButtonNextOveredSpriteName = "inlineselectorNextButtonOvered";
	inlineselectorButtonNextNotOveredSpriteName = "inlineselectorNextButtonNotOvered";
	inlineselectorButtonNextInactiveSpriteName = "inlineselectorNextButtonInactive";

	inlineselectorBackgroundGradientType = Gorilla::Gradient_NorthSouth;
	inlineselectorBackgroundGradientStart = transparent;
	inlineselectorBackgroundGradientEnd = transparent;

	inlineselectorText = Ogre::ColourValue::Black;
	inlineselectorTextSize = 14;

	inlineselectorBorder = transparent;
	inlineselectorBorderHighlight = Gorilla::rgb(255, 100, 100, 170);
	inlineselectorBorderSize = 1;

	// Listbox
	// same value as the combobox
	listboxButtonPreviousOveredSpriteName = comboboxButtonPreviousOveredSpriteName;
	listboxButtonPreviousNotOveredSpriteName = comboboxButtonPreviousNotOveredSpriteName;
	listboxButtonPreviousInactiveSpriteName = comboboxButtonPreviousInactiveSpriteName;

	listboxButtonNextOveredSpriteName = comboboxButtonNextOveredSpriteName;
	listboxButtonNextNotOveredSpriteName = comboboxButtonNextNotOveredSpriteName;
	listboxButtonNextInactiveSpriteName = comboboxButtonNextInactiveSpriteName;

	listboxBackgroundGradientType = comboboxBackgroundGradientType;
	listboxBackgroundGradientStart = comboboxBackgroundGradientStart;
	listboxBackgroundGradientEnd = comboboxBackgroundGradientEnd;

	listboxOveredElement = comboboxOveredElement;
	listboxNotOveredElement = comboboxNotOveredElement;
	listboxOveredSelectedElement = comboboxSelectedElement;
	listboxNotOveredSelectedElement = comboboxSelectedElement + Gorilla::rgb(0, 0, 0, -50);

	listboxText = comboboxText;
	listboxTextSize = 14;

	listboxBorder = comboboxBorder;
	listboxBorderHighlight = comboboxBorderHighlight;
	listboxBorderSize = comboboxBorderSize;

	// Scrollbar
	scrollbarCursorOveredSpriteName = "scrollbarOvered";
	scrollbarCursorNotOveredSpriteName = "scrollbarNotOvered";
	scrollbarCursorSelectedSpriteName = "scrollbarSelected";

	scrollbarBackgroundGradientType = Gorilla::Gradient_NorthSouth;
	scrollbarBackgroundGradientStart = Gorilla::rgb(255, 255, 255, 80);
	scrollbarBackgroundGradientEnd = Gorilla::rgb(255, 255, 255, 40);

	scrollbarProgressbarGradientType = Gorilla::Gradient_WestEast;
	scrollbarProgressbarGradientStart = Gorilla::rgb(120, 20, 120, 40);
	scrollbarProgressbarGradientEnd = Gorilla::rgb(120, 20, 120, 80);

	scrollbarText = Ogre::ColourValue::Black;
	scrollbarTextSize = 14;

	scrollbarBorder = Gorilla::rgb(70, 70, 70, 50);
	scrollbarBorderHighlight = Gorilla::rgb(255, 100, 100, 170);
	scrollbarCursorBorder =  Gorilla::rgb(120, 20, 120, 150);
	scrollbarBorderSize = 1;
	scrollbarCursorBorderSize = 1;

	// TextZone
	textzoneBackgroundOveredGradientType = Gorilla::Gradient_NorthSouth;
	textzoneBackgroundOveredGradientStart = Gorilla::rgb(55, 255, 255, 128);
	textzoneBackgroundOveredGradientEnd = Gorilla::rgb(55, 255, 255, 90);

	textzoneBackgroundNotOveredGradientType = Gorilla::Gradient_NorthSouth;
	textzoneBackgroundNotOveredGradientStart = Gorilla::rgb(55, 255, 255, 50);
	textzoneBackgroundNotOveredGradientEnd = Gorilla::rgb(55, 255, 255, 20);

	textzoneBackgroundSelectedGradientType = Gorilla::Gradient_NorthSouth;
	textzoneBackgroundSelectedGradientStart = Gorilla::rgb(20, 200, 200, 170);
	textzoneBackgroundSelectedGradientEnd = Gorilla::rgb(20, 200, 200, 140);

	textzoneText = Ogre::ColourValue::Black;
	textzoneTextSize = 24;

	textzoneBorder = Gorilla::rgb(70, 70, 70, 50);
	textzoneBorderHighlight = Gorilla::rgb(255, 100, 100, 255);
	textzoneBorderSelected = Gorilla::rgb(255, 200, 200, 200);
	textzoneBorderSize = 1;

	// Caption
	captionBackgroundGradientType = Gorilla::Gradient_NorthSouth;
	captionBackgroundGradientStart = Gorilla::rgb(20, 200, 200, 170);
	captionBackgroundGradientEnd = Gorilla::rgb(20, 200, 200, 140);

	captionBorder = transparent;
	captionText = Ogre::ColourValue::Black;
	captionTextSize = 14;
	captionBorderSize = 0;

	// ProgressBar
	progressbarBackgroundGradientType = Gorilla::Gradient_NorthSouth;
	progressbarBackgroundGradientStart = Gorilla::rgb(25, 255, 255, 50);
	progressbarBackgroundGradientEnd = Gorilla::rgb(25, 255, 255, 70);

	progressbarLoadingBarGradientType = Gorilla::Gradient_NorthSouth;
	progressbarLoadingBarGradientStart = Gorilla::rgb(12, 20, 120, 200);
	progressbarLoadingBarGradientEnd = Gorilla::rgb(12, 20, 120, 150);

	progressbarText = Ogre::ColourValue::Black;
	progressbarTextSize = 14;

	progressbarBorder = Gorilla::rgb(70, 70, 70, 50);
	progressbarBorderHighlight = Gorilla::rgb(255, 100, 100, 170);
	progressbarBorderSize = 1;

	panelCursorSpriteName = "mousepointer";

}

Ogre::Vector2 gkGUI3D::getScreenCenterMouseDistance()
{
	if (mMousePointer)
	{
		Ogre::Real posX = (mMousePointer->position().x - mViewport->getActualWidth())
			/ mViewport->getActualWidth();
		Ogre::Real posY = (mMousePointer->position().y - mViewport->getActualHeight())
			/ mViewport->getActualHeight();

		return Ogre::Vector2(posX + 0.5, posY + 0.5);
	}
	return Ogre::Vector2(0,0);
}

bool gkGUI3D::mouseMoved(const OIS::MouseEvent &arg)
{
	// Set the new camera smooth direction movement
	Ogre::Vector2 distance(getScreenCenterMouseDistance());
//	mCamera->setDirection(cameraDirection
//		+ Ogre::Vector3(distance.x, -distance.y, 0) / 30);
//	gkLogger::write("MouseMoved: X:"+gkToString(arg.state.X.abs)+" Y:"+gkToString(arg.state.Y.abs)+" Z:"+gkToString(arg.state.Z.abs)+" Btn:"+gkToString(arg.state.buttons)+" width:"+gkToString(arg.state.width)+" height:"+gkToString(arg.state.height),true);

	// Raycast for the actual panel
	Ogre::Real xMove = static_cast<Ogre::Real>(arg.state.X.abs);
	Ogre::Real yMove = static_cast<Ogre::Real>(arg.state.Y.abs);

	mNormalizedMousePosition.x = xMove / mViewport->getActualWidth();
	mNormalizedMousePosition.y = yMove / mViewport->getActualHeight();

	mNormalizedMousePosition.x = std::max<Ogre::Real>(mNormalizedMousePosition.x, 0);
	mNormalizedMousePosition.y = std::max<Ogre::Real>(mNormalizedMousePosition.y, 0);
	mNormalizedMousePosition.x = std::min<Ogre::Real>(mNormalizedMousePosition.x, 1);
	mNormalizedMousePosition.y = std::min<Ogre::Real>(mNormalizedMousePosition.y, 1);


	if (mMousePointer)
	{
		mMousePointer->position(
			mNormalizedMousePosition.x * mViewport->getActualWidth(),
			mNormalizedMousePosition.y * mViewport->getActualHeight());
	}


	Panel2DList::Iterator iter2d(m_panel2dList);
	while (iter2d.hasMoreElements())
	{
		Gui3D::ScreenPanel* panel = iter2d.getNext();

		bool evtConsumed = panel->injectMouseMoved(mNormalizedMousePosition.x * mViewport->getActualWidth() , mNormalizedMousePosition.y * mViewport->getActualHeight());

//		if (evtConsumed)
//			return true;
	}


	Panel3DList::Iterator iter(m_panel3dList);
	while (iter.hasMoreElements())
	{
		Gui3D::Panel* panel = iter.getNext();

		bool evtConsumed = panel->injectMouseMoved(mCamera->getCameraToViewportRay(
	            mNormalizedMousePosition.x, mNormalizedMousePosition.y));

//		if (evtConsumed)
//			return true;
	}

	return false;
}
bool gkGUI3D::buttonPressed(Gui3D::PanelElement* e)
{
	mClicksOnButton++;
	std::ostringstream s;
	s << "The button has been pressed " << mClicksOnButton << " times";
	captionButton->text(s.str());
	return true;
}

bool gkGUI3D::checkboxChecked(Gui3D::PanelElement* e)
{
	Gui3D::Checkbox* c = (Gui3D::Checkbox*) e;
	if (c->getChecked())
		captionChecked->text("The checkbox is checked");
	else
		captionChecked->text("The checkbox isn't checked");
	return true;
}

bool gkGUI3D::comboboxValueChanged(Gui3D::PanelElement* e)
{
	Gui3D::Combobox* c = (Gui3D::Combobox*) e;
	std::ostringstream s;
	s << "Combobox selected value : " << c->getValue();
	captionCombobox->text(s.str());
	return true;
}

bool gkGUI3D::keyPressed(const OIS::KeyEvent &e)
{
	Panel2DList::Iterator iter2d(m_panel2dList);
	while (iter2d.hasMoreElements())
	{
		Gui3D::ScreenPanel* panel = iter2d.getNext();

		panel->injectKeyPressed(e);
	}

	Panel3DList::Iterator iter(m_panel3dList);
	while (iter.hasMoreElements())
	{
		Gui3D::Panel* panel = iter.getNext();

		panel->injectKeyPressed(e);

	}
	if (mConsole && mConsole->isVisible())
		mConsole->onKeyPressed(e);

	return true;
}

bool gkGUI3D::keyReleased(const OIS::KeyEvent &e)
{
	Panel2DList::Iterator iter2d(m_panel2dList);
	while (iter2d.hasMoreElements())
	{
		Gui3D::ScreenPanel* panel = iter2d.getNext();

		panel->injectKeyReleased(e);
	}

	Panel3DList::Iterator iter(m_panel3dList);
	while (iter.hasMoreElements())
	{
		Gui3D::Panel* panel = iter.getNext();

		panel->injectKeyReleased(e);

	}

	if (e.key == OIS::KC_F1)
	   {
        if (mConsole)
        	mConsole->setVisible(!mConsole->isVisible());
	    return true;
	   }
	return true;
}

bool gkGUI3D::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
//	gkLogger::write("MousePressed:"+gkToString((size_t)evt.device),true);
//	gkLogger::write("MousePressed: X:"+gkToString(evt.state.X.abs)+" Y:"+gkToString(evt.state.Y.abs)+" Z:"+gkToString(evt.state.Z.abs)+" Btn:"+gkToString(evt.state.buttons)+" width:"+gkToString(evt.state.width)+" height:"+gkToString(evt.state.height),true);

	Panel2DList::Iterator iter2d(m_panel2dList);
	mouseMoved(evt);
	while (iter2d.hasMoreElements())
	{
		Gui3D::ScreenPanel* panel = iter2d.getNext();
		panel->injectMousePressed(evt, id);
	}


	Panel3DList::Iterator iter(m_panel3dList);
	mouseMoved(evt);
	while (iter.hasMoreElements())
	{
		Gui3D::Panel* panel = iter.getNext();
		panel->injectMousePressed(evt, id);
	}

	return true;
}

bool gkGUI3D::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
//	gkLogger::write("MousePressed: X:"+gkToString(evt.state.X.abs)+" Y:"+gkToString(evt.state.Y.abs)+" Z:"+gkToString(evt.state.Z.abs)+" Btn:"+gkToString(evt.state.buttons)+" width:"+gkToString(evt.state.width)+" height:"+gkToString(evt.state.height),true);

	Panel2DList::Iterator iter2d(m_panel2dList);
	mouseMoved(evt);
	while (iter2d.hasMoreElements())
	{
		Gui3D::ScreenPanel* panel = iter2d.getNext();
		panel->injectMouseReleased(evt, id);
	}

	Panel3DList::Iterator iter(m_panel3dList);
	while (iter.hasMoreElements())
	{
		Gui3D::Panel* panel = iter.getNext();
		panel->injectMouseReleased(evt, id);
	}
	return true;
}

void whoami(std::vector<string>& t)
{
 OgreConsole::getSingleton().print("Your you!");
}

void gkGUI3D::addPanel3d(Gui3D::Panel* panel)
{
	if (!m_panel3dList.find(panel))
	{
		gkDebugScreen::printTo("added panel!");
		m_panel3dList.push_back(panel);
	}
}

void gkGUI3D::removePanel3d(Gui3D::Panel* panel)
{
	m_panel3dList.erase(panel);
}

void gkGUI3D::addPanel2d(Gui3D::ScreenPanel* panel)
{
	if (!m_panel2dList.find(panel))
	{
		gkDebugScreen::printTo("added panel!");
		m_panel2dList.push_back(panel);
	}
}

void gkGUI3D::removePanel2d(Gui3D::ScreenPanel* panel)
{
	m_panel2dList.erase(panel);
}

void gkGUI3D::addScriptMapping(Gui3D::PanelElement* panelElem, gsGUI3DElement* scriptElem)
{
	if (m_scriptMapping.find(panelElem)==UT_NPOS)
	{
		m_scriptMapping.insert(panelElem,scriptElem);
	}
}

void gkGUI3D::removeScriptMapping(Gui3D::PanelElement* panelElem)
{
	m_scriptMapping.erase(panelElem);
}

gsGUI3DElement* gkGUI3D::getScriptMapping(Gui3D::PanelElement* panelElement)
{
	gsGUI3DElement** elem = m_scriptMapping.get(panelElement);
	if (elem)
		return *elem;
	return 0;
}


UT_IMPLEMENT_SINGLETON(gkGUI3D);
