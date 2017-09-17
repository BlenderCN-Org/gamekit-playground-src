/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2012 Thomas Trocha

    Contributor(s): oneminute.
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
#ifndef _gkCurveObject_h_
#define _gkCurveObject_h_

#include "gkGameObject.h"
#include "gkSerialize.h"


class gkCurve : public gkGameObject
{
public:
	gkCurve(gkInstancedManager* creator, const gkResourceName& name, const gkResourceHandle& handle);
	virtual ~gkCurve();


	gkCurveProperties* getCurveProperties(int subcurve=0,bool create=true);

	void updateProperties(void);

	int getPointCount(int subCurveNr=0);

	const gkVector3 getPoint(int nr,int subCurveNr=0);

	// generate a Ogre SimpleSpline and get the interpolated value by the delta [0,1]
	const gkVector3 getDeltaPoint(float dt,int curveNr=0);

	bool isCyclic(int subCurveNr=0) { gkCurveProperties* props = getCurveProperties(subCurveNr);return props->m_isCyclic; }

	void generateBezierPoints(gkScalar t);

	void showDebug();

	bool isBezier(int subCurveNr=0) { gkCurveProperties* props = getCurveProperties(subCurveNr); return props->m_type == gkCurveProperties::CU_Bezier;}

	int getSubCurveAmount() { return m_subcurves.size();}

private:
	gkGameObject* clone(const gkString& name);
	void makeBezier(gkScalar _t, utArray<gkVector3>& _first, utArray<gkVector3>& _second);	

	typedef utHashTable<utIntHashKey,gkCurveProperties*> SubCurveMap;
	typedef utHashTable<utIntHashKey,Ogre::SimpleSpline*> SubCurveSplineMap;

	SubCurveMap m_subcurves;
	SubCurveSplineMap m_subcurveSplines;

	utArray<gkVector3>		m_bezierPoints;

	gkCurveProperties::CurvePoints _normPoints;

	virtual void createInstanceImpl(void);
	virtual void destroyInstanceImpl(void);

	void generateCurve(int subcurve);

	void _normalize(float start,float end, float distance);

};

#endif//_gkCurveObject_h_
