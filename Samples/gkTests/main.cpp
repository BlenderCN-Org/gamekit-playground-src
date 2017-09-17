#include <gkScene.h>
#include "gkEngine.h"
#include "gkLogger.h"
#include "Loaders/Blender2/gkBlendFile.h"
#include "Loaders/Blender2/gkBlendLoader.h"
#include "gkCoreApplication.h"
#include "gkWindowSystem.h"
#include "gkInput.h"

#include "gkGameObject.h"

#include "gkMathUtils.h"

class ttMain : public gkCoreApplication, public gkWindowSystem::Listener
{
public:
	ttMain() : m_sceneFile(0),m_outer(0),m_inner(0),m_core(0)
	{
		m_prefs.grabInput=false;
	}

	~ttMain() {}
	virtual bool setup();
	void tick(gkScalar tick);
	virtual void keyPressed(const gkKeyboard& key, const gkScanCode& sc);

private:
	gkBlendFile* m_sceneFile;
	gkGameObject* m_outer;
	gkGameObject* m_inner;
	gkGameObject* m_core;
};

bool ttMain::setup()
{
	gkWindowSystem::getSingleton().addListener(this);
	m_sceneFile = gkBlendLoader::getSingleton().loadFile("data/scene.blend");
	gkScene* scene = m_sceneFile->getSceneByName("Scene");

	scene->createInstance();

	m_outer = scene->getObject("outer");
	m_inner = scene->getObject("inner");
	m_core = scene->getObject("core");

	return true;
}

void ttMain::keyPressed(const gkKeyboard& key, const gkScanCode& sc)
{
	if (sc == KC_ESCKEY)
	{
		m_engine->requestExit();
	}
}


void ttMain::tick(gkScalar tick)
{
	gkLogger::write("TICK",true);

	m_core->rotate(gkEuler(0.1f,0.1f,0.1f));
	m_inner->rotate(gkEuler(0.2f,0.25f,0.3f));
	m_outer->rotate(gkEuler(0.4f,0.2f,0.1f));

}

int main()
{

	ttMain m;
	m.run();

}
