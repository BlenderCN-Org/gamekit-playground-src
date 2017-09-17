/*
 * AndroidGame.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: ttrocha
 */

#include <gkMathNodeImpl.h>
#include <gkNodeTreeNodeImpl.h>
#include <gkGamekit.h>
#include <Samples/EmscriptenDemo/Shared/gkEmscriptenGame.h>
#include "Loaders/Blender2/gkBlendLoader.h"
#include "Loaders/Blender2/gkBlendFile.h"
#include "gkUtils.h"
#include "gkPath.h"
#include "gkLogger.h"
#include "gkWindow.h"
#include "gkWindowSystem.h"
#include "gkScene.h"

#include "gkMessageManager.h"


gkEmscriptenGame::gkEmscriptenGame(const gkString& initialBlend)
	: m_initialBlendName(initialBlend)
{
}

gkEmscriptenGame::~gkEmscriptenGame() {
	// TODO Auto-generated destructor stub
}

bool gkEmscriptenGame::setup()
{
	_LOG_FOOT_;

	Gamekit_addNodeInstancer();
	gkNodeTreeConverter::addNodeInstancer(new MathNodeInstancer());
	gkNodeTreeConverter::addNodeInstancer(new gkNodeTreeNodeCustomInstancer());

	gkLogger::enable("ogrkeit",true);

	// try to load the blend
	if (m_initialBlendName.empty()) // check filename
	{
		_LOG_FOOT_
		gkLogger::write("No blend-file specified!!! Abort!!",true);
		return false;
	}
	else if (gkPath(m_initialBlendName).exists()) // check file (sd-card)
	{
		_LOG_FOOT_
		m_initialBlend = gkBlendLoader::getSingleton().loadFile(gkUtils::getFile(m_initialBlendName), gkBlendLoader::LO_ALL_SCENES);
	}
	else
	{
		_LOG_FOOT_
		gkLogger::write("Couldn't find blend-file:"+m_initialBlendName+"!!! Abort!!",true);
		return false;
	}

	if (!m_initialBlend)
	{
		gkLogger::write("ERROR: Something went wrong loading blend-file:"+m_initialBlendName+"!!! Abort!!",true);
		return false;
	}

	// retrieve a first scene from the current blend-file
	// try to find a scene called "StartScene" (case-sensitive)
	// if not found take the scene that was active when writing the blend-file
	// retrieve scenes afterward with:
	// gkSceneManager::getSingleton().getByName("StartScene")

	gkScene* m_scene = m_initialBlend->getSceneByName("StartScene");

	if (!m_scene)
		m_scene = m_initialBlend->getMainScene();

	if (!m_scene)
	{
		gkLogger::write("No usable scenes found in blend.\n",true);
		return false;
	}

	// create the scene
	m_scene->createInstance();

	gkWindow* win = gkWindowSystem::getSingleton().getMainWindow();



	return true;
}


