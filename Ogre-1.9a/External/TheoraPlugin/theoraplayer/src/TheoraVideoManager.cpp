/************************************************************************************
This source file is part of the Theora Video Playback Library
For latest info, see http://libtheoraplayer.sourceforge.net/
*************************************************************************************
Copyright (c) 2008-2013 Kresimir Spes (kspes@cateia.com)
This program is free software; you can redistribute it and/or modify it under
the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
*************************************************************************************/
#include "TheoraVideoManager.h"
#include "TheoraWorkerThread.h"
#include "TheoraVideoClip.h"
#include "TheoraAudioInterface.h"
#include "TheoraUtil.h"
#include "TheoraDataSource.h"
#include "TheoraException.h"
#include <stdlib.h>

#ifdef __THEORA
	#include <theora/codec.h>
	#include <vorbis/codec.h>
	#include "TheoraVideoClip_Theora.h"
	// declaring function prototype here so I don't have to put it in a header file
	// it only needs to be used by this plugin and called once
	void createYUVtoRGBtables();
#endif
#ifdef __AVFOUNDATION
	#include "TheoraVideoClip_AVFoundation.h"

#endif

TheoraVideoManager* g_ManagerSingleton=0;

void theora_writelog(std::string output)
{
	printf("%s\n",output.c_str());
}

void (*g_LogFuction)(std::string)=theora_writelog;

void TheoraVideoManager::setLogFunction(void (*fn)(std::string))
{
	g_LogFuction=fn;
}

TheoraVideoManager* TheoraVideoManager::getSingletonPtr()
{
    return g_ManagerSingleton;
}

TheoraVideoManager& TheoraVideoManager::getSingleton()
{  
    return *g_ManagerSingleton;  
}

TheoraVideoManager::TheoraVideoManager(int num_worker_threads) : 
	mDefaultNumPrecachedFrames(8)
{
	if (num_worker_threads < 1) throw TheoraGenericException("Unable to create TheoraVideoManager, at least one worker thread is reqired");

	g_ManagerSingleton = this;

	logMessage("Initializing Theora Playback Library (" + getVersionString() + ")\n" +
#ifdef __THEORA
	           "  - libtheora version: " + th_version_string() + "\n" + 
	           "  - libvorbis version: " + vorbis_version_string() + "\n" +
#endif
#ifdef __AVFOUNDATION
			   "  - using Apple AVFoundation classes\n"
#endif
			   "------------------------------------");
	mAudioFactory = NULL;
	mWorkMutex = new TheoraMutex();

#ifdef __THEORA
	// for CPU based yuv2rgb decoding
	createYUVtoRGBtables();
#endif
	createWorkerThreads(num_worker_threads);
}

TheoraVideoManager::~TheoraVideoManager()
{
	destroyWorkerThreads();

	ClipList::iterator ci;
	for (ci=mClips.begin(); ci != mClips.end();ci++)
		delete (*ci);
	mClips.clear();
	delete mWorkMutex;
}

void TheoraVideoManager::logMessage(std::string msg)
{
	g_LogFuction(msg);
}

TheoraVideoClip* TheoraVideoManager::getVideoClipByName(std::string name)
{
	foreach(TheoraVideoClip*,mClips)
		if ((*it)->getName() == name) return *it;

	return 0;
}

void TheoraVideoManager::setAudioInterfaceFactory(TheoraAudioInterfaceFactory* factory)
{
	mAudioFactory=factory;
}

TheoraAudioInterfaceFactory* TheoraVideoManager::getAudioInterfaceFactory()
{
	return mAudioFactory;
}

TheoraVideoClip* TheoraVideoManager::createVideoClip(std::string filename,
													 TheoraOutputMode output_mode,
													 int numPrecachedOverride,
													 bool usePower2Stride)
{
	TheoraDataSource* src=new TheoraFileDataSource(filename);
	return createVideoClip(src,output_mode,numPrecachedOverride,usePower2Stride);
}

TheoraVideoClip* TheoraVideoManager::createVideoClip(TheoraDataSource* data_source,
													 TheoraOutputMode output_mode,
													 int numPrecachedOverride,
													 bool usePower2Stride)
{
	mWorkMutex->lock();

	TheoraVideoClip* clip = NULL;
	int nPrecached = numPrecachedOverride ? numPrecachedOverride : mDefaultNumPrecachedFrames;
	logMessage("Creating video from data source: "+data_source->repr());
	
#ifdef __AVFOUNDATION
	TheoraFileDataSource* fileDataSource = dynamic_cast<TheoraFileDataSource*>(data_source);
	std::string filename;
	if (fileDataSource == NULL)
	{
		TheoraMemoryFileDataSource* memoryDataSource = dynamic_cast<TheoraMemoryFileDataSource*>(data_source);
		if (memoryDataSource != NULL) filename = memoryDataSource->getFilename();
		// if the user has his own data source, it's going to be a problem for AVAssetReader since it only supports reading from files...
	}
	else filename = fileDataSource->getFilename();

	if (filename.size() > 4 && filename.substr(filename.size() - 4, filename.size()) == ".mp4")
	{
		clip = new TheoraVideoClip_AVFoundation(data_source,output_mode,nPrecached,usePower2Stride);
	}
#endif
#if defined(__AVFOUNDATION) && defined(__THEORA)
	else
#endif
#ifdef __THEORA
		clip = new TheoraVideoClip_Theora(data_source,output_mode,nPrecached,usePower2Stride);
#endif
	clip->load(data_source);
	mClips.push_back(clip);
	mWorkMutex->unlock();
	return clip;
}

void TheoraVideoManager::destroyVideoClip(TheoraVideoClip* clip)
{
	if (clip)
	{
		th_writelog("Destroying video clip: "+clip->getName());
		mWorkMutex->lock();
		bool reported=0;
		while (clip->mAssignedWorkerThread)
		{
			if (!reported) { th_writelog("Waiting for WorkerThread to finish decoding in order to destroy"); reported=1; }
			_psleep(1);
		}
		if (reported) th_writelog("WorkerThread done, destroying..");
		foreach(TheoraVideoClip*,mClips)
			if ((*it) == clip)
			{
				mClips.erase(it);
				break;
			}
		delete clip;
		th_writelog("Destroyed video.");
		mWorkMutex->unlock();
	}
}

TheoraVideoClip* TheoraVideoManager::requestWork(TheoraWorkerThread* caller)
{
	if (!mWorkMutex) return NULL;
	mWorkMutex->lock();
	TheoraVideoClip* c = NULL;

	float priority, last_priority = 100000;

	foreach (TheoraVideoClip*, mClips)
	{
		if ((*it)->isBusy()) continue;
		priority = (*it)->getPriorityIndex();
		if (priority < last_priority || (priority == last_priority && rand()%2 == 0)) // use randomization to prevent clip starvation in resource demanding situations
		{
			last_priority = priority;
			c = *it;
		}
	}
	if (c) c->mAssignedWorkerThread = caller;
	
	mWorkMutex->unlock();
	return c;
}

void TheoraVideoManager::update(float time_increase)
{
	foreach(TheoraVideoClip*, mClips)
	{
		if ((*it)->wantAutoupdate())
		{
			(*it)->update(time_increase);
			(*it)->decodedAudioCheck();
		}
	}
}

int TheoraVideoManager::getNumWorkerThreads()
{
	return mWorkerThreads.size();
}

void TheoraVideoManager::createWorkerThreads(int n)
{
	TheoraWorkerThread* t;
	for (int i=0;i<n;i++)
	{
		t=new TheoraWorkerThread();
		t->startThread();
		mWorkerThreads.push_back(t);
	}
}

void TheoraVideoManager::destroyWorkerThreads()
{
	foreach(TheoraWorkerThread*,mWorkerThreads)
	{
		(*it)->waitforThread();
		delete (*it);
	}
	mWorkerThreads.clear();
}

void TheoraVideoManager::setNumWorkerThreads(int n)
{
	if (n == getNumWorkerThreads()) return;
	if (n < 1) throw TheoraGenericException("Unable to change the number of worker threads in TheoraVideoManager, at least one worker thread is reqired");

	th_writelog("changing number of worker threats to: "+str(n));

	destroyWorkerThreads();
	createWorkerThreads(n);
}

std::string TheoraVideoManager::getVersionString()
{
	int a, b, c;
	getVersion(&a, &b, &c);
	std::string out = str(a) + "." + str(b);
	if (c != 0)
	{
		if (c < 0) out += " RC" + str(-c);
		else       out += "." + str(c);
	}
	return out;
}

void TheoraVideoManager::getVersion(int* a, int* b, int* c) // TODO, return a struct instead of the current solution.
{
	*a = 1;
	*b = 0;
	*c = -4;
}

std::vector<std::string> TheoraVideoManager::getSupportedDecoders()
{
	std::vector<std::string> lst;
#ifdef __THEORA
	lst.push_back("Theora");
#endif
#ifdef __AVFOUNDATION
	lst.push_back("AVFoundation");
#endif
	
	return lst;
}
