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
#ifndef _gkNavPath_h_
#define _gkNavPath_h_

#include "gkCommon.h"
#include "gkMathUtils.h"
#include "gkRecast.h"
#include "PolylineSegmentedPathwaySegmentRadii.h"

#include "gkCurve.h"
class gkGameObject;
class gkScene;

class gkNavPath : public OpenSteer::PolylineSegmentedPathwaySegmentRadii
{
public:

	gkNavPath();

	~gkNavPath();

#ifdef OGREKIT_COMPILE_RECAST
	bool createFromRecast(PDT_NAV_MESH navMesh, const gkVector3& from, const gkVector3& to, const gkVector3& polyPickExt, int maxPathPolys, gkScalar pathRadius);
#endif

	void createFromCurve(gkCurve* curve,float radius=1.0f,float ab=0.1);

	void addPoint(const gkVector3& vec,const float radius=1.0f)
	{
		m_path.push_back(vec);
		m_pathRadius.push_back(radius);
	}

	void showPath();

	bool empty() const { return m_path.empty(); }

	void clear() { m_path.clear(); m_pathRadius.clear(); }

	gkVector3 getPoint(int index) { return m_path.at(index); }
	float getRadius(int index) { return m_pathRadius.at(index);}
	int getPointAmount() const { return m_path.size(); }

	void build();

	void setCyclic(bool isCyclic) { m_isCyclic = isCyclic; }
	bool isCyclic() { return m_isCyclic; }
private:

	PATH_POINTS m_path;

	typedef std::vector<gkScalar> PATH_RADIUS;
	PATH_RADIUS m_pathRadius;

	gkScene* m_scene;
	bool m_isCyclic;
};

#endif//_gkNavPath_h_
