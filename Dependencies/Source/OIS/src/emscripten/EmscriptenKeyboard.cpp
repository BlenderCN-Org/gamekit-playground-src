#include "OISException.h"

#include "emscripten/EmscriptenKeyboard.h"
#include "emscripten/EmscriptenInputManager.h"

#include <assert.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <stdlib.h>
#include <memory.h>
#include <cstdio>
#include "OISLog.h"
#include <iosfwd>

namespace OIS
{

EmscriptenKeyboard::EmscriptenKeyboard(InputManager* creator)
	:	Keyboard(creator->inputSystemName(), false, 1, 0)
{
	resetBuffer();

	initMapping();

	printf("Emscripten keyboard!\n");
	fflush(stdout);

    emscripten_set_keypress_callback(NULL, (void*)this, 1, &EmscriptenKeyboard::keypress_callback);
    emscripten_set_keydown_callback(NULL, (void*)this, 1, &EmscriptenKeyboard::keydown_callback);
    emscripten_set_keyup_callback(NULL, (void*)this, 1, &EmscriptenKeyboard::keyup_callback);
}

EM_BOOL EmscriptenKeyboard::keydown_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
	if (keyEvent->keyCode == 0)
		return false;
	EmscriptenKeyboard* thizz = static_cast<EmscriptenKeyboard*>(userData);
	int modifiers = convertModifiers(keyEvent);
	thizz->injectKey(0, modifiers, keyEvent->keyCode,keyEvent);
    return false;
}

EM_BOOL EmscriptenKeyboard::keyup_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
	if (keyEvent->keyCode == 0)
		return false;
	EmscriptenKeyboard* thizz = static_cast<EmscriptenKeyboard*>(userData);
	int modifiers = convertModifiers(keyEvent);
	thizz->injectKey(1, modifiers, keyEvent->keyCode,keyEvent);
    return false;
}

EM_BOOL EmscriptenKeyboard::keypress_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
//	if (keyEvent->keyCode == 0)
//		return false;
//	EmscriptenKeyboard* thizz = static_cast<EmscriptenKeyboard*>(userData);
//	int modifiers = convertModifiers(keyEvent);
//	thizz->injectKey(0, modifiers, keyEvent->keyCode,keyEvent);
    return false;
}

void EmscriptenKeyboard::resetBuffer()
{
	memset(mKeyBuffer, 0, sizeof(mKeyBuffer));
}

void EmscriptenKeyboard::copyKeyStates(char keys[256]) const
{
	memcpy(keys, mKeyBuffer, sizeof(keys));
}

const std::string& EmscriptenKeyboard::getAsString(KeyCode kc)
{
	OIS2Char::iterator it = m_ois2charMapping.find(kc);
	if (it==m_ois2charMapping.end())
		return "";
	return std::string(1,it->second);
}

bool EmscriptenKeyboard::isKeyDown(KeyCode key) const
{
	return key < 256 && mKeyBuffer[key] != 0;
}

void EmscriptenKeyboard::injectKey(int action, int modifiers, int keyCode,const EmscriptenKeyboardEvent* keyEvent)
{
	printf("injectKey(int %i action, int %i modifiers, int %i keyCode)\n",action,modifiers,keyCode);
	KeyEvent evt = EmscriptenKeyboard::convertKey(action, keyEvent,modifiers);
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

#define TXT_SHIFT_NOSHIFT(SHIFT,NOSHIFT) (modifier & Shift)?SHIFT:NOSHIFT

#define OISMAP(ch,oiscode) m_keyboardOisMapping.insert(std::pair<char,int>(ch,oiscode)); \
						   m_ois2charMapping.insert(std::pair<int,char>(oiscode,ch));

void EmscriptenKeyboard::initMapping()
{
	// already initalized? skip the step
	if (m_keyboardOisMapping.size()>0)
		return;

	OISMAP('a',OIS::KC_A);
	OISMAP('b',OIS::KC_B);
	OISMAP('c',OIS::KC_C);
	OISMAP('d',OIS::KC_D);
	OISMAP('e',OIS::KC_E);
	OISMAP('f',OIS::KC_F);
	OISMAP('g',OIS::KC_G);
	OISMAP('h',OIS::KC_H);
	OISMAP('i',OIS::KC_I);
	OISMAP('j',OIS::KC_J);
	OISMAP('k',OIS::KC_K);
	OISMAP('l',OIS::KC_L);
	OISMAP('m',OIS::KC_M);
	OISMAP('n',OIS::KC_N);
	OISMAP('o',OIS::KC_O);
	OISMAP('p',OIS::KC_P);
	OISMAP('q',OIS::KC_Q);
	OISMAP('r',OIS::KC_R);
	OISMAP('s',OIS::KC_S);
	OISMAP('t',OIS::KC_T);
	OISMAP('u',OIS::KC_U);
	OISMAP('v',OIS::KC_V);
	OISMAP('w',OIS::KC_W);
	OISMAP('x',OIS::KC_X);
	OISMAP('y',OIS::KC_Y);
	OISMAP('z',OIS::KC_Z);
	OISMAP('A',OIS::KC_A);
	OISMAP('B',OIS::KC_B);
	OISMAP('C',OIS::KC_C);
	OISMAP('D',OIS::KC_D);
	OISMAP('E',OIS::KC_E);
	OISMAP('F',OIS::KC_F);
	OISMAP('G',OIS::KC_G);
	OISMAP('H',OIS::KC_H);
	OISMAP('I',OIS::KC_I);
	OISMAP('J',OIS::KC_J);
	OISMAP('K',OIS::KC_K);
	OISMAP('L',OIS::KC_L);
	OISMAP('M',OIS::KC_M);
	OISMAP('N',OIS::KC_N);//	char buf[2] = { ""(char)convertKey(0, kc, mModifiers).key, 0 };

	OISMAP('O',OIS::KC_O);
	OISMAP('P',OIS::KC_P);
	OISMAP('Q',OIS::KC_Q);
	OISMAP('R',OIS::KC_R);
	OISMAP('S',OIS::KC_S);
	OISMAP('T',OIS::KC_T);
	OISMAP('U',OIS::KC_U);
	OISMAP('V',OIS::KC_V);
	OISMAP('W',OIS::KC_W);
	OISMAP('X',OIS::KC_X);
	OISMAP('Y',OIS::KC_Y);
	OISMAP('Z',OIS::KC_Z);

	OISMAP('0',OIS::KC_0);
	OISMAP('1',OIS::KC_1);
	OISMAP('2',OIS::KC_2);
	OISMAP('3',OIS::KC_3);
	OISMAP('4',OIS::KC_4);
	OISMAP('5',OIS::KC_5);
	OISMAP('6',OIS::KC_6);
	OISMAP('7',OIS::KC_7);
	OISMAP('8',OIS::KC_8);
	OISMAP('9',OIS::KC_9);
	OISMAP(';',OIS::KC_SEMICOLON);
	OISMAP(':',OIS::KC_COLON);
	OISMAP(',',OIS::KC_COMMA);
	OISMAP('.',OIS::KC_PERIOD);
	OISMAP('^',OIS::KC_POWER);
	OISMAP(' ',OIS::KC_SPACE);
	OISMAP('*',OIS::KC_MULTIPLY);
	OISMAP('/',OIS::KC_DIVIDE);
	OISMAP('\\',OIS::KC_BACKSLASH);

}

//copied from ogre3d android samplebrower
KeyEvent EmscriptenKeyboard::convertKey(int action, const EmscriptenKeyboardEvent* keyEvent, int modifier)
{

	const char* ckey = &keyEvent->key[0];

	printf("Convert em-input %s | %c\n",ckey,ckey[0]);

	if (strlen(ckey)==1)
	{
		KeyboardOISMapping::iterator it = m_keyboardOisMapping.find(ckey[0]);

		if (it!=m_keyboardOisMapping.end())
		{
			return KeyEvent(0, (KeyCode)it->second,(unsigned int)ckey[0]);
		}
		else
		{
			printf("Unknown emscripten key:%s\n",(unsigned int)ckey[0]);
		}
	}

	std::string key(ckey);
	if (key[0]==' ')
	{
		return KeyEvent(0,OIS::KC_SPACE,' ');
	}
	else if (key=="Esc")
	{
		return KeyEvent(0,OIS::KC_ESCAPE,0);
	}
	else if (key=="Tab")
	{
		return KeyEvent(0,OIS::KC_TAB,0);
	}
	else if (key=="PageUp")
	{
		return KeyEvent(0,OIS::KC_PGUP,0);
	}
	else if (key=="PageDown")
	{
		return KeyEvent(0,OIS::KC_PGDOWN,0);
	}
	else if (key=="Left")
	{
		return KeyEvent(0,OIS::KC_LEFT,0);
	}
	else if (key=="Right")
	{
		return KeyEvent(0,OIS::KC_RIGHT,0);
	}
	else if (key=="Up")
	{
		return KeyEvent(0,OIS::KC_UP,0);
	}
	else if (key=="Down")
	{
		return KeyEvent(0,OIS::KC_DOWN,0);
	}
	else if (key=="Enter")
	{
		return KeyEvent(0,OIS::KC_RETURN,0);
	}
	else if (key=="Backspace")
	{
		return KeyEvent(0,OIS::KC_BACK,0);
	}
	else if (key=="Control")
	{
		printf("Control location:%lu\n",keyEvent->location);
		if (keyEvent->location == 1)
			return KeyEvent(0,OIS::KC_LCONTROL,0);
		else if (keyEvent->location == 2)
			return KeyEvent(0,OIS::KC_RCONTROL,0);
		else {
			return KeyEvent(0,OIS::KC_RCONTROL,0);
		}
	}
	else if (key=="F1")
	{
		return KeyEvent(0,OIS::KC_F1,0);
	}
	else if (key=="F2")
	{
		return KeyEvent(0,OIS::KC_F2,0);
	}
	else if (key=="F3")
	{
		return KeyEvent(0,OIS::KC_F3,0);
	}
	else if (key=="F4")
	{
		return KeyEvent(0,OIS::KC_F4,0);
	}
	else if (key=="F5")
	{
		return KeyEvent(0,OIS::KC_F5,0);
	}
	else if (key=="F6")
	{
		return KeyEvent(0,OIS::KC_F6,0);
	}
	else if (key=="F7")
	{
		return KeyEvent(0,OIS::KC_F7,0);
	}
	else if (key=="F8")
	{
		return KeyEvent(0,OIS::KC_F8,0);
	}
	else if (key=="F9")
	{
		return KeyEvent(0,OIS::KC_F9,0);
	}
	else if (key=="Del")
	{
		return KeyEvent(0,OIS::KC_DELETE,0);
	}
	else if (key=="Home")
	{
		return KeyEvent(0,OIS::KC_HOME,0);
	}
	else if (key=="End")
	{
		return KeyEvent(0,OIS::KC_END,0);
	}
	else if (key=="Insert")
	{
		return KeyEvent(0,OIS::KC_INSERT,0);
	}
	else if (key=="Shift")
	{
		if (keyEvent->location == 1)
			return KeyEvent(0,OIS::KC_LSHIFT,0);
		else if (keyEvent->location == 2)
			return KeyEvent(0,OIS::KC_RSHIFT,0);
		else {
			return KeyEvent(0,OIS::KC_LSHIFT,0);
		}
	}
//	else if (key=="")
//	{
//		return KeyEvent(0,OIS::KC_,0);
//	}
	else
	{
		printf("Unknown key:%s",key.c_str());
	}




	return KeyEvent(0, OIS::KC_UNASSIGNED, 0);
}
}
