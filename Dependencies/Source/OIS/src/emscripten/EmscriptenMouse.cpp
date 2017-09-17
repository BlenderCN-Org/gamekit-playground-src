#include "OISException.h"

#include "emscripten/EmscriptenMouse.h"
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

EmscriptenMouse::EmscriptenMouse(bool buffered,EmscriptenInputManager* creator)
	: Mouse(creator->inputSystemName(),buffered,0,creator)
{

}

void EmscriptenMouse::_updateScreenSize()
{
	EmscriptenInputManager* creator = static_cast<EmscriptenInputManager*>(getCreator());
	mState.width = creator->getScreenWidth();
	mState.height = creator->getScreenHeight();
}

void EmscriptenMouse::_initialize()
{
	_updateScreenSize();
	emscripten_set_mousedown_callback(0,this,false,EmscriptenMouse::mousedown_callback);
	emscripten_set_mouseup_callback(0,this,false,EmscriptenMouse::mouseup_callback);
	emscripten_set_mousemove_callback(0,this,false,EmscriptenMouse::mousemove_callback);
	emscripten_set_wheel_callback(0,this,false,EmscriptenMouse::mousewheel_callback);
}

void EmscriptenMouse::setMousePosition(int eventType,const EmscriptenMouseEvent* mouseEvent)
{
	mState.X.abs = mouseEvent->canvasX;
	mState.Y.abs = mouseEvent->canvasY;

	mState.X.rel = mouseEvent->movementX;
	mState.Y.rel = mouseEvent->movementY;

	//printf("MouseEvent:%i %i x %i\n",eventType,mouseEvent->canvasX,mouseEvent->canvasY);
}

EM_BOOL EmscriptenMouse::mousedown_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	EmscriptenMouse* thizz = static_cast<EmscriptenMouse*>(userData);
	thizz->setMousePosition(eventType,mouseEvent);
	thizz->mListener->mousePressed(MouseEvent(thizz,thizz->mState),emBtn2oisBtn(mouseEvent->button));

	return false;
}
EM_BOOL EmscriptenMouse::mouseup_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	EmscriptenMouse* thizz = static_cast<EmscriptenMouse*>(userData);
	thizz->setMousePosition(eventType,mouseEvent);
	thizz->mListener->mouseReleased(MouseEvent(thizz,thizz->mState),emBtn2oisBtn(mouseEvent->button));

	return false;
}
EM_BOOL EmscriptenMouse::mousemove_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	EmscriptenMouse* thizz = static_cast<EmscriptenMouse*>(userData);
	thizz->setMousePosition(eventType,mouseEvent);
	thizz->mListener->mouseMoved(MouseEvent(thizz,thizz->mState));

	return false;
}
EM_BOOL EmscriptenMouse::mousewheel_callback(int eventType, const EmscriptenWheelEvent* mouseWheelEvent, void* userData)
{
	EmscriptenMouse* thizz = static_cast<EmscriptenMouse*>(userData);
	//printf("Wheel type:%lu  | %d | %d | %d ",mouseWheelEvent->deltaMode,mouseWheelEvent->deltaX,mouseWheelEvent->deltaY,mouseWheelEvent->deltaZ);
	thizz->mState.Z.rel = (int)(mouseWheelEvent->deltaY*10);
	thizz->mListener->mouseMoved(MouseEvent(thizz,thizz->mState));

	return false;
}
}
