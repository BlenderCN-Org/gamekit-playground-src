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
#include <Blender.h>
#include "OgreTexture.h"
#include "OgreTextureManager.h"
#include "OgreFreeImageCodec.h"
#include "gkTextureLoader.h"
#include "gkLogger.h"
#include "gkEngine.h"
#include "gkUserDefs.h"
#include "OgreImage.h"



gkTextureLoader::gkTextureLoader(Blender::Image* ima)
	:   m_stream(0),m_rawLoad(false),m_image(0)
{
	GK_ASSERT(ima);
	Blender::PackedFile* pack = ima->packedfile;
	if (pack)
	{
		m_stream = new utMemoryStream;
		m_stream->open(pack->data, pack->size, utStream::SM_READ);
	}
}

gkTextureLoader::gkTextureLoader(Ogre::Image* img, Ogre::ushort uWidth, Ogre::ushort uHeight,
        Ogre::PixelFormat format, Ogre::TextureType texType,
        int numMipmaps) : m_image(img), m_rawLoad(true),m_uWidth(uWidth), m_uHeight(uHeight),
                m_format(format), m_texType(texType), m_numMipmaps(numMipmaps),m_stream(0)
{}


gkTextureLoader::~gkTextureLoader()
{
	if (m_stream)
		delete m_stream;
	if (m_image)
		delete m_image;
	m_stream=0;
	m_image=0;
}


void gkTextureLoader::loadResource(Ogre::Resource* resource)
{
	Ogre::Texture* texture = static_cast<Ogre::Texture*>(resource);

	if (!m_stream && !m_image)
	{
		gkPrintf("Warning: Skipping image %s no packed file information is present!", texture->getName().c_str());
		return;
	}
	gkString texName(texture->getName());


	if (!m_rawLoad){
		Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream(m_stream->ptr(), m_stream->size()));
		Ogre::Image ima;
		ima.load(stream);


		texture->setUsage(Ogre::TU_DEFAULT);
		texture->setTextureType(Ogre::TEX_TYPE_2D);
		texture->setNumMipmaps(gkEngine::getSingleton().getUserDefs().defaultMipMap);
		texture->setWidth(ima.getWidth());
		texture->setHeight(ima.getHeight());
		texture->setDepth(ima.getDepth());
		texture->setFormat(ima.getFormat());

		Ogre::ConstImagePtrList ptrs;
		ptrs.push_back(&ima);
		texture->_loadImages(ptrs);
	} else {
		// the texture have to be set with all that data before like this:
		/*

	Ogre::TexturePtr tex =     Ogre::TextureManager::getSingleton().TextureManager::createManual(
			Rocket::Core::String(16, "%d", texture_id++).CString(),
			DEFAULT_ROCKET_RESOURCE_GROUP,
			Ogre::TEX_TYPE_2D,
			(uint)source_dimensions.x,
			(uint)source_dimensions.y,
			0,
			Ogre::PF_A8B8G8R8,
			Ogre::TU_DEFAULT,
			loader);


		 */


		texture->setTextureType(Ogre::TEX_TYPE_2D);
		texture->setNumMipmaps(0);
		texture->setGamma(1.0f);
		texture->setHardwareGammaEnabled(false);
		Ogre::ConstImagePtrList ptrs;
		ptrs.push_back(m_image);
		texture->_loadImages(ptrs);
//		texture->loadRawData(stream, (Ogre::ushort)m_uWidth, (Ogre::ushort)m_uHeight, Ogre::PF_A8B8G8R8);
		//
	}
}

utHashTable<utCharHashKey,gkString> gkTextureLoader::m_videoTextureToFileMapping;

void gkTextureLoader::registerVideoTexture(const gkString& resName,const gkString& filePath)
{
	m_videoTextureToFileMapping.insert(resName.c_str(),filePath);
}
bool  gkTextureLoader::isVideoTexture(const gkString& resName)
{
	return m_videoTextureToFileMapping.get(resName.c_str())!=0;
}
gkString  gkTextureLoader::getVideoTextureFile(const gkString& resName)
{
	if (m_videoTextureToFileMapping.find(resName.c_str())!=UT_NPOS){
		return *m_videoTextureToFileMapping.get(resName.c_str());
	}
	// TODO: Throw exeception or such
	return "THIS RESOURCE IS NOT A VIDEOTEXTURE!";
}
