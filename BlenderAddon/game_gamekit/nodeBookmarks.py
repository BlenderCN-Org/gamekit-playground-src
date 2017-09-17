import bpy,traceback,SpaceContext
from bpointers import *
from asyncio import Task,sleep,coroutine

class IntGroup(bpy.types.PropertyGroup):
    value = bpy.props.IntProperty()

class BookMarkData(bpy.types.PropertyGroup):
    nodetreeID = bpy.props.IntProperty()
    nodeIDS = bpy.props.CollectionProperty(type=IntGroup)    
   
class OpBookmark(bpy.types.Operator):
    bl_idname = "node.bookmark"    
    bl_label ="Create Bookmark"

    mode = bpy.props.StringProperty(default="set")
    selectionID = bpy.props.IntProperty(default=-1)
    newName = bpy.props.StringProperty(default="set")

    @classmethod
    def poll(cls,context):
        space = context.space_data
        tree = space.node_tree
        node_selected = context.selected_nodes
        return space.type == 'NODE_EDITOR' 
    
    def execute(self,context):
        if self.mode=="set":
            return self.set(context)
        elif self.mode=="remove":
            return self.remove(context)
        elif self.mode=="show":
            return self.show(context)
        elif self.mode=="rename":
            return self.rename(context)
        else:
            print("UNKNOWN MODE:"+self.mode)
            return {'CANCELLED'}

    def invoke(self, context, event):
        if self.mode=="rename":
            wm = context.window_manager
            return wm.invoke_props_dialog(self)
        else:
            return self.execute(context)
            
    def set(self,context):    
        space = context.space_data
        tree = space.node_tree
        node_selected = context.selected_nodes
        world = bpy.data.worlds[0]
        newBookmark = world.bookmarks.add()
        newBookmark.name = context.active_node.name
        newBookmark.nodetreeID = tree.bpid
        
        for node in node_selected:
            nodeSet = newBookmark.nodeIDS.add()
            nodeSet.value=node.bpid
            print("SET ID:"+str(node.bpid))
            
        return {'FINISHED'}
    
    def remove(self,context):
        space = context.space_data
        tree = space.node_tree
        node_selected = context.selected_nodes
        world = bpy.data.worlds[0]
        try:
            world.bookmarks.remove(self.selectionID)
        except:
            print(traceback.format_exc())
            return {'CANCELLED'}        
        return {'FINISHED'}
    
    def rename(self,context):
        space = context.space_data
        tree = space.node_tree
        node_selected = context.selected_nodes
        world = bpy.data.worlds[0]
        try:
            bookmark = world.bookmarks[self.selectionID]
            bookmark.name = self.newName        
        except:
            print(traceback.format_exc())
            return {'CANCELLED'}
        return {'FINISHED'}    
            
    def show(self,context):
        print("SHOW:"+str(self.selectionID))
        if (self.selectionID!=-1):
            space = context.space_data
            world = bpy.data.worlds[0]
            
            
            try:
                #select the nodes:
                bookmark = world.bookmarks[self.selectionID]

                tree = BPointer.getByID(bookmark.nodetreeID).get()
                treeName = tree.name
                spaceContext = SpaceContext.getCurrentSpaceContext(context)
                spaceContext.logicTreeBrowsing = True
                spaceContext.logicTreeName = treeName
                
                
                for node in tree.nodes:
                    node.select=False
                                    
                for id in bookmark.nodeIDS:
                    print("TRY TO GET BPOINTER:"+str(id.value))
                    bpNode = BPointer.getByID(id.value)
                    bpNode.get().select=True
                    print("B:"+bpNode.get().name)
                    
#                bpy.ops.node.view_selected()
                Task(focusNodetree(context.window,context.screen,context.area,context.region,context.space_data))                   
            except:
                print(traceback.format_exc())
                        
        return {'FINISHED'}


    def draw(self, context):
        print("CUSTOM")
        layout = self.layout
        layout.label("RENAME")

        space = context.space_data
        tree = space.node_tree
        node_selected = context.selected_nodes
        world = bpy.data.worlds[0]
        
        bookmark = world.bookmarks[self.selectionID]
        layout.label("OldName:" + str(bookmark.name))
        col = layout.prop(self,"newName")

@coroutine
def focusNodetree(window,screen,area,region,space_data):
    print( ("IN FOCUS TASK: %s %s %s %s %s") % ( str(window),str(screen),str(area),str(region),str(space_data)  ))
    yield from sleep(0.01)
    print("AFTER SLEEP")
    override = {'window': window, 'screen': screen, 'area': area,'region':region,'space_data':space_data}
    bpy.ops.node.view_selected(override)
    print("AFTER OP")

class BookmarksPanel(bpy.types.Panel):
    """Creates a Panel in the scene context of the properties editor"""
    bl_label = "Bookmarks"
    bl_idname = "Nodetree_Bookmarks"
    bl_space_type = 'NODE_EDITOR'
    bl_region_type = 'UI'

    def draw(self, context):
        layout = self.layout
        space = context.space_data
        tree = space.node_tree

        if not tree:
            return

        layout.label("Bookmarks:")
        
        i = 0

        split = layout.split(0.6)
        col1 = split.column()
        col2 = split.column()


        split = col1.split(0.1)
        nr = split.column()
        op = split.column()
        
        split = col2.split()
        colRen = split.column()
        colDel = split.column()
        
        world = bpy.data.worlds[0]
        
        if len(world.bookmarks)==0:
            col1.label("NO BOOKMARKS")
        else:
            for bookmark in world.bookmarks:
                nr.label(str(i+1))
                show = op.operator("node.bookmark",text=bookmark.name)
                show.mode="show"
                show.selectionID = i
                rename = colRen.operator("node.bookmark",text="Rename")
                rename.mode="rename"
                rename.newName=""
                rename.selectionID = i
                remove = colDel.operator("node.bookmark",text="X")
                remove.mode="remove"
                remove.newName=""
                remove.selectionID = i            
                i=i+1
             
        layout.operator("node.bookmark",text="SET BOOKMARK").mode="set"
        




def register():
    bpy.utils.register_class(IntGroup)
    bpy.utils.register_class(BookMarkData)
    bpy.types.World.bookmarks = bpy.props.CollectionProperty(type=BookMarkData)
    bpy.utils.register_class(OpBookmark)
    bpy.utils.register_class(BookmarksPanel)

def unregister():
    bpy.utils.unregister_class(IntGroup)
    bpy.utils.unregister_class(BookMarkData)
    bpy.utils.unregister_class(OpBookmark)
    bpy.utils.unregister_class(BookmarksPanel)
    