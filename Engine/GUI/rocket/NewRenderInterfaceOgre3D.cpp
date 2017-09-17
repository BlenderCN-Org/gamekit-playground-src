/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "NewRenderInterfaceOgre3D.h"
#include <Ogre.h>
#include "gkGUI.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreCommon.h"
#include "gkEngine.h"
#include "gkWindow.h"
#include "gkScene.h"
#include "OgreRenderWindow.h"
#include "OgreSceneManager.h"
#include "utStreams.h"
#include "gkTextureLoader.h"
#include "gkUtils.h"
#include "../../gkValue.h"

#include "gkPath.h"

#include "OgreTextureUnitState.h"
struct RocketOgre3DVertex
{
	float x, y, z;
	Ogre::uint32 diffuse;
	float u, v;
};

// The structure created for each texture loaded by Rocket for Ogre.
struct RocketOgre3DTexture
{
	RocketOgre3DTexture(Ogre::TexturePtr texture) : texture(texture)
	{
	}

	Ogre::TexturePtr texture;
};

// The structure created for each set of geometry that Rocket compiles. It stores the vertex and index buffers and the
// texture associated with the geometry, if one was specified.
struct RocketOgre3DCompiledGeometry
{
	Ogre::RenderOperation render_operation;
	RocketOgre3DTexture* texture;
	bool valid;
};

RenderInterfaceOgre3D::RenderInterfaceOgre3D(unsigned int window_width, unsigned int window_height)
{
	render_system = Ogre::Root::getSingleton().getRenderSystem();

	BuildProjectionMatrix(projMatrix);

	// Configure the colour blending mode.
	colour_blend_mode.blendType = Ogre::LBT_COLOUR;
	colour_blend_mode.source1 = Ogre::LBS_DIFFUSE;
	colour_blend_mode.source2 = Ogre::LBS_TEXTURE;
	colour_blend_mode.operation = Ogre::LBX_MODULATE;

	// Configure the alpha blending mode.
	alpha_blend_mode.blendType = Ogre::LBT_ALPHA;
	alpha_blend_mode.source1 = Ogre::LBS_DIFFUSE;
	alpha_blend_mode.source2 = Ogre::LBS_TEXTURE;
	alpha_blend_mode.operation = Ogre::LBX_MODULATE;

	scissor_enable = false;

	scissor_left = 0;
	scissor_top = 0;
	scissor_right = (int) window_width;
	scissor_bottom = (int) window_height;

	m_sceneMgr = gkEngine::getSingleton().getActiveScene()->getManager();

	try {
	d2Material = Ogre::MaterialManager::getSingletonPtr()->getByName("librocket2D").staticCast<Ogre::Material>().staticCast<Ogre::Material>();
	  if (d2Material.isNull() == false)
	  {
		d2pass = d2Material->getTechnique(0)->getPass(0);

		if(d2pass->hasVertexProgram())
		{
		  Ogre::GpuProgramPtr gpuPtr = d2pass->getVertexProgram();
		  gpuPtr->load();
		}

		if(d2pass->hasFragmentProgram())
		{
		  Ogre::GpuProgramPtr gpuPtr = d2pass->getFragmentProgram();
		  gpuPtr->load();
		}

	  }

		Ogre::MaterialPtr d2Material_notex = Ogre::MaterialManager::getSingletonPtr()->getByName("librocket2D_notex").staticCast<Ogre::Material>();
		  if (d2Material_notex.isNull() == false)
		  {
			d2pass_notex = d2Material_notex->getTechnique(0)->getPass(0);

			if(d2pass_notex->hasVertexProgram())
			{
			  Ogre::GpuProgramPtr gpuPtr = d2pass_notex->getVertexProgram();
			  gpuPtr->load();
			}

			if(d2pass_notex->hasFragmentProgram())
			{
			  Ogre::GpuProgramPtr gpuPtr = d2pass_notex->getFragmentProgram();
			  gpuPtr->load();
			}

		  }
	}
	catch (...) {
		gkLogger::write("Caught Exception!",true);
	}
}

RenderInterfaceOgre3D::~RenderInterfaceOgre3D()
{
	if (!m_loaders.empty())
	{
		ManualResourceLoaderList::Iterator it = m_loaders.iterator();
		while (it.hasMoreElements())
			delete it.getNext();
	}
}

// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void RenderInterfaceOgre3D::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
	// We've chosen to not support non-compiled geometry in the Ogre3D renderer.
	// But if you want, you can uncomment this code, so borders will be shown.
	/*
	Rocket::Core::CompiledGeometryHandle gh = CompileGeometry(vertices, num_vertices, indices, num_indices, texture);
	RenderCompiledGeometry(gh, translation);
	ReleaseCompiledGeometry(gh);
	*/
}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
Rocket::Core::CompiledGeometryHandle RenderInterfaceOgre3D::CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture)
{
	RocketOgre3DCompiledGeometry* geometry = new RocketOgre3DCompiledGeometry();
	geometry->valid = true;
	geometry->texture = texture == NULL ? NULL : (RocketOgre3DTexture*) texture;

	geometry->render_operation.vertexData = new Ogre::VertexData();
	geometry->render_operation.vertexData->vertexStart = 0;
	geometry->render_operation.vertexData->vertexCount = num_vertices;

	geometry->render_operation.indexData = new Ogre::IndexData();
	geometry->render_operation.indexData->indexStart = 0;
	geometry->render_operation.indexData->indexCount = num_indices;

	geometry->render_operation.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;


	// Set up the vertex declaration.
	Ogre::VertexDeclaration* vertex_declaration = geometry->render_operation.vertexData->vertexDeclaration;
	size_t element_offset = 0;
	vertex_declaration->addElement(0, element_offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	element_offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	vertex_declaration->addElement(0, element_offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	element_offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
	vertex_declaration->addElement(0, element_offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);


	// Create the vertex buffer.
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vertex_declaration->getVertexSize(0), num_vertices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	geometry->render_operation.vertexData->vertexBufferBinding->setBinding(0, vertex_buffer);

	// Fill the vertex buffer.
	RocketOgre3DVertex* ogre_vertices = (RocketOgre3DVertex*) vertex_buffer->lock(0, vertex_buffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
	for (int i = 0; i < num_vertices; ++i)
	{
		ogre_vertices[i].x = vertices[i].position.x;
		ogre_vertices[i].y = vertices[i].position.y;
		ogre_vertices[i].z = 0;

		Ogre::ColourValue diffuse(vertices[i].colour.red / 255.0f, vertices[i].colour.green / 255.0f, vertices[i].colour.blue / 255.0f, vertices[i].colour.alpha / 255.0f);
		render_system->convertColourValue(diffuse, &ogre_vertices[i].diffuse);

		ogre_vertices[i].u = vertices[i].tex_coord[0];
		ogre_vertices[i].v = vertices[i].tex_coord[1];
	}
	vertex_buffer->unlock();


	Ogre::HardwareIndexBufferSharedPtr index_buffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, num_indices, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	geometry->render_operation.indexData->indexBuffer = index_buffer;
	geometry->render_operation.useIndexes = true;
    unsigned short * ogre_indices = (unsigned short*)index_buffer->lock(0, index_buffer->getSizeInBytes(), Ogre::HardwareBuffer::HBL_NORMAL);
    for(int i=0;i<num_indices;i++)
        ogre_indices[i] = indices[i];

	index_buffer->unlock();


	return reinterpret_cast<Rocket::Core::CompiledGeometryHandle>(geometry);
}

// Called by Rocket when it wants to render application-compiled geometry.
void RenderInterfaceOgre3D::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation)
{
	RocketOgre3DCompiledGeometry* ogre3d_geometry = (RocketOgre3DCompiledGeometry*) geometry;
	if (!ogre3d_geometry->valid)
		return;

	Ogre::Matrix4 transform;
	transform.makeTrans(translation.x, translation.y, 0);
	render_system->_setWorldMatrix(transform);

	Ogre::Vector4 transl(translation.x, translation.y, 0,0);


//	render_system->_setWorldMatrix( Ogre::Matrix4::IDENTITY );
//	render_system->_setProjectionMatrix( Ogre::Matrix4::IDENTITY );
//	render_system->_setViewMatrix( Ogre::Matrix4::IDENTITY );

	render_system = Ogre::Root::getSingleton().getRenderSystem();

	if (ogre3d_geometry->texture != NULL)
	{
		render_system->_setTexture(0, true, ogre3d_geometry->texture->texture,Ogre::TextureUnitState::BT_FRAGMENT);

		// Ogre can change the blending modes when textures are disabled - so in case the last render had no texture,
		// we need to re-specify them.
		render_system->_setTextureBlendMode(0, colour_blend_mode);
		render_system->_setTextureBlendMode(0, alpha_blend_mode);
		d2pass->getTextureUnitState(0)->setTexture(ogre3d_geometry->texture->texture);
		Ogre::GpuProgramParametersSharedPtr params = d2pass->getVertexProgramParameters();
		const Ogre::GpuConstantDefinition* forty = params->_findNamedConstantDefinition("forty");
//		if (!params.isNull()){
//			params.get()->setNamedConstant("forty",2.0f);
//		}

		m_sceneMgr->_setPass(d2pass);
		d2pass->getVertexProgramParameters()->setNamedConstant("proj",projMatrix);
		d2pass->getVertexProgramParameters()->setNamedConstant("trans",transl);
		m_sceneMgr->_markGpuParamsDirty(0xffff);
		m_sceneMgr->updateGpuProgramParameters(d2pass);

//		d2pass->getVertexProgramParameters()->setNamedConstant("worldViewProj",view);

	}
	else{



//		const Ogre::GpuNamedConstantsPtr ptr(d2pass_notex->getVertexProgram()->getNamedConstants());
//		d2pass_notex->getVertexProgram()->_getBindingDelegate()->getDefaultParameters()->_setNamedConstants(ptr);
//		if (!params.isNull()){
//			const Ogre::GpuConstantDefinition* def = params.get()->_findNamedConstantDefinition("forty");
//			if (def) {
//				bool isFloat = def->isFloat();
//				int a=0;
//			}
//			params.get()->setNamedConstant("forty",2.0f);
//		}
//		static_cast<Ogre::GLES2RenderSystem*>(render_system)->bindGpuProgramParameters()
//		static_cast<Ogre::GLES2GpuProgram*>(d2pass_notex->getVertexProgram().get())->bindProgramParameters(params);
//		render_system->_makeProjectionMatrix(0,0,1280,800,-1,1,)
		m_sceneMgr->_setPass(d2pass_notex);

		d2pass_notex->getVertexProgramParameters()->setNamedConstant("proj",projMatrix);
		d2pass_notex->getVertexProgramParameters()->setNamedConstant("trans",transl);
		m_sceneMgr->_markGpuParamsDirty(0xffff);
		m_sceneMgr->updateGpuProgramParameters(d2pass_notex);
//		d2pass_notex->getVertexProgramParameters()->setNamedConstant("worldViewProj",view);
		render_system->_disableTextureUnit(0,Ogre::TextureUnitState::BT_FRAGMENT);
	}

	render_system->_setCullingMode(Ogre::CULL_NONE);

	render_system->_render(ogre3d_geometry->render_operation);
}

// Called by Rocket when it wants to release application-compiled geometry.
void RenderInterfaceOgre3D::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
{
	RocketOgre3DCompiledGeometry* ogre3d_geometry = reinterpret_cast<RocketOgre3DCompiledGeometry*>(geometry);
	if (ogre3d_geometry->valid){
		ogre3d_geometry->valid = false;
		delete ogre3d_geometry->render_operation.vertexData;
		delete ogre3d_geometry->render_operation.indexData;
		delete ogre3d_geometry;
	}
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.
void RenderInterfaceOgre3D::EnableScissorRegion(bool enable)
{
//	if (enable)
//		gkLogger::write("EnableScissorRegion:"+gkToString(enable),true);
	scissor_enable = enable;

	if (!scissor_enable)
		render_system->setScissorTest(false);
	else
		render_system->setScissorTest(true, scissor_left, scissor_top, scissor_right, scissor_bottom);
}

// Called by Rocket when it wants to change the scissor region.
void RenderInterfaceOgre3D::SetScissorRegion(int x, int y, int width, int height)
{
	scissor_left = x;
	scissor_top = y;
	scissor_right = x + width;
	scissor_bottom = y + height;

//	gkLogger::write("SetScissorRegion:"+gkToString(x)+":"+gkToString(y)+":"+gkToString(width)+":"+gkToString(height),true);

	if (scissor_enable)
		render_system->setScissorTest(true, scissor_left, scissor_top, scissor_right, scissor_bottom);
}

// Called by Rocket when a texture is required by the library.
bool RenderInterfaceOgre3D::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
	Ogre::TextureManager* texture_manager = Ogre::TextureManager::getSingletonPtr();
	_LOG_FOOT_
	Ogre::TexturePtr ogre_texture = texture_manager->getByName(Ogre::String(source.CString())).staticCast<Ogre::Texture>();
	_LOG_FOOT_
	if (ogre_texture.isNull())
	{
		_LOGI_("TextureName:%s",source.CString())

		try {
			ogre_texture = texture_manager->load(Ogre::String(source.CString()),
								DEFAULT_ROCKET_RESOURCE_GROUP,
								Ogre::TEX_TYPE_2D,
								0);
		} catch(...) {

			try {
				gkPath p(source.CString());
				gkString onlyFilename = p.base();
				gkString t2 = p.directory();
				ogre_texture = texture_manager->load(onlyFilename,
												DEFAULT_ROCKET_RESOURCE_GROUP,
												Ogre::TEX_TYPE_2D,
												0);
			}
			catch (...)
			{

			}

			gkLogger::write("Couldn't get "+gkString(source.CString()),true);
		}
	}

	if (ogre_texture.isNull())
		return false;

	_LOG_FOOT_
	texture_dimensions.x = ogre_texture->getWidth();
	_LOG_FOOT_
	texture_dimensions.y = ogre_texture->getHeight();
	_LOGI_("TextureName:%s",source.CString())
	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(new RocketOgre3DTexture(ogre_texture));
	_LOG_FOOT_
	return true;
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool RenderInterfaceOgre3D::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
	static int texture_id = 1;
#if !defined(OGREKIT_BUILD_ANDROID)
	Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream((void*) source, source_dimensions.x * source_dimensions.y * sizeof(unsigned int)));
	       Ogre::TexturePtr ogre_texture = Ogre::TextureManager::getSingleton().loadRawData(
	                                                                               Rocket::Core::String(16, "%d", texture_id++).CString(),
	                                                                               DEFAULT_ROCKET_RESOURCE_GROUP,
	                                                                               stream,
	                                                                               (Ogre::ushort)source_dimensions.x,
	                                                                               (Ogre::ushort)source_dimensions.y,
	                                                                               Ogre::PF_A8B8G8R8,
	                                                                               Ogre::TEX_TYPE_2D,
	                                                                               0);
	if (ogre_texture.isNull())
	return false;
#else
	_LOG_FOOT_
	Ogre::DataStreamPtr stream(OGRE_NEW Ogre::MemoryDataStream((void*) source, source_dimensions.x * source_dimensions.y * sizeof(unsigned int)));

	// load the image to keep it in the resource-loader
	// this is actually only needed for android
	_LOG_FOOT_
	Ogre::Image* img = new Ogre::Image;
	_LOG_FOOT_
	img->loadRawData(stream,(Ogre::ushort)source_dimensions.x, (Ogre::ushort)source_dimensions.y, Ogre::PF_A8B8G8R8);

	_LOG_FOOT_
	gkTextureLoader* loader = new gkTextureLoader(img,(Ogre::ushort)source_dimensions.x,
			(Ogre::ushort)source_dimensions.y,
			Ogre::PF_A8B8G8R8,
			Ogre::TEX_TYPE_2D,
			0);


	Ogre::TexturePtr ogre_texture = Ogre::TextureManager::getSingleton().create(Rocket::Core::String(16, "%d", texture_id++).CString(),
			DEFAULT_ROCKET_RESOURCE_GROUP, true, loader).staticCast<Ogre::Texture>();
	ogre_texture->load();

	if (ogre_texture.isNull()){
		delete loader;
		return false;
	} else {
		m_loaders.push_back(loader);
	}
#endif



	texture_handle = reinterpret_cast<Rocket::Core::TextureHandle>(new RocketOgre3DTexture(ogre_texture));
	return true;
}

// Called by Rocket when a loaded texture is no longer required.
void RenderInterfaceOgre3D::ReleaseTexture(Rocket::Core::TextureHandle texture)
{
	delete ((RocketOgre3DTexture*) texture);
}

// Returns the native horizontal texel offset for the renderer.
float RenderInterfaceOgre3D::GetHorizontalTexelOffset()
{
	return -render_system->getHorizontalTexelOffset();
}

// Returns the native vertical texel offset for the renderer.
float RenderInterfaceOgre3D::GetVerticalTexelOffset()
{
	return -render_system->getVerticalTexelOffset();
}
void RenderInterfaceOgre3D::BuildProjectionMatrix(Ogre::Matrix4& projection_matrix)
{
	float z_near = -1;
	float z_far = 1;

	projection_matrix = Ogre::Matrix4::ZERO;
	Ogre::RenderWindow* m_window;
	m_window = gkEngine::getSingleton().getActiveScene()->getDisplayWindow()->getRenderWindow();
	// Set up matrices.
	projection_matrix[0][0] = 2.0f / m_window ->getWidth();
	projection_matrix[0][3]= -1.0000000f;
	projection_matrix[1][1]= -2.0f / m_window->getHeight();
	projection_matrix[1][3]= 1.0000000f;
	projection_matrix[2][2]= -2.0f / (z_far - z_near);
	projection_matrix[3][3]= 1.0000000f;
}

void RenderInterfaceOgre3D::onWindowResize()
{
	BuildProjectionMatrix(projMatrix);
}
