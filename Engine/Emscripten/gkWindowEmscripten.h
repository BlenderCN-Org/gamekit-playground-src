/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 harkon.kr

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

#ifndef _gkWindowEmscripten_h_
#define _gkWindowEmscripten_h_
#include <emscripten/html5.h>
#include "OISJoyStick.h"


// Internal interface
class gkWindowEmscripten :
	public gkWindow
{
	virtual bool setupInput(const gkUserDefs& prefs);

public:
	gkWindowEmscripten();
	virtual ~gkWindowEmscripten();
	
	virtual void dispatch(void);
	virtual void process(void);
	
	void transformInputState(OIS::MultiTouchState& state);

	bool axisMoved( const OIS::JoyStickEvent &arg, int axis );

	void windowResized(Ogre::RenderWindow* rw);

    static EM_BOOL fullscreenCallback(int eventType, const EmscriptenFullscreenChangeEvent* event, void* userData);

private:
//	OIS::Keyboard*		    m_keyboard;
//	OIS::JoyStick*			m_iacc;
	// additional for listener (gkGUI3D)

	// pass this to the optional mouselistener (gkGUI3D)
	OIS::MouseState m_forwardState;
	OIS::MouseEvent m_forwardEvent;

};

#endif //_gkWindowIOS_h_
