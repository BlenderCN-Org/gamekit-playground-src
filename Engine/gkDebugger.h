/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Xavier T.

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
#ifndef _gkDebugger_h_
#define _gkDebugger_h_


#include "gkCommon.h"
#include "gkMathUtils.h"
#include "OgreSimpleRenderable.h"
#include "OgreHardwareVertexBuffer.h"

#include "OgreManualObject.h"

#include "gkNavPath.h"
#ifdef OGREKIT_COMPILE_RECAST
# include "DebugDraw.h"
# include "RecastDebugDraw.h"
# include "Recast.h"
#endif

class gkSoundProperties;
class gkCurve;

// for debugging / building line lists
class gkDebugger : public Ogre::SimpleRenderable
{
public:

	struct DebugVertex
	{
		gkVector3 v;
		unsigned int color;
	};


	typedef utArray<DebugVertex> Buffer;


public:
	gkDebugger(gkScene* parent);
	virtual ~gkDebugger();

	void drawLine(const gkVector3& from, const gkVector3& to, const gkVector3& color);
	void drawCircle(const gkVector3& position, float radius,const gkVector3& color=gkVector3(1.0f,1.0f,1.0f));

	void clear(void);

	void drawCurve(gkCurve* curve,const gkVector3& color=gkVector3(1, 0.5, 0.0));
	void drawPath(gkNavPath* path);

#ifdef OGREKIT_OPENAL_SOUND
	void draw3dSound(const gkSoundProperties& props);
#endif

	void flush(void);




protected:


	Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
	Ogre::Real getBoundingRadius(void) const;


	void verifyNode(void);
	void growBuffer(UTsize newSize);

	Ogre::SceneNode*    m_node;
	gkScene*            m_parent;
	Ogre::Real          m_radius;
	Buffer              m_lineBuf;
	Ogre::Vector3       m_bbmin, m_bbmax;
	UTsize              m_bufSize;
	int                 m_flags;
	bool                m_d3dColor;

	Ogre::HardwareVertexBufferSharedPtr m_buffer;

	// duDraw related
	float 				m_size;
	bool				m_quadRendering;


};


class gkRecastDebugger : public duDebugDraw
{
public:
	gkRecastDebugger(gkScene* scene=0);

	virtual ~gkRecastDebugger(){
// TOOD: seems that scenegraph is taking care of getting rid of the manualobject
//		if (m_currentManualObject)
//			delete m_currentManualObject;
	}

	virtual void depthMask(bool state);

	// Begin drawing primitives.
	// Params:
	//  prim - (in) primitive type to draw, one of rcDebugDrawPrimitives.
	//  nverts - (in) number of vertices to be submitted.
	//  size - (in) size of a primitive, applies to point size and line width only.
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);

	// Submit a vertex
	// Params:
	//  pos - (in) position of the verts.
	//  color - (in) color of the verts.
	virtual void vertex(const float* pos, unsigned int color);

	// Submit a vertex
	// Params:
	//  x,y,z - (in) position of the verts.
	//  color - (in) color of the verts.
	virtual void vertex(const float x, const float y, const float z, unsigned int color);

	/// Submit a vertex
	///  @param pos [in] position of the verts.
	///  @param color [in] color of the verts.
	virtual void vertex(const float* pos, unsigned int color, const float* uv);

	/// Submit a vertex
	///  @param x,y,z [in] position of the verts.
	///  @param color [in] color of the verts.
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);

	virtual void texture(bool state)
	{
		int a = 0;
		// TODO
	}

	// End drawing primitives.
	virtual void end();

	void flush(const gkVector3& pos=gkVector3(0,0,0),const gkEuler& rot=gkEuler(90,0,0));

	void drawLine(const gkVector3& from, const gkVector3& to, unsigned int color);
	void drawTriangle(const gkVector3& t1,const gkVector3& t2,const gkVector3& t3, unsigned int color);
	void clear()
	{
		m_currentManualObject->clear();
	}

	void drawPath(gkNavPath* navPath,const gkColor& col);

	void setYZConversionMode(bool b) { m_convertVertex = b; }
	bool isYZConversionMode() { return m_convertVertex; }

private:
	bool 				m_convertVertex;

	rcPolyMeshDetail	m_recastPolyMeshDetail;
	Ogre::ManualObject* m_currentManualObject;
	gkScene* m_scene;
	float m_size;
	bool m_quadRendering;
	Ogre::SceneNode* mSceneNode;



};

#endif//_gkDebugger_h_
