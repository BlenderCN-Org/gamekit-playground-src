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
#include "gkPath.h"

#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include "gkResourceGroupManager.h"
#include "gkLogger.h"

#include "gkValue.h"


#if GK_PLATFORM == GK_PLATFORM_LINUX
#include <linux/limits.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 240
#endif

#ifndef S_ISREG
# define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
# define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif


#if GK_PLATFORM == GK_PLATFORM_WIN32
# include <direct.h>
#else
# include <unistd.h>
#endif


#if GK_PLATFORM == GK_PLATFORM_WIN32
const gkString gkPath::SEPERATOR = "\\";
#else
const gkString gkPath::SEPERATOR = "/";
#endif

#ifdef OGREKIT_BUILD_ANDROID
# include "OgreArchiveManager.h"
# include "Android/OgreAPKFileSystemArchive.h"
# include "Android/OgreAPKZipArchive.h"
#include "utStreams.h"
#endif

void gkGetCurrentDir(gkString& buf)
{
	char buffer[PATH_MAX];
	if(getcwd(buffer, sizeof(buffer)) != NULL)
		buf = gkString(buffer);
	else
		buf = gkString("");
}



gkPath::gkPath()
{
}


gkPath::gkPath(const gkString& file) :
	m_path(file)
{
	if (m_path[0]=='/' && m_path[1]=='/')
		m_path = m_path.substr(2);
}


gkPath::gkPath(const char* file) :
	m_path(file)
{
	if (m_path[0]=='/' && m_path[1]=='/')
		m_path = m_path.substr(2);
}


gkPath::~gkPath()
{
}


const gkString& gkPath::getPath(void) const
{
	return m_path;
}


gkString gkPath::getAbsPath(void) const
{
	if (!exists())
	{
		// cannot get path
		return gkStringUtils::BLANK;
	}

	if (isAbs())
		return m_path;

	gkString curDir;
	gkGetCurrentDir(curDir);
	return curDir + SEPERATOR + m_path;
}


int gkPath::getFileSize(void) const
{
	struct stat st;
	if (stat(m_path.c_str(), &st) == 0)
		return (int)st.st_size;
	return -1;
}


void gkPath::append(const gkString& v)
{
	if (m_path.at(m_path.size() - 1) != SEPERATOR[0])
		m_path += SEPERATOR;

	m_path += v;
}

utMemoryStream* gkPath::getAsStream() const
{
	try
	{
		Ogre::DataStreamPtr ogreStream = _getAsOgreStream();
		int size = ogreStream->size();

		utMemoryStream* ustream = new utMemoryStream(utStream::SM_READ);
		ustream->reserve(size);
		ogreStream->read(ustream->ptr(),size);
		// set the size manully since I read from ogre directly into the utMemoryBuffer
		ustream->_setSize(size);

		return ustream;
	}
	catch (...)
	{
		gkLogger::write("getAsStream with ogre didn't work:"+m_path,true);
	}

	utMemoryStream* result = _getFromFileStream();

	if (result)
		return result;

	return 0;
}

utMemoryStream*	gkPath::_getFromFileStream(void) const {
	int fileSize = getFileSize();

	// empty file
	if (fileSize <= 0)
		return 0;



	FILE* fp = fopen(m_path.c_str(), "rb");  // always in binary
	GK_ASSERT(fp);

	if (fp)
	{
		utMemoryStream* ustream = new utMemoryStream;
		ustream->reserve(fileSize);

		// read directly into the memoryStream
		fread(ustream->ptr(), fileSize, 1, fp);
		fclose(fp);
		// update size, since we directly wrote into the utStream-Data
		ustream->_setSize(fileSize);

		return ustream;
	}

	return 0;
}

Ogre::DataStreamPtr gkPath::_getAsOgreStream() const
{
	return Ogre::ResourceGroupManager::getSingleton().openResource(m_path);
}

gkString gkPath::getAsString() const
{
	if (!isFile())
		return "";



	int fileSize = getFileSize();

	// empty file
	if (fileSize > 0)
	{
		char* data = new char[fileSize+1];


		FILE* fp = fopen(m_path.c_str(), "rb");  // always in binary
		GK_ASSERT(fp);

		if (fp)
		{
			fread(data, fileSize, 1, fp);
			fclose(fp);
			// term
			data[fileSize] = 0;
			gkString result(data);
			delete[] data;
			return result;
		}

	}

	// try the ogre way. the order is intentionally, since on script loading local path counts more
	// than the ogre-path. so you can use local version  of e.g. the lua-libs without the need to
	// recompile on every change
	if (Ogre::ResourceGroupManager::getSingletonPtr())
	{
		try
		{
			Ogre::DataStreamPtr data = Ogre::ResourceGroupManager::getSingleton().openResource(m_path);
			if (!data.isNull())
				return data->getAsString();
		}
		catch (...)
		{
			gkLogger::write("TEST: getAsString via ogre didn't work:"+m_path,true);
		}

	}

	return "";
}



gkString gkPath::directory(void) const
{
	if (isDir())
		return m_path;

	normalizePlatform();

	gkStringVector sp;
	split(sp);

	gkString dir;

	if (isAbs())
	{
#if GK_PLATFORM == GK_PLATFORM_WIN32
		dir = m_path.substr(0, 3);
#else
		dir = m_path.substr(0, 1);
#endif
	}

	if (!sp.empty())
	{
		size_t size = sp.size() - 1;
		for (size_t i = 0; i < size; i++)
		{
			dir += sp[i];
			if ((i + 1) != size)
				dir += SEPERATOR;
		}
	}
	return dir;
}


gkString gkPath::base(void) const
{
	normalizePlatform();

	gkStringVector arr;
	split(arr);

	if (arr.empty())
		return gkStringUtils::BLANK;
	return arr.at(arr.size() - 1);
}


gkString gkPath::extension(void) const
{
	// split base on .
	gkString bn = base();

	if (bn.empty())
		return gkStringUtils::BLANK;

	gkStringVector arr;
	utStringUtils::split(arr, bn, ".");
	if (arr.empty())
		return gkStringUtils::BLANK;
	return gkString(".") + arr.at(arr.size() - 1);
}


void gkPath::normalize(void) const
{
	utStringUtils::replace(m_path, "\\\\", "/");
}


void gkPath::normalizePlatform(void) const
{
#if GK_PLATFORM == GK_PLATFORM_WIN32
	utStringUtils::replace(m_path, "/", "\\");
#else
	utStringUtils::replace(m_path, "\\\\", "/");
#endif
}


bool gkPath::isAbs(void) const
{
	// assumes (Drive:) || or root /
	if (m_path.empty())
		return false;
#if GK_PLATFORM == GK_PLATFORM_WIN32
	if (m_path.size() > 2)
		return m_path[1] == ':';
#else
	if (m_path.size() >= 1)
		return m_path[0] == '/';
#endif
	return false;
}

gkString gkPath::getBundlePath(void) const
{
#ifdef __APPLE__
#ifndef MAXPATHLEN
#define MAXPATHLEN 512
#endif
	CFURLRef bundleURL;
	CFStringRef pathStr;
	static char path[MAXPATHLEN];
	memset(path, 0, MAXPATHLEN);
	CFBundleRef mainBundle = CFBundleGetMainBundle();

	bundleURL = CFBundleCopyBundleURL(mainBundle);
	pathStr = CFURLCopyFileSystemPath(bundleURL, kCFURLPOSIXPathStyle);
	CFStringGetCString(pathStr, path, MAXPATHLEN, kCFStringEncodingASCII);
	CFRelease(pathStr);
	CFRelease(bundleURL);

	char* lastSlash = 0;
	if (lastSlash = strrchr((char*)path, '/'))
		* lastSlash = '\0';

	return path;
#endif
	return "";
}


bool gkPath::isRel(void) const
{
	return !isAbs();
}


bool gkPath::exists(void) const
{
	return isFile() || isDir();
}


bool gkPath::isFile(void) const
{
	// skip blender relative paths. TODO: WHY???
	// stat can sometimes be extremely slow!
	// (not sure if fopen() == NULL would be any better.)
	if (m_path.size() < 2 || (m_path[0] == '/' && m_path[1] == '/'))
		return false;

	bool result = false;

	if (Ogre::ResourceGroupManager::getSingletonPtr() && Ogre::ResourceGroupManager::getSingleton().resourceExistsInAnyGroup(m_path))
		return true;

	struct stat st;

	if (stat(m_path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
		return true;

	return false;
}


bool gkPath::isDir(void) const
{
	// skip blender relative paths.
	if (m_path.size() < 2 || (m_path[0] == '/' && m_path[1] == '/'))
		return false;

	// todo: check this for ogre-resources

	struct stat st;

	return stat(m_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

bool gkPath::isFileInBundle(void) const
{
#ifdef __APPLE__
	char newName[1024];

#ifdef OGREKIT_BUILD_IPHONE
	sprintf(newName, "%s/%s", getBundlePath().c_str(), m_path.c_str());
#else
	sprintf(newName, "%s/%s/%s", getBundlePath().c_str(), "Contents/Resources", m_path.c_str());
#endif

	struct stat st;
	return stat(newName, &st) == 0 && S_ISREG(st.st_mode);
#else
	return isFile();
#endif

}


void gkPath::split(gkStringVector& arr) const
{
	utStringUtils::split(arr, m_path, SEPERATOR);
}

/*

 checks path from top to down:
 e.g.
 given path: /home/ttrocha/_dev/projects/gamekit/test.blend
 1)
*/

gkString gkPath::findValidPath(void) const
{
	// check the subfolders
	utStringArray pathSplit;
	split(pathSplit);

	gkString temp;

	for (int i = pathSplit.size()-1; i>=0 ; i--)
	{
		if (temp.empty())
			temp = pathSplit[i];
		else
			temp = pathSplit[i] + gkPath::SEPERATOR + temp;

		if (gkPath(temp).exists())
			return temp;

	}
	return "";
}


#ifdef OGREKIT_BUILD_ANDROID

void gkPath::setAssetManager(AAssetManager* assetManager) {
	m_assetManager = assetManager;
}

void gkPath::initAssetManager()
{
	Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKFileSystemArchiveFactory(m_assetManager) );
	Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKZipArchiveFactory(m_assetManager) );

	Ogre::ResourceGroupManager& mgr = Ogre::ResourceGroupManager::getSingleton();
	mgr.addResourceLocation("/","APKFileSystem","General");
	mgr.addResourceLocation(".","APKFileSystem","General");
//	mgr.addResourceLocation("/media","APKFileSystem","General");
//	mgr.addResourceLocation("/assets.zip","APKZip","General");
//	mgr.addResourceLocation("/media/images","APKFileSystem","General");
//	mgr.addResourceLocation("/media/tilesets","APKFileSystem","General");
//	mgr.addResourceLocation("/media/scripts","APKFileSystem","General");
//	mgr.addResourceLocation("/media/scripts","APKFileSystem","General");
//	mgr.addResourceLocation("/media/textures","APKFileSystem","General");
//	mgr.addResourceLocation("/media/programs","APKFileSystem","General");
//	mgr.addResourceLocation("/media/programs/GLSLES","APKFileSystem","General");
	gkResourceGroupManager::getSingleton().initialiseResourceGroup("General");
	try{
		gkLogger::write("ME",true);
		Ogre::StringVectorPtr stp = mgr.findResourceLocation("General","res.cfg");
		gkLogger::write("ANZ:"+gkToString((int)stp->size()),true);
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("res.cfg");

		gkLogger::write(gkString("Ogre-Resource-lookup: success"),true);
		gkResourceGroupManager::getSingleton().initialiseResourceGroup("android");
	} catch (...)
	{
		gkLogger::write("Loading via ogre didn'T work out");
	}

//	Ogre::ConfigFile cf;
//	cf.load()




}

void gkPath::setSDCardPath(const gkString& path)
{
	m_sdcardPath = path;
	gkLogger::write("set SD-Card-Base to:"+path);
}

utMemoryStream* gkPath::androidLoadFromAssets(){
	if (!m_assetManager)
	{
		gkLogger::write("Called _loadAPKFile without an assetManager set!",true);
		return 0;
	}

	AAsset* asset = AAssetManager_open(m_assetManager, getPath().c_str(), AASSET_MODE_UNKNOWN);
	if (NULL == asset) {
		return 0;
	}

	long bufferSize = AAsset_getLength(asset);


	void* buffer = malloc(bufferSize+1);
	memset(buffer,0,bufferSize+1);
	AAsset_read (asset,buffer,bufferSize);

	utMemoryStream* memoryStream = new utMemoryStream();
	memoryStream->open(buffer,bufferSize+1,utStream::SM_READ);
	free(buffer);

	AAsset_close(asset);

	return memoryStream;
}



Ogre::DataStreamPtr gkPath::androidOpenAsOgreStream()
{
    Ogre::DataStreamPtr stream;
    AAsset* asset = AAssetManager_open(m_assetManager, getPath().c_str(), AASSET_MODE_BUFFER);
    if(asset)
    {
        off_t length = AAsset_getLength(asset);
        void* membuf = OGRE_MALLOC(length, Ogre::MEMCATEGORY_GENERAL);
        memcpy(membuf, AAsset_getBuffer(asset), length);
        AAsset_close(asset);

        stream = Ogre::DataStreamPtr(new Ogre::MemoryDataStream(membuf, length, true, true));
    }
    return stream;
}



gkString gkPath::androidGetAsStringFromAssets() {
	long size = 0;
	utMemoryStream* stream = androidLoadFromAssets();
	gkString scriptText((char*)stream->ptr()+'\0');
	delete stream;
	return scriptText;
}

bool gkPath::androidExsitsInAssets() const
{
	if (!m_assetManager)
	{
		gkLogger::write("Called apkFileExists without an assetManager set!",true);
		return false;
	}
	AAsset* asset = AAssetManager_open(m_assetManager, getPath().c_str(), AASSET_MODE_UNKNOWN);
	if(asset)
	{
		AAsset_close(asset);
		return true;
	}
	return false;
}

gkString gkPath::m_sdcardPath;
AAssetManager* gkPath::m_assetManager=0;



#endif
