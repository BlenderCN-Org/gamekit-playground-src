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
#include "gkCommon.h"

#include "OgreException.h"
#include "OgreRoot.h"
#include "OgreTexture.h"
#include "OgreTextureManager.h"

#include "gkBlendInternalFile.h"
#include "gkBlendFile.h"
#include "gkBlendLoader.h"
#include "gkSceneManager.h"
#include "gkScene.h"
#include "gkGameObject.h"

#include "Converters/gkAnimationConverter.h"
#include "Converters/gkParticleConverter.h"

#include "gkBlenderDefines.h"
#include "gkBlenderSceneConverter.h"
#include "gkTextureLoader.h"
#include "gkPath.h"
#include "gkLogger.h"

#include "gkTextManager.h"
#include "gkTextFile.h"
#include "gkEngine.h"
#include "gkUserDefs.h"

#include "gkResourceGroupManager.h"
#ifdef OGREKIT_OPENAL_SOUND
# include "Sound/gkSoundManager.h"
# include "Sound/gkSound.h"
#endif

#if defined(OGREKIT_COMPILE_OGRE_SCRIPTS) || defined(OGREKIT_COMPILE_LIBROCKET)
#include "gkFontManager.h"
#include "gkFont.h"
#include "gkUtils.h"
#include "gkValue.h"
#endif

#ifdef OGREKIT_USE_THEORA
# include "../../../Ogre-1.9a/External/TheoraPlugin/include/OgreVideoManager.h"
#endif

//using namespace Ogre;

gkBlendFile::gkBlendFile(const gkString& blendToLoad, const gkString& group)
	:	m_name(blendToLoad),
		m_group(group),
		m_animFps(24),
		m_activeScene(0),
		m_findScene(""),
		m_hasBFont(false),
		m_file(0),
		m_stream(0)
{
	if (gkBlendLoader::getSingleton().isLibraryLoaded(blendToLoad))
	{
		gkLogger::write("Warning: File already loaded:"+blendToLoad,true);
		return;
	}
	gkBlendLoader::getSingleton().addLoadedLibraryName(blendToLoad,m_group);

}

gkBlendFile::gkBlendFile(utMemoryStream* mem,  const gkString& group)
	:	m_name(""),
		m_group(group),
		m_animFps(24),
		m_activeScene(0),
		m_findScene(""),
		m_hasBFont(false),
		m_file(0),
		m_stream(mem)
{
}

gkBlendFile::~gkBlendFile()
{
	if (!m_loaders.empty())
	{
		ManualResourceLoaderList::Iterator it = m_loaders.iterator();
		while (it.hasMoreElements())
			delete it.getNext();
	}
	if (m_stream){
		delete m_stream;
	}
	if (m_file)
		delete m_file;

}



bool gkBlendFile::parse(int opts, const gkString& scene)
{	
	m_file = new gkBlendInternalFile();

	if (!m_name.empty())
	{
		if (!m_file->parse(m_name))
		{
			delete m_file;
			m_file = 0;
			return false;
		}
	}
	else
	{
		if (m_stream)
		{
			m_file->parse(m_stream->ptr(),m_stream->size());
		}
	}

	doVersionTests();

	m_findScene = scene;

	if (opts & gkBlendLoader::LO_IS_LIBRARY)
		loadLibrary();
	else if (opts & gkBlendLoader::LO_ONLY_ACTIVE_SCENE)
		loadActive();
	else
		createInstances();


	return true;
}

void gkBlendFile::readCurSceneInfo(Blender::Scene* scene)
{
	if (!scene) return;

	m_animFps = scene->r.frs_sec / scene->r.frs_sec_base;

	gkUserDefs& defs = gkEngine::getSingleton().getUserDefs();
	defs.animFps = m_animFps;
	defs.rtss = (scene->gm.matmode == GAME_MAT_GLSL);
}


void gkBlendFile::loadActive(void)
{
	// Load / convert only the active scene.

	Blender::FileGlobal* fg = m_file->getFileGlobal();
	if (fg)
	{		
		if (!fg->curscene)
			fg->curscene = m_file->getFirstScene();

		Blender::Scene* sc = fg->curscene;

		readCurSceneInfo(sc);

		buildAllTextures();
		buildAllFonts();
		buildTextFiles();
		buildAllSounds();
		buildAllActions();
		buildAllParticles();

		// parse & build
		if (sc)
		{
			gkBlenderSceneConverter conv(this, sc);
			conv.convert(false);
			conv.convertGroupInstances();
			m_activeScene = (gkScene*)gkSceneManager::getSingleton().getByName(gkResourceName(GKB_IDNAME(sc), m_group));
			if (m_activeScene)
				m_scenes.push_back(m_activeScene);
		}
	}

}



void gkBlendFile::createInstances(void)
{
	GK_ASSERT(m_file);

	Blender::FileGlobal* fg = m_file->getFileGlobal();

	Blender::Scene* curscene = fg ? fg->curscene : 0;

	readCurSceneInfo(curscene);

	if (!gkEngine::getSingleton().getUserDefs().ignoreLibraries){
		gkBlendListIterator libiter = m_file->getLibraryList();
		int libCount = 0;
		while (libiter.hasMoreElements())
		{
			Blender::Library* lib = (Blender::Library*)libiter.getNext();

			gkString base = gkBlendFile::getLibraryPath(lib->filepath);


			if (gkBlendLoader::getSingleton().isLibraryLoaded(base) || base.empty() )
				continue;
			else
				gkLogger::write("Loading Library: "+base,true);

				// load library-blend and keep give the blend-loader the pointer
				// to be able to dispose the blend when needed or in destructor

			gkString groupName = gkUtils::getUniqueName("BLEND");
			bool inGlobalPool = true;
			gkResourceGroupManager::getSingleton().createResourceGroup(groupName, inGlobalPool);

			gkBlendFile* blend = new gkBlendFile(base, groupName);
			gkBlendLoader::getSingleton().addLibraryBlend(blend);

			if (!blend->parse(gkBlendLoader::LO_ALL_SCENES | gkBlendLoader::LO_IS_LIBRARY))
			{
				gkLogger::write("Problem loading Library:"+base,true);
			}
		}
	}
	// Load / convert all
	buildAllTextures();
	buildAllFonts();
	buildTextFiles();
	buildAllSounds();
	buildAllActions();
	buildAllParticles();

	gkBlendListIterator iter = m_file->getSceneList();
	while (iter.hasMoreElements())
	{
		Blender::Scene* sc = (Blender::Scene*)iter.getNext();

		if (m_findScene.empty() || m_findScene == GKB_IDNAME(sc))
		{			
			gkBlenderSceneConverter conv(this, sc);
			conv.convert(false);

			gkScene* gks = (gkScene*)gkSceneManager::getSingleton().getByName(gkResourceName(GKB_IDNAME(sc), m_group));
			if (gks)
				m_scenes.push_back(gks);
		}
	}
	// a second pass for creating groupinstances. groups from all scenes have to be converted before the
	// group-instances can be created.
	iter = m_file->getSceneList();
	while (iter.hasMoreElements())
	{
		Blender::Scene* sc = (Blender::Scene*)iter.getNext();

		if (m_findScene.empty() || m_findScene == GKB_IDNAME(sc))
		{
			gkBlenderSceneConverter conv(this, sc);
			conv.convertGroupInstances();
		}
	}


	if (curscene)
	{
		// Grab the main scene
		m_activeScene = (gkScene*) gkSceneManager::getSingleton().getByName(gkResourceName(GKB_IDNAME(curscene), m_group));
	}

	if (m_activeScene == 0 && !m_scenes.empty())
		m_activeScene = m_scenes.front();
}

gkString gkBlendFile::getLibraryPath(const gkString& libPath)
{
	gkPath path(libPath);
	gkString base;

	if (gkPath(path.base()).exists()) {
		base = path.base();
	}
	else {
		// check the subfolders
		utStringArray pathSplit;
		path.split(pathSplit);

		gkString temp(path.base());
		for (int i = pathSplit.size()-2; i>=0 ; i--)
		{
			temp = pathSplit[i] + gkPath::SEPERATOR + temp;
			if (gkPath(temp).exists()) {
				base = temp;
				break;
			}
		}
	}
	return base;
}

// like createInstances() but discards the libraries OnInit.lua
void gkBlendFile::loadLibrary(void)
{
	GK_ASSERT(m_file);

	Blender::FileGlobal* fg = m_file->getFileGlobal();

	Blender::Scene* curscene = fg ? fg->curscene : 0;

	readCurSceneInfo(curscene);

	gkBlendListIterator libiter = m_file->getLibraryList();
	int libCount = 0;
	while (libiter.hasMoreElements())
	{
		Blender::Library* lib = (Blender::Library*)libiter.getNext();


		gkString base = gkBlendFile::getLibraryPath(lib->filepath);
		if (base.empty())
		{
			gkLogger::write("Couldn't find Library : "+gkString(lib->filepath),true);
			continue;
		}

		if (gkBlendLoader::getSingleton().isLibraryLoaded(base))
			continue;
		else
			gkLogger::write("Loading Library: "+base,true);

		// load library-bend and keep give the blend-loader the pointer
		// to be able to dispose the blend when needed or in destructor

		gkString groupName = gkUtils::getUniqueName("BLEND");
		bool inGlobalPool = true;
		gkResourceGroupManager::getSingleton().createResourceGroup(groupName, inGlobalPool);

		gkBlendFile* blend = new gkBlendFile(base, groupName);
		gkBlendLoader::getSingleton().addLibraryBlend(blend);

		if (!blend->parse(gkBlendLoader::LO_ALL_SCENES | gkBlendLoader::LO_IS_LIBRARY))
		{
			gkLogger::write("Problem loading Library:"+base,true);
		}
	}
	// Load / convert all
	buildAllTextures();
	buildAllFonts();
	// call with library flag on to discard OnInit.lua-blocks
	buildTextFiles(true);
	buildAllSounds();
	buildAllActions();
	buildAllParticles();

	gkBlendListIterator iter = m_file->getSceneList();
	while (iter.hasMoreElements())
	{
		Blender::Scene* sc = (Blender::Scene*)iter.getNext();

		if (m_findScene.empty() || m_findScene == GKB_IDNAME(sc))
		{
			gkBlenderSceneConverter conv(this, sc);
			conv.convert(false);

			gkScene* gks = (gkScene*)gkSceneManager::getSingleton().getByName(gkResourceName(GKB_IDNAME(sc), m_group));
			if (gks)
				m_scenes.push_back(gks);
		}
	}
	// a second pass for creating groupinstances. groups from all scenes have to be converted before the
	// group-instances can be created.
	iter = m_file->getSceneList();
	while (iter.hasMoreElements())
	{
		Blender::Scene* sc = (Blender::Scene*)iter.getNext();

		if (m_findScene.empty() || m_findScene == GKB_IDNAME(sc))
		{
			gkBlenderSceneConverter conv(this, sc);
			conv.convertGroupInstances();
		}
	}


	if (curscene)
	{
		// Grab the main scene
		m_activeScene = (gkScene*) gkSceneManager::getSingleton().getByName(gkResourceName(GKB_IDNAME(curscene), m_group));
	}

	if (m_activeScene == 0 && !m_scenes.empty())
		m_activeScene = m_scenes.front();
}




gkScene* gkBlendFile::getSceneByName(const gkString& name)
{

	Scenes::Iterator it = m_scenes.iterator();
	while (it.hasMoreElements())
	{
		gkScene* ob = it.getNext();
		if (ob->getName() == name)
			return ob;
	}
	return 0;
}


void gkBlendFile::buildTextFiles(bool isLibrary)
{
	// Create a list of all internal text blocks

	gkTextManager& txtMgr = gkTextManager::getSingleton();
	gkBlendListIterator iter = m_file->getTextList();

	while (iter.hasMoreElements())
	{
		Blender::Text* txt = (Blender::Text*)iter.getNext();
	
		gkString txtBlockName(GKB_IDNAME(txt));

		// discard OnInit.lua for libraries
		if (isLibrary && txtBlockName == "OnInit.lua")
			continue;

		Blender::TextLine* tl = (Blender::TextLine*)txt->lines.first;
		std::stringstream ss;

		while (tl)
		{
			tl->line[tl->len] = 0;

			ss << tl->line << '\n';
			tl = tl->next;
		}

		gkString str = ss.str();

		gkResourceName txtName(txtBlockName, m_group);

		if (!str.empty() && !txtMgr.exists(txtName))
		{
			gkTextFile* tf = (gkTextFile*)txtMgr.create(txtName);
			tf->setText(str);

			if (!m_hasBFont)
				m_hasBFont = tf->getType() == gkTextManager::TT_BFONT;
		}
	}

	txtMgr.parseScripts(m_group);
}



void gkBlendFile::buildAllTextures(void)
{
	gkBlendListIterator iter = m_file->getImageList();

	while (iter.hasMoreElements()) {
		Blender::Image* ima = (Blender::Image*) iter.getNext();
		// don't try & convert zero users
		// and skip linked images
		if (ima->id.us <= 0 || ima->id.lib != 0)
			continue;


		gkString name(GKB_IDNAME(ima));


		if (name.empty())
		{
			name = ima->name;

//			if (name.find_first_of("//") == 0){
//				name =  name.substr(2);
//				gkPath blendPath(m_file->getFileGlobal()->filename);
//				gkPath locPath(blendPath.directory());
//				gkPath texDir(name);
//				locPath.append(texDir.directory());
//				gkString newPath = locPath.getPath();
//				Ogre::Root::getSingleton().addResourceLocation(locPath.getPath(), "FileSystem", m_group);
//			}

			gkPath path(name);
			name = path.base();
		}




		gkPath vidPath(ima->name);
		bool isVideo = false;
#ifdef OGREKIT_USE_THEORA
		if (vidPath.extension()==".ogv" || vidPath.extension()==".ogg" )
		{
			isVideo = true;





			gkString validPath = vidPath.findValidPath();

			if (!validPath.empty())
			{
				gkTextureLoader::registerVideoTexture(name,validPath);

				continue;
			}

			// didn't it for some reason. But since we know it is a video, we can skip here:
			continue;
		}
#endif

		Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName(
				name, m_group).staticCast<Ogre::Texture>();

		if (tex.isNull()) {
			if (ima->packedfile) // is the texture packed with blender?
			{
				gkTextureLoader* loader = new gkTextureLoader(ima);

				gkString imageName(ima->name);
				if (imageName.empty())
					imageName = GKB_IDNAME(ima);
				else
				{
					gkPath path(imageName);
					imageName = path.base();
				}
//<<<<<<< HEAD
				tex = Ogre::TextureManager::getSingleton().create(
						imageName, m_group, true, loader).staticCast<Ogre::Texture>();
/*=======
				tex = Ogre::TextureManager::getSingleton().create(name, m_group, true, loader);
>>>>>>> master*/

				if (!tex.isNull())
					m_loaders.push_back(loader);
				else
					delete loader;
//<<<<<<< HEAD
			} else {
				gkString texName = GKB_IDNAME(ima);
				gkString stTexPath(ima->name);
				gkPath texPath (gkString(ima->name));
				texName = texPath.base();
				bool found = false;

				try {
					gkLogger::write(
							"Texture " + texName
									+ " not packed! Try to locate it via ogre-resources in group:'"
									+ m_group + "'", true);

					tex =
							Ogre::TextureManager::getSingleton().load(texName,
									m_group, Ogre::TEX_TYPE_2D,
									gkEngine::getSingleton().getUserDefs().defaultMipMap);
/*=======
			}
			else
			{
//				gkString texName = GKB_IDNAME(ima);
				bool found = false;

				try
				{
					gkLogger::write("Texture "+name+" not packed! Try to locate it via ogre-resources in group:'"+m_group+"'",true);

					tex = Ogre::TextureManager::getSingleton().load(name, m_group, Ogre::TEX_TYPE_2D, gkEngine::getSingleton().getUserDefs().defaultMipMap);
>>>>>>> master*/

					if (!tex.isNull()) {
						found = true;
					}
//<<<<<<< HEAD
				} catch (...) {
					gkString texPath(ima->name);

					if (texPath.find_first_of("//") == 0) {
/*=======
				}
				catch (...)
				{
					{
>>>>>>> master*/
						gkPath blendPath(m_file->getFileGlobal()->filename);
						gkPath locPath(blendPath.directory());
						gkPath texDir(name);
						locPath.append(texDir.directory());

//<<<<<<< HEAD
						gkLogger::write(
								"Texture " + texName
										+ " not found! Try to add relative FileSystem location in group:'"
										+ m_group + "' " + locPath.getPath()
										+ " - " + blendPath.getPath(), true);

						Ogre::Root::getSingleton().addResourceLocation(
								locPath.getPath(), "FileSystem", m_group);

						try {
							tex =
									Ogre::TextureManager::getSingleton().load(
											texName, m_group, Ogre::TEX_TYPE_2D,
											gkEngine::getSingleton().getUserDefs().defaultMipMap);
/*=======
						gkLogger::write("Texture "+name+" not found! Try to add relative FileSystem location in group:'"+m_group+"' " + locPath.getPath() + " - " + blendPath.getPath(), true);



						try
						{
							tex = Ogre::TextureManager::getSingleton().load(name, m_group, Ogre::TEX_TYPE_2D, gkEngine::getSingleton().getUserDefs().defaultMipMap);
>>>>>>> master*/

							if (!tex.isNull()) {
								found = true;
							}
						} catch (...) {
						}
					}
				}

//<<<<<<< HEAD
				if (found) {
					gkLogger::write("FOUND(" + texName + ")!!!", true);
				} else {
					gkLogger::write("NOT FOUND(" + texName + ")!!!", true);
/*=======
				if (found)
				{
					gkLogger::write("FOUND("+name+")!!!",true);
				}
				else
				{
					gkLogger::write("NOT FOUND("+name+")!!!",true);
>>>>>>> master*/
				}
			}
		}

	}
}


void gkBlendFile::buildAllParticles(void)
{
#ifdef OGREKIT_USE_PARTICLE
	gkParticleConverter conv(m_group, m_animFps);

	gkBlendListIterator iter = m_file->getParticleList();
	while (iter.hasMoreElements())	
	{
		Blender::ParticleSettings* ps = (Blender::ParticleSettings*)iter.getNext();

		conv.convertParticle(ps);
	}
#endif
}

void gkBlendFile::buildAllSounds(void)
{
#ifdef OGREKIT_OPENAL_SOUND
	gkSoundManager* mgr = gkSoundManager::getSingletonPtr();

	if (!mgr->isValidContext())
		return;
	
	gkBlendListIterator iter = m_file->getSoundList();

	while (iter.hasMoreElements())	
	{
		Blender::bSound* sound = (Blender::bSound*)iter.getNext();

		// skip zero users
		if (sound->id.us <= 0)
			continue;

		gkString soundPath(sound->name);
		if (soundPath[0]=='/' && soundPath[1]=='/')
			soundPath = soundPath.substr(2);

		gkPath pth(soundPath);
		bool isFile = pth.isFile();

		if (sound->packedfile || sound->newpackedfile || isFile)
		{
			Blender::PackedFile* pak = sound->packedfile ? sound->packedfile : sound->newpackedfile;
			if (((pak && pak->data) || isFile) && !mgr->exists(gkResourceName(GKB_IDNAME(sound), m_group)))
			{
				gkSound* sndObj = mgr->create<gkSound>(gkResourceName(GKB_IDNAME(sound), m_group));
				if (!sndObj)
					continue;

				if (isFile)
				{
					// Attempt to stream from file
					if (!sndObj->load(pth.getPath().c_str()))
					{
						mgr->destroy(sndObj);
						continue;
					}

					gkLogMessage("Sound: Loaded file " << pth.getPath() << " as" << GKB_IDNAME(sound) << ".");
				}
				else if (pak && pak->data)
				{
					// load from buffer
					if (!sndObj->load(pak->data, pak->size))
					{
						mgr->destroy(sndObj);
						continue;
					}

					gkLogMessage("Sound: Loaded buffer " << GKB_IDNAME(sound) << ".");
				}
				else
					GK_ASSERT(0);
			} else {

			}
		}
	}

#endif
}

void gkBlendFile::buildAllFonts(void)
{
#if defined(OGREKIT_COMPILE_OGRE_SCRIPTS) || defined(OGREKIT_COMPILE_LIBROCKET)
	if (!m_hasBFont)
		return;
	
	gkFontManager& fmgr = gkFontManager::getSingleton();

	gkBlendListIterator iter = m_file->getVFontList();
	while (iter.hasMoreElements())	
	{
		Blender::VFont* vf = (Blender::VFont*)iter.getNext();
	
		if (vf->id.us <= 0 || !vf->packedfile)
			continue;

		Blender::PackedFile* pak = vf->packedfile;

		gkFont* fnt = (gkFont*)fmgr.create(gkResourceName(GKB_IDNAME(vf), m_group));
		if(fnt)
			fnt->setData(pak->data, pak->size);
	}
#endif
}


void gkBlendFile::buildAllActions(void)
{
	gkAnimationLoader anims(m_group);
		
    gkBlendListIterator iter = m_file->getActionList();
	anims.convertActions(iter, m_file->getVersion() <= 249, m_animFps);
}



void gkBlendFile::doVersionTests(void)
{	
	int version = m_file->getVersion();

	
	Blender::FileGlobal* fg = m_file->getFileGlobal() ;

	if (version <= 242 && fg && fg->curscene == 0)
	{
		fg->curscene = m_file->getFirstScene();
	}

	if (version <= 249)
	{
		gkBlendListIterator iter = m_file->getObjectList();
		while (iter.hasMoreElements())	
		{
			Blender::Object* ob = (Blender::Object*)iter.getNext();		

			if (ob->gameflag & OB_DYNAMIC)
				ob->body_type = ob->gameflag & OB_RIGID_BODY ? OB_BODY_TYPE_RIGID : OB_BODY_TYPE_DYNAMIC;
			else if (ob->gameflag & OB_RIGID_BODY)
				ob->body_type = OB_BODY_TYPE_RIGID;
			else
				ob->body_type = OB_BODY_TYPE_STATIC;
		}
	}

	if (version <= 250)
	{
		gkBlendListIterator iter = m_file->getObjectList();
		while (iter.hasMoreElements())	
		{
			Blender::Object* ob = (Blender::Object*)iter.getNext();	
		
			for (Blender::bConstraint* bc = (Blender::bConstraint*)ob->constraints.first; bc; bc = bc->next)
			{
				// convert rotation types to radians
				if (bc->type == CONSTRAINT_TYPE_ROTLIMIT)
				{
					Blender::bRotLimitConstraint* lr = (Blender::bRotLimitConstraint*)bc->data;
					lr->xmax *= gkRPD;
					lr->xmin *= gkRPD;
					lr->ymax *= gkRPD;
					lr->ymin *= gkRPD;
					lr->zmax *= gkRPD;
					lr->zmin *= gkRPD;
				}
			}

			ob = (Blender::Object*)ob->id.next;
		}
	}

	// BFont test
	{
		m_hasBFont = false;
	
		gkBlendListIterator iter = m_file->getTextList();
		while (iter.hasMoreElements())
		{
			Blender::Text* txt = (Blender::Text*)iter.getNext();
					
			if (gkString(txt->id.name).find(".bfont"))
			{
				m_hasBFont = true;
				break;
			}
		}
	}
}
