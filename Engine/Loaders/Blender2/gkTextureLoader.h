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
#ifndef _gkTextureLoader_h_
#define _gkTextureLoader_h_

#include "gkLoaderCommon.h"
#include "OgreResource.h"
#include "utStreams.h"
#include "OgrePrerequisites.h"
#include "OgrePixelFormat.h"
#include "OgreTexture.h"
#include "OgreImage.h"



class gkTextureLoader : public Ogre::ManualResourceLoader
{
public:
	gkTextureLoader(Blender::Image* ima);
	gkTextureLoader(Ogre::Image* img, Ogre::ushort uWidth, Ogre::ushort uHeight,
	        Ogre::PixelFormat format, Ogre::TextureType texType,
	        int numMipmaps);
	virtual ~gkTextureLoader();

	void loadResource(Ogre::Resource* resource);

	static void registerVideoTexture(const gkString& resName,const gkString& filePath);
	static bool isVideoTexture(const gkString& resName);
	static gkString getVideoTextureFile(const gkString& resName);
protected:
	utMemoryStream*      m_stream;

	static utHashTable<utCharHashKey,gkString> m_videoTextureToFileMapping;
	Ogre::Image*		 m_image;
	bool				 m_rawLoad;
	Ogre::ushort m_uWidth;
	Ogre::ushort m_uHeight;
	Ogre::PixelFormat m_format;
	Ogre::TextureType m_texType;
	int m_numMipmaps;

};





#endif//_gkTextureLoader_h_
