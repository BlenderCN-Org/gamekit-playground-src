/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

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
#include "gkOgreVideoBackground.h"
#include "OgreRectangle2D.h"
#include "OgreSceneNode.h"
#include "OgreAxisAlignedBox.h"
#include "gkScene.h"
#include "OgreMaterialManager.h"
#include "OgrePrerequisites.h"

gkVideoBackground::~gkVideoBackground()
{
	if (m_rect)
		delete m_rect;
}


void gkVideoBackground::init()
{
	   // Create background rectangle covering the whole screen
	   m_rect = new Ogre::Rectangle2D(true);
	   m_rect->setCorners(-1.0, 1.0, 1.0, -3.0);
	   // Render the background before everything else
	   m_rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
	   // Set the bounding box to something big
	   m_rect->setBoundingBox(Ogre::AxisAlignedBox(-100000.0*gkVector3::UNIT_SCALE, 100000.0*gkVector3::UNIT_SCALE));
	   // Attach background to the scene
	   Ogre::SceneNode* node = gkEngine::getSingleton().getActiveScene()->getManager()->getRootSceneNode()->createChildSceneNode("Background");
	   node->attachObject(m_rect);
	   m_initalized=true;
}

void gkVideoBackground::setMaterial(const gkString& material) {
   Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(material);
   if (mat.isNull())
   {
	   gkLogger::write("ERROR: unknown videobackground material: "+material);
	   return;
   }

   if (!m_initalized)
		init();

   mat->setDepthWriteEnabled(false);
   m_rect->setMaterial(material);

}


UT_IMPLEMENT_SINGLETON(gkVideoBackground)

