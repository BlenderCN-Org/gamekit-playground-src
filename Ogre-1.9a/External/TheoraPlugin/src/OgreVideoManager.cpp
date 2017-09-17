/************************************************************************************
This source file is part of the Ogre3D Theora Video Plugin
For latest info, see http://ogrevideo.sourceforge.net/
*************************************************************************************
Copyright (c) 2008-2010 Kresimir Spes (kreso@cateia.com)
This program is free software; you can redistribute it and/or modify it under
the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
*************************************************************************************/
#ifndef OGRE_MAC_FRAMEWORK
  #include "OgreRoot.h"
#else
  #include <Ogre/OgreRoot.h>
#endif
#include "OgreVideoManager.h"
#include "OgreTheoraDataStream.h"

#ifndef OGRE_MAC_FRAMEWORK
#include "OgreTextureManager.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"
#include "OgreHardwarePixelBuffer.h"
#else
#include <Ogre/OgreTextureManager.h>
#include <Ogre/OgreMaterialManager.h>
#include <Ogre/OgreMaterial.h>
#include <Ogre/OgreTechnique.h>
#include <Ogre/OgreStringConverter.h>
#include <Ogre/OgreLogManager.h>
#include <Ogre/OgreHardwarePixelBuffer.h>
#endif

#include "TheoraVideoFrame.h"
#include "TheoraTimer.h"
#include <vector>

namespace Ogre
{


	int nextPow2(int x)
	{
		int y;
		for (y=1;y<x;y*=2);
		return y;
	}

	class ManualTimer : public TheoraTimer
	{
	public:
		void update(float t)
		{
		
		}

		void setTime(float time)
		{
			mTime=time;
		}
	};

	OgreVideoManager::CmdAlpha OgreVideoManager::msCmdAlpha;

	OgreVideoManager::OgreVideoManager(int num_worker_threads)
		: TheoraVideoManager(num_worker_threads), m_texState(0), m_isAlpha(false), m_isCyclic(false), dropCount(0)
	{
//		ExternalTextureSource::mPlugInName = "TheoraVideoPlugin";
//		ExternalTextureSource::mDictionaryName = mPlugInName;
		mbInit=false;
		mTechniqueLevel=mPassLevel=mStateLevel=0;

		iface_factory = new OpenAL_AudioInterfaceFactory();
		this->setAudioInterfaceFactory(iface_factory);

//		initialise();
	}



	bool OgreVideoManager::initialise()
	{
//		if (mbInit) return false;
//		mbInit=true;
		addBaseParams(); // ExternalTextureSource's function
		return true;
	}
	
	OgreVideoManager::~OgreVideoManager()
	{
		shutDown();
	}

	void OgreVideoManager::shutDown()
	{
		if (!mbInit) return;

		mbInit=false;
	}
    
    bool OgreVideoManager::setParameter(const String &name,const String &value)
    {
        return ExternalTextureSource::setParameter(name, value);
    }
    
    String OgreVideoManager::getParameter(const String &name) const
    {
        return ExternalTextureSource::getParameter(name);
    }

    void OgreVideoManager::addBaseParams() {
    	mPluginName="TheoraVideoPlugin";
    	mDictionaryName="TheoraVideoPlugin";
    	Ogre::ExternalTextureSource::addBaseParams();

    	ParamDictionary* dict = getParamDictionary();
    	dict->addParameter(ParameterDef("alpha",
    	                "is using alpha"
    	                , PT_BOOL),
    	                &OgreVideoManager::msCmdAlpha);

    }

//    void OgreVideoManager::createNamedTexture(const String& name,const String& group_name, const String& fileName)
//    {
//		TheoraVideoClip* clip=createVideoClip(new OgreTheoraDataStream(mInputFileName,group_name),TH_BGRX,0,1);
//		int w=nextPow2(clip->getWidth()),h=nextPow2(clip->getHeight());
//
//		TexturePtr t = TextureManager::getSingleton().createManual(name,group_name,TEX_TYPE_2D,w,h,1,0,PF_X8R8G8B8,TU_DYNAMIC_WRITE_ONLY);
//
//		if (t->getFormat() != PF_X8R8G8B8) logMessage("ERROR: Pixel format is not X8R8G8B8 which is what was requested!");
//		// clear it to black
//
//		unsigned char* texData=(unsigned char*) t->getBuffer()->lock(HardwareBuffer::HBL_DISCARD);
//		memset(texData,0,w*h*4);
//		t->getBuffer()->unlock();
//		mTextures[name]=t;
//
//    }

    TheoraVideoClip* OgreVideoManager::createTexture(const String& material_name,const String& group_name)
    {
		if (m_texName.empty())
			m_texName = mInputFileName;

		TheoraVideoClip* clip=createVideoClip(new OgreTheoraDataStream(mInputFileName,group_name),TH_RGB,6,0);
		clip->setName(m_texName);
		int w=nextPow2(m_isAlpha?clip->getWidth() / 2 : clip->getWidth()),h=nextPow2(clip->getHeight());

//		clip->setAutoRestart(m_isCyclic || mMode==TextureEffectPlay_Looping);
		clip->setAutoRestart(m_isCyclic);
//		clip->setNumPrecachedFrames(10);
//		clip->setPlaybackSpeed(0.1f);

		
		TexturePtr t = TextureManager::getSingleton().getByName(m_texName, group_name);
		if (t.isNull())
		{
			t = TextureManager::getSingleton().createManual(m_texName,group_name,TEX_TYPE_2D,w,h,1,0,PF_R8G8B8A8,TU_DYNAMIC_WRITE_ONLY);
		}


		unsigned char* texData=(unsigned char*) t->getBuffer()->lock(HardwareBuffer::HBL_DISCARD);
		memset(texData,0,w*h*4);
		t->getBuffer()->unlock();

		mTextures[m_texName]=t;
		clip->setAlpha(m_isAlpha);
		return clip;
    }

	void OgreVideoManager::createDefinedTexture(const String& material_name,const String& group_name)
	{
		TheoraVideoClip* clip = createTexture(material_name,group_name);
		int cW = clip->getWidth();
		int cY = clip->getHeight();

		int w=nextPow2(clip->getWidth()),h=nextPow2(clip->getHeight());

		if (m_texName.empty())
			m_texName = mInputFileName;

		//		// attach it to a material
		MaterialPtr material = MaterialManager::getSingleton().getByName(material_name).staticCast<Ogre::Material>();

		TextureUnitState* ts = m_texState? m_texState
											: material->getTechnique(mTechniqueLevel)->getPass(mPassLevel)->getTextureUnitState(mStateLevel);

		//Now, attach the texture to the material texture unit (single layer) and setup properties
		ts->setTextureName(m_texName,TEX_TYPE_2D);
		ts->setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_NONE);
		ts->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);

		// scale tex coords to fit the 0-1 uv range
		Matrix4 mat=Matrix4::IDENTITY;
		mat.setScale(Vector3((float) clip->getWidth()/w, (float) clip->getHeight()/h,1));
		ts->setTextureTransform(mat);

	}


	void OgreVideoManager::destroyAdvancedTexture(const String& material_name,const String& groupName)
	{
		logMessage("Destroying ogg_video texture on material: "+material_name);

		//logMessage("Error destroying ogg_video texture, texture not found!");
	}

	bool OgreVideoManager::frameStarted(const FrameEvent& evt)
	{
		if (evt.timeSinceLastFrame > 0.3f)
			update(0.3f);
		else
		    update(evt.timeSinceLastFrame);

		// update playing videos
		std::vector<TheoraVideoClip*>::iterator it;
		TheoraVideoFrame* f;
		for (it=mClips.begin();it!=mClips.end();it++)
		{
			String name = (*it)->getName();
			if ((*it)->isDone() && (*it)->isPaused())
				continue;

			f=(*it)->getNextFrame();
			if (f && f->mReady)
			{

				int fW = f->getWidth();
				int fH = f->getHeight();
				int fS = f->getStride();
				int frameNr = f->getFrameNumber();

				int w=(*it)->getWidth(),h=(*it)->getHeight();
				int dw = w*2;

				TexturePtr t=mTextures[(*it)->getName()];

				unsigned char *texData=(unsigned char*) t->getBuffer()->lock(HardwareBuffer::HBL_DISCARD);
				unsigned char *videoData=f->getBuffer();

				int texWidth = t->getWidth() ;
//				int srcLineWidth = w ;
//				int dstLineWidth = w*2;

				if ((*it)->useAlpha())
				{
					for (int y=0;y<h;y++)
					{
						for (int x=0;x<w;x++)
						{
							unsigned char alpha = 255 - videoData[(y*dw+x+w)*3+2];
							texData[(y*texWidth + x)*4 + 3] = alpha;

//							LogManager::getSingleton().logMessage("Alpha:"+StringConverter::toString(alpha));
							if (alpha==1)
							{
								texData[(y*texWidth + x)*4] = 255;
								texData[(y*texWidth + x)*4 + 1] = 255;
								texData[(y*texWidth + x)*4 + 2] = 255;
							}
							else
							{
								texData[(y*texWidth + x)*4] = videoData[(y*dw+x)*3+2];
								texData[(y*texWidth + x)*4 + 1] = videoData[(y*dw+x)*3 + 1];
								texData[(y*texWidth + x)*4 + 2] = videoData[(y*dw+x)*3 ];
	//							texData[y*srcLineWidth + x*4 + 3] = videoData[y*dstLineWidth+x*4 + 3];
							}
						}
					}
				}
				else
//					memcpy(texData,videoData,t->getWidth()*t->getHeight()*4-1);
				{
					int i,j,k,x,y,w=f->getWidth();

					for (int y=0;y<f->getHeight();y++)
					{
						for (int x=0;x<f->getWidth();x++)
						{
							unsigned char alpha = 255;
							texData[(y*texWidth + x)*4 + 3] = alpha;
							texData[(y*texWidth + x)*4] = videoData[ (y*w+x)*3+2];
							texData[(y*texWidth + x)*4 + 1] = videoData[ (y*w+x)*3+1];
							texData[(y*texWidth + x)*4 + 2] = videoData[ (y*w+x)*3];
						}
					}
				}

				t->getBuffer()->unlock();
				(*it)->popFrame();
			} else {
				dropCount++;
//				printf("drop:%i\n",dropCount);
			}
		}
		return true;
	}

    String OgreVideoManager::CmdAlpha::doGet(const void* target) const
    {
//    	return "true";
        return (static_cast<const OgreVideoManager*>(target))->isUsingAlpha()?"true":"false";
    }
    void OgreVideoManager::CmdAlpha::doSet(void* target, const String& val)
    {
        static_cast<OgreVideoManager*>(target)->setAlpha( val == "true" );
    }


} // end namespace Ogre
