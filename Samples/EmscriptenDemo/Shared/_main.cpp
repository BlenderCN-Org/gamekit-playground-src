/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <screen/screen.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include <bps/orientation.h>
#include <bps/virtualkeyboard.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


#include <EGL/egl.h>
#include "gkEngine.h"
#include "input/event_types.h"
#include "blackberry/BlackBerryInputManager.h"
#include <input/screen_helpers.h>
#include "gkWindowSystem.h"
#include "gkWindow.h"
#include "Sound/gkSoundManager.h"
#include <sys/keycodes.h>
#include "gkMessageManager.h"
#include "gkCommon.h"
#include <bps/sensor.h>
#include <Samples/EmscriptenDemo/Shared/gkEmscriptenGame.h>
#include "gkValue.h"




static const int ACCELEROMETER_RATE = 25000;
float force_x, force_y, force_z;
static screen_context_t screen_cxt;
static gkBlackberryGame* game=0;
static OIS::BlackBerryInputManager* oisinput;
static bool shutdown = false;
int exit_application =0;
//static int orientation = NAVIGATOR_PORTRAIT;
static int angle=0;
static gkWindow* gkWin=0;
static Ogre::RenderWindow* renderWin=0;
static bool windowActive = true;
static navigator_window_state_t state = NAVIGATOR_WINDOW_FULLSCREEN;

// Message listener to be called from gamekits message-system for e.g. showing the keyboard
// or other platformspecfic stuff

class EventListener : public gkMessageManager::GenericMessageListener
{
	void handleMessage(gkMessageManager::Message* message)
	{
		if (message) {
			if (message->m_subject=="__showkeyboard") {
				if (message->m_body=="true") {
					virtualkeyboard_show();
				} else {
					virtualkeyboard_hide();
				}
			}
			else if (message->m_subject=="__keyboardlayout") {
				if (message->m_body == "default") {
					virtualkeyboard_change_options(VIRTUALKEYBOARD_LAYOUT_DEFAULT, VIRTUALKEYBOARD_ENTER_DEFAULT);
				}
				else if (message->m_body == "url") {
					virtualkeyboard_change_options(VIRTUALKEYBOARD_LAYOUT_URL, VIRTUALKEYBOARD_ENTER_DEFAULT);
				}
				else if (message->m_body == "web") {
					virtualkeyboard_change_options(VIRTUALKEYBOARD_LAYOUT_WEB, VIRTUALKEYBOARD_ENTER_DEFAULT);
				}
				else if (message->m_body == "email") {
					virtualkeyboard_change_options(VIRTUALKEYBOARD_LAYOUT_EMAIL, VIRTUALKEYBOARD_ENTER_DEFAULT);
				}
			}
			else if (message->m_subject=="start_sensor")
			{
				if (message->m_body=="accel")
				{
					if (sensor_is_supported(SENSOR_TYPE_ACCELEROMETER))
					{
						sensor_set_rate(SENSOR_TYPE_ACCELEROMETER, ACCELEROMETER_RATE);
						sensor_set_skip_duplicates(SENSOR_TYPE_ACCELEROMETER, true);
						sensor_request_events(SENSOR_TYPE_ACCELEROMETER);
					}
					else
					{
							gkLogger::write("Accelerometer is not supported by bb-device!",true);
					}

				}
			}

		}
	}
};

void handleWindowState(bps_event_t *event)
{
	navigator_window_state_t currentState;
	currentState = navigator_event_get_window_state(event);
	if (currentState!=state){
		switch (currentState) {
			case NAVIGATOR_WINDOW_FULLSCREEN:
				if (gkSoundManager::getSingletonPtr())
					gkSoundManager::getSingleton().restart();
				break;
			case NAVIGATOR_WINDOW_THUMBNAIL:
				if (gkSoundManager::getSingletonPtr())
					gkSoundManager::getSingleton().stop();
				break;
			case NAVIGATOR_WINDOW_INVISIBLE:
				if (gkSoundManager::getSingletonPtr())
					gkSoundManager::getSingleton().stop();
				break;
		}
		state = currentState;
	}
}


bool handleNavigatorEvent(bps_event_t *event) {
	switch (bps_event_get_code(event)) {



	case NAVIGATOR_ORIENTATION_CHECK: {

		navigator_orientation_check_response(event, true);
		break;
	}

	case NAVIGATOR_WINDOW_INACTIVE:
		windowActive = false;
		if (gkSoundManager::getSingletonPtr())
			gkSoundManager::getSingleton().stop();
		break;

	case NAVIGATOR_WINDOW_ACTIVE:
		windowActive = true;

		break;

	case NAVIGATOR_ORIENTATION: {
#ifdef __BB10__
		angle = navigator_event_get_orientation_angle(event);
//		orientation = navigator_event_get_orientation_mode(event);

		int width = atoi(getenv("WIDTH"));
		int height = atoi(getenv("HEIGHT"));
//		if (orientation == NAVIGATOR_PORTRAIT) {
//			renderWin->resize(width, height);
//		} else if (orientation == NAVIGATOR_LANDSCAPE) {
//			renderWin->resize(height, width);
//		}
#endif
		gkWin->windowResized(renderWin);

		navigator_done_orientation(event);
		break;
	}

	case NAVIGATOR_WINDOW_STATE:
		handleWindowState(event);
		break;

	case NAVIGATOR_SWIPE_DOWN:
		//do nothing
		break;

	case NAVIGATOR_EXIT:

		exit_application = 1;
		break;

	default:

		return false;
	}

	return true;
}


int convertToOISModifiers(int bbModifiers) {
	int result = 0;

	if (bbModifiers & KEYMOD_SHIFT)
		result |= OIS::Keyboard::Shift;
	if (bbModifiers & KEYMOD_CTRL)
		result |= OIS::Keyboard::Ctrl;
	if (bbModifiers & KEYMOD_ALT)
		result |= OIS::Keyboard::Alt;

	return result;
}

static void handleScreenEvent(bps_event_t *event) {
    int screen_val, buttons;

    static bool mouse_pressed = false;

    screen_event_t screen_event = screen_event_get_event(event);

    //Query type of screen event and its location on the screen
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE,
            &screen_val);

    if (screen_val == SCREEN_EVENT_MTOUCH_TOUCH || screen_val == SCREEN_EVENT_MTOUCH_MOVE || screen_val == SCREEN_EVENT_MTOUCH_RELEASE)
    {
        mtouch_event_t mte;
        int rc = screen_get_mtouch_event(screen_event, &mte,0);

        oisinput->injectMultiTouch(screen_val,mte.contact_id,mte.x,mte.y,mte.width,mte.pressure);
    } else if (screen_val == SCREEN_EVENT_POINTER) {
        int pair[2];
        screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION,
                pair);

        //This is a mouse move event, it is applicable to a device with a usb mouse or simulator
        screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS,
                &buttons);

        if (buttons == SCREEN_LEFT_MOUSE_BUTTON) {
            //Left mouse button is pressed
            if (mouse_pressed) {
                oisinput->injectTouch(SCREEN_EVENT_MTOUCH_MOVE, pair[0], pair[1]);
            } else {
            	mouse_pressed = true;
                oisinput->injectTouch(SCREEN_EVENT_MTOUCH_TOUCH, pair[0], pair[1]);
            }
        } else {
            if (mouse_pressed) {
                //Left mouse button was released, handle left click
                oisinput->injectTouch(SCREEN_EVENT_MTOUCH_RELEASE, pair[0], pair[1]);
                mouse_pressed=false;
            }
        }
    }
    else if (screen_val == SCREEN_EVENT_KEYBOARD) {
    	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_FLAGS, &screen_val);
    	if (screen_val & (KEY_DOWN | KEY_REPEAT)) {
        	int modifiers;
    		// check the keyboard-modifiers
        	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_MODIFIERS,&modifiers);
    		modifiers =convertToOISModifiers(modifiers);

    		// check the keycode
    		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM,&screen_val);
        	oisinput->injectKey(0,modifiers,screen_val);

    	} else {
        	int modifiers;
    		// check the keyboard-modifiers
        	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_MODIFIERS,&modifiers);
    		modifiers =convertToOISModifiers(modifiers);

    		screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_KEY_SYM,&screen_val);
        	oisinput->injectKey(1,modifiers,screen_val);
    	}
    }
}

static void handleKeyboardEvent(bps_event_t *event) {

	int code = bps_event_get_code(event);

	switch (code)
	{
		case VIRTUALKEYBOARD_EVENT_VISIBLE:
			gkMessageManager::getSingleton().sendMessage("","",GK_MSG_INTERNAL_GAME_VIRTUALKEYBOARD_VISIBLE,"");
			break;
		case VIRTUALKEYBOARD_EVENT_HIDDEN:
			gkMessageManager::getSingleton().sendMessage("","",GK_MSG_INTERNAL_GAME_VIRTUALKEYBOARD_HIDDEN,"false");
			break;
	}
}

static void handleAccelerometer(bps_event_t* event)
{
	int code = bps_event_get_code(event);
	if (code == SENSOR_ACCELEROMETER_READING)
	{
	       sensor_event_get_xyz(event,&force_x,&force_y,&force_z);
	       // Handle accelerometer readings here
	       gkMessageManager::getSingleton().sendMessage("","","accel",gkToString(force_x)+":"+gkToString(force_y)+":"+gkToString(force_z));
	}
}

void startBlackBerryApp() {

//	blackBerryMultiTouch = new BlackBerryMultiTouch();

	screen_create_context(&screen_cxt, 0);
	//Initialize BPS library
	bps_initialize();

	screen_request_events(screen_cxt);
	virtualkeyboard_request_events(0);
	navigator_request_events(0);
	navigator_rotation_lock(true);



	game = new gkBlackberryGame("gk_android.blend");
	game->getPrefs().resources="./app/native/res.cfg";
	game->getPrefs()._bbScreen = &screen_cxt;
	game->getPrefs().debugFps=true;
	game->getPrefs().buildStaticGeometry=true;
	game->initialize();
	gkWin = gkWindowSystem::getSingleton().getMainWindow();
	renderWin = gkWin->getRenderWindow();
	oisinput = static_cast<OIS::BlackBerryInputManager*>(gkWin->getInputManager());
	oisinput->setWindowSize(gkWin->getWidth(),gkWin->getHeight());

	// register the eventlistener
	EventListener evtListener;
	gkMessageManager::getSingleton().addListener(&evtListener);

	gkEngine::getSingleton().initializeStepLoop();


	//Initialize BPS library
//	while (!exit_application) {
	while (!exit_application) {
		//Request and process all available BPS events
		bps_event_t *event = NULL;

		for (;;) {
			if (BPS_SUCCESS != bps_get_event(&event, 0)) {
				fprintf(stderr, "bps_get_event failed\n");
				break;
			}

			if (event) {
				int domain = bps_event_get_domain(event);

				if (domain == screen_get_domain())
				{
					handleScreenEvent(event);
				}
				else if (domain == navigator_get_domain())
				{
					handleNavigatorEvent(event);
				}
				else if (domain == virtualkeyboard_get_domain())
				{
					handleKeyboardEvent(event);
				}
				else if (domain == sensor_get_domain())
				{
					handleAccelerometer(event);
				}

			} else {
				break;
			}
		}


		if (state==NAVIGATOR_WINDOW_FULLSCREEN)
			gkEngine::getSingleton().stepOneFrame();

	}
	gkEngine::getSingleton().finalizeStepLoop();
	//Stop requesting events from libscreen
	screen_stop_events(screen_cxt);
	screen_destroy_context(screen_cxt);
	//Shut down BPS library for this process
	bps_shutdown();


}



int main(int argc, char *argv[]) {
	startBlackBerryApp();
}

//static float width, height, angle;
//static int shutdown, menu_active, menu_hide_animation, menu_show_animation;
//
//
//void update();
//void render();
//
//static void handleClick(float x,float y) {}
//

//

//
//static void handle_events() {
//    int rc = BPS_SUCCESS;
//
//    //Request and process available BPS events
//    for(;;) {
//        bps_event_t *event = NULL;
//        rc = bps_get_event(&event, 0);
//
//        assert(rc == BPS_SUCCESS);
//
//        if (event) {
//            int domain = bps_event_get_domain(event);
//
//            if (domain == screen_get_domain()) {
//                handleScreenEvent(event);
//            } else if (domain == navigator_get_domain()) {
//                handleNavigatorEvent(event);
//            }
//        } else {
//            break;
//        }
//    }
//}
//
//int resize(bps_event_t *event) {
//    //Query width and height of the window surface created by utility code
//    EGLint surface_width, surface_height;
//
//    if (event) {
//        int angle = navigator_event_get_orientation_angle(event);
//
//        //Let bbutil rotate current screen surface to this angle
//        if (EXIT_FAILURE == bbutil_rotate_screen_surface(angle)) {
//            fprintf(stderr, "Unable to handle orientation change\n");
//            return EXIT_FAILURE;
//        }
//
//    }
//
//    eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
//    eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);
//
//    EGLint err = eglGetError();
//    if (err != 0x3000) {
//        fprintf(stderr, "Unable to query EGL surface dimensions\n");
//        return EXIT_FAILURE;
//    }
//
//    width = (float) surface_width;
//    height = (float) surface_height;
//
//
//    update();
//
//    if (event) {
//        render();
//
//        navigator_done_orientation(event);
//    }
//
//    return EXIT_SUCCESS;
//}
//
//int initialize() {
//    //Load shadow and button textures
//
//}
//
//
//
//void update() {
//
//}
//
//void render() {
//
//}
//
//
//}

//int main(int argc, char *argv[]) {
//    //Create a screen context that will be used to create an EGL surface to to receive libscreen events
//    screen_create_context(&screen_cxt, 0);
//
//    //Initialize BPS library
//    bps_initialize();
//
//    //Use utility code to initialize EGL for rendering with GL ES 1.1
//    if (EXIT_SUCCESS != bbutil_init_egl(screen_cxt)) {
//        fprintf(stderr, "bbutil_init_egl failed\n");
//        screen_destroy_context(screen_cxt);
//        return 0;
//    }
//
//    //Initialize application logic
//    if (EXIT_SUCCESS != initialize()) {
//        fprintf(stderr, "initialize failed\n");
//        screen_destroy_context(screen_cxt);
//        return 0;
//    }
//
//    //Signal BPS library that navigator and screen events will be requested
//    if (BPS_SUCCESS != screen_request_events(screen_cxt)) {
//        fprintf(stderr, "screen_request_events failed\n");
//        screen_destroy_context(screen_cxt);
//        return 0;
//    }
//
//    if (BPS_SUCCESS != navigator_request_events(0)) {
//        fprintf(stderr, "navigator_request_events failed\n");
//        screen_destroy_context(screen_cxt);
//        return 0;
//    }
//
//    //Signal BPS library that navigator orientation is not to be locked
//    if (BPS_SUCCESS != navigator_rotation_lock(false)) {
//        fprintf(stderr, "navigator_rotation_lock failed\n");
//        screen_destroy_context(screen_cxt);
//        return 0;
//    }
//
//    while (!shutdown) {
//        // Handle user input and accelerometer
//        handle_events();
//        // Update scene contents
//        update();
//        // Draw Scene
//        render();
//    }
//
//    //Stop requesting events from libscreen
//    screen_stop_events(screen_cxt);
//
//
//    //Shut down BPS library for this process
//    bps_shutdown();
//
//    //Destroy libscreen context
//    screen_destroy_context(screen_cxt);
//    return 0;
//}
