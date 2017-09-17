/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Nestor Silveira.
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
#ifndef _gkMotionNode_h_
#define _gkMotionNode_h_

#include "gkLogicNode.h"

class gkMotionNode : public gkLogicNode
{
public:

	enum
	{
		UPDATE,
		X,
		Y,
		Z,
		DAMPING,
		OBJECT,
		RESULT
	};

//	typedef gkString  _SOCKET_TYPE_OBJECT;    \
//	    inline gkLogicSocket< gkString >* getOBJECT() {
//			gkLogicSocket<gkString>* sock = getSocket< gkString > ( (OBJECT));
//			return sock;
//	}
	DECLARE_SOCKET_TYPE(UPDATE, bool);
	DECLARE_SOCKET_TYPE(X, gkScalar);
	DECLARE_SOCKET_TYPE(Y, gkScalar);
	DECLARE_SOCKET_TYPE(Z, gkScalar);
	DECLARE_SOCKET_TYPE(DAMPING, gkScalar);
	DECLARE_SOCKET_TYPE(OBJECT, gkString);
	DECLARE_SOCKET_TYPE(RESULT, gkScalar);

	DECLARE_PROPERTY(MotionType,int,Int);
	DECLARE_PROPERTY(Min,gkVector3,Vector3);
	DECLARE_PROPERTY(Max,gkVector3,Vector3);

	gkMotionNode(gkLogicTree* parent, size_t id, const gkString& name="");
	virtual ~gkMotionNode() {}
	void initialize();

	void update(gkScalar tick);
	bool evaluate(gkScalar tick);


	GK_INLINE void setSpace(int ts)                 {m_space = ts;}
	GK_INLINE void keepVelocity(void)               {m_keep = true;}

	// todo, just pass object ptr
	void setOtherObject(const gkString& obname) {m_otherName = obname; m_current=0;}

private:
	GK_INLINE gkMotionTypes _getMotionType() { return (gkMotionTypes)getPropMotionType(); }

	int     getLRS();
	void    applyConstraints(int lrs);
	void    applyObject(gkVector3& vec);
	void    updateOthername();

	int             m_space;
	bool            m_keep;

	short           m_flag;

	gkGameObject*   m_current;
	gkString        m_otherName;
};







#endif//_gkMotionNode_h_
