#import sys
#sys.path.append("/home/ttrocha/_dev/extprojects/blender-git/build_linux/bin/2.76/scripts/addons/game_gamekit")

import bpy,re
from bpy.types import NodeTree, Node, NodeSocket
import runtimedata
import SpaceContext 
from bpy.app.handlers import persistent
import traceback
from asyncio import coroutine, sleep, Task, wait_for
import asynciobridge
import NodetreeDebugger
import bpy.utils.previews, os
from bpointers import *
import runtime




# Implementation of custom nodes from Python

class DefaultMap(bpy.types.PropertyGroup):
    value=bpy.props.StringProperty()

try: 
    bpy.utils.unregister_class(bpy.types.DefaultMap)
except:
    pass
bpy.utils.register_class(DefaultMap)

preview_collections = {}


def enum_previews_from_directory_items(self, context):
    """EnumProperty callback"""
    enum_items = []

    if context is None:
        return enum_items

    space = context.space_data
    if space is None:
        return enum_items;
    
    nodetree = space.node_tree
    directory = nodetree.my_previews_dir

    # Get the preview collection (defined in register func).
    pcoll = getPCOLL(directory)

    if pcoll and directory == pcoll.my_previews_dir:
        return pcoll.my_previews

    print("Scanning directory: %s" % directory)

    if directory and os.path.exists(directory):
        # Scan the directory for png files
        image_paths = []
        
        list_files = os.listdir(directory)
        list_files.sort()
        for fn in list_files:
            if fn.lower().endswith(".png"):
                image_paths.append(fn)

        for i, name in enumerate(image_paths):
            # generates a thumbnail preview for a file.
            filepath = os.path.join(directory, name)
            thumb = pcoll.load(filepath, filepath, 'IMAGE')
            enum_items.append((name, name, "", thumb.icon_id, i))
            #print ( ("Create (%s,%s,'',%s,%i)") % (name,name,str(thumb.icon_id),i) )

    pcoll.my_previews = enum_items
    pcoll.my_previews_dir = directory
    return pcoll.my_previews

def getPCOLL(name):
    global preview_collections
    try:
        result = preview_collections[name]
        return result
    except:    
        pcoll = bpy.utils.previews.new()
        pcoll.my_previews_dir = ""
        pcoll.my_previews = ()
        preview_collections[name]=pcoll
        return pcoll


@coroutine
def createDebugTree(treename,debug_treename):
    print("CREATE:" +debug_treename+" from:"+treename)
    if treename[:6]=="debug_" or treename[:6]=="zz____":
        print ("Tried to create invalid debug-tree "+debug_treename)
        return
    
    if (debug_treename not in bpy.data.node_groups):
        tree = bpy.data.node_groups[treename]
        debugClone = tree.copy()
        debugClone.name=debug_treename
    
        frame = debugClone.nodes.new("NodeFrame")
        frame.label="DEBUG " + debug_treename
        frame.label_size=64
        frame.use_custom_color=True
        frame.color=(1.0,0.2,0.2)
        
        for node in debugClone.nodes:
            if not node.parent:
                node.parent=frame
    
    

runtimedata.RuntimeData.debugRunning = False

#TODO: Pack all this data in a PropertyGroup and put it in the node-tree
bpy.types.Scene.showOptions=bpy.props.BoolProperty(name="Show Options")
bpy.types.NodeTree.show_options=bpy.props.BoolProperty(name="Show Options")
bpy.types.NodeTree.debugPossibleObjects=bpy.props.CollectionProperty(type=bpy.types.DefaultMap) # save all objects that currently use this tree in debugmode
bpy.types.NodeTree.my_previews_dir = bpy.props.StringProperty(
        name="Preview Image Path",
        subtype='DIR_PATH',
        default=""
)


bpy.types.Node.enabled__=bpy.props.BoolProperty(default=True)
bpy.types.Node.bakColor=bpy.props.FloatVectorProperty()
bpy.types.Node.cover_mode = bpy.props.BoolProperty(default=False)
bpy.types.Node.my_previews = bpy.props.EnumProperty(
        items=enum_previews_from_directory_items,
)

 
bpy.types.Object.gk_nodetree=bpy.props.StringProperty()
bpy.types.World.gk_nodetree = bpy.props.StringProperty() # field for browsing-mode
bpy.types.World.gk_nodetree_browsing = bpy.props.BoolProperty() # browse the trees or select lopic for current obj
bpy.types.World.gk_nodetree_show_ioSockets = bpy.props.BoolProperty(default=False)
bpy.types.World.gk_validNodeTrees = bpy.props.CollectionProperty(type=bpy.types.DefaultMap)


def singlestepToggle(self,context):
    if self.singleStep:
        print("START SINGLESTEP")
        NodetreeDebugger.singleStep("start")
        pass
    else:
        NodetreeDebugger.singleStep("stop")
        print("STOP SINGLESTEP")
        pass

bpy.types.World.singleStep=bpy.props.BoolProperty(update=singlestepToggle)



def removeDebugTrees():
    for nodetree in bpy.data.node_groups:
        if nodetree.name[:6]=="debug_" or nodetree.name[:6]=="zz____":
            print("Removing debug-tree:" + nodetree.name)
            try:
                bpy.data.node_groups.remove(nodetree)
                print ("success")
            except:
                print("fail")
                #on fail at least rename it, so we can start fresh
                if nodetree.name[:6]=="debug_":
                    nodetree.name="zz____"+nodetree.name

def hookDebugMode(self,context):
    world = bpy.data.worlds[0]
    if (world.gk_nodetree_debugmode):
        removeDebugTrees()
        runtimedata.RuntimeData.debugSession={}
        runtimedata.RuntimeData.debugRunning=True 
        asynciobridge.startAsyncServer()
        NodetreeDebugger.startDebugPolling()
        
        print("SET RUNTIME-DEBUG-Data")
    else:
        NodetreeDebugger.stopDebugPolling()
        runtimedata.RuntimeData.debugRunning=False
        
bpy.types.World.gk_nodetree_debugmode = bpy.props.BoolProperty(default=False, update=hookDebugMode)

@persistent
def load_handler(dummy):
    
#     print("Load Handler:", bpy.data.filepath)
#     if (hasattr(runtimedata.RuntimeData,"currentFile") and runtimedata.RuntimeData.currentFile==bpy.data.filepath):
#         print("BLOCK!")
#         return
        
    runtimedata.RuntimeData.currentFile = bpy.data.filepath
    bpy.data.worlds[0].gk_nodetree_debugmode=False
    # try to delete debug-trees
    removeDebugTrees()
    updateValidNodeTrees()
    
    
    runtimedata.RuntimeData.asycBridgeRunning=False    
    asynciobridge.startAsyncServer()
    
    runtimedata.RuntimeData.runtime = runtime.BlenderRuntime()
    runtimedata.RuntimeData.runtime.processNodetrees()

    def getLogic(node):
        return runtimedata.RuntimeData.runtime.getLogic(node.bpid)

    bpy.types.Node.logic = property(getLogic)
    
    def getNodetreePointer(nt):
        return BPointer.getByID(nt.bpid)
    
    bpy.types.NodeTree.logic = property(getNodetreePointer)
        
    # init the elements. IMPORTANT, because the elements interested will add themselfs to the update-queue
    runtimedata.RuntimeData.runtime.callStartup() 
    @coroutine
    def runtimeCaller():
        if hasattr(runtimedata.RuntimeData,"runtime_running") and runtimedata.RuntimeData.runtime_running:
            return

        runtimedata.RuntimeData.runtime_running = True

        runtimedata.RuntimeData.runtime.tick()
        yield from sleep(0.1)
        runtimedata.RuntimeData.runtime.tick()


        while runtimedata.RuntimeData.runtime_running:
            print ("RUNTIME DATA-TICK")
            runtimedata.RuntimeData.runtime.tick()
            yield from sleep(3) 
        print("LEFT TICK-LOOP")
            
    Task(runtimeCaller())
    
bpy.app.handlers.load_post.append(load_handler)

def updateValidNodeTrees():
    print ("UPDATE VALID TREES")
    validTrees = bpy.data.worlds[0].gk_validNodeTrees
    i = 0
    for validTree in validTrees:
        if validTree.name not in bpy.data.node_groups: # a fromer valid tree disappeared? remove it
            validTrees.remove(i)
        i = i + 1
            
    for tree in bpy.data.node_groups: # search for valid groups that are not yet added
        if tree.name[:6]!="debug_" and tree.name[:6]!="zz____" and validTrees.find(tree.name)==-1:
            addValidTree(tree.name)
            
def addValidTree(validTreeName):
    validTrees = bpy.data.worlds[0].gk_validNodeTrees
    newItem = validTrees.add()
    newItem.name = validTreeName
    print("ADDED VALID TREE:" + newItem.name)
            




class SocketGroup(bpy.types.PropertyGroup):
    name = bpy.props.StringProperty()
    extName = bpy.props.StringProperty()
    isInput = bpy.props.BoolProperty()

try:
    bpy.utils.unregister_class(bpy.types.SocketGroup)
except:
    pass
bpy.utils.register_class(SocketGroup)   

class /*name|post#Operator:treename*/GamekitOperator/*endname*/(bpy.types.Operator):
    """Tooltip"""
    bl_idname = /*name|l,dq,pre#object.,post#_operator,set#operator:treename*/"object.simple_operator"/*endname*/
    bl_label = "Create Nodetree"

    op_treename = bpy.props.StringProperty(name="Treename:")

    @classmethod
    def poll(cls, context):
        return context.active_object is not None

    def execute(self, context):
        scene = context.scene
        name = self.op_treename
        addValidTree(name)
        bpy.data.node_groups.new(name,/*name|dq:treename*/"Gamekit"/*endname*/)
        return {'FINISHED'}
    

    def invoke(self, context, event):
        wm = context.window_manager
        result = wm.invoke_props_dialog(self)
        return result    

class /*name|post#UpdateTreeOperator:treename*/GamekitUpdateTreeOperator/*endname*/(bpy.types.Operator):
    """update trees"""
    bl_idname = /*name|l,dq,pre#object.,post#_updatetrees_operator,set#operator:treename*/"object.gamkit_updatetrees_operator"/*endname*/
    bl_label = "Update Tree-Signatur"


    @classmethod
    def poll(cls, context):
        return True

    def execute(self, context):
        for tree in bpy.data.node_groups.values():
            if tree.bl_idname==/*name|dq:treename*/"Gamekit"/*endname*/:
                print ("Gamekit-Nodetree:"+tree.name)
                # process nodes
                for node in tree.nodes.values():
                    try:
                        node.doUpdate()
                    except:
                        pass
                    
        return {'FINISHED'}

        
try:
    bpy.utils.unregister_class(bpy.types./*name|post#UpdateTreeOperator:treename*/GamekitUpdateTreeOperator/*endname*/)    
except:
    pass    
bpy.utils.register_class(/*name|post#UpdateTreeOperator:treename*/GamekitUpdateTreeOperator/*endname*/)

class /*name:treename*/Gamekit/*endname*/AddSocketOperator(bpy.types.Operator):
    """Tooltip"""
    bl_idname = /*name|l,dq,post#.addsocket:treename*/"gamekit.addsocket"/*endname*/
    bl_label = "add socket to node"

    op_inputSocket = bpy.props.BoolProperty(name="InputSocket")
    op_socketName = bpy.props.StringProperty(name="SocketName:")
    op_type = bpy.props.EnumProperty(items=[("bool","bool","bool"),("string","string","string"),("float","float","float"),("int","int","int")
                                            ,("object","object","object"),("vec3","vec3","vec3"),("color","color","color")])
    @classmethod
    def poll(cls, context):
        space = context.space_data
        return space.type == 'NODE_EDITOR'

    def execute(self, context):
        space = context.space_data
        node_tree = space.node_tree
        node_active = context.active_node
        
        print ("ActiveNode:"+node_active.name)
        
        socketType = "NodeTreeBOOL"
        if self.op_type == "float":
            socketType = "NodeTreeFLOAT"
        elif self.op_type == "int":
            socketType = "NodeTreeINT"
        elif self.op_type == "string":
            socketType = "NodeTreeSTRING"
        elif self.op_type == "object":
            socketType = "NodeTreeObject"
        elif self.op_type == "vector":
            socketType = "NodeTreeVEC3"
        elif self.op_type == "color":
            socketType = "NodeTreeCOLOR"
        
        if (self.op_inputSocket):
            node_active.inputs.new(socketType,self.op_socketName)
            newS = node_active.NEW_IN_SOCKETS.add()
            newS.isInput=True
            newS.name=self.op_socketName
        else:
            node_active.outputs.new(socketType,self.op_socketName)
            newS = node_active.NEW_OUT_SOCKETS.add()
            newS.isInput=False
            newS.name=self.op_socketName
        
        self.op_type="bool"
        self.op_socketName="";
                    
        return {'FINISHED'} 
    

    def invoke(self, context, event):
        wm = context.window_manager
        result = wm.invoke_props_dialog(self)
        return result
        
try:
    bpy.utils.unregister_class(bpy.types./*name:treename*/Gamekit/*endname*/AddSocketOperator)    
except:
    pass    
bpy.utils.register_class(/*name:treename*/Gamekit/*endname*/AddSocketOperator)



class /*name:treename*/Gamekit/*endname*/RemoveSocketOperator(bpy.types.Operator):
    """Tooltip"""
    bl_idname = "/*name|l:treename*/gamekit/*endname*/.removesocket"
    bl_label = "remove socket to node"

    isInputSocket = bpy.props.BoolProperty();
        
    @classmethod
    def poll(cls, context):
        space = context.space_data
        return space.type == 'NODE_EDITOR'

    def execute(self, context):
        space = context.space_data
        node_tree = space.node_tree
        node_active = context.active_node

        print ("Removed:"+node_active.inputSockets)
        
        if (self.isInputSocket):
            node_active.inputs.remove(node_active.inputs[node_active.inputSockets])
            node_active.NEW_IN_SOCKETS.remove(node_active.NEW_SOCKETS.find(node_active.inputSockets))
        else:
            node_active.outputs.remove(node_active.outputs[node_active.outputSockets])
            node_active.NEW_OUT_SOCKETS.remove(node_active.NEW_SOCKETS.find(node_active.outputSockets))
     
            
        
        return {'FINISHED'} 
    
        
try:
    bpy.utils.unregister_class(bpy.types./*name:treename*/Gamekit/*endname*/RemoveSocketOperator)    
except:
    pass    
bpy.utils.register_class(/*name:treename*/Gamekit/*endname*/RemoveSocketOperator)

class /*name:treename*/Gamekit/*endname*/SelectAllOperator(bpy.types.Operator):
    """Tooltip"""
    bl_idname = "/*name|l:treename*/Gamekit/*endname*/.select_all_of_this_type"
    bl_label = "Select all nodes of this type"   
    bl_space_type = 'NODE_EDITOR'
    bl_region_type = 'UI'
        
    @classmethod
    def poll(cls,context):
        return context.active_node is not None
    
    def execute(self,context):
        nodetree = context.space_data.node_tree
        current_node = context.active_node
        current_type = current_node.bl_idname
        
        for node in nodetree.nodes:
            if node.bl_idname == current_type:
                node.select=True
            else:
                node.select=False
                
        return {'FINISHED'} 
                
try:
    bpy.utils.unregister_class(bpy.types./*name:treename*/Gamekit/*endname*/SelectAllOperator)    
except:
    pass    
bpy.utils.register_class(/*name:treename*/Gamekit/*endname*/SelectAllOperator)   


class /*name:treename*/Gamekit/*endname*/ToggleEnableOperator(bpy.types.Operator):
    """Tooltip"""
    bl_idname = "/*name|l:treename*/gamekit/*endname*/.enable_nodes"
    bl_label = "(de)activate all selected nodes"   
    bl_space_type = 'NODE_EDITOR'
    bl_region_type = 'UI'
    
    enableMode = bpy.props.BoolProperty()
    
    @classmethod
    def poll(cls,context):
        return context.active_node is not None
    
    def execute(self,context):
        for node in context.selected_nodes:
            if (self.enableMode and not node.enabled__):
                node.use_custom_color=True
                node.color=node.bakColor
            elif (not self.enableMode and node.enabled__):
                node.use_custom_color=True
                node.bakColor = node.color
                node.color = (1,0,0)
                
            node.enabled__ = self.enableMode
            
        return {'FINISHED'}
try:
    bpy.utils.unregister_class(bpy.types.GamekitToggleEnableOperator)    
except:
    pass    
bpy.utils.register_class(GamekitToggleEnableOperator)  


class GamekitToggleOperator(bpy.types.Operator):
    """Tooltip"""
    bl_idname = "gamekit.toggler"
    bl_label = "toggle some values"   
    bl_space_type = 'NODE_EDITOR'
    bl_region_type = 'UI'
    
    mode = bpy.props.IntProperty()
    # 0 = toggle ioSocket
    
    @classmethod
    def poll(cls,context):
        return True
    
    def execute(self,context):
        if self.mode == 0:
            bpy.data.worlds[0].gk_nodetree_show_ioSockets = not bpy.data.worlds[0].gk_nodetree_show_ioSockets
             
        return {'FINISHED'}

    def draw(self,context):
        layout = self.layout
        layout.props(self,"mode",text="MIXI")
try:
    bpy.utils.unregister_class(bpy.types.GamekitToggleOperator)    
except:
    pass    
bpy.utils.register_class(GamekitToggleOperator) 



######### PANEL ##########################################################################
    

class /*name|post#Panel:treename*/LayoutDemoPanel/*endname*/(bpy.types.Panel):
    """Creates a Panel in the scene context of the properties editor"""
    bl_label = /*name|dq,post#_NodeTree:treename*/"Gamekit Nodetree"/*endname*/
    #bl_idname = "SCENE_PT_layout"
    bl_space_type = 'NODE_EDITOR'
    bl_region_type = 'UI'
    bl_context = "object"
    
    def onupdate(self, context):
        return None   
    
    @classmethod
    def poll(cls,ctx):
        # only show panel if current tree is of type /*name|q:treename*/'CustomTreeType'/*endname*/
        tree = ctx.space_data.node_tree
        return not tree or tree.rna_type.identifier == /*name|q:treename*/'CustomTreeType'/*endname*/

    def draw(self, context):
        layout = self.layout

        spaceContext = SpaceContext.getCurrentSpaceContext(context)
        scene = context.scene
        obj = context.object
        tree = None
        if spaceContext and spaceContext.logicTreeBrowsing and spaceContext.logicTreeName!="" and bpy.data.node_groups.find(spaceContext.logicTreeName)!=-1:
            tree = bpy.data.node_groups[spaceContext.logicTreeName]
        else:
            tree = context.space_data.node_tree
        
        # Create a simple row.

        # Different sizes in a row
        layout.label(text="Tools:")

        world = bpy.data.worlds[0]

        layout.prop(world,"gk_nodetree_debugmode","Node-Tree debugmode")

        #bpy.ops.utils.spacecontext('EXEC_DEFAULT')
        
        #signatur = SpaceContext.getCurrentSpaceSignature(context)

        
        layout.prop(world,"singleStep","SingleStep debugging")
        
        if (world.singleStep):
            layout.operator("debugger.singlestep")
        
        if spaceContext:
            layout.prop(spaceContext,"logicTreeBrowsing","Node-Tree browsing")
        else: 
            layout.label(text="SIGN")
            #bpy.ops.utils.spacecontext('INVOKE_DEFAULT')
            layout.operator("utils.spacecontext")

        searchName = "Obj Logic:"
        if spaceContext and spaceContext.logicTreeBrowsing:
            searchName="Browse:"  
            #for now go back to this view, using gk_validNodeTrees is not stable enough
            layout.prop_search(spaceContext,"logicTreeName",bpy.data,"node_groups")
#            layout.prop_search(spaceContext,"logicTreeName",bpy.data.worlds[0],"gk_validNodeTrees",searchName)
#            layout.template_ID(context.space_data, "node_tree", new="node.new_node_tree")
        else:
            if obj:
                layout.prop_search(obj,"gk_nodetree",bpy.data,"node_groups",searchName)
                #layout.prop_search(obj,"gk_nodetree",bpy.data.worlds[0],"gk_validNodeTrees",searchName)

        if world.gk_nodetree_debugmode and spaceContext:
            if spaceContext.logicTreeBrowsing and tree  :
                layout.prop_search(spaceContext,"debugObject",tree,"debugPossibleObjects")
            else:
                layout.label("debugObject: "+obj.name)
        else:
            layout.label("NO DEBUGMODE AND SPCTX")

        layout.operator("/*name|l:treename*/Gamekit/*endname*/.select_all_of_this_type")
        layout.operator("gamekit.enable_nodes","activate selected nodes").enableMode=1
        layout.operator("gamekit.enable_nodes","deactivate selected nodes").enableMode=0
        layout.operator("scene.gamekit_start_game", text="Start Game", icon='GAME')  
                       
        layout.prop(scene,"showOptions")
        if scene.showOptions:
            layout.operator(/*name|l,dq,pre#object.,post#_operator,set#operator:treename*/"object.simple_operator"/*endname*/)
            layout.operator(/*name|l,dq,pre#object.,post#_updatetrees_operator,set#operator:treename*/"object.gamkit_updatetrees_operator"/*endname*/)
            layout.operator("/*name|l:treename*/Gamekit/*endname*/.addsocket")
            layout.operator("gamekit.toggler",text="show/hide ioSocket-Flag").mode=0
            if tree:
                layout.prop(tree, "my_previews_dir")
                
            row = layout.row()
            row.operator("gamekit.asyncio_start")
            row.operator("gamekit.asyncio_stop")
            
            layout.operator("gamekit.removeprintdebug")



/*block:nodetree*/
# Derived from the NodeTree base type, similar to Menu, Operator, Panel, etc.
class /*name:treename*/MyCustomTree/*endname*/(NodeTree):
    # Description string
    '''A custom node tree type that will show up in the node editor header'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    bl_idname = /*name|q:treename*/'CustomTreeType'/*endname*/
    # Label for nice name display
    bl_label = /*name|q:label*/'Custom Node Tree'/*endname*/
    # Icon identifier
    bl_icon = /*name|dq:icon*/'NODETREE'/*endname*/

    @classmethod
    def get_from_context(cls, context):
        spaceContext = SpaceContext.getCurrentSpaceContext(context)
        world = bpy.data.worlds[0]
        browseMode = spaceContext and spaceContext.logicTreeBrowsing
        ob = context.object
        if browseMode:
            ob = world
        
        space_data = context.space_data
        
        if ob is not None:
            #obj = bpy.data.objects[ob.name]
            if not browseMode:
                if "gk_nodetree" in ob and ob.gk_nodetree!="":
                    treename = ob["gk_nodetree"]
                    
                    if runtimedata.RuntimeData.debugRunning and treename[:6]!="debug_":
                        debug_treename = "debug_"+treename+"_"+ob.name
                        if debug_treename not in bpy.data.node_groups:
                            print("CALL:"+treename+" - "+debug_treename)
                            Task(createDebugTree(treename,debug_treename))
                            print ("CREATED NEW NODETREE:"+debug_treename)
                        treename = debug_treename
                            
                    if treename in bpy.data.node_groups:
                        tree = bpy.data.node_groups[treename]
                        return tree, tree, ob
                    else:
                        return space_data.node_tree,space_data.node_tree,ob 
                return None,None,None
            else:
                tree = None
                if spaceContext:
                    if spaceContext.logicTreeName=="" or bpy.data.node_groups.find(spaceContext.logicTreeName)==-1:
                        return None,None,None
                    
                    tree = bpy.data.node_groups[spaceContext.logicTreeName]
                    treename = spaceContext.logicTreeName
                    
                    if not runtimedata.RuntimeData.debugRunning:
                        #print ("NOT IN DEBUGMODE!")
                        return tree,tree,ob 
                    
                    if runtimedata.RuntimeData.debugRunning and spaceContext.debugObject!="": 
                        #ob = bpy.data.objects[spaceContext.debugObject]
                        debug_treename = "debug_"+treename+"_"+spaceContext.debugObject
                        print ("BROWSING debugtree:" + debug_treename)
                        if debug_treename not in bpy.data.node_groups:
                            print("TRY TO CREATE!")
                            Task(createDebugTree(treename,debug_treename))
                        
                        print("f1")
                        if debug_treename in bpy.data.node_groups:
                            print("f2")
                            dbgtree = bpy.data.node_groups[debug_treename]
                            return dbgtree, dbgtree, ob
                        else:
                            print("f3")
                            return tree,tree,ob                         
                    print("f4")    
                    return tree,tree,ob   
                return space_data.node_tree,space_data.node_tree,ob
            return context.space_data.node_tree, context.space_data.node_tree, None

/*endblock:nodetree*/ 


class NodeTreeObject(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display
    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.BoolProperty()
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        layout.label(text)        
            
        if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
            layout.prop(self,"isIOSocket")
            if (self.isIOSocket):
                layout.prop(self,"ioSocketName")
    # Socket color
    def draw_color(self, context, node):
        if self.isIOSocket:
            return (1.0, 0.1, 0.1, 0.9)
        else:
            return (0.3,0.55,0.19,1.0)

# Custom socket type
class NodeTreeBOOL(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display
    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.BoolProperty()
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        if self.is_output or self.is_linked or not self.dataVisible:
            layout.label(text)        
        else:
            layout.prop(self, "value",text=text)
            
        if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
            layout.prop(self,"isIOSocket")
            if (self.isIOSocket):
                layout.prop(self,"ioSocketName")
    # Socket color
    def draw_color(self, context, node):
        if bpy.data.worlds[0].gk_nodetree_debugmode:
            if self.value:
                return (0.0,1.0,0.0,1.0)
            else:
                return (1.0,.0,0.0,1.0)
                
        else:
            if self.isIOSocket:
                return (1.0, 0.1, 0.1, 0.9)
            else:
                return (0.5,0.8,0.0,1.0)
        
class NodeTreeSTRING(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display
    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.StringProperty()
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        if bpy.data.worlds[0].gk_nodetree_debugmode:
            layout.prop(self,"value",text=text)
        else:        
            if self.is_output or self.is_linked or not self.dataVisible:
                layout.label(text)        
            else:
                layout.prop(self, "value",text=text)
            
        if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
            layout.prop(self,"isIOSocket")
            if (self.isIOSocket):
                layout.prop(self,"ioSocketName")
    # Socket color
    def draw_color(self, context, node):
        if self.isIOSocket:
            return (1.0, 0.1, 0.1, 0.9)
        else:
            return (0.3,0.2,0.6,1.0)        

class NodeTreeINT(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display

    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.IntProperty()
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        if bpy.data.worlds[0].gk_nodetree_debugmode:
            layout.prop(self,"value",text=text)
        else:
            if self.is_output or self.is_linked or not self.dataVisible:
                layout.label(text)        
            else:
                layout.prop(self, "value",text=text)
            
        if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
            layout.prop(self,"isIOSocket")
            if (self.isIOSocket):
                layout.prop(self,"ioSocketName")
    # Socket color
    def draw_color(self, context, node):
        if self.isIOSocket:
            return (1.0, 0.1, 0.1, 0.9)
        else:
            return (0.0,0.3,0.8,1.0)

class NodeTreeFLOAT(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display
    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.FloatProperty(precision=3)
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        if bpy.data.worlds[0].gk_nodetree_debugmode:
            layout.prop(self,"value",text=text)
        else:
            if self.is_output or self.is_linked or not self.dataVisible:
                if self.isIOSocket and self.ioSocketName!="":
                    layout.label(self.ioSocketName)
                else:
                    layout.label(text)        
            else:
                if (self.isIOSocket and self.ioSocketName!=""):
                    layout.prop(self, "value",text=self.ioSocketName)
                else:
                    layout.prop(self, "value",text=text)
                
            if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
                layout.prop(self,"isIOSocket")
                if (self.isIOSocket):
                    layout.prop(self,"ioSocketName")
    # Socket color
    def draw_color(self, context, node):
        if self.isIOSocket:
            return (1.0, 0.1, 0.1, 0.9)
        else:
            return (0.3,0.6,0.9,1.0)
        
class NodeTreeVec3(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display
    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.FloatVectorProperty(size=3)
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        if bpy.data.worlds[0].gk_nodetree_debugmode:
            layout.prop(self,"value",text=text)
        else:        
            if self.is_output or self.is_linked or not self.dataVisible:
                layout.label(text)        
            else:
                layout.prop(self, "value",text=text)
                
            if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
                layout.prop(self,"isIOSocket")
                if (self.isIOSocket):
                    layout.prop(self,"ioSocketName")
    # Socket color
    def draw_color(self, context, node):
        if self.isIOSocket:
            return (1.0, 0.1, 0.1, 0.9)
        else:
            return (0.0,0.3,0.8,1.0)
        

class NodeTreeCOLOR(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    # Label for nice name display
    dataVisible = bpy.props.BoolProperty(default=True)
    value = bpy.props.FloatVectorProperty(size=4,subtype="COLOR",min=0.0,max=1.0,precision=3)
    isIOSocket = bpy.props.BoolProperty()
    ioSocketName = bpy.props.StringProperty()
    # Optional function for drawing the socket input value
    def draw(self, context, layout, node, text):
        if self.is_output or self.is_linked or not self.dataVisible:
            layout.label(text)        
        else:
            layout.prop(self, "value",text=text)
            
        if bpy.data.worlds[0].gk_nodetree_show_ioSockets:
            layout.prop(self,"isIOSocket")
            if (self.isIOSocket):
                layout.prop(self,"ioSocketName")
        # Socket color
    def draw_color(self, context, node):
        if self.isIOSocket:
            return (1.0, 0.1, 0.1, 0.9)
        else:
            return (0.25,0.9,0.3,1.0)        

# Mix-in class for all custom nodes in this tree type.
# Defines a poll function to enable instantiation.
class /*name:treename*/MyCustomTree/*endname*/Node:
    @classmethod
    def poll(cls, ntree):
        return ntree.bl_idname == /*name|q:treename*/'CustomTreeType'/*endname*/


################################# NODE ############################################################


/*block:node*/
# Derived from the Node base type.
class /*name:nodeName*/MyCustomNode/*endname*/(Node, /*name:treename*/MyCustomTree/*endname*/Node):
    # === Basics ===
    # Description string
    '''/*name:description*/A custom node/*endname*/'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    bl_idname = /*name|q:nodeName*/'CustomNodeType'/*endname*/
    # Label for nice name display
    bl_label = /*name|q:label*/'Custom Node'/*endname*/
    # Icon identifier
    bl_icon = /*name|q:icon*/'SOUND'/*endname*/

    # === Custom Properties ===
    # These work just like custom properties in ID data blocks
    # Extensive information can be found under
    # http://wiki.blender.org/index.php/Doc:2.6/Manual/Extensions/Python/Properties

    # subtypes:
    # StringProperty: /*map:stringType:filepath*/FILE_PATH/*endmap*/, /*map:stringType:dirpath*/DIR_PATH/*endmap*/, /*map:stringType:filename*/FILE_NAME/*endmap*/, /*map:stringType:bytestring*/BYTE_STRING/*endmap*/, /*map:stringType:password*/PASSWORD/*endmap*/, /*map:stringType:none*/NONE/*endmap*/]


/*block:properties*/ 

/*block:updateableCaller*/
    #wrapper to the implemented update-call, to ensure no exception will be thrown
    def /*name:nodeName*/nodeTreeNode/*endname*/_/*name:propName*/prop/*endname*/_updateCaller(self,context):
        try:
            print("Check for logic:"+str(type(self)))
            logic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
            if logic:
                try: # first check if there is a corresponding method in the logic 
                    logic.update_/*name:propName*/prop/*endname*/(self,context) 
                    return
                except:
                    print (traceback.format_exc())
                    pass
            else:
                print("NO LOGIC")
            # now try it the old way
            /*name:nodeName*/nodeTreeNode/*endname*/_/*name:propName*/prop/*endname*/_update(self,context)
        except:
            print(traceback.format_exc())
            
/*endblock:updateableCaller*/
    
/*block:string*/    
    /*name:propName*/myStringProperty/*endname*/ = bpy.props.StringProperty(
        name=/*name|dq:propName*/"name"/*endname*/
        ,default=/*name|dq:defaultValue*/"maxii"/*endname*/
        ,subtype=/*name|dq:subtype:stringType*/"NONE"/*endname*/
        ,maxlen=/*name:maxlen*/4/*endname*/
        ,description=/*name|dq:description*/"The NODE"/*endname*/
/*block:updateable*/
        ,update=/*name:nodeName*/nodeTreeNode/*endname*/_/*name:propName*/prop/*endname*/_updateCaller
/*endblock:updateable*/        
        )
/*endblock:string*/    


    # FloatTypes: /*map:floatType:pixel*/PIXEL/*endmap*/, /*map:floatType:unsigned*/UNSIGNED/*endmap*/, /*map:floatType:percentage*/PERCENTAGE/*endmap*/, /*map:floatType:factor*/FACTOR/*endmap*/, /*map:floatType:angle*/ANGLE/*endmap*/, /*map:floatType:time*/TIME/*endmap*/, /*map:floatType:distance*/DISTANCE/*endmap*/, /*map:floatType:none*/NONE/*endmap*/
    # FloatUnits: /*map:floatUnit:none*/NONE/*endmap*/, /*map:floatUnit:length*/LENGTH/*endmap*/, /*map:floatUnit:area*/AREA/*endmap*/, /*map:floatUnit:volume*/VOLUME/*endmap*/, /*map:floatUnit:rotation*/ROTATION/*endmap*/, /*map:floatUnit:time*/TIME/*endmap*/, /*map:floatUnit:velocity*/VELOCITY/*endmap*/, /*map:floatUnit:acceleration*/ACCELERATION/*endmap*/
    #FloatVectorType /*map:floatvecType:color*/COLOR/*endmap*/, /*map:floatvecType:translation*/TRANSLATION/*endmap*/, /*map:floatvecType:direction*/DIRECTION/*endmap*/, /*map:floatvecType:velocity*/VELOCITY/*endmap*/, /*map:floatvecType:acceleration*/ACCELERATION/*endmap*/, /*map:floatvecType:matrix*/MATRIX/*endmap*/, /*map:floatvecType:euler*/EULER/*endmap*/, /*map:floatvecType:quaternion*/QUATERNION/*endmap*/, /*map:floatvecType:axisangle*/AXISANGLE/*endmap*/, /*map:floatvecType:xyz*/XYZ/*endmap*/, /*map:floatvecType:colorgamma*/COLOR_GAMMA/*endmap*/, /*map:floatvecType:layer*/LAYER/*endmap*/, /*map:floatvecType:none*/NONE/*endmap*/

/*block:number*/
    /*name:propName*/myFloatProperty/*endname*/ = bpy.props./*name:numberType*/Float/*endname*/Property(
        name=/*name|dq:propName*/"name"/*endname*/
/*block:default*/
        ,default=/*name:defaultValue*/3.1415926/*endname*/
/*endblock:default*/        
        ,subtype=/*name|dq:subtype:floatType*/"NONE"/*endname*/
/*block:floatSpecific*/
        ,unit=/*name|dq:unit:floatUnit*/"NONE"/*endname*/
        ,precision=/*name:precision*/1/*endname*/
/*endblock:floatSpecific*/     
/*block:vectorSpecific*/
        ,size=/*name:size*/3/*endname*/
/*endblock:vectorSpecific*/   
        ,description=/*name|dq:description*/"TheFloatNode"/*endname*/
/*block:step*/
        ,step=/*name:step*/1/*endname*/
/*endblock:step*/
/*block:hardBounds*/
        ,min=/*name:min*/0.0/*endname*/
        ,max=/*name:max*/1.0/*endname*/
/*endblock:hardBounds*/        
/*block:softBounds*/
        ,soft_min=/*name:min*/0.0/*endname*/
        ,soft_max=/*name:max*/1.0/*endname*/
/*endblock:softBounds*/    
/*block:updateable*/
        ,update=/*name:nodeName*/nodeTreeNode/*endname*/_/*name:propName*/prop/*endname*/_updateCaller
/*endblock:updateable*/       
        )
/*endblock:number*/  
/*block:enum*/  
    /*name:propName*/my/*endname*/_items = [
/*block:item*/        (/*name|dq:id*/"DOWN2"/*endname*/, /*name|dq:name*/"Down2"/*endname*/, /*name|dq:description*/"Where your feet are"/*endname*/),
/*endblock:item*/
/*block:rip*/        ("UP", "Up", "Where your head should be"),
        ("LEFT", "Left", "Not right"),
        ("RIGHT", "Right", "Not left")
/*endblock:rip*/        
    ]
    /*name:propName*/my_/*endname*/ = bpy.props.EnumProperty(description=/*name|dq:enumDescription*/"Just an example"/*endname*/, items=/*name:propName*/my/*endname*/_items, default=/*name|dq:defaultValue*/'UP'/*endname*/)
/*endblock:enum*/    
/*block:rip*/
    myInt = bpy.props.IntProperty(default=95)
    myBool = bpy.props.BoolProperty()
    myBoolVec = bpy.props.BoolVectorProperty(size=5)
/*endblock:rip*/    
/*block:FloatVector*/ 
    /*name:propName*/ = bpy.props./*name:numberType*/Float/*endname*/VectorProperty(name="Hex Value", subtype='COLOR', default=[0.0,0.0,0.0])
/*endblock:FloatVector*/    
/*endblock:properties*/

    printDebug = bpy.props.BoolProperty()

    inputsockets=[
             /*block:inUpdateSocket*//*name|dq:name*/'ACTIVE'/*endname*/,/*endblock:inUpdateSocket*/
            ]        
    outputsockets=[
             /*block:outUpdateSocket*//*name|dq:name*/'ACTIVE'/*endname*/,/*endblock:outUpdateSocket*/
            ]

    NEW_IN_SOCKETS = bpy.props.CollectionProperty(type=bpy.types.SocketGroup)
    NEW_OUT_SOCKETS = bpy.props.CollectionProperty(type=bpy.types.SocketGroup)

    PRIORITY = bpy.props.IntProperty(default=0)
    STATUS = bpy.props.StringProperty(default="")
    
    def getInputs(self, context):
        result = []
        for insock in self.NEW_IN_SOCKETS:
            result.append((insock.name,insock.name,insock.name))
        return result

    def getOutputs(self, context):
        result = []
        for outsock in self.NEW_OUT_SOCKETS:
            result.append((outsock.name,outsock.name,outsock.name))
        return result



    


    inputSockets = bpy.props.EnumProperty(items=getInputs)
    outputSockets = bpy.props.EnumProperty(items=getOutputs)   

    # === Optional Functions ===
    # Initialization function, called when a new node is created.
    # This is the most common place to create the sockets for a node, as shown below.
    # NOTE: this is not the same as the standard __init__ function in Python, which is
    #       a purely internal Python method and unknown to the node system!
    def init(self, context):
        print("init node:"+self.name+" contextype:"+str(type(context)))
        bpNode_tree = BPointer.create(self.id_data)
        node_tree = bpNode_tree.get()
        # create a pointer for this node for further usage. just that we have a bpid that we can use
        bpNode = BPointer.create(self) 

        
        
        self.PRIORITY = /*name:priority*/0/*endname*/
/*block:color*/        
        self.use_custom_color=True
        self.color=/*name:colorString*/(1.0,0.5,0)/*endname*/
/*endblock:color*/        
/*block:inputSocket*/        /*name:name*/Hello/*endname*/ = self.inputs.new(/*name|q:socketType*/'CustomSocketType'/*endname*/, /*name|dq:name*/"Hello"/*endname*/)
/*block:defaultValue*/        
        /*name:name*/Hello/*endname*/.value = /*name:defaultValue*/1/*endname*/
/*endblock:defaultValue*/
/*block:visibility*/        
        /*name:name*/Hello/*endname*/.dataVisible = /*name:isVisible*/True/*endname*/
/*endblock:visibility*/
        
/*endblock:inputSocket*/
/*block:rip*/        self.inputs.new('NodeSocketFloat', "World")
        self.inputs.new('NodeSocketVector', "!")
/*endblock:rip*/
/*block:outputSocket*/        /*name:name*/Hello/*endname*/=self.outputs.new(/*name|q:socketType*/'CustomSocketType'/*endname*/, /*name|dq:name*/"Hello"/*endname*/)
/*block:defaultValue*/
        /*name:name*/Hello/*endname*/.value=/*name:defaultValue*/1/*endname*/
/*endblock:defaultValue*/        
/*endblock:outputSocket*/
/*block:rip*/        self.outputs.new('NodeSocketColor', "are")
        self.outputs.new('NodeSocketFloat', "you")
/*endblock:rip*/       
        try:
            /*name:nodeName*/nodeTreeNode/*endname*/_init(self,context)
        except:
            pass
        
        try:
            logic = runtimedata.RuntimeData.runtime.addElement(self,/*name|q:nodeName*/'CustomNodeType'/*endname*/,"nocatyet")
            if logic:
                ntLogic = node_tree.logic
                logic.startup({"nodetree":ntLogic})
                
            print("ADDED ELEMENT "+self.name+" to runtime")
        except:
            print("Something went wrong adding element to runtime")
            print(traceback.format_exc())

    # Copy function to initialize a copied node from an existing one.
    def copy(self, node):
        try:
#             logic = runtimedata.RuntimeData.runtime.addElement(self,/*name|q:nodeName*/'CustomNodeType'/*endname*/,"nocatyet")
            print("COPY COPY")
            print("COPY COPY")
            print("SELF:"+str(self.name))
            print("OTHER:"+str(node.name))
            
            self.bpid=-1
            #create a bpointer
            bpNode = BPointer.create(self) 
            
            if bpy.data.worlds[0].gk_nodetree_debugmode:
                return
            
            logic = runtimedata.RuntimeData.runtime.addElement(self,/*name|q:nodeName*/'CustomNodeType'/*endname*/,"nocatyet")
            if logic:
                node_tree = BPointer.getByID(self.id_data.bpid)
                
                logic.startup({"nodetree":node_tree})
                
            print("ADDED ELEMENT "+self.name+" to runtime")
        except:
            print("Something went wrong adding element to runtime")
            print(traceback.format_exc())
            
    # Free function to clean up on removal.
    def free(self):
        print("Removing node ", self, ", Goodbye!")
        try:
            logic = runtimedata.RuntimeData.runtime.removeElement(self)
            print("removed ELEMENT "+self.name+" from runtime")
        except:
            print("Something went wrong removing element to runtime")
            print(traceback.format_exc())
        

    # Additional buttons displayed on the node.
    def draw_buttons(self, context, layout):
        if bpy.data.worlds[0].gk_nodetree_debugmode and self.STATUS!="":
            layout.prop(self,"STATUS",text="Status:")
            
        layout.label(self.bl_idname)
/*block:onNode*/
        layout.prop(self, /*name|dq:propName*/"myFloatProperty"/*endname*/)
/*endblock:onNode*/

/*block:onNodeCollection*/
        layout.prop_search(self, /*name|dq:propName*/"myColString"/*endname*/, bpy.data, /*name|dq:colName*/"objects"/*endname*/, icon=/*name|dq:icon*/'OBJECT_DATA'/*endname*/)
/*endblock:onNodeCollection*/
        try:
            /*name:nodeName*/nodeTreeNode/*endname*/_draw_buttons(self,context,layout)
        except:
            pass
        if self.cover_mode:
            row = layout.row()
            row.template_icon_view(self, "my_previews")   

        
    # Detail buttons in the sidebar.
    # If this function is not defined, the draw_buttons function is used instead
    def draw_buttons_ext(self, context, layout):
        layout.label("Node settings")
        #layout.prop(self, "myFloatProperty")
        # myStringProperty button will only be visible in the sidebar
        layout.prop(self,"PRIORITY")
        layout.prop(self,"enabled__",text="node enabled?")
/*block:onSideBar*/
        layout.prop(self, /*name|dq:propName*/"myStringProperty"/*endname*/)
/*endblock:onSideBar*/
/*block:onSideBarCollection*/
        layout.prop_search(self, /*name|dq:propName*/"myColString"/*endname*/, bpy.data, /*name|dq:colName*/"objects"/*endname*/, icon=/*name|dq:icon*/'OBJECT_DATA'/*endname*/)
/*endblock:onNodeCollection*/
        space = context.space_data
        node_tree = space.node_tree

        layout.operator("/*name|l:treename*/gamekit/*endname*/.enable_nodes", text="enable nodes").enableMode=True
        layout.operator("/*name|l:treename*/gamekit/*endname*/.enable_nodes", text="disable nodes").enableMode=False

        layout.separator()
        
        layout.prop(node_tree,"show_options")
        if (node_tree.show_options):
            layout.operator("/*name|l:treename*/gamekit/*endname*/.addsocket")
            layout.separator()
            layout.prop(self,"inputSockets")
            layout.operator("/*name|l:treename*/gamekit/*endname*/.removesocket","remove input-socket").isInputSocket=True
            layout.separator()
            layout.prop(self,"outputSockets")
            layout.operator("/*name|l:treename*/gamekit/*endname*/.removesocket","remove output-socket").isInputSocket=False
            row = layout.row()
            row.prop(self,"cover_mode")
            row = layout.row()
            row.template_icon_view(self, "my_previews")
            row = layout.row()
            row.prop(self,"my_previews")
            layout.prop(self,"printDebug")         
                        
 
        try:
            /*name:nodeName*/nodeTreeNode/*endname*/_draw_buttons_ext(self,context,layout)
        except:
            pass 

    # Optional: custom label
    # Explicit user label overrides this, but here we can define a label dynamically
    def draw_label(self):

        if self.label!="":
            return self.label
        
        try: # custom label
            return /*name:nodeName*/nodeTreeNode/*endname*/_draw_label(self)
        except:
            pass        
        
        if hasattr(self,"NAME"):
            return self.NAME
        else:
            return self.name
        
    def doUpdate(self):
        for inputSocket in self.inputs:
            if inputSocket.name not in self.inputsockets and inputSocket.name not in self.NEW_IN_SOCKETS:
                if hasattr(self,"extInputs"):
                    found=False
                    for elem in self.extInputs:
                        if elem.extName == inputSocket.name:
                            found = True
                    
                    if found:
                        continue 

                self.inputs.remove(inputSocket)
                print("Remove InSocket:"+inputSocket.name)
                
        for outputSocket in self.outputs:
            if outputSocket.name not in self.outputsockets and outputSocket.name not in self.NEW_OUT_SOCKETS:
                if hasattr(self,"extOutputs"):
                    found=False
                    for elem in self.extOutputs:
                        if elem.extName == outputSocket.name:
                            found = True
                    
                    if found:
                        continue 

                self.outputs.remove(outputSocket)
                print("Remove InSocket:"+outputSocket.name)

        keys = self.inputs.keys()
/*block:updateInputSocket*/        
        if /*name|dq:name*/"START_TRIGGER"/*endname*/ not in keys:
    /*block:setinput*/            
            START_TRIGGER=self.input.new('NodeSocketBool', "START_TRIGGER")
/*endblock:setinput*/            
/*endblock:updateInputSocket*/      
        keys = self.outputs.keys()
/*block:updateOutputSocket*/      
        if /*name|dq:name*/"END_TRIGGER"/*endname*/ not in keys:
    /*block:setoutput*/            
            END_TRIGGER=self.outputs.new('NodeSocketBool', "END_TRIGGER")
/*endblock:setoutput*/            
/*endblock:updateOutputSocket*/
/*endblock:node*/
### Node Categories ###
# Node categories are a python system for automatically
# extending the Add menu, toolbar panels and search operator.
# For more examples see release/scripts/startup/nodeitems_builtins.py

import nodeitems_utils
from nodeitems_utils import NodeCategory, NodeItem


# our own base class with an appropriate poll function,
# so the categories only show up in our own tree type
class /*name:treename*/MyNode/*endname*/Category(NodeCategory):
    @classmethod
    def poll(cls, context):
        return context.space_data.tree_type == /*name|q:treename*/'CustomTreeType'/*endname*/

# all categories in a list
/*name:treename*/node/*endname*/_categories = [
/*block:category*/
    # identifier, label, items list
    /*name:treename*/MyNode/*endname*/Category(/*name|dq:catName*/"SOMENODES"/*endname*/, /*name|dq:catName*/"Some Nodes"/*endname*/, items=[
        # our basic node
/*block:item*/        
        NodeItem(/*name|dq:nodeName*/"CustomNodeType"/*endname*/),
/*endblock:item*/        
        ]),
/*endblock:category*/
/*name:treename*/MyNode/*endname*/Category("blender","blender", items=[
   NodeItem("NodeFrame")                                                                       
]),                                                                                                                                                            
/*block:rip*/
    MyNodeCategory("OTHERNODES", "Other Nodes", items=[
        # the node item can have additional settings,
        # which are applied to new nodes
        # NB: settings values are stored as string expressions,
        # for this reason they should be converted to strings using repr()
        NodeItem("CustomNodeType", label="Node A", settings={
            "myStringProperty": repr("Lorem ipsum dolor sit amet"),
            "myFloatProperty": repr(1.0),
            }),
        NodeItem("CustomNodeType", label="Node B", settings={
            "myStringProperty": repr("consectetur adipisicing elit"),
            "myFloatProperty": repr(2.0),
            }),
        ]),
/*endblock:rip*/                   
    ]

/*block:customFile*/
/*name:data*/
# here you can place custom-data from additional files
/*endname*/    
/*endblock:customFile*/


def register():
    try:
        bpy.utils.unregister_class(NodeTreeObject)
        bpy.utils.unregister_class(NodeTreeBOOL)
        bpy.utils.unregister_class(NodeTreeSTRING)
        bpy.utils.unregister_class(NodeTreeFLOAT)
        bpy.utils.unregister_class(NodeTreeINT)
        bpy.utils.unregister_class(NodeTreeCOLOR)
        bpy.utils.unregister_class(NodeTreeVec3)
    except:
        pass
    bpy.utils.register_class(NodeTreeObject)
    bpy.utils.register_class(NodeTreeBOOL)
    bpy.utils.register_class(NodeTreeSTRING)
    bpy.utils.register_class(NodeTreeFLOAT)
    bpy.utils.register_class(NodeTreeINT)
    bpy.utils.register_class(NodeTreeCOLOR)
    bpy.utils.register_class(NodeTreeVec3)
/*block:registerClass*/    try:
        bpy.utils.unregister_class(/*name:className*/MyCustomTree/*endname*/)
    except:
        pass
    bpy.utils.register_class(/*name:className*/MyCustomTree/*endname*/)
/*endblock:registerClass*/
/*block:rip*/
    bpy.utils.register_class(MyCustomNode)
/*endblock:rip*/
    try:
        nodeitems_utils.unregister_node_categories("/*name:treename*/CUSTOM/*endname*/_NODES")
    except:
        pass
    nodeitems_utils.register_node_categories("/*name:treename*/CUSTOM/*endname*/_NODES", /*name:treename*/node/*endname*/_categories)
    try:
        customRegister()
        import runtimeMappings
        runtimeMappings.custom_register()
    except:
        print(traceback.format_exc())
        pass 

def unregister():
    nodeitems_utils.unregister_node_categories("/*name:treename*/CUSTOM/*endname*/_NODES")

/*block:unregisterClass*/    bpy.utils.unregister_class(/*name:className*/MyCustomTree/*endname*/)
/*endblock:unregisterClass*/    

    bpy.utils.unregister_class(NodeTreeObject)
    bpy.utils.unregister_class(NodeTreeBOOL)
    bpy.utils.unregister_class(NodeTreeSTRING)
    bpy.utils.unregister_class(NodeTreeFLOAT)
    bpy.utils.unregister_class(NodeTreeINT)
    bpy.utils.unregister_class(NodeTreeCOLOR)
    bpy.utils.unregister_class(NodeTreeVec3)
    try:
        customUnregister()
        import runtimeMappings
        runtimeMappings.custom_unregister()
    except:
        print(traceback.format_exc())
        pass    
/*block:rip*/
    bpy.utils.unregister_class(MyCustomNode)
/*endblock:rip*/


if __name__ == "__main__":
    register()