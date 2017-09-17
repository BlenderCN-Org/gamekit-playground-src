#Copyright (c) 2010 Xavier Thomas (xat)
#
# ***** BEGIN MIT LICENSE BLOCK *****
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.
#
# ***** END MIT LICENCE BLOCK *****

import bpy,bgl,ctypes
from bgl import *
from bpy.props import *
import os
from . import config
from . import helpers
import runtimedata
from . import NodetreeDebugger

## renderer
import math
import mathutils
####



# Use some of the existing buttons.
#import properties_game
from bl_ui import properties_game
properties_game.RENDER_PT_game_player.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.RENDER_PT_game_performance.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.RENDER_PT_game_shading.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.RENDER_PT_game_sound.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.RENDER_PT_game_stereo.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.WORLD_PT_game_context_world.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.WORLD_PT_game_world.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_game.WORLD_PT_game_mist.COMPAT_ENGINES.add('GAMEKIT_RENDER')

#properties_game.WORLD_PT_game_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.PHYSICS_PT_game_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.PHYSICS_PT_game_collision_bounds.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_game.SCENE_PT_game_navmesh.COMPAT_ENGINES.add('GAMEKIT_RENDER')

del properties_game

from bl_ui import properties_particle
properties_particle.PARTICLE_PT_context_particles.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_emission.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_velocity.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_rotation.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_particle.PARTICLE_PT_boidbrain.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_render.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_draw.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_particle.PARTICLE_PT_children.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_field_weights.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_particle.PARTICLE_PT_force_fields.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_particle.PARTICLE_PT_vertexgroups.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_particle
    
from bl_ui import properties_material
properties_material.MATERIAL_PT_context_material.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_custom_props.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_diffuse.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_flare.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_halo.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_mirror.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_options.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_physics.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_shading.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_shadow.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_specular.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_sss.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_strand.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_transp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_material.MATERIAL_PT_game_settings.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_density.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_integration.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_lighting.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_options.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_shading.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_material.MATERIAL_PT_volume_transp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_material

from bl_ui import properties_data_mesh
properties_data_mesh.DATA_PT_context_mesh.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_custom_props_mesh.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_normals.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_data_mesh.DATA_PT_settings.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_shape_keys.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_data_mesh.DATA_PT_texface.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_uv_texture.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_vertex_colors.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.DATA_PT_vertex_groups.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.MESH_MT_shape_key_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_mesh.MESH_MT_vertex_group_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_data_mesh
 
from bl_ui import properties_texture
properties_texture.TEXTURE_MT_envmap_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_MT_specials.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_blend.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_clouds.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_colors.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_context_texture.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_custom_props.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_distortednoise.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_envmap.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_envmap_sampling.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_image.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_image_mapping.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_image_sampling.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_influence.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_magic.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_mapping.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_marble.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_musgrave.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_texture.TEXTURE_PT_plugin.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_pointdensity.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_pointdensity_turbulence.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_preview.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_stucci.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_voronoi.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_voxeldata.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TEXTURE_PT_wood.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_texture.TextureSlotPanel.COMPAT_ENGINES.add('GAMEKIT_RENDER')
#properties_texture.TextureTypePanel.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_texture


from bl_ui import properties_data_camera
properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_camera_display.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_context_camera.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_custom_props_camera.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_camera.DATA_PT_camera_dof.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_data_camera

from bl_ui import properties_data_lamp
properties_data_lamp.DATA_PT_area.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_context_lamp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_custom_props_lamp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_falloff_curve.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_lamp.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_preview.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_shadow.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_spot.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.DATA_PT_sunsky.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_data_lamp.LAMP_MT_sunsky_presets.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_data_lamp

from bl_ui import properties_world


class WORLD_PT_world(properties_world.WorldButtonsPanel, bpy.types.Panel):
    bl_label = "World"
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw(self, context):
        layout = self.layout
        world = context.world

        row = layout.row()
        row.prop(world, "use_sky_paper")
        row.prop(world, "use_sky_blend")
        row.prop(world, "use_sky_real")

        row = layout.row()
        row.column().prop(world, "horizon_color")
        col = row.column()
        col.prop(world, "zenith_color")
        col.active = world.use_sky_blend
        row.column().prop(world, "ambient_color")



class WORLD_PT_mist(properties_world.WorldButtonsPanel, bpy.types.Panel):
    bl_label = "Mist"
    bl_options = {'DEFAULT_CLOSED'}
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw_header(self, context):
        world = context.world

        self.layout.prop(world.mist_settings, "use_mist", text="")

    def draw(self, context):
        layout = self.layout
        world = context.world

        layout.active = world.mist_settings.use_mist

        split = layout.split()

        col = split.column()
        col.prop(world.mist_settings, "intensity", slider=True)
        col.prop(world.mist_settings, "start")

        col = split.column()
        col.prop(world.mist_settings, "depth")
        col.prop(world.mist_settings, "height")

        layout.prop(world.mist_settings, "falloff")

# Base class for ou panels
class RenderButtonsPanel():
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "render"
    # COMPAT_ENGINES must be defined in each subclass, external engines can add themselves here
    
    @classmethod
    def poll(self, context):
        rd = context.scene.render
        return (rd.engine in self.COMPAT_ENGINES)

# Runtime panel
class RENDER_PT_gamekit_runtime(RenderButtonsPanel, bpy.types.Panel):
    bl_label = "Gamekit Runtime"
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw(self, context):
        layout = self.layout

        scene = context.scene
        gks = scene.gamekit

        col = layout.column()
        
        col.prop(gks, "gk_window_title")
        col.prop(gks, "gk_runtime_exec_path")
        col.prop(gks, "gk_runtime_working_dir")
        col.prop(gks, "gk_log_file")

# Main Gamekit panel
class RENDER_PT_gamekit(RenderButtonsPanel, bpy.types.Panel):
    bl_label = "Gamekit"
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw(self, context):
        gks = context.scene.gamekit
        render = context.scene.render
        layout = self.layout
        
        layout.label("Start Layer:")
        row = layout.row()
        row.prop(gks,"gk_start_layers_1_1",text="")
        row.prop(gks,"gk_start_layers_1_2",text="")
        
        row = layout.row()
        row.prop(gks,"gk_start_layers_2_1",text="")
        row.prop(gks,"gk_start_layers_2_2",text="")
        
        
        split = layout.split()
        
        col = split.column()
        col.operator("scene.gamekit_start_game", text="Start Game", icon='GAME')
        col.operator("scene.gamekit_export_startup_config", text="Save Config", icon='FILE')
        col.operator("gamekit.server") 
        #print ("AS:"+str(runtimedata.RuntimeData.running))
        row = layout.row()
        row.operator("gamekit.asyncio_start")
        row.operator("gamekit.asyncio_stop")
             

            

        split = layout.split()
        
        col = split.column()
        col.label(text="General::")
        col.prop(gks, "gk_render_system", text="")
        col.prop(bpy.data.worlds[0],"gk_activate_pbs",text="activate PBS")
        col.prop(gks, "gk_grab_input")
        col.prop(gks, "gk_matblending")
        col.prop(gks, "gk_verbose")
        
        #col = split.column()
        col.label(text="Animation:")
        col.prop(render, "fps")
        col.prop(gks, "gk_start_frame")
        
        #split = layout.split()
        
        col = split.column()
        col.label(text="Shadows:")
        col.prop(gks, "gk_use_shadows")
        row = col.row()
        row.prop(gks, "gk_shadow_type", text="")
        row.active = gks.gk_use_shadows
        row = col.row()
        row.prop(gks, "gk_shadow_color", text="")
        row.active = gks.gk_use_shadows
        row = col.row()
        row.prop(gks, "gk_far_dist_shadow", text="Far")
        row.active = gks.gk_use_shadows


class RENDER_PT_gamekit_performance(RenderButtonsPanel, bpy.types.Panel):
    bl_label = "Performance"
    COMPAT_ENGINES = {'GAMEKIT_RENDER'}

    def draw(self, context):
        layout = self.layout

        gs = context.scene.game_settings
        gks = context.scene.gamekit
        
        split = layout.split()

        col = split.column()
        col.label(text="Debug:")
        col.prop(gs, "show_debug_properties", text="Properties")
        col.prop(gs, "show_framerate_profile", text="Framerate and Profile")
        col.prop(gs, "show_physics_visualization", text="Physics Visualization")
        col.prop(gks, "gk_debug_physicsaabb")
        col.prop(gks, "gk_debug_sound")

        col = split.column()

        col.label(text="Optimisation:")
        col.prop(gks, "gk_frustrum_culling")
        col.prop(gks, "gk_build_instances")


# Setting for this addon
class GamekitSettings(bpy.types.PropertyGroup):
    pass  
    
    
# prevent the user from chaning the name
class NODE_PT_active_node_generic_gk(bpy.types.Panel):
    bl_space_type = 'NODE_EDITOR'
    bl_region_type = 'UI'
    bl_label = "Node"
#    bl_options = {'HIDE_HEADER'}

    @classmethod
    def poll(cls, context):
        return context.active_node is not None

    def draw(self, context):
        layout = self.layout
        node = context.active_node

        row = layout.row()
        row.enabled=False
        row.prop(node, "name")
        
        layout.prop(node, "label", icon='NODE')
        
###############################################################################
# Declare gamekit as a render engine
gkScreenshot = [0,0,None,False] 

def setData(width,height,raw):
    global gkScreenshot
    gkScreenshot = [width,height,raw,True]
    print ("#######SET DATA "+str(width)+"x"+str(height)+" lenraw:"+str(len(raw)))
    try:
        #runtimedata.RuntimeData.glRegion.tag_redraw()
        pass
    except:
        print("NO GLREGION!")



class GamekitRender(bpy.types.RenderEngine):
    bl_idname = 'GAMEKIT_RENDER'
    bl_label = "Gamekit"
    #We do not implement the render function because Gamekit 
    #is not really a render engine.
    
    def __init__(self):
        print ("DO THE TEX")

    
    def init(self):
        print("INIT RENDERENGINE!")
        print("INIT RENDERENGINE!")
        print("INIT RENDERENGINE!")
        print("INIT RENDERENGINE!")
        print("INIT RENDERENGINE!")
        print("INIT RENDERENGINE!")
    
    def view_update(self, context):
        print("VIEW UPDATE")
        region = context.region  # area.regions[4]
        width = int(region.width)
        height = int(region.height) 
        
        
        
    def view_draw(self, context):
        try:
            print("TexData:"+str(runtimedata.RuntimeData.tex))
        except:
            runtimedata.RuntimeData.tex = Buffer(GL_INT,[1]) 
            glGenTextures(1,runtimedata.RuntimeData.tex)
            print("Created tex:" + str(runtimedata.RuntimeData.tex))

        (loc, rot, projection, near, far, is_ortho) = helpers.extractEye(context)
        
        print ("Location:" + str(loc))
        print ("rot:"+str(rot))
        print ("is_ortho:"+str(is_ortho))        
        print ("near:"+str(near))
        print ("far:"+str(far))
        print("proj:"+str(projection))
        region = context.region  # area.regions[4]
        runtimedata.RuntimeData.glRegion = region
        width = int(region.width)
        height = int(region.height) 

        view = context.region_data
        vmat = view.view_matrix.copy()
        vmat_inv = vmat.inverted()
        pmat = view.perspective_matrix * vmat_inv

        print("VMAT:"+str(vmat))
        print("PMAT:"+str(pmat))
        NodetreeDebugger.requestScreenshot(width,height,vmat, projection, near, far, is_ortho,setData)
        #NodetreeDebugger.requestScreenshot(width,height,loc, rot, projection, near, far, is_ortho,setData)
        
        global gkScreenshot
        (w,h,raw,new) = gkScreenshot
        
        if not new:
            return
        
        if raw is not None: # is raw data-available?
            print("DRAW!")
            gkScreenshot[3]=False
            self.view_draw_image2(context,width,height,raw)
        else:
            print("SRY NO DATA")
    
    def view_draw_image2(self,context,width,height,raw):
#         region = context.region
#         view = context.region_data
# 
#         vmat = view.view_matrix.copy()
#         vmat_inv = vmat.inverted()
#         pmat = view.perspective_matrix * vmat_inv
# 
#         viewport = [region.x, region.y, region.width, region.height]
# 
#         self.update_view(vmat, pmat, viewport)


        size = width * height * 4
 
        buffer = bgl.Buffer(bgl.GL_BYTE, height*width*4,raw)
        
#        runtimedata.RuntimeData.display = (ctypes.c_ubyte * (size))()
#        ctypes.memmove(ctypes.byref(runtimedata.RuntimeData.display, 0), raw, size)

        glGetError()
        glPushAttrib(GL_ALL_ATTRIB_BITS)

        glDisable(GL_DEPTH_TEST)
        glDisable(GL_CULL_FACE)
        glEnable(GL_TEXTURE_2D)

        glClearColor(0, 0, 1, 1)
        glClear(GL_COLOR_BUFFER_BIT)

        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        glMatrixMode(GL_PROJECTION)
        glPushMatrix()
        glLoadIdentity()

        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, runtimedata.RuntimeData.tex[0])
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)

        glBegin(GL_QUADS)
        glColor3f(1.0, 1.0, 1.0)
        glTexCoord2f(0.0, 1.0)
        glVertex3i(-1, -1, 0)
        glTexCoord2f(1.0, 1.0)
        glVertex3i(1, -1, 0)
        glTexCoord2f(1.0, 0.0)
        glVertex3i(1, 1, 0)
        glTexCoord2f(0.0, 0.0)
        glVertex3i(-1, 1, 0)
        glEnd()

        glPopMatrix()
        glMatrixMode(GL_MODELVIEW)
        glPopMatrix()

        glPopAttrib()        
    
    def view_draw_image(self, context, width, height, raw):
        region = context.region
        view = context.region_data

        vmat = view.view_matrix.copy()
        vmat_inv = vmat.inverted()
        pmat = view.perspective_matrix * vmat_inv

        viewport = [region.x, region.y, region.width, region.height]

        self.update_view(vmat, pmat, viewport)
        #raw = [[255, 0, 0, 255]] * (width * height)
        pixel_count = width * height
        print("VIEW DRAW RAW:"+str(len(raw))+" count:"+str(pixel_count*4))
        bitmap = bgl.Buffer(bgl.GL_BYTE, [pixel_count*4], raw)
        # bgl.glBitmap(self.size_x, self.size_y, 0, 0, 0, 0, bitmap)
        scalex = 1.0 / width * 2
        scaley = 1.0 / height * 2
        bgl.glRasterPos2i(0,0)
       # bgl.glWindowPos2i(0,0)
        #bgl.glPixelZoom( scalex,-scaley );
        bgl.glDrawPixels(width, height,
                         # Blender.BGL.GL_BGRA, Blender.BGL.GL_UNSIGNED_BYTE,
                         # 0x80E1 0x1908
                         0x1908, 0x1401,
                         # bgl.GL_BGRA, bgl.GL_UNSIGNED_BYTE,
                         bitmap
                         )

#     def view_draw_image2(self,width,height,raw):
#         global tex
#         glPushAttrib(GL_ALL_ATTRIB_BITS)
# 
#         glDisable(GL_DEPTH_TEST)
#         glDisable(GL_CULL_FACE)
#         glDisable(GL_STENCIL_TEST)
#         glEnable(GL_TEXTURE_2D)
# 
#         glClearColor(0, 0, 1, 1)
#         glClear(GL_COLOR_BUFFER_BIT)
# 
#         glMatrixMode(GL_MODELVIEW)
#         glPushMatrix()
#         glLoadIdentity()
#         glMatrixMode(GL_PROJECTION)
#         glPushMatrix()
#         glLoadIdentity()
# 
#         glActiveTexture(GL_TEXTURE0)
#         glBindTexture(GL_TEXTURE_2D, tex)
#         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, self.width, self.height, 0, GL_RGBA,
#             GL_UNSIGNED_BYTE, raw)
#         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
#         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
# 
#         glBegin(GL_QUADS)
#         glColor3f(1.0, 1.0, 1.0)
#         glTexCoord2f(0.0, 0.0)
#         glVertex3i(-1, -1, 0)
#         glTexCoord2f(1.0, 0.0)
#         glVertex3i(1, -1, 0)
#         glTexCoord2f(1.0, 1.0)
#         glVertex3i(1, 1, 0)
#         glTexCoord2f(0.0, 1.0)
#         glVertex3i(-1, 1, 0)
#         glEnd()
# 
#         glPopMatrix()
#         glMatrixMode(GL_MODELVIEW)
#         glPopMatrix()
# 
#         glPopAttrib()        
    
    def update(self, data, scene):
        print("UPDATE")
    
    def render(self, scene):
        print("RENDER")    

###################################################################################################



def remProperties():
    # bpy.types.Scene.RemoveProperty("gamekit")
    bpy.utils.unregister_class(GamekitRender)
    bpy.utils.unregister_class(GamekitSettings)  
    
    bpy.utils.unregister_class(bpy.types.NODE_PT_active_node_generic_gk)
  #  bpy.utils.register_class(NODE_PT_active_node_generic)
  
    
def addProperties():
    bpy.utils.unregister_class(bpy.types.NODE_PT_active_node_generic)
    bpy.utils.register_class(NODE_PT_active_node_generic_gk)
    
    bpy.utils.register_class(WORLD_PT_world)
    bpy.utils.register_class(WORLD_PT_mist)

    bpy.utils.register_class(RENDER_PT_gamekit)
    bpy.utils.register_class(RENDER_PT_gamekit_runtime)
    bpy.utils.register_class(RENDER_PT_gamekit_performance)

    bpy.utils.register_class(GamekitRender)
    bpy.utils.register_class(GamekitSettings)    
    
    #bpy.types.Scene.PointerProperty(attr="gamekit", type=GamekitSettings, name="Gamekit", description="Gamekit Settings")
    bpy.types.Scene.gamekit = bpy.props.PointerProperty(type=GamekitSettings, name="Gamekit", description="Gamekit Settings")
    
    
    cfg = config.GamekitConfig()
    cfg.load_defaults()
        
    GamekitSettings.gk_runtime_exec_path = bpy.props.StringProperty(
                    name="Runtime",
                    description="Path of the gamekit executable",
                    maxlen = 512,
                    default = cfg.get('runtime'),
                    subtype='FILE_PATH')
    
    GamekitSettings.gk_start_layers_1_1 = bpy.props.BoolVectorProperty(size=5)
    GamekitSettings.gk_start_layers_1_2 = bpy.props.BoolVectorProperty(size=5)
    GamekitSettings.gk_start_layers_2_1 = bpy.props.BoolVectorProperty(size=5)
    GamekitSettings.gk_start_layers_2_2 = bpy.props.BoolVectorProperty(size=5)
                    
                    
    GamekitSettings.gk_runtime_working_dir = bpy.props.StringProperty(
                    name="Working Directory",
                    description="Directory in which to launch the game",
                    maxlen = 512,
                    default = cfg.get('workingdir'),
                    subtype='FILE_PATH')

    GamekitSettings.gk_render_system = bpy.props.EnumProperty(
                    name="Render system",
                    items=(('GL', 'OpenGL', 'gl'),
                    ('D3D9', 'Direct 3D 9', 'd3d9'),
                    ('D3D10', 'Direct 3D 10', 'd3d10'),
                    ('D3D11', 'Direct 3D 11', 'd3d11')),
                    default = 'GL')

    GamekitSettings.gk_log_file = bpy.props.StringProperty(
                    name="Log",
                    description="Name of the log file that will be created in the working directory.",
                    maxlen=512,
                    default="OgreKit.log")

    GamekitSettings.gk_window_title = bpy.props.StringProperty(
                    name="Window title",
                    description="Name of the log file that will be created in the working directory.",
                    maxlen=512,
                    default="OgreKit")

    GamekitSettings.gk_grab_input = bpy.props.BoolProperty(
                    name="Grab input",
                    description="Grab mouse and keyboard event (cause other app to be unusable while game is runing).",
                    default=True)

    GamekitSettings.gk_matblending = bpy.props.BoolProperty(
                    name="Material blending",
                    description="Enable material blending mode by ramp blend.",
                    default=False)

    GamekitSettings.gk_verbose = bpy.props.BoolProperty(
                    name="Verbose",
                    description="Verbose output on the console.",
                    default=False)

    GamekitSettings.gk_start_frame = bpy.props.IntProperty(
                    name="Start frame",
                    min=1,
                    soft_min=1,
                    default =1)

    GamekitSettings.gk_frustrum_culling = bpy.props.BoolProperty(
                    name="Frustrum culling",
                    default=True)

    GamekitSettings.gk_debug_sound = bpy.props.BoolProperty(
                    name="Debug sound",
                    default=False)

    GamekitSettings.gk_debug_physicsaabb = bpy.props.BoolProperty(
                    name="Physics AABB",
                    description="Show physics axis aligned bounding boxes",
                    default=False)

    GamekitSettings.gk_build_instances = bpy.props.BoolProperty(
                    name="Build instances",
                    default=False)

    GamekitSettings.gk_use_shadows = bpy.props.BoolProperty(
                    name="Enable shadows",
                    default=True)

    GamekitSettings.gk_shadow_type = bpy.props.EnumProperty(
                    name="Shadow type",
                    items=(('NONE', 'None', 'n'),
                    ('STENCIL_MODULATIVE', 'Stencil modulative', 'sm'),
                    ('STENCIL_ADDITIVE', 'Stencil additive', 'sa'),
                    ('TEXTURE_MODULATIVE', 'Texture modulative', 'tm'),
                    ('TEXTURE_ADDITIVE', 'Texture additive', 'ta'),
                    ('TEXTURE_MODULATIVE_INTEGRATED', 'Texture modulative integrated', 'tmi'),
                    ('TEXTURE_ADDITIVE_INTEGRATED', 'Texture additive integrated', 'tai')),
                    description = "Ogre shadow technique.",
                    default = 'STENCIL_ADDITIVE')
    
    bpy.types.World.gk_activate_pbs = bpy.props.BoolProperty(name="activate PBS",default=True)

    GamekitSettings.gk_shadow_color = bpy.props.FloatVectorProperty(
                    name="Sadow color",
                    subtype='COLOR',
                    min=0.0,
                    max=1.0,
                    soft_min=0.0,
                    soft_max=1.0,
                    default = (0.0, 0.0, 0.0))

    GamekitSettings.gk_far_dist_shadow = bpy.props.FloatProperty(
                    name="Far distance shadow",
                    min=0.0,
                    max=1.0,
                    soft_min=0.0,
                    soft_max=1.0,
                    default =0.0)

from bl_ui import properties_scene
properties_scene.SCENE_PT_scene.COMPAT_ENGINES.add('GAMEKIT_RENDER')
del properties_scene

from bl_ui import properties_render
properties_render.RENDER_PT_bake.COMPAT_ENGINES.add('GAMEKIT_RENDER')
properties_render.RENDER_PT_dimensions.COMPAT_ENGINES.add('GAMEKIT_RENDER')

del properties_render

