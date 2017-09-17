/*
 * AndroidGame.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: ttrocha
 */

#include "gkBlackberryGame.h"

#include "Loaders/Blender2/gkBlendLoader.h"
#include "Loaders/Blender2/gkBlendFile.h"
#include "gkUtils.h"
#include "gkPath.h"
#include "gkLogger.h"
#include "gkWindow.h"
#include "gkWindowSystem.h"
#include "gkScene.h"
#include "Addons.h"
#include "gkMessageManager.h"
#include <bps/virtualkeyboard.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkMathNodeImpl.h>
#include <BNodes/out-src/src-nodes/NodesImpl/gkNodeTreeNodeImpl.h>
#include "gkGamekit.h"


gkBlackberryGame::gkBlackberryGame(const gkString& initialBlend)
	: m_initialBlendName(initialBlend)
{
}

gkBlackberryGame::~gkBlackberryGame() {
	// TODO Auto-generated destructor stub
}

bool gkBlackberryGame::setup()
{
	_LOG_FOOT_;

	gkLogger::enable("ogrkeit",true);


	Gamekit_addNodeInstancer();
	gkNodeTreeConverter::addNodeInstancer(new MathNodeInstancer());
	gkNodeTreeConverter::addNodeInstancer(new gkNodeTreeNodeCustomInstancer());

	gkAddonAdder::addAddons();
	gkAddonAdder::setupAddons();

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


