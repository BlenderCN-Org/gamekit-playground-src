/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira.

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
#include "Ogre.h"
#include "gkNavPath.h"
#include "gkNavMeshData.h"
#include "gkScene.h"
#include "gkEngine.h"
#include "gkDynamicsWorld.h"
#include "gkPhysicsDebug.h"

#include "gkValue.h"
using namespace OpenSteer;

gkNavPath::gkNavPath() : m_isCyclic(false)
{
	m_scene = gkEngine::getSingleton().getActiveScene();

	GK_ASSERT(m_scene);
}

gkNavPath::~gkNavPath()
{
}

#ifdef OGREKIT_COMPILE_RECAST
bool gkNavPath::createFromRecast(PDT_NAV_MESH navMesh, const gkVector3& from, const gkVector3& to, const gkVector3& polyPickExt, int maxPathPolys, gkScalar pathRadius)
{
	m_path.clear();
	if (gkRecast::findPath(navMesh, from, to, polyPickExt, maxPathPolys, m_path))
	{
		for (int i=0;i<m_path.size();i++)
		{
			gkVector3 vec(m_path.at(i));
			gkLogger::write("Path "+gkToString(i)+": "+gkToString(vec),true);
		}

		m_pathRadius = PATH_RADIUS(m_path.size(), pathRadius);

		GK_ASSERT(sizeof(OpenSteer::Vec3) == sizeof(gkVector3));

		setPathway(m_path.size(), (OpenSteer::Vec3*)&(m_path[0]), &(m_pathRadius[0]), false);

		return true;
	}
	return false;
}
#endif

void gkNavPath::createFromCurve(gkCurve* curve,float radius,float ab)
{
	// if bezier-curve generate the points first from the beziers
	if (curve->isBezier()) {
		curve->generateBezierPoints(ab);
	}

	for (int i=0;i<curve->getPointCount();i++)
	{
		addPoint(curve->getPoint(i),radius);
	}

	setCyclic(curve->isCyclic());

	build();
}



void gkNavPath::build()
{
	setPathway(m_path.size(), (OpenSteer::Vec3*)&(m_path[0]), &(m_pathRadius[0]), isCyclic());
}

void gkNavPath::showPath()
{
	gkDebugger* debug = m_scene->getDebugger();

	if (debug)
	{
		unsigned int n = m_path.size();

		if (n)
		{
			static const gkVector3 RED_COLOR(1, 0, 0);

			gkVector3 oldPoint = m_path.at(0);

			for (unsigned int i = 1; i < n; i++)
			{
				gkVector3 point = m_path.at(i);

				debug->drawLine(
				    oldPoint,
				    point,
				    RED_COLOR
				);

				oldPoint = point;
			}
		}
	}
}
