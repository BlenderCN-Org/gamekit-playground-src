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
#ifndef __EM_MOUSE
#define __EM_MOUSE

#include <emscripten/html5.h>
#include "OISInputManager.h"
#include "OISFactoryCreator.h"
#include "OISKeyboard.h"
#include "OISMultiTouch.h"
#include "OISJoyStick.h"

#include "OISMouse.h"

namespace OIS
{
	class EmscriptenInputManager;

	class EmscriptenMouse : public Mouse
	{
	public:
		EmscriptenMouse(bool buffered,EmscriptenInputManager* creator);
		virtual void setBuffered(bool buffered) { mBuffered = buffered;}
		virtual void capture(){}
		virtual void _initialize();
		void _updateScreenSize();
		virtual Interface* queryInterface(Interface::IType) {return 0;}

	private:
	   void setMousePosition(int eventType,const EmscriptenMouseEvent* mouseEvent);
	   static inline OIS::MouseButtonID emBtn2oisBtn(int emButton)
	   {
		   if (emButton==0) return OIS::MB_Left;
		   else if(emButton==1) return OIS::MB_Middle;
		   else if(emButton==2) return OIS::MB_Right;
		   else return (OIS::MouseButtonID)emButton;
	   }

	   static EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
	   static EM_BOOL mouseup_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
	   static EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
	   static EM_BOOL mousewheel_callback(int eventType, const EmscriptenWheelEvent* mouseEvent, void* userData);
	};
}

#endif
