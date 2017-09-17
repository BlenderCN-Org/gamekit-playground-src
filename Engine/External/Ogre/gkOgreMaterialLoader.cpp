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
#include "gkCommon.h"
#include "gkMesh.h"
#include "gkScene.h"
#include "gkOgreMaterialLoader.h"
#include "gkOgreSkyBoxGradient.h"
#include "gkEngine.h"
#include "gkUserDefs.h"
#include "OgreMeshManager.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreSceneManager.h"
#include "OgreRoot.h"
#include "gkLogger.h"
#include "gkValue.h"
#include "gkTextureLoader.h"

#ifdef OGREKIT_USE_THEORA
# include "OgreVideoManager.h"
#endif

#ifdef OGREKIT_USE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"
#include "OgreShaderGenerator.h"



#endif

gkMaterialLoader::gkMaterialLoader()
{
#ifdef OGREKIT_USE_RTSHADER_SYSTEM
	Ogre::MaterialManager::getSingleton().addListener(this);
#endif
}

gkMaterialLoader::~gkMaterialLoader()
{
#ifdef OGREKIT_USE_RTSHADER_SYSTEM
	Ogre::MaterialManager::getSingleton().removeListener(this);
#endif
}

Ogre::MaterialPtr gkMaterialLoader::createRTSSMaterial(const gkString& matName, bool enableLight)
{
#ifdef OGREKIT_USE_RTSHADER_SYSTEM
	Ogre::RTShader::ShaderGenerator* shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	GK_ASSERT(shaderGenerator);

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(matName, 
			Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME).staticCast<Ogre::Material>();

	if (material.isNull())
	{
		material = Ogre::MaterialManager::getSingleton().create(matName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).staticCast<Ogre::Material>();
	}
	
	material->setLightingEnabled(enableLight);

	shaderGenerator->createShaderBasedTechnique(matName, 
		Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
	shaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, matName);
	material->getTechnique(0)->getPass(0)->setVertexProgram(
		material->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
	material->getTechnique(0)->getPass(0)->setFragmentProgram(
		material->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());

	GK_ASSERT(!material->getTechnique(0)->getPass(0)->getVertexProgram().isNull());
	GK_ASSERT(!material->getTechnique(0)->getPass(0)->getFragmentProgram().isNull());

	return material;
#else
	return Ogre::MaterialPtr();
#endif
}


//copied from ogre sample browser
Ogre::Technique* gkMaterialLoader::handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String& schemeName, 
	Ogre::Material* originalMaterial, unsigned short lodIndex, const Ogre::Renderable* rend)
{	
#ifdef OGREKIT_USE_RTSHADER_SYSTEM
	Ogre::RTShader::ShaderGenerator* shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	GK_ASSERT(shaderGenerator);

	Ogre::Technique* generatedTech = 0;

	if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
	{
		bool techniqueCreated;

		techniqueCreated = shaderGenerator->createShaderBasedTechnique(
			originalMaterial->getName(), Ogre::MaterialManager::DEFAULT_SCHEME_NAME, schemeName);	

		if (techniqueCreated)
		{
			shaderGenerator->validateMaterial(schemeName, originalMaterial->getName());
				
			Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();

			while (itTech.hasMoreElements())
			{
				Ogre::Technique* curTech = itTech.getNext();

				if (curTech->getSchemeName() == schemeName)
				{
					generatedTech = curTech;
					break;
				}
			}				
		}
	}

	return generatedTech;
#else
	return 0;
#endif
}


gkSkyBoxGradient* gkMaterialLoader::loadSceneSkyMaterial(class gkScene* sc, const gkSceneMaterial& material)
{
	//return 0;

	//skybox material should be exist in the global resource pool.
	//multiple skybox materials don't working in multi window/scene.
	gkString groupName = sc->getGroupName(); //GK_BUILTIN_GROUP; //

	
	// use user defined
	Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().getByName(material.m_name).staticCast<Ogre::Material>();
	if (!matptr.isNull())
	{
		sc->getManager()->setSkyBox(true, material.m_name, material.m_distance, true, gkEuler(-90, 0, 0).toQuaternion());
		return 0;
	}

	if (material.m_type == gkSceneMaterial::FLAT)
		return 0;

	gkSkyBoxGradient* grad = new gkSkyBoxGradient(material, groupName); //sc->getGroupName());
	sc->getManager()->setSkyBox(true, material.m_name, material.m_distance, true);
	return grad;
}



Ogre::MaterialPtr gkMaterialLoader::loadSubMeshMaterial(gkSubMesh* mesh, const gkString& group)
{
	using namespace Ogre;

	gkMaterialProperties& gma = mesh->getMaterial();
	if (gma.m_name.empty())
		gma.m_name = "<gkBuiltin/DefaultMaterial>";

	Ogre::MaterialPtr oma = Ogre::MaterialManager::getSingleton().getByName(gma.m_name.c_str(), group).staticCast<Ogre::Material>();
	bool isOgreMaterial = false;
	if (oma.isNull())
	{
		oma = Ogre::MaterialManager::getSingleton().create(gma.m_name, group).staticCast<Ogre::Material>();
	}
	else
	{
		isOgreMaterial = true;

		if (!gma.m_ignoreBlenderTextures)
			return oma;
	}

	bool hasNormap = false;
	bool rtss = gkEngine::getSingleton().getUserDefs().rtss;
	Ogre::Pass* pass = oma->getTechnique(0)->getPass(0);

	for (int i = 0; i < gma.m_totaltex; ++i)
	{		
		gkTextureProperties& gte = gma.m_textures[i];

		// check if this texture is set active inside blender
		// TODO: We might need a way to preserve the texture-state position.
		//       maybe something like a null-texture!?
		if (!gte.m_isActive)
			continue;

#ifdef OGREKIT_USE_THEORA
			// if it is a video-texture create the texture
			if (gkTextureLoader::isVideoTexture(gte.m_name))
			{
				OgreVideoManager* ovm = static_cast<OgreVideoManager*>(OgreVideoManager::getSingletonPtr());
				ovm->setTextureName(gte.m_name);
				ovm->setInputName(gkTextureLoader::getVideoTextureFile(gte.m_name));
				ovm->setFPS(24);
				ovm->setPlayMode(TextureEffectPlay_Looping);
				ovm->setDefaultNumPrecachedFrames(50);
				ovm->setTextureTecPassStateLevel(0,0,i);
				ovm->setAlpha((gma.m_mode & gkMaterialProperties::MA_ALPHACLIP)
								|| (gma.m_mode & gkMaterialProperties::MA_ALPHABLEND)
							  );
				ovm->createTexture(gma.m_name, group);
			}
#endif

#ifdef OGREKIT_USE_RTSHADER_SYSTEM
		if (gte.m_mode & gkTextureProperties::TM_NORMAL)
		{
			hasNormap = true;



			continue;
		}
#endif
		// TODO: Do I need to create clones?
		Ogre::TextureUnitState* otus = NULL;

		if (i < pass->getNumTextureUnitStates())
		{
			otus = pass->getTextureUnitState(i);
			otus->setTextureName(gte.m_image);
		}

		if (!otus) {
			otus = pass->createTextureUnitState(gte.m_image, gte.m_layer);
			otus->setTextureScale(gte.m_scale[0],gte.m_scale[1]);
		}



		LayerBlendOperationEx op = LBX_MODULATE;

		switch (gte.m_blend)
		{
		case GK_BT_ADDITIVE:
			op = LBX_ADD;
			break;

		case GK_BT_SUBTRACT:			
			op = LBX_SUBTRACT;
			break;

		case GK_BT_DARKEN:	
		case GK_BT_LIGHTEN:	
		case GK_BT_SCREEN:
		case GK_BT_COLOR:
			//break; TODO: support more mode

		case GK_BT_MULTIPLY:
		case GK_BT_MIXTURE:
		default:
			op = LBX_MODULATE;
			break;
		}

		if (i == 0)
			otus->setColourOperationEx(op, LBS_DIFFUSE, LBS_TEXTURE);		
		else
			otus->setColourOperationEx(op);



#ifdef OGREKIT_USE_THEORA
		if (gkTextureLoader::isVideoTexture(gte.m_name))
		{
			OgreVideoManager* ovm = static_cast<OgreVideoManager*>(OgreVideoManager::getSingletonPtr());
			ovm->setTextureState(otus);
			ovm->setTextureName(gte.m_name);
			ovm->setInputName(gkTextureLoader::getVideoTextureFile(gte.m_name));
			ovm->setFPS(24);
//			ovm->setPlayMode(TextureEffectPlay_Looping);
			ovm->setDefaultNumPrecachedFrames(50);
			ovm->setTextureTecPassStateLevel(0,0,i);
			ovm->setAlpha((gma.m_mode & gkMaterialProperties::MA_ALPHACLIP)
							|| (gma.m_mode & gkMaterialProperties::MA_ALPHABLEND)
						  );
			ovm->setCyclic(gte.m_isCyclic);
			ovm->createDefinedTexture(gma.m_name, group);
		}
#endif

	}

	if (!gma.m_ignoreBlenderTextures)
	{
		if (gma.m_mode & gkMaterialProperties::MA_INVISIBLE)
		{
			// disable writing to this material
			oma->setReceiveShadows(false);
			oma->setColourWriteEnabled(false);
			oma->setDepthWriteEnabled(false);
			oma->setDepthCheckEnabled(false);
			oma->setLightingEnabled(false);
			return oma;
		}

		if (gma.m_mode & gkMaterialProperties::MA_TWOSIDE)
		{
			oma->setCullingMode(Ogre::CULL_NONE);
			oma->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
		}

		// apply lighting params

		bool enableLights = (gma.m_mode & gkMaterialProperties::MA_LIGHTINGENABLED) != 0;
		oma->setReceiveShadows((gma.m_mode & gkMaterialProperties::MA_RECEIVESHADOWS) != 0);

		oma->setLightingEnabled(enableLights);
		if (enableLights)
		{
			gkColor emissive, ambient, specular, diffuse;

			emissive    = gma.m_diffuse * gma.m_emissive;
			ambient     = gma.m_diffuse * gma.m_ambient;
			specular    = gma.m_specular * gma.m_spec;
			diffuse     = gma.m_diffuse * (gma.m_emissive + gma.m_refraction);

			emissive.a = ambient.a = specular.a = 1.f;

			oma->setSelfIllumination(emissive);
			oma->setAmbient(ambient);
			oma->setSpecular(specular);
			oma->setDiffuse(diffuse);
			oma->setShininess(gma.m_hardness);
		}
		else
		{
			gkColor ambient, diffuse;
			diffuse     = gma.m_diffuse * (gma.m_emissive + gma.m_refraction);
			ambient     = gma.m_diffuse * gma.m_ambient;
			oma->setAmbient(ambient);
			oma->setDiffuse(diffuse);
		}



		bool matBlending = gkEngine::getSingleton().getUserDefs().matblending;

		if (matBlending && (gma.m_mode & gkMaterialProperties::MA_HASRAMPBLEND))
		{
			switch (gma.m_rblend)
			{
			case GK_BT_MULTIPLY:
				pass->setSceneBlending(SBT_MODULATE);
				break;
			case GK_BT_SUBTRACT:
				pass->setSceneBlending(SBF_ONE_MINUS_SOURCE_COLOUR, SBF_ONE);
				break;
			case GK_BT_DARKEN:
				pass->setSceneBlendingOperation(SBO_MIN);
				pass->setSceneBlending(SBF_ONE, SBF_ONE);
				break;
			case GK_BT_LIGHTEN:
				pass->setSceneBlendingOperation(SBO_MAX);
				pass->setSceneBlending(SBF_ONE, SBF_ONE);
				break;
			case GK_BT_SCREEN:
				pass->setSceneBlending(SBF_ONE_MINUS_DEST_COLOUR, SBF_ONE);
				break;
			case GK_BT_ADDITIVE:
				pass->setSceneBlending(SBT_ADD);
				break;
			case GK_BT_MIXTURE:
			default:
				pass->setSceneBlending(SBF_ONE, SBF_ZERO);
				break;
			}
		}





		if (gma.m_mode & gkMaterialProperties::MA_ALPHABLEND)
		{
			pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
			pass->setDepthWriteEnabled(false);
		}

		if (gma.m_mode & gkMaterialProperties::MA_ALPHACLIP)
		{
			pass->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, 128);
		}

	}

	if (!gma.m_vertexProgram.empty() && !gma.m_fragmentProgram.empty())
	{
		pass->setVertexProgram(gma.m_vertexProgram);
		pass->setFragmentProgram(gma.m_fragmentProgram);
	}
#ifdef OGREKIT_USE_RTSHADER_SYSTEM
	else if (rtss)
	{
		//pass->setSpecular(ColourValue::Black);
		//pass->setShininess(0.0);

		RTShader::RenderState* rs = 0;
		RTShader::ShaderGenerator* sg = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
		bool ok = sg->createShaderBasedTechnique(gma.m_name, group, 
			Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

		if (ok && hasNormap)
		{
			rs = sg->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, gma.m_name, 0);
			rs->reset();

			for (int i = 0; i < gma.m_totaltex; ++i)
			{
				gkTextureProperties& gte = gma.m_textures[i];

				if (gte.m_mode & gkTextureProperties::TM_NORMAL)
				{
					GK_ASSERT(rs);

					RTShader::SubRenderState* srs= sg->createSubRenderState(RTShader::NormalMapLighting::Type);
				
					RTShader::NormalMapLighting* nsrs = static_cast<RTShader::NormalMapLighting*>(srs);
					if (gte.m_texmode & gkTextureProperties::TX_OBJ_SPACE)
						nsrs->setNormalMapSpace(RTShader::NormalMapLighting::NMS_OBJECT);
					else
						nsrs->setNormalMapSpace(RTShader::NormalMapLighting::NMS_TANGENT);
					nsrs->setNormalMapTextureName(gte.m_name);
					nsrs->setTexCoordIndex(gte.m_layer);

					rs->addTemplateSubRenderState(srs);
				}
			}

			sg->invalidateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, gma.m_name);
		}
	}
#endif
	return oma;
}


void gkMaterialLoader::setAlphaRejectSettings(const gkString& materialName, gkCompareFunction func, 
	unsigned char value, bool alphaToCoverageEnabled)
{
	if (!Ogre::MaterialManager::getSingleton().resourceExists(materialName))
		return;

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName).staticCast<Ogre::Material>();
	GK_ASSERT(!mat.isNull() && mat->getTechnique(0) && mat->getTechnique(0)->getPass(0));

	mat->getTechnique(0)->getPass(0)->setAlphaRejectSettings((Ogre::CompareFunction)func, value, alphaToCoverageEnabled);	
}



void gkMaterialLoader::setAlphaRejectValue(const gkString& materialName, unsigned char value)
{
	if (!Ogre::MaterialManager::getSingleton().resourceExists(materialName))
		return;

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName).staticCast<Ogre::Material>();
	GK_ASSERT(!mat.isNull() && mat->getTechnique(0) && mat->getTechnique(0)->getPass(0));

	mat->getTechnique(0)->getPass(0)->setAlphaRejectValue(value);	
}

