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

#ifndef OIS_EmscriptenInputManager_H
#define OIS_EmscriptenInputManager_H

#include <emscripten/EmscriptenKeyboard.h>
#include <emscripten/html5.h>
#include "OISInputManager.h"
#include "OISFactoryCreator.h"
#include "OISKeyboard.h"
#include "OISMultiTouch.h"
#include "OISJoyStick.h"

#include "OISMouse.h"


namespace OIS
{
class EmscriptenMouse;
class EmscriptenKeyboard;
class EmscriptenInputManager;


//class EmscriptenJoystick : public JoyStick
//{
//public:
//	EmscriptenJoystick(InputManager* creator, bool buffered);
//	virtual ~EmscriptenJoystick();
//
//	/** @copydoc Object::setBuffered */
//	virtual void setBuffered(bool buffered);
//
//	/** @copydoc Object::capture */
//	virtual void capture();
//
//	/** @copydoc Object::queryInterface */
//	virtual Interface* queryInterface(Interface::IType type);
//
//	/** @copydoc Object::_initialize */
//	virtual void _initialize();
//
//	void _injectValues(int player,int stickNr,float x,float y);
//	void _injectButton(int player,int button,bool pressed);
//};




class EmscriptenInputManager : public InputManager, public FactoryCreator
{

//	typedef std::vector< OuyaJoystick* > JoystickList;
//	JoystickList mJoysticks;


public:
	EmscriptenInputManager();
	~EmscriptenInputManager();

	void _initialize(ParamList &paramList);

	DeviceList freeDeviceList();		
	int totalDevices(Type iType);	
	int freeDevices(Type iType);
	int getNumberOfDevices( Type iType ){ return totalDevices(iType);}

	bool vendorExist(Type iType, const std::string & vendor);

	Object* createObject(InputManager* creator, Type iType, bool bufferMode, const std::string & vendor);
	void destroyObject(Object* obj);

	int getScreenWidth() { return mScreenXSize;}
	int getScreenHeight() { return mScreenYSize;}

	void _updateScreenSize();
protected:
	EmscriptenKeyboard* mKeyboard;
	EmscriptenMouse* mMouse;

	int mScreenXSize;
	int mScreenYSize;
//	void injectJoystick(int player,int stick,float x,float y)
//	{
//		OuyaJoystick* joystick = mJoysticks[player];
//		if (!joystick)
//		{
//			joystick = new OuyaJoystick(this,true);
//			mJoysticks[player]=joystick;
//		}
//		joystick->_injectValues(player,stick,x,y);
//	}
//
//	void injectJoystickButton(int player,int button,bool pressed)
//	{
//		OuyaJoystick* joystick = mJoysticks[player];
//		if (!joystick)
//		{
//			joystick = new OuyaJoystick(this,true);
//			mJoysticks[player]=joystick;
//		}
//		joystick->_injectButton(player,button,pressed);
//	}

	void setOffsets(int x, int y) {  }
	void setWindowSize(int w, int h) { mScreenXSize=w;mScreenYSize=h; }
};

}

#endif
