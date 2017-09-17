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
#ifndef __EM_KEYBOARD
#define __EM_KEYBOARD

#include <emscripten/html5.h>
#include "OISInputManager.h"
#include "OISFactoryCreator.h"
#include "OISKeyboard.h"
#include "OISMultiTouch.h"
#include "OISJoyStick.h"

#include "OISMouse.h"

namespace OIS
{

class EmscriptenKeyboard : public Keyboard
{
	char mKeyBuffer[256];
public:
	EmscriptenKeyboard(InputManager* creator);

	virtual void setBuffered(bool buffered) {}
	virtual void capture(){}
	virtual Interface* queryInterface(Interface::IType type) {return 0;}
	virtual void _initialize() {}
	virtual bool isKeyDown(KeyCode key) const;
	virtual const std::string& getAsString(KeyCode kc);

	virtual void copyKeyStates(char keys[256]) const;

	void injectKey(int action, int uniChar, int keyCode, const EmscriptenKeyboardEvent* keyEvent);
	void resetBuffer();

	static KeyEvent convertKey(int action, const EmscriptenKeyboardEvent* keyEvent, int modifier);
private:

	static int convertModifiers(const EmscriptenKeyboardEvent* keyEvent)
	{
		printf("em-key: code:%lu char:%s code:%s\n",keyEvent->keyCode,keyEvent->key,keyEvent->code);
		fflush(stdout);
		int modifiers = 0;
		if (keyEvent->shiftKey > 0)
			modifiers |= OIS::Keyboard::Shift;
		if (keyEvent->altKey > 0)
			modifiers |= OIS::Keyboard::Alt;
		if (keyEvent->ctrlKey > 0)
			modifiers |= OIS::Keyboard::Ctrl;
		return modifiers;
	}

	static EM_BOOL keydown_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
	static EM_BOOL keyup_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
	static EM_BOOL keypress_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);

	typedef std::map<char,int> KeyboardOISMapping;
	static KeyboardOISMapping m_keyboardOisMapping;

	typedef std::map<int,char> OIS2Char;
	static OIS2Char m_ois2charMapping;

	void initMapping();
};

}

#endif
