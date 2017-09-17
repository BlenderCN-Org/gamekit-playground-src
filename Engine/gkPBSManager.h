/*
 * gkPBSManager.h
 *
 *  Created on: Jan 9, 2016
 *      Author: ttrocha
 */

#ifndef ENGINE_GKPBSMANAGER_H_
#define ENGINE_GKPBSMANAGER_H_
#include "gkString.h"
#include "OgrePrerequisites.h"
#include "OgreTexture.h"

namespace Ogre
{
	class HlmsManager;
	class PbsMaterial;
	class Entity;
}

class gkScene;


class gkPBSManager{
public:


	gkPBSManager(gkScene* scene);
	virtual ~gkPBSManager();

	void init();

	void createHLMSMaterial(Ogre::Entity *ent, Ogre::PbsMaterial* pbsMaterial, const gkString& matName);

	static Ogre::TexturePtr loadTexture(gkString path,Ogre::TextureType type);
private:
	gkScene* m_scene;
	bool m_initialized;
	Ogre::HlmsManager* m_hlmsManager;

	Ogre::vector<Ogre::PbsMaterial*>::type mPBSMaterialList;
	Ogre::vector<gkString>::type mMaterialList;

	static void ensureHasTangents(Ogre::MeshPtr mesh);
	static bool hasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertexDecl);

};

#endif /* ENGINE_GKPBSMANAGER_H_ */
