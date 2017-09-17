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
#ifndef _gkVideoBackground_h_
#define _gkVideoBackground_h_


#include "OgreImage.h"
#include "OgreResource.h"
#include "gkSerialize.h"

#include "utSingleton.h"
// simple skybox creator
namespace Ogre
{
	class Rectangle2D;
}

class gkVideoBackground : public utSingleton<gkVideoBackground>
{
public:
	gkVideoBackground() : m_initalized(false),m_rect(0)
	{
	}

	~gkVideoBackground();
protected:
	void init();

	bool m_initalized;

	Ogre::Rectangle2D* m_rect;

public:
	UT_DECLARE_SINGLETON(gkVideoBackground);

	void setMaterial(const gkString& material);
};



#endif//_gkOgreSkyboxGradient_h_
