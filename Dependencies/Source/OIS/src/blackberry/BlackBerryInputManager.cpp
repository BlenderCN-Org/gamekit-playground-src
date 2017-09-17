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
#include "blackberry/BlackBerryInputManager.h"
#include <sys/keycodes.h>

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <cstdio>
#include "OISLog.h"

namespace OIS
{

BlackBerryInputManager::BlackBerryInputManager()
	:	InputManager("BlackBerryInputManager"),
		mKeyboard(0),
		mTouch(0),
		mAccelerometer(0)
{
	mFactories.push_back(this);

	_OIS_LOG_FOOT_;
}

BlackBerryInputManager::~BlackBerryInputManager()
{

}

void BlackBerryInputManager::_initialize( ParamList &paramList )
{
	ParamList::iterator i = paramList.find("WINDOW");
	if(i != paramList.end())
	{
		size_t whandle = strtoul(i->second.c_str(), 0, 10);		
    }

	//TODO: setup window
}

DeviceList BlackBerryInputManager::freeDeviceList()
{
	DeviceList ret;

	//if( bAccelerometerUsed == false )
		ret.insert(std::make_pair(OISKeyboard, mInputSystemName));

	//if( bMultiTouchUsed == false )
		ret.insert(std::make_pair(OISMultiTouch, mInputSystemName));

		if (mAccelerometer){
			ret.insert(std::make_pair(OISJoyStick, mInputSystemName));
		}

	return ret;
}

int BlackBerryInputManager::totalDevices(Type iType)
{
	switch(iType)
	{
        case OISKeyboard: return 1;
        case OISMultiTouch: return 1;
        case OISJoyStick : return 1;
        default: return 0;
	}
}

int BlackBerryInputManager::freeDevices(Type iType)
{
	switch(iType)
	{
        case OISKeyboard: return 1; //bAccelerometerUsed ? 0 : 1;
        case OISMultiTouch: return 1; //bMultiTouchUsed ? 0 : 1;
        case OISJoyStick : return 1;
        default: return 0;
	}
}

bool BlackBerryInputManager::vendorExist(Type iType, const std::string & vendor)
{
	if( ( iType == OISMultiTouch || iType == OISKeyboard || iType == OISJoyStick) && vendor == mInputSystemName )
		return true;

	return false;
}



Object* BlackBerryInputManager::createObject(InputManager* creator, Type iType, bool bufferMode, const std::string & vendor)
{
	Object *obj = 0;
	
	_OIS_LOG_FOOT_;

	switch(iType)
	{
	case OISKeyboard: 
		{		
			BlackBerryKeyboard *keyboard = new BlackBerryKeyboard(this);
			obj = keyboard;
			if (!mKeyboard) mKeyboard = keyboard;		
			break;
		}
		

	case OISMultiTouch:
		{
			BlackBerryMultiTouch* touch = new BlackBerryMultiTouch(this);
			obj = touch;
			if (!mTouch) mTouch = touch;		
			break;
		}
	case OISJoyStick:
		{
			BlackBerryAccelerometer* accel = new BlackBerryAccelerometer(this,true);
			obj = accel;
			if (!mAccelerometer)
				mAccelerometer = accel;
			break;
		}
	default:
		break;
	}

	_OIS_LOG_FOOT_;

	if( obj == 0 )
		OIS_EXCEPT(E_InputDeviceNonExistant, "No devices match requested type.");

	_OIS_LOG_FOOT_;

	return obj;
}

void BlackBerryInputManager::destroyObject(Object* obj)
{
	delete obj;

	if (obj == mTouch) mTouch = 0;
	else if (obj == mKeyboard) mKeyboard = 0;
}


//--

BlackBerryKeyboard::BlackBerryKeyboard(InputManager* creator)
	:	Keyboard(creator->inputSystemName(), false, 1, 0) 
{
	resetBuffer();
}

void BlackBerryKeyboard::resetBuffer()
{
	memset(mKeyBuffer, 0, sizeof(mKeyBuffer));
}

void BlackBerryKeyboard::copyKeyStates(char keys[256]) const
{
	memcpy(keys, mKeyBuffer, sizeof(keys));
}

const std::string& BlackBerryKeyboard::getAsString(KeyCode kc)
{
	char buf[2] = { (char)convertKey(0, kc).key, 0 };
	return std::string(buf);
}

bool BlackBerryKeyboard::isKeyDown(KeyCode key) const
{ 
	return key < 256 && mKeyBuffer[key] != 0;
}

void BlackBerryKeyboard::injectKey(int action, int modifiers, int keyCode)
{
	// modifiers need to use OIS-enums
	mModifiers = modifiers;

	KeyEvent evt = BlackBerryKeyboard::convertKey(action, keyCode);
	evt.device = this;
	bool pressed = (action == 0);

	assert(evt.key < 256);
	mKeyBuffer[evt.key] = pressed;

	if (!mListener) return;
	
	if (pressed)
		mListener->keyPressed(evt);
	else
		mListener->keyReleased(evt);
}



//copied from ogre3d android samplebrower
KeyEvent BlackBerryKeyboard::convertKey(int action, int keyCode)
{
	KeyCode kc = OIS::KC_UNASSIGNED;
	unsigned int txt = 0;
		
	switch(keyCode){
	case KEYCODE_ZERO:
		kc = OIS::KC_0;
//		txt = '0';
		break;
	case KEYCODE_ONE:
		kc = OIS::KC_1;
//		txt = '1';
		break;
	case KEYCODE_TWO:
		kc = OIS::KC_2;
//		txt = '2';
		break;
	case KEYCODE_THREE:
		kc = OIS::KC_3;
//		txt = '3';
		break;
	case 0x0034:
		kc = OIS::KC_4;
//		txt = '4';
		break;
	case 0x0035:
		kc = OIS::KC_5;
//		txt = '5';
		break;
	case 0x0036:
		kc = OIS::KC_6;
//		txt = '6';
		break;
	case 0x0037:
		kc = OIS::KC_7;
//		txt = '7';
		break;
	case 0x0038:
		kc = OIS::KC_8;
//		txt = '8';
		break;
	case 0x0039:
		kc = OIS::KC_9;
//		txt = '9';
		break;
	case KEYCODE_MINUS:
		kc = OIS::KC_MINUS;
//		txt = '-';
		break;
	case KEYCODE_EQUAL:
		kc = OIS::KC_EQUALS;
//		txt = '=';
		break;
	case KEYCODE_TAB:
		kc = OIS::KC_TAB;
//		txt = '\t';
		break;
	case KEYCODE_BACKSPACE:
		kc = OIS::KC_BACK;
//		txt = '\b';
		break;
	case KEYCODE_A:
		kc = OIS::KC_A;
//		txt = 'A';
		break;
	case 0x0062:
		kc = OIS::KC_B;
//		txt = 'B';
		break;
	case 0x0063:
		kc = OIS::KC_C;
//		txt = 'C';
		break;
	case 0x0064:
		kc = OIS::KC_D;
//		txt = 'D';
		break;
	case 0x0065:
		kc = OIS::KC_E;
//		txt = 'E';
		break;
	case KEYCODE_F:
		kc = OIS::KC_F;
//		txt = 'F';
		break;
	case 0x0067:
		kc = OIS::KC_G;
//		txt = 'G';
		break;
	case 0x0068:
		kc = OIS::KC_H;
//		txt = 'H';
		break;
	case 0x0069:
		kc = OIS::KC_I;
//		txt = 'I';
		break;
	case KEYCODE_J:
		kc = OIS::KC_J;
//		txt = 'J';
		break;
	case KEYCODE_K:
		kc = OIS::KC_K;
//		txt = 'K';
		break;
	case KEYCODE_L:
		kc = OIS::KC_L;
//		txt = 'L';
		break;
	case KEYCODE_M:
		kc = OIS::KC_M;
//		txt = 'M';
		break;
	case KEYCODE_N:
		kc = OIS::KC_N;
//		txt = 'N';
		break;
	case KEYCODE_O:
		kc = OIS::KC_O;
//		txt = 'O';
		break;
	case KEYCODE_P:
		kc = OIS::KC_P;
//		txt = 'P';
		break;
	case KEYCODE_Q:
		kc = OIS::KC_Q;
//		txt = 'Q';
		break;
	case KEYCODE_R:
		kc = OIS::KC_R;
//		txt = 'R';
		break;
	case KEYCODE_S:
		kc = OIS::KC_S;
//		txt = 'S';
		break;
	case KEYCODE_T:
		kc = OIS::KC_T;
//		txt = 'T';
		break;
	case KEYCODE_U:
		kc = OIS::KC_U;
//		txt = 'U';
		break;
	case KEYCODE_V:
		kc = OIS::KC_V;
//		txt = 'V';
		break;
	case KEYCODE_W:
		kc = OIS::KC_W;
//		txt = 'W';
		break;
	case KEYCODE_X:
		kc = OIS::KC_X;
//		txt = 'X';
		break;
	case KEYCODE_Y:
		kc = OIS::KC_Y;
//		txt = 'Y';
		break;
	case KEYCODE_Z:
		kc = OIS::KC_Z;
//		txt = 'Z';
		break;
	case KEYCODE_LEFT_BRACKET:
		kc = OIS::KC_LBRACKET;
//		txt = '[';
		break;
	case KEYCODE_RIGHT_BRACKET:
		kc = OIS::KC_RBRACKET;
//		txt = ']';
		break;
	case KEYCODE_RETURN:
		kc = OIS::KC_RETURN;
//		txt = '\n';
		break;
	case KEYCODE_SEMICOLON:
		kc = OIS::KC_SEMICOLON;
//		txt = ';';
		break;
	case KEYCODE_APOSTROPHE:
		kc = OIS::KC_APOSTROPHE;
//		txt = '\'';
		break;
	case KEYCODE_BACK_SLASH:
		kc = OIS::KC_BACKSLASH;
//		txt = '\\';
		break;
	case KEYCODE_COMMA:
		kc = OIS::KC_COMMA;
//		txt = ',';
		break;
	case KEYCODE_PERIOD:
		kc = OIS::KC_PERIOD;
//		txt = '.';
		break;
	case KEYCODE_SLASH:
		kc = OIS::KC_SLASH;
//		txt = '/';
		break;
	case KEYCODE_UP:
		kc = OIS::KC_UP;
		break;
	case KEYCODE_DOWN:
		kc = OIS::KC_DOWN;
		break;
	case KEYCODE_LEFT:
		kc = OIS::KC_LEFT;
		break;
	case KEYCODE_RIGHT:
		kc = OIS::KC_RIGHT;
		break;
	}
		
	return KeyEvent(0, kc, (char)keyCode);
}


//--

//copied from ogre3d android samplebrower
void BlackBerryMultiTouch::injectTouch(int action, float x, float y)
{
	OIS::MultiTouchState &state = getMultiTouchState(0);


	switch(action)
	{
	case 100:
		state.touchType = OIS::MT_Pressed;
		break;
	case 102:
		state.touchType = OIS::MT_Released;
		break;
	case 101:
		state.touchType = OIS::MT_Moved;
		break;
	default:
		state.touchType = OIS::MT_None;
		break;
	}

	if (state.touchType != OIS::MT_None)
	{
		int last = state.X.abs;
		state.X.abs = mOffsetX + (int)x;
		state.X.rel = state.X.abs - last;

		last = state.Y.abs;
		state.Y.abs = mOffsetY + (int)y;
		state.Y.rel = state.Y.abs - last;

		//last = state.Z.abs;
		state.Z.abs = 0;
		state.Z.rel = 0;

		if (mListener)
		{
			OIS::MultiTouchEvent evt(this, state);

			switch(state.touchType){
			case OIS::MT_Pressed:
				mListener->touchPressed(evt);
				break;
			case OIS::MT_Released:
				mListener->touchReleased(evt);
				break;
			case OIS::MT_Moved:
				mListener->touchMoved(evt);
				break;
			case OIS::MT_Cancelled:
				mListener->touchCancelled(evt);
				break;
			default:
				break;
			}
		}
	}
}





void BlackBerryMultiTouch::injectMultiTouch(int action,int pointerId, float x, float y, float size, float pressure)
{
    OIS::MultiTouchState &state = getMultiTouchState(pointerId);

     switch(action)
     {
         case 100:
             state.touchType = OIS::MT_Pressed;
             break;
         case 102:
             state.touchType = OIS::MT_Released;
             break;
         case 101:
             state.touchType = OIS::MT_Moved;
             break;
         default:
             state.touchType = OIS::MT_None;
             break;
     }

     if(state.touchType != OIS::MT_None)
     {
         int last = state.X.abs;
         state.X.abs =  (int)x;
         state.X.rel = state.X.abs - last;

         last = state.Y.abs;
         state.Y.abs = (int)y;
         state.Y.rel = state.Y.abs - last;

         state.Z.abs = 0;
         state.Z.rel = 0;

         OIS::MultiTouchEvent evt(this, state);

         switch(state.touchType)
         {
             case OIS::MT_Pressed:
                 mListener->touchPressed(evt);
                 break;
             case OIS::MT_Released:
            	 mListener->touchReleased(evt);
                 break;
             case OIS::MT_Moved:
            	 mListener->touchMoved(evt);
                 break;
             case OIS::MT_Cancelled:
            	 mListener->touchCancelled(evt);
                 break;
             default:
                 break;
         }
     }
 }


BlackBerryAccelerometer::BlackBerryAccelerometer(InputManager* creator, bool buffered) : JoyStick(creator->inputSystemName(), true, 0, 0) {}
BlackBerryAccelerometer::~BlackBerryAccelerometer() {}

/** @copydoc Object::setBuffered */
void BlackBerryAccelerometer::setBuffered(bool buffered){
	// TODO?
	mBuffered=buffered;
}

//    void setUpdateInterval(float interval) {
//        mUpdateInterval = interval;
//        setUpdateInterval(1.0f / mUpdateInterval);
//    }


/** @copydoc Object::capture */
void BlackBerryAccelerometer::capture(){
    mState.clear();
    mState.mVectors[0] = mTempState;

    if(mListener && mBuffered){
        mListener->axisMoved(JoyStickEvent(this, mState), 0);
    } 
}



/** @copydoc Object::_initialize */
void BlackBerryAccelerometer::_initialize(){
	// Clear old joy state
    mState.mVectors.resize(1);
	mState.clear();
	mTempState.clear();
}

void BlackBerryAccelerometer::injectAcceleration(float x,float y,float z) {
	mTempState.clear();
	mTempState.x = x;
	mTempState.y = y;
	mTempState.z = z;
}
}
