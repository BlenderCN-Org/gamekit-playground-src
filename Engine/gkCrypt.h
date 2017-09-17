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
#ifndef _gkCryptObject_h_
#define _gkCryptObject_h_

#include "gkGameObject.h"
#include "gkSerialize.h"


class gkCrypt
{
public:
	gkCrypt(){};
	virtual ~gkCrypt() {}

	static gkString md5(const gkString& md5);

	static gkString blowfishEncrypt(const gkString& keydata, const gkString& in);
	static gkString blowfishDecrypt(const gkString& keydata, const gkString& in);

};

#endif//_gkCryptObject_h_
