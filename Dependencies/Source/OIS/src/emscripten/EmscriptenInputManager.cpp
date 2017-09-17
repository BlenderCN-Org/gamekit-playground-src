/*
The zlib/libpng License

Copyright (c) 2005-2010 harkon.kr(http://gamekit.googlecode.com/)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#include "OISException.h"
#include "emscripten/EmscriptenInputManager.h"

#include <assert.h>
#include <emscripten/emscripten.h>
#include <emscripten/EmscriptenKeyboard.h>
#include <emscripten/html5.h>
#include <stdlib.h>
#include <memory.h>
#include <cstdio>
#include "OISLog.h"
#include <iosfwd>

#include "emscripten/EmscriptenMouse.h"
namespace OIS
{

EmscriptenInputManager::EmscriptenInputManager()
	:	InputManager("EmscriptenInputManager"),
		mKeyboard(0),mScreenXSize(0),mScreenYSize(0)
{
	mFactories.push_back(this);
	printf("OIS Emscripten InputManager!\n");
	fflush(stdout);

	_OIS_LOG_FOOT_;
}

EmscriptenInputManager::~EmscriptenInputManager()
{

}

void EmscriptenInputManager::_initialize( ParamList &paramList )
{
	_updateScreenSize();
}

void EmscriptenInputManager::_updateScreenSize()
{
	int xSize;
	int ySize;
	int fullScreen;
	emscripten_get_canvas_size(&xSize,&ySize,&fullScreen);
	setWindowSize(xSize,ySize);
	if (mMouse)
		mMouse->_updateScreenSize();
}

DeviceList EmscriptenInputManager::freeDeviceList()
{
	DeviceList ret;

	ret.insert(std::make_pair(OISKeyboard, mInputSystemName));
	ret.insert(std::make_pair(OISMouse, mInputSystemName));

	return ret;
}

int EmscriptenInputManager::totalDevices(Type iType)
{
	switch(iType)
	{
        case OISKeyboard: return 1;
        case OISMouse: return 1;
//        case OISJoyStick : return 1;
        default: return 0;
	}
}

int EmscriptenInputManager::freeDevices(Type iType)
{
	switch(iType)
	{
        case OISKeyboard: return 1; //bAccelerometerUsed ? 0 : 1;
//        case OISJoyStick : return 1;
        case OISMouse: return 1;
        default: return 0;
	}
}

bool EmscriptenInputManager::vendorExist(Type iType, const std::string & vendor)
{
	if( (  iType == OISKeyboard || iType==OISMouse /*|| iType == OISJoyStick*/) && vendor == mInputSystemName )
		return true;


	return false;
}

Object* EmscriptenInputManager::createObject(InputManager* creator, Type iType, bool bufferMode, const std::string & vendor)
{
	Object *obj = 0;
	
	_OIS_LOG_FOOT_;

	switch(iType)
	{
	case OISKeyboard: 
	{
		EmscriptenKeyboard *keyboard = new EmscriptenKeyboard(this);
		obj = keyboard;
		if (!mKeyboard) mKeyboard = keyboard;
		break;
	}
	case OISMouse:
	{
		EmscriptenMouse* mouse = new EmscriptenMouse(bufferMode,this);
		obj = mouse;
		if (!mMouse) mMouse = mouse;
		break;
	}
		
//	case OISJoyStick:
//		{
//			OuyaJoystick* joy = new OuyaJoystick(this,false);
//			obj = joy;
//			mJoysticks.push_back(joy);
//			break;
//		}
	default:
		break;
	}

	_OIS_LOG_FOOT_;

	if( obj == 0 )
		OIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");

	_OIS_LOG_FOOT_;

	return obj;
}

void EmscriptenInputManager::destroyObject(Object* obj)
{
	if (obj == mKeyboard) mKeyboard = 0;
	delete obj;
}


//--

//OuyaJoystick::OuyaJoystick(InputManager* creator, bool buffered)
//	: JoyStick("ouya",true,0,creator)
//{
//	mState.mButtons.resize(13);
//	mState.mAxes.resize(4);
//}
//
//OuyaJoystick::~OuyaJoystick()char mask[4] = {0,1,4,2};
//{
//
//}
//
///** @copydoc Object::setBuffered */
//void OuyaJoystick::setBuffered(bool buffered)
//{
//
//}
//
///** @copydoc Object::capture */
//void OuyaJoystick::capture()
//{
//    if(mListener && mBuffered){
//        mListener->axisMoved(JoyStickEvent(this, mState), 0);
//    }
//}
//
///** @copydoc Object::queryInterface */
//Interface* OuyaJoystick::queryInterface(Interface::IType type)
//{
//
//}
//
///** @copydoc Object::_initialize */
//void OuyaJoystick::_initialize()
//{
//
//}
//void OuyaJoystick::_injectValues(int player,int stickNr,float x,float y)
//{
//	mState.mAxes[stickNr*2].abs=x;
//	mState.mAxes[stickNr*2+1].abs=y;
//	mListener->axisMoved(JoyStickEvent(this,mState),stickNr*2);
//	mListener->axisMoved(JoyStickEvent(this,mState),stickNr*2+1);
//}
//
//
///*
//  public static final int BUTTON_O = 96;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_U = 99;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_Y = 100;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_A = 97;
// *
// * public static final int BUTTON_L1 = 102;
//
//  // Field descriptor #114 I (deprecated)
//  @java.lang.Deprecated
//  public static final int BUTTON_L2 = 104;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_R1 = 103;
//
//  // Field descriptor #114 I (deprecated)
//  @java.lang.Deprecated
//  public static final int BUTTON_R2 = 105;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_MENU = 82;
//
//
//  public static final int BUTTON_DPAD_UP = 19;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_DPAD_RIGHT = 22;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_DPAD_DOWN = 20;
//
//  // Field descriptor #114 I
//  public static final int BUTTON_DPAD_LEFT = 21;
//
//
// *
// */
//
//void OuyaJoystick::_injectButton(int player, int button,bool pressed)
//{
//	int btn = -1;
//	switch(button)
//	{
//	case 96:
//		btn = 0;
//		break;
//	case 99:
//		btn = 1;
//		break;
//	case 100:
//		btn = 2;
//		break;
//	case 97:
//		btn = 3;
//		break;
//
//	case 102:
//		btn = 4;
//		break;
//	case 103:
//		btn = 5;
//		break;
//	case 104:
//		btn = 6;
//		break;
//	case 105:
//		btn = 7;
//		break;
//	case 82:
//		btn = 8;
//		break;
//	case 19:
//		btn = 9;
//		break;
//	case 20:
//		btn = 10;
//		break;
//	case 21:
//		btn = 11;
//		break;
//	case 22:
//		btn = 12;
//		break;
//	}
//
//	if (button == -1)
//	{
//		_LOGI_("Unknown buttonmapping: %i",button);
//		return;
//	}
//
//	mState.mButtons[btn]=pressed;
//	if (pressed)
//	{
//		mListener->buttonPressed(JoyStickEvent(this,mState),btn);
//	}
//	else
//	{
//		mListener->buttonReleased(JoyStickEvent(this,mState),btn);
//	}
//}



}

