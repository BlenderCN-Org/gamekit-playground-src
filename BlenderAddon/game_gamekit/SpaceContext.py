# snippet to add data to specific space-views by dertom
# alter SpaceContext-PropertyGroup to fit your needs
# import the SpaceContext to your script: import SpaceContext
# when needed get the space-context (your SpaceContext-PropertyGroup) via:
# currentSpaceContext = SpaceContext.retrieveSpaceContext(context)
# 
# For now you have to press a button to c

import bpy

import asynciobridge
from asyncio import coroutine, sleep, Task, wait_for
import runtimedata

@coroutine
def createSpaceProperty(signature):
    print("TRY TO CREATE!")
    world = bpy.data.worlds[0] 
    if world.spaceMapping.find(signature)==-1:
        data = world.spaceMapping.add()
        data.name = signature
    print("FINISHED CREATING SIGNATURE:"+signature)
    
def hookChangeTreeBrowsing(self,context):
    currentTree = context.space_data.node_tree
    spaceCtx = getCurrentSpaceContext(context)
    if spaceCtx and spaceCtx.logicTreeBrowsing:
        spaceCtx.logicTreeName = currentTree.name
        checkDebugTreeBrowsingState(self,context)

# make sure no invalid combinations can be made
def checkDebugTreeBrowsingState(self,context):
    print("CHECK STATE")
    spaceCtx = getCurrentSpaceContext(context)        
    currentTree = bpy.data.node_groups[spaceCtx.logicTreeName]
    if spaceCtx.debugObject not in currentTree.debugPossibleObjects:
        try:
            spaceCtx.debugObject = currentTree.debugPossibleObjects[0].name
        except:
            spaceCtx.debugObject = ""
    


def hookChangedDebugObject(self,context):
    currentTree = context.space_data.node_tree
    
    spaceCtx = getCurrentSpaceContext(context)
    
    if spaceCtx:
        objName = spaceCtx.debugObject 
        if objName!="":
            spaceCtx.debugTreeHandle = "debug_"+currentTree.name+"_"+objName
            
            
            
#             # create a copy of the treenode, so you can show the logictree-data for a specfic object
#             if spaceCtx.debugTreeHandle not in bpy.data.node_groups:
#                 debugClone = currentTree.copy()
#                 debugClone.name=spaceCtx.debugTreeHandle
#                 print ("CREATED NEW NODETREE:"+spaceCtx.debugTreeHandle)
# 
# 
#             if spaceCtx.debugTreeHandle in runtimedata.RuntimeData.debugSession:
#                 data = runtimedata.RuntimeData.debugSession[spaceCtx.debugTreeHandle]
#                 print ("Data of "+spaceCtx.debugTreeHandle+": "+(str(data)))

# The spacemapper: define per spacedata you want to keep here:
class SpaceContext(bpy.types.PropertyGroup):
#    debugObject = bpy.props.StringProperty(update=hookChangedDebugObject)
    debugObject = bpy.props.StringProperty()
    debugTreeHandle = bpy.props.StringProperty()
    logicTreeBrowsing = bpy.props.BoolProperty(update=hookChangeTreeBrowsing)
    logicTreeName = bpy.props.StringProperty(update=checkDebugTreeBrowsingState)

# persist the spaceMapping-data with the world-object


class SpaceContextOp(bpy.types.Operator):
    
    """Tooltip"""
    bl_idname = "utils.spacecontext"
    bl_label = "Create SpaceContext"

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def execute(self, context):
        spaceSignature = getCurrentSpaceSignature(context)
        # check if there is already data for this space
        world = bpy.data.worlds[0] 
        pos = world.spaceMapping.find(spaceSignature)
        data = None
        if pos!=-1: # we found data
            data = world.spaceMapping[pos]
        else: # first time call: we create the mapping to this space
            data = world.spaceMapping.add()
            data.name = spaceSignature
    
        return {'FINISHED'} 


def getCurrentSpaceContext(context):
    signature = getCurrentSpaceSignature(context)
    world = bpy.data.worlds[0] 
    pos = world.spaceMapping.find(signature)
    if pos == -1:
#         print("NO CONTEXT FOUND FOR SIG:"+signature)
        Task(createSpaceProperty(signature))
        return None
    else:
#         print("FOUND CONTEXT FOR SIG:"+signature)
        return world.spaceMapping[pos]
                
def getCurrentSpaceSignature(context):
    current_space = context.space_data
    
    for screenIdx in range(0,len(bpy.data.screens)):
        screen = bpy.data.screens[screenIdx]
        
        for areaIdx in range(0,len(screen.areas)):
            area = screen.areas[areaIdx]
            
            for spaceIdx in range(0,len(area.spaces)):
                space = area.spaces[spaceIdx]
                if (space == current_space):
                    name = str(screenIdx)+str(areaIdx)+str(spaceIdx) 
                    return name

def register():   
    
    
    
    try:
        bpy.utils.unregister_class(SpaceContext)                     
    except:
        pass
    bpy.utils.register_class(SpaceContext)
    bpy.types.World.spaceMapping = bpy.props.CollectionProperty(type=SpaceContext)
    bpy.utils.register_class(SpaceContextOp)
        
def unregister():
    bpy.utils.unregister_class(SpaceContextOp)
    bpy.utils.unregister_class(SpaceContext)

    