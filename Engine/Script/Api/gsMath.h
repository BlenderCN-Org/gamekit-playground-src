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
#ifndef _gsMath_h_
#define _gsMath_h_

#include "gsCommon.h"
#include "gsMath.h"


class gsVector2
#ifndef SWIG
	: public gkVector2
#endif
{
public:
#ifdef SWIG
	float x, y;
#else
	gsVector2(const gkVector2& rhs) : gkVector2(rhs.x, rhs.y) {}
#endif
	gsVector2() : gkVector2(0, 0) {}
	gsVector2(float _x, float _y) : gkVector2(_x, _y) {}
	gsVector2(const gsVector2& rhs) : gkVector2(rhs.x, rhs.y) {}
	const char* __str__(void)
	{
		static char buffer[72];
		sprintf(buffer, "%f, %f, %f", x, y);
		return buffer;
	}

};

class gsQuaternion;

class gsVector3
#ifndef SWIG
	: public gkVector3
#endif
{
public:


#ifdef SWIG
	float x, y, z;
#else
	gsVector3(const gkVector3& rhs) : gkVector3(rhs.x, rhs.y, rhs.z) {}
#endif
	gsVector3() : gkVector3(0, 0, 0) {}
	gsVector3(float _x, float _y, float _z) : gkVector3(_x, _y, _z) {}
	gsVector3(const gsVector3& rhs) : gkVector3(rhs.x, rhs.y, rhs.z) {}
#ifdef OGREKIT_COMPILE_OPENSTEER
	gsVector3(const OpenSteer::Vec3 v) : gkVector3(v.x,v.y,v.z){}
#endif

	const char* __str__(void)
	{
		static char buffer[72];
		sprintf(buffer, "%f, %f, %f", x, y, z);
		return buffer;
	}

	void 		set(float _x,float _y, float _z) { x = _x; y=_y; z=_z;}
	float       normalize(void)                    { return normalise(); }
	float       dot(const gsVector3& v)            { return dotProduct(v); }
	gsVector3   cross(const gsVector3& v)          { return gsVector3(crossProduct(v));}
	float       length(void)                       { return gkVector3::length(); }
	float       length2(void)                      { return gkVector3::squaredLength(); }
	float       distance(const gsVector3& v)       { return gkVector3::distance(v); }
	float       distance2(const gsVector3& v)      { return gkVector3::squaredDistance(v); }
	bool      isNull(float epsilon=0.000001) { return (gkFuzzyEq(x,0,epsilon) && gkFuzzyEq(y,0,epsilon) && gkFuzzyEq(z,0,epsilon)); }
	gsQuaternion getRotationTo(const gsVector3& v);

	gsVector3 operator- (void)                { return -(gkVector3) * this; }
	gsVector3 operator+ (const gsVector3& v)  { return ((const gkVector3&) * this).operator + (v); }
	gsVector3 operator- (const gsVector3& v)  { return ((const gkVector3&) * this).operator - (v); }
	gsVector3 operator* (const gsVector3& v)  { return ((const gkVector3&) * this).operator * (v); }
	gsVector3 operator* (float v)             { return ((const gkVector3&) * this).operator * (v); }
	gsVector3 operator/ (const gsVector3& v)  { return ((const gkVector3&) * this).operator / (v); }
	gsVector3 operator/ (float v)             { return ((const gkVector3&) * this).operator / (v); }
};

class gsVector4
#ifndef SWIG
	: public gkVector4
#endif
{
public:


#ifdef SWIG
	float x, y, z, w;
#else
	gsVector4(const gkVector4& rhs) : gkVector4(rhs.x, rhs.y, rhs.z, rhs.w) {}
#endif
	gsVector4() : gkVector4(0, 0, 0,0) {}
	gsVector4(float _x, float _y, float _z, float _w) : gkVector4(_x, _y, _z, _w) {}
	gsVector4(const gsVector4& rhs) : gkVector4(rhs.x, rhs.y, rhs.z, rhs.w) {}

	const char* __str__(void)
	{
		static char buffer[72];
		sprintf(buffer, "%f, %f, %f, %f", x, y, z , w);
		return buffer;
	}

	float       dot(const gsVector4& v)            { return dotProduct(v); }
	void 		set(float _x,float _y, float _z, float _w) { x = _x; y=_y; z=_z; w=_w;}
	gsVector4 operator- (void)                { return -(gkVector4) * this; }
	gsVector4 operator+ (const gsVector4& v)  { return ((const gkVector4&) * this).operator + (v); }
	gsVector4 operator- (const gsVector4& v)  { return ((const gkVector4&) * this).operator - (v); }
	gsVector4 operator* (const gsVector4& v)  { return ((const gkVector4&) * this).operator * (v); }
	gsVector4 operator* (float v)             { return ((const gkVector4&) * this).operator * (v); }
	gsVector4 operator/ (const gsVector4& v)  { return ((const gkVector4&) * this).operator / (v); }
	gsVector4 operator/ (float v)             { return ((const gkVector4&) * this).operator / (v); }
};


class gsQuaternion
#ifndef SWIG
	: public gkQuaternion
#endif
{
public:

#ifdef SWIG
	float w, x, y, z;
#else
	gsQuaternion(const gkQuaternion& rhs) : gkQuaternion(rhs.w, rhs.x, rhs.y, rhs.z) {}
#endif
	gsQuaternion() : gkQuaternion(1, 0, 0, 0) {}
	gsQuaternion(float _w, float _x, float _y, float _z) : gkQuaternion(_w, _x, _y, _z) {}
	gsQuaternion(const gsQuaternion& rhs) : gkQuaternion(rhs.w, rhs.x, rhs.y, rhs.z) {}


	const char* __str__(void)
	{
		static char buffer[72];
		sprintf(buffer, "%f, %f, %f, %f", w, x, y, z);
		return buffer;
	}

	float       normalize(void)                 { return normalise(); }
	float       length2(void)                   { return Norm(); }
	float       dot(const gsQuaternion& v)        { return Dot(v); }
	gsQuaternion  inverse(void)                   { return Inverse(); }
	gsQuaternion operator- (void)                 { return -(gkQuaternion) * this; }
	gsQuaternion operator+ (const gsQuaternion& v)  { return ((const gkQuaternion&) * this).operator + (v); }
	gsQuaternion operator- (const gsQuaternion& v)  { return ((const gkQuaternion&) * this).operator - (v); }
	gsQuaternion operator* (const gsQuaternion& v)  { return ((const gkQuaternion&) * this).operator * (v); }
	gsQuaternion operator* (float v)              { return ((const gkQuaternion&) * this).operator * (v); }
	gsVector3    operator* (const gsVector3& v)     { return ((const gkQuaternion&) * this).operator * (v); }
	gsVector3    xAxis() { gkVector3 x,y,z; ToAxes(x,y,z); return x; }
	gsVector3    yAxis() { gkVector3 x,y,z; ToAxes(x,y,z); return y; }
	gsVector3    zAxis() { gkVector3 x,y,z; ToAxes(x,y,z); return z; }
};


class gsMatrix4
#ifndef SWIG
	: public gkMatrix4
#endif
{
public:
#ifndef SWIG
	gsMatrix4(gkMatrix4 orig) : m_matrix(orig){}
	gkMatrix4 get() { return m_matrix; }
#endif

	gsMatrix4(gsVector4 r1,gsVector4 r2,gsVector4 r3,gsVector4 r4)
		: gkMatrix4(r1.x,r1.y,r1.z,r1.w,r2.x,r2.y,r2.z,r2.w,r3.x,r3.y,r3.z,r3.w,r4.x,r4.y,r4.z,r4.w)
	{}

	void inverse() { m_matrix = m_matrix.inverse();}
private:
	gkMatrix4 m_matrix;
};

extern gsQuaternion lerp(const gsQuaternion& q1, const gsQuaternion& q2, float dt,bool fast);
extern gsVector3 lerp(const gsVector3& vec1,const gsVector3& vec2, float t);
extern gkVector3 getTriNormal(const gkVertex& v0,const gkVertex& v1,const gkVertex& v2);

extern gsVector3 str2vec3(const gkString& input);
extern gsVector4 str2vec4(const gkString& input);
extern gsMatrix4 str2mat4(const gkString& input);

class gsRay
#ifndef SWIG
	: public Ogre::Ray
#endif
{
public:
	gsRay() {}
	gsRay(const gsVector3& origin, const gsVector3& direction) : Ogre::Ray(origin, direction) {}

	void setOrigin(const gsVector3& origin)       {Ogre::Ray::setOrigin(origin);}
	void setDirection(const gsVector3& origin)    {Ogre::Ray::setDirection(origin);}

	gsVector3 getOrigin()       {return Ogre::Ray::getOrigin();}
	gsVector3 getDirection()    {return Ogre::Ray::getDirection();}
};




inline gsVector3 gsVec3Negate(const gsVector3& in)
{
	return -(gkVector3)in;
}



inline gsVector3 gsVec3AddVec3(const gsVector3& a, const gsVector3& b)
{
	return (gkVector3)a + (gkVector3)b;
}



inline gsVector3 gsVec3SubVec3(const gsVector3& a, const gsVector3& b)
{
	return (gkVector3)a - (gkVector3)b;
}



inline gsVector3 gsVec3MulVec3(const gsVector3& a, const gsVector3& b)
{
	return (gkVector3)a / (gkVector3)b;
}



inline gsVector3 gsVec3DivVec3(const gsVector3& a, const gsVector3& b)
{
	return (gkVector3)a / (gkVector3)b;
}



inline gsVector3 gsVec3Mulf(const gsVector3& a, const float& b)
{
	return (gkVector3)a / b;
}



inline gsVector3 gsVec3Divf(const gsVector3& a, const float& b)
{
	return (gkVector3)a / b;
}



inline gsQuaternion gsQuatNegate(const gsQuaternion& in)
{
	return -(gkQuaternion)in;
}



inline gsQuaternion gsQuatAddQuat(const gsQuaternion& a, const gsQuaternion& b)
{
	return (gkQuaternion)a + (gkQuaternion)b;
}



inline gsQuaternion gsQuatSubQuat(const gsQuaternion& a, const gsQuaternion& b)
{
	return (gkQuaternion)a - (gkQuaternion)b;
}



inline gsQuaternion gsQuatMulQuat(const gsQuaternion& a, const gsQuaternion& b)
{
	return (gkQuaternion)a * (gkQuaternion)b;
}



inline gsQuaternion gsQuatMulf(const gsQuaternion& a, const float& b)
{
	return (gkQuaternion)a * b;
}



inline gsVector3 gsQuatMulVec3(const gsQuaternion& a, const gsVector3& b)
{
	return (gkQuaternion)a * (gkVector3)b;
}

inline gsQuaternion gsVec3RotationTo(const gsVector3& a, const gsVector3& b) 
{ 
	return ((gkVector3)a).getRotationTo(b); 
}


#endif//_gsMath_h_
