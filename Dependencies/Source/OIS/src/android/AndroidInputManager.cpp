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
#include "android/AndroidInputManager.h"

#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <cstdio>
#include "OISLog.h"

#ifdef __ANDROID__
#include <android/log.h>
#define _LOGI_(...)  __android_log_print(ANDROID_LOG_INFO,  "OgreKit", __VA_ARGS__);
#define _LOGE_(...)  __android_log_print(ANDROID_LOG_ERROR, "OgreKit", __VA_ARGS__);
#define _LOG_FOOT_   _LOGI_("%s (%d): %s", __FILE__, __LINE__, __FUNCTION__);
#else
#define _LOGI_(...)
#define _LOGE_(...)
#define _LOG_FOOT_
#endif

namespace OIS
{

AndroidInputManager::AndroidInputManager()
	:	InputManager("AndroidInputManager"),
		mKeyboard(0),
		mTouch(0)
{
	mFactories.push_back(this);

	_OIS_LOG_FOOT_;
}

AndroidInputManager::~AndroidInputManager()
{

}

void AndroidInputManager::_initialize( ParamList &paramList )
{
	ParamList::iterator i = paramList.find("WINDOW");
	if(i != paramList.end())
	{
		size_t whandle = strtoul(i->second.c_str(), 0, 10);		
    }

	//TODO: setup window
}

DeviceList AndroidInputManager::freeDeviceList()
{
	DeviceList ret;

	//if( bAccelerometerUsed == false )
		ret.insert(std::make_pair(OISKeyboard, mInputSystemName));

	//if( bMultiTouchUsed == false )
		ret.insert(std::make_pair(OISMultiTouch, mInputSystemName));

//		if (mAccelerometer){
//			ret.insert(std::make_pair(OISJoyStick, mInputSystemName));
//		}

	return ret;
}

int AndroidInputManager::totalDevices(Type iType)
{
	switch(iType)
	{
        case OISKeyboard: return 1;
        case OISMultiTouch: return 1;
        case OISJoyStick : return 1;
        default: return 0;
	}
}

int AndroidInputManager::freeDevices(Type iType)
{
	switch(iType)
	{
        case OISKeyboard: return 1; //bAccelerometerUsed ? 0 : 1;
        case OISMultiTouch: return 1; //bMultiTouchUsed ? 0 : 1;
        case OISJoyStick : return 1;
        default: return 0;
	}
}

bool AndroidInputManager::vendorExist(Type iType, const std::string & vendor)
{
	if( ( iType == OISMultiTouch || iType == OISKeyboard || iType == OISJoyStick) && vendor == mInputSystemName )
		return true;

	return false;
}



Object* AndroidInputManager::createObject(InputManager* creator, Type iType, bool bufferMode, const std::string & vendor)
{
	Object *obj = 0;
	
	_OIS_LOG_FOOT_;

	switch(iType)
	{
	case OISKeyboard: 
		{		
			AndroidKeyboard *keyboard = new AndroidKeyboard(this);
			obj = keyboard;
			if (!mKeyboard) mKeyboard = keyboard;		
			break;
		}
		

	case OISMultiTouch:
		{
			AndroidMultiTouch* touch = new AndroidMultiTouch(this);
			obj = touch;
			if (!mTouch) mTouch = touch;		
			break;
		}
	case OISJoyStick:
		{
			OuyaJoystick* joy = new OuyaJoystick(this,false);
			obj = joy;
			mJoysticks.push_back(joy);
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

void AndroidInputManager::destroyObject(Object* obj)
{
	delete obj;

	if (obj == mTouch) mTouch = 0;
	else if (obj == mKeyboard) mKeyboard = 0;
}


//--

AndroidKeyboard::AndroidKeyboard(InputManager* creator) 
	:	Keyboard(creator->inputSystemName(), false, 1, 0) 
{
	resetBuffer();
}

void AndroidKeyboard::resetBuffer()
{
	memset(mKeyBuffer, 0, sizeof(mKeyBuffer));
}

void AndroidKeyboard::copyKeyStates(char keys[256]) const
{
	memcpy(keys, mKeyBuffer, sizeof(keys));
}

const std::string& AndroidKeyboard::getAsString(KeyCode kc)
{
	char buf[2] = { (char)convertKey(0, kc, mModifiers).key, 0 };
	return std::string(buf);
}

bool AndroidKeyboard::isKeyDown(KeyCode key) const 
{ 
	return key < 256 && mKeyBuffer[key] != 0;
}

void AndroidKeyboard::injectKey(int action, int modifiers, int keyCode)
{
	KeyEvent evt = AndroidKeyboard::convertKey(action, keyCode,modifiers);
	evt.device = this;
	bool pressed = (action == 0);

	// the modifier-data must be packed using the OIS-enumerations (like it is done in MainActivity.java)
	mModifiers = modifiers;

	assert(evt.key < 256);
	mKeyBuffer[evt.key] = pressed;

	if (!mListener) return;

	if (pressed)
		mListener->keyPressed(evt);
	else
		mListener->keyReleased(evt);
}

void AndroidKeyboard::injectTextCommit(char* txt,int newCursorPos)
{
	KeyEvent evt(0,KC_UNASSIGNED,txt[0]);
	evt.device = this;
	bool pressed = true;

	// the modifier-data must be packed using the OIS-enumerations (like it is done in MainActivity.java)

	if (!mListener) return;
	
	if (pressed)
		mListener->keyPressed(evt);
	else
		mListener->keyReleased(evt);
}

#define TXT_SHIFT_NOSHIFT(SHIFT,NOSHIFT) (modifier & Shift)?SHIFT:NOSHIFT

//copied from ogre3d android samplebrower
KeyEvent AndroidKeyboard::convertKey(int action, int keyCode, int modifier)
{
	KeyCode kc = OIS::KC_UNASSIGNED;
	unsigned int txt = 0;
		
	switch(keyCode){
	case 4:
		kc = OIS::KC_BACK;
		break;
	case 7:
		kc = OIS::KC_0;
		txt = TXT_SHIFT_NOSHIFT(')','0');
		break;
	case 8:
		kc = OIS::KC_1;
		txt = TXT_SHIFT_NOSHIFT('!','1');
		break;
	case 9:
		kc = OIS::KC_2;
		txt =  TXT_SHIFT_NOSHIFT('@','2');
		break;
	case 10:
		kc = OIS::KC_3;
		txt = TXT_SHIFT_NOSHIFT('#','2');
		break;
	case 11:
		kc = OIS::KC_4;
		txt = TXT_SHIFT_NOSHIFT('$','4');
		break;
	case 12:
		kc = OIS::KC_5;
		txt = TXT_SHIFT_NOSHIFT('%','5');
		break;
	case 13:
		kc = OIS::KC_6;
		txt = '6';
		break;
	case 14:
		kc = OIS::KC_7;
		txt = TXT_SHIFT_NOSHIFT('&','7');
		break;
	case 15:
		kc = OIS::KC_8;
		txt = TXT_SHIFT_NOSHIFT('*','8');
		break;
	case 16:
		kc = OIS::KC_9;
		txt = TXT_SHIFT_NOSHIFT('(','9');
		break;
	case 69:
		kc = OIS::KC_MINUS;
		txt = TXT_SHIFT_NOSHIFT('_','-');
		break;
	case 70:
		kc = OIS::KC_EQUALS;
		txt = TXT_SHIFT_NOSHIFT('+','=');
		break;
	case 61:
		kc = OIS::KC_TAB;
		txt = '\t';
		break;
	case 67:
		kc = OIS::KC_BACK;
		txt = '\b';
		break;
	case 29:
		kc = OIS::KC_A;
		txt = TXT_SHIFT_NOSHIFT('A','a');
		break;
	case 30:
		kc = OIS::KC_B;
		txt = TXT_SHIFT_NOSHIFT('B','b');
		break;
	case 31:
		kc = OIS::KC_C;
		txt = TXT_SHIFT_NOSHIFT('C','c');
		break;
	case 32:
		kc = OIS::KC_D;
		txt = TXT_SHIFT_NOSHIFT('D','d');
		break;
	case 33:
		kc = OIS::KC_E;
		txt = TXT_SHIFT_NOSHIFT('E','e');
		break;
	case 34:
		kc = OIS::KC_F;
		txt = TXT_SHIFT_NOSHIFT('F','f');
		break;
	case 35:
		kc = OIS::KC_G;
		txt = TXT_SHIFT_NOSHIFT('G','g');
		break;
	case 36:
		kc = OIS::KC_H;
		txt = TXT_SHIFT_NOSHIFT('H','h');
		break;
	case 37:
		kc = OIS::KC_I;
		txt = TXT_SHIFT_NOSHIFT('I','i');
		break;
	case 38:
		kc = OIS::KC_J;
		txt = TXT_SHIFT_NOSHIFT('J','j');
		break;
	case 39:
		kc = OIS::KC_K;
		txt = TXT_SHIFT_NOSHIFT('K','k');
		break;
	case 40:
		kc = OIS::KC_L;
		txt = TXT_SHIFT_NOSHIFT('L','l');
		break;
	case 41:
		kc = OIS::KC_M;
		txt = TXT_SHIFT_NOSHIFT('M','m');
		break;
	case 42:
		kc = OIS::KC_N;
		txt = TXT_SHIFT_NOSHIFT('N','n');
		break;
	case 43:
		kc = OIS::KC_O;
		txt = TXT_SHIFT_NOSHIFT('O','o');
		break;
	case 44:
		kc = OIS::KC_P;
		txt = TXT_SHIFT_NOSHIFT('P','p');
		break;
	case 45:
		kc = OIS::KC_Q;
		txt = TXT_SHIFT_NOSHIFT('Q','q');
		break;
	case 46:
		kc = OIS::KC_R;
		txt = TXT_SHIFT_NOSHIFT('R','r');
		break;
	case 47:
		kc = OIS::KC_S;
		txt = TXT_SHIFT_NOSHIFT('S','s');
		break;
	case 48:
		kc = OIS::KC_T;
		txt = TXT_SHIFT_NOSHIFT('T','t');
		break;
	case 49:
		kc = OIS::KC_U;
		txt = TXT_SHIFT_NOSHIFT('U','u');
		break;
	case 50:
		kc = OIS::KC_V;
		txt = TXT_SHIFT_NOSHIFT('V','v');
		break;
	case 51:
		kc = OIS::KC_W;
		txt = TXT_SHIFT_NOSHIFT('W','w');
		break;
	case 52:
		kc = OIS::KC_X;
		txt = TXT_SHIFT_NOSHIFT('X','x');
		break;
	case 53:
		kc = OIS::KC_Y;
		txt = TXT_SHIFT_NOSHIFT('Y','y');
		break;
	case 54:
		kc = OIS::KC_Z;
		txt = TXT_SHIFT_NOSHIFT('Z','z');
		break;
	case 71:
		kc = OIS::KC_LBRACKET;
		txt = TXT_SHIFT_NOSHIFT('{','[');
		break;
	case 72:
		kc = OIS::KC_RBRACKET;
		txt = TXT_SHIFT_NOSHIFT('}',']');
		break;
	case 66:
		kc = OIS::KC_RETURN;
		txt = '\n';
		break;
	case 74:
		kc = OIS::KC_SEMICOLON;
		txt = ';';
		break;
	case 75:
		kc = OIS::KC_APOSTROPHE;
		txt = TXT_SHIFT_NOSHIFT('\"','\'');
		break;
	case 73:
		kc = OIS::KC_BACKSLASH;
		txt = '\\';
		break;
	case 55:
		kc = OIS::KC_COMMA;
		txt = TXT_SHIFT_NOSHIFT('<',',');
		break;
	case 56:
		kc = OIS::KC_PERIOD;
		txt = TXT_SHIFT_NOSHIFT('>','.');
		break;
	case 76:
		kc = OIS::KC_SLASH;
		txt = TXT_SHIFT_NOSHIFT('?','/');
		break;
	case 19:
		kc = OIS::KC_UP;
		break;
	case 20:
		kc = OIS::KC_DOWN;
		break;
	case 21:
		kc = OIS::KC_LEFT;
		break;
	case 22:
		kc = OIS::KC_RIGHT;
		break;
	case 24:
		kc = OIS::KC_VOLUMEUP;
		break;
	case 25:
		kc = OIS::KC_VOLUMEDOWN;
		break;
	case 82:
		kc = OIS::KC_LMENU;
		break;
	case 62:
		kc = OIS::KC_SPACE;
		break;


	default:
		_LOGI_("Unknown-Key: %i",keyCode);
		break;
	}
		
	return KeyEvent(0, kc, txt);
}


//--

AndroidMultiTouch::AndroidMultiTouch(InputManager* creator) 
	:	MultiTouch(creator->inputSystemName(), false, 0, 0), 
		mOffsetX(0), mOffsetY(0)
{
	OIS::MultiTouchState state;
	mStates.push_back(state);
}

//copied from ogre3d android samplebrower
void AndroidMultiTouch::injectTouch(int action, float x, float y)
{
	assert(!mStates.empty());

	OIS::MultiTouchState &state = mStates[0];
	state.width = mWidth;
	state.height = mHeight;
		
	switch(action)
	{
	case 0:
		state.touchType = OIS::MT_Pressed;
		break;
	case 1:
		state.touchType = OIS::MT_Released;
		break;
	case 2:
		state.touchType = OIS::MT_Moved;
		break;
	case 3:
		state.touchType = OIS::MT_Cancelled;
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

		if (abs(state.X.rel)>50 || abs(state.Y.rel)>50) {
			state.X.rel = 0;
			state.Y.rel = 0;
		}

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





void AndroidMultiTouch::injectMultiTouch(int action,int pointerId, float x, float y, float size, float pressure)
{
    OIS::MultiTouchState &state = getMultiTouchState(pointerId);

     switch(action)
     {
         case 0:
             state.touchType = OIS::MT_Pressed;
             break;
         case 1:
             state.touchType = OIS::MT_Released;
             break;
         case 2:
             state.touchType = OIS::MT_Moved;
             break;
         case 3:
             state.touchType = OIS::MT_Cancelled;
             break;
         default:
             state.touchType = OIS::MT_None;
             break;
     }

     state.pressure = pressure;
     state.size = size;

     if(state.touchType != OIS::MT_None)
     {
         int last = state.X.abs;
         state.X.abs =  mOffsetX + (int)x;
         state.X.rel = state.X.abs - last;

         last = state.Y.abs;
         state.Y.abs = mOffsetY + (int)y;
         state.Y.rel = state.Y.abs - last;

 		if (abs(state.X.rel)>50 || abs(state.Y.rel)>50) {
 			state.X.rel = 0;
 			state.Y.rel = 0;
 		}


         state.Z.abs = 0;
         state.Z.rel = 0;

         OIS::MultiTouchEvent evt(this, state);

         switch(state.touchType)
         {
//             if (pointerId==0)
         	   {
				 case OIS::MT_Pressed:
					 mListener->touchPressed(evt);
					 break;
				 case OIS::MT_Released:
//					 state.X.abs = state.X.rel = 0;
//					 state.Y.abs = state.Y.rel = 0;
					 mListener->touchReleased(evt);
					 break;
				 case OIS::MT_Moved:
					 mListener->touchMoved(evt);
					 break;
				 case OIS::MT_Cancelled:
					 state.X.abs = state.X.rel = 0;
					 state.Y.abs = state.Y.rel = 0;
					 mListener->touchCancelled(evt);
					 break;
				 default:
					 break;
             }
         }
     } else {
    	 OIS::MultiTouchEvent evt(this, state);
		 state.X.rel = 0;
		 state.Y.rel = 0;
		 mListener->touchCancelled(evt);
     }
 }


AndroidAccelerometer::AndroidAccelerometer(InputManager* creator, bool buffered) : JoyStick(creator->inputSystemName(), true, 0, 0) {}
AndroidAccelerometer::~AndroidAccelerometer() {}

/** @copydoc Object::setBuffered */
void AndroidAccelerometer::setBuffered(bool buffered){
	// TODO?
	mBuffered=buffered;
}

//    void setUpdateInterval(float interval) {
//        mUpdateInterval = interval;
//        setUpdateInterval(1.0f / mUpdateInterval);
//    }


/** @copydoc Object::capture */
void AndroidAccelerometer::capture(){
    mState.clear();
    mState.mVectors[0] = mTempState;

    if(mListener && mBuffered){
        mListener->axisMoved(JoyStickEvent(this, mState), 0);
    }
}



/** @copydoc Object::_initialize */
void AndroidAccelerometer::_initialize(){
	// Clear old joy state
    mState.mVectors.resize(1);
	mState.clear();
	mTempState.clear();
}

void AndroidAccelerometer::injectAcceleration(float x,float y,float z) {
	mTempState.clear();
	mTempState.x = x;
	mTempState.y = y;
	mTempState.z = z;
}


OuyaJoystick::OuyaJoystick(InputManager* creator, bool buffered)
	: JoyStick("ouya",true,0,creator)
{
	mState.mButtons.resize(13);
	mState.mAxes.resize(4);
}

OuyaJoystick::~OuyaJoystick()
{

}

/** @copydoc Object::setBuffered */
void OuyaJoystick::setBuffered(bool buffered)
{

}

/** @copydoc Object::capture */
void OuyaJoystick::capture()
{
    if(mListener && mBuffered){
        mListener->axisMoved(JoyStickEvent(this, mState), 0);
    }
}

/** @copydoc Object::queryInterface */
Interface* OuyaJoystick::queryInterface(Interface::IType type)
{

}

/** @copydoc Object::_initialize */
void OuyaJoystick::_initialize()
{

}
void OuyaJoystick::_injectValues(int player,int stickNr,float x,float y)
{
	mState.mAxes[stickNr*2].abs=x;
	mState.mAxes[stickNr*2+1].abs=y;
	mListener->axisMoved(JoyStickEvent(this,mState),stickNr*2);
	mListener->axisMoved(JoyStickEvent(this,mState),stickNr*2+1);
}


/*
  public static final int BUTTON_O = 96;

  // Field descriptor #114 I
  public static final int BUTTON_U = 99;

  // Field descriptor #114 I
  public static final int BUTTON_Y = 100;

  // Field descriptor #114 I
  public static final int BUTTON_A = 97;
 *
 * public static final int BUTTON_L1 = 102;

  // Field descriptor #114 I (deprecated)
  @java.lang.Deprecated
  public static final int BUTTON_L2 = 104;

  // Field descriptor #114 I
  public static final int BUTTON_R1 = 103;

  // Field descriptor #114 I (deprecated)
  @java.lang.Deprecated
  public static final int BUTTON_R2 = 105;

  // Field descriptor #114 I
  public static final int BUTTON_MENU = 82;


  public static final int BUTTON_DPAD_UP = 19;

  // Field descriptor #114 I
  public static final int BUTTON_DPAD_RIGHT = 22;

  // Field descriptor #114 I
  public static final int BUTTON_DPAD_DOWN = 20;

  // Field descriptor #114 I
  public static final int BUTTON_DPAD_LEFT = 21;


 *
 */

void OuyaJoystick::_injectButton(int player, int button,bool pressed)
{
	int btn = -1;
	switch(button)
	{
	case 96:
		btn = 0;
		break;
	case 99:
		btn = 1;
		break;
	case 100:
		btn = 2;
		break;
	case 97:
		btn = 3;
		break;

	case 102:
		btn = 4;
		break;
	case 103:
		btn = 5;
		break;
	case 104:
		btn = 6;
		break;
	case 105:
		btn = 7;
		break;
	case 82:
		btn = 8;
		break;
	case 19:
		btn = 9;
		break;
	case 20:
		btn = 10;
		break;
	case 21:
		btn = 11;
		break;
	case 22:
		btn = 12;
		break;
	}

	if (button == -1)
	{
		_LOGI_("Unknown buttonmapping: %i",button);
		return;
	}

	mState.mButtons[btn]=pressed;
	if (pressed)
	{
		mListener->buttonPressed(JoyStickEvent(this,mState),btn);
	}
	else
	{
		mListener->buttonReleased(JoyStickEvent(this,mState),btn);
	}
}
}

