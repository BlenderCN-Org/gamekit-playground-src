#include <emscripten/html5.h>
#include <Samples/EmscriptenDemo/Shared/gkEmscriptenGame.h>

#include "gkEngine.h"

#include "gkLogger.h"

#include "gkValue.h"

#include "emscripten/EmscriptenInputManager.h"

#include "gkWindow.h"

#include "OgreRenderWindow.h"

#include "gkWindowSystem.h"
static EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL mouseup_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL mousemove_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
static EM_BOOL mousewheel_callback(int eventType, const EmscriptenWheelEvent* mouseEvent, void* userData);
static const char* beforeunload_callback(int eventType, const void* reserved, void* userData);
static void _mainloop(void* target);

static OIS::EmscriptenInputManager* oisinput;
static gkWindow* gkWin=0;
static Ogre::RenderWindow* renderWin=0;

int main( int argc, const char* argv[] ) {
	try
	{
		int xSize;
		int ySize;
		int fullScreen;
		emscripten_get_canvas_size(&xSize,&ySize,&fullScreen);

		gkLogger::write("Emscripten: res "+gkToString(xSize)+"x"+gkToString(ySize)+" fullscreen:"+gkToString(fullScreen),true);

		gkEmscriptenGame* game = new gkEmscriptenGame("game.dat");

		game->getPrefs().verbose=true;
		game->getPrefs().debugFps=true;
		game->getPrefs().winsize = gkVector2(xSize,ySize);
		game->getPrefs().fsaa=false;
		game->getPrefs().fsaaSamples=0;
		game->initialize();

		gkEngine::getSingleton().initializeStepLoop();
		emscripten_set_main_loop_arg(_mainloop, static_cast<void*>(game), 0, 1);
		gkEngine::getSingleton().finalizeStepLoop();
		delete game;
        game = NULL;
	}
	catch (std::exception& e)
	{
		emscripten_run_script((std::string("alert('") + e.what() + "')").c_str());
	}
	return 0;
}



static void _mainloop(void* target)
{
	gkEmscriptenGame* game = static_cast<gkEmscriptenGame*>(target);
	gkEngine::getSingleton().stepOneFrame();
}
