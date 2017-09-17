/*
 * gkPBSManager.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: ttrocha
 */

#include "gkPBSManager.h"

#include "OgreHlmsManager.h"
#include "OgreMaterialManager.h"

#include "OgreSubEntity.h"

#include "OgrePrerequisites.h"

#include "OgrePass.h"

#include "OgreHlmsPbsMaterial.h"

#include "gkScene.h"

#include "gkPath.h"

#include "gkString.h"
gkPBSManager::gkPBSManager(gkScene* scene)
	: m_scene(scene), m_hlmsManager(0), m_initialized(false)
{}

gkPBSManager::~gkPBSManager() {
	if (m_hlmsManager)
	{
		for (size_t i = 0; i < mPBSMaterialList.size(); ++i)
		{
			delete mPBSMaterialList[i];
		}

		for (size_t i = 0; i < mMaterialList.size(); ++i)
		{
			Ogre::MaterialManager::getSingleton().unload(mMaterialList[i]);
			Ogre::MaterialManager::getSingleton().remove(mMaterialList[i]);
		}

		m_hlmsManager->unbindAll("pbs");

		delete m_hlmsManager;
	}
}

void gkPBSManager::init()
{
	if (m_scene && !m_hlmsManager)
	{
		m_hlmsManager = new Ogre::HlmsManager(m_scene->getManager());
	}
}

void gkPBSManager::createHLMSMaterial(Ogre::Entity *ent, Ogre::PbsMaterial* pbsMaterial, const gkString& matName)
{
	gkPBSManager::ensureHasTangents(ent->getMesh());
	size_t count = ent->getNumSubEntities();
	for (size_t i = 0; i < count; i++)
	{
		Ogre::SubEntity* subEnt = ent->getSubEntity(i);
		Ogre::MaterialPtr newMat = subEnt->getMaterial()->clone(matName + "_" + Ogre::StringConverter::toString(i));

		newMat->getBestTechnique()->removeAllPasses();

		Ogre::Pass* pass = newMat->getBestTechnique()->createPass();
		pass->setName("pbs");

		subEnt->setMaterial(newMat);

		mMaterialList.push_back(gkString(newMat->getName()));
		m_hlmsManager->bind(subEnt, pbsMaterial, "pbs");
	}
}

void gkPBSManager::ensureHasTangents(Ogre::MeshPtr mesh)
{
	bool generateTangents = false;
	if (mesh->sharedVertexData)
	{
		Ogre::VertexDeclaration *vertexDecl = mesh->sharedVertexData->vertexDeclaration;
		generateTangents |= gkPBSManager::hasNoTangentsAndCanGenerate(vertexDecl);
	}

	for (size_t i = 0; i<mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh *subMesh = mesh->getSubMesh(i);
		if (subMesh->vertexData)
		{
			Ogre::VertexDeclaration *vertexDecl = subMesh->vertexData->vertexDeclaration;
			generateTangents |= hasNoTangentsAndCanGenerate(vertexDecl);
		}
	}

	try
	{
		if (generateTangents)
		{
			mesh->buildTangentVectors();
		}


	}
	catch (...) {}
}

bool gkPBSManager::hasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertexDecl)
{
	bool hasTangents = false;
	bool hasUVs = false;
	const Ogre::VertexDeclaration::VertexElementList &elementList = vertexDecl->getElements();
	Ogre::VertexDeclaration::VertexElementList::const_iterator itor = elementList.begin();
	Ogre::VertexDeclaration::VertexElementList::const_iterator end = elementList.end();

	while (itor != end && !hasTangents)
	{
		const Ogre::VertexElement &vertexElem = *itor;
		if (vertexElem.getSemantic() == Ogre::VES_TANGENT)
			hasTangents = true;
		if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES)
			hasUVs = true;

		++itor;
	}

	return !hasTangents && hasUVs;
}

Ogre::TexturePtr gkPBSManager::loadTexture(gkString pathName,Ogre::TextureType type)
{
	gkPath path(pathName);
	// use only the filename, which means the directory have to be in ogre-resource
	gkString base = path.base();
	if (Ogre::ResourceGroupManager::getSingleton().resourceExistsInAnyGroup(base))
	{
		Ogre::TexturePtr tex = Ogre::TextureManager::getSingletonPtr()->load(base, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, type);
		return tex;
	}
	return Ogre::TexturePtr();
}
