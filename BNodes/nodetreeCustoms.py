# this code-hooks will be inserted in the generated tree-file

@persistent
def load_handler(dummy):
    print("Custom Loadhandler", bpy.data.filepath)
#     startup()
    
bpy.app.handlers.load_post.append(load_handler)

# NODE_FUNCS = {}
# NODE_CTX = {}
# UPDATE_NODES = []
# 
# def getNodeCTX(node):
#     global NODE_CTX
#     
#     if node not in NODE_CTX:
#         NODE_CTX[node]={}
#         
#     return NODE_CTX  
# 
# # iterate over all nodes and check if there is a startup-hook-set
# def startup():
#     global NODE_FUNCS
#     global UPDATE_NODES
#      
#     for nodetree in bpy.data.node_groups:
#         if nodetree.rna_type.identifier=="Gamekit":
#             for node in nodetree.nodes:
#                 nodetype = node.rna_type.identifier
#                 try:
#                     funcs = NODE_FUNCS[nodetype]
#                     addToUpdateQueue = funcs["startup"](node)
#                     if addToUpdateQueue:
#                         UPDATE_NODES.append(node)
#                 except:
#                     # no startup for this nodetype
#                     pass
                

def NodeTreeNode_draw_buttons(self,context,layout):
    layout.context_pointer_set("node",self)
    layout.operator("gamekit.nodetreenode")

class GamekitNodeTreeNodeOperator(bpy.types.Operator):
    """Tooltip"""
    bl_idname = "gamekit.nodetreenode"
    bl_label = "refresh subtree-sockets"
    bl_space_type = 'NODE_EDITOR'

        
    @classmethod 
    def poll(cls, context):
        current_node = context.active_node
        current_type = current_node.bl_idname
        return True
#        return current_type=="NodeTreeNode"

    def execute(self, context):
        nodetree = context.space_data.node_tree
        #current_node = context.active_node
        current_node = context.node
        current_type = current_node.bl_idname
        
        if current_node.nodetree=='':
            print ("NO NODETREE SELECTED!")
            return {'FINISHED'}
        
        if current_node.nodetree==current_node.currentTree:
            print("nothing to do")
            return {'FINISHED'}
        
        current_node.currentTree = current_node.nodetree

        #clear the sockets currently added from another nodetree
        for iS in current_node.extInputs:
            current_node.inputs.remove(current_node.inputs[iS.extName])
        current_node.extInputs.clear()
        for oS in current_node.extOutputs:
            current_node.outputs.remove(current_node.outputs[oS.extName])
        current_node.extOutputs.clear()
        
        # get the target-nodetree which is include via nodetreenode
        targetNT = bpy.data.node_groups[current_node.nodetree]
        # get all input and output IO-sockets
        for node in targetNT.nodes:
            for iS in node.inputs:
                try:
                    if iS.isIOSocket:
                        print("ioSock:" + iS.name);
                        ioSock = current_node.extInputs.add()
                        ioSock.name = node.name+"|"+iS.name
                        ioSock.extName = iS.ioSocketName
                        if ioSock.extName == "":
                            ioSock.extName = iS.name
                        ioSockElem = current_node.inputs.new(iS.bl_idname,ioSock.extName)
                except:
                    pass
            for oS in node.outputs:
                try:
                    if oS.isIOSocket:
                        ioSock = current_node.extOutputs.add()
                        ioSock.name = node.name+"|"+oS.name
                        ioSock.extName = oS.ioSocketName
                        if ioSock.extName == "":
                            ioSock.extName = oS.name
                        ioSockElem = current_node.outputs.new(oS.bl_idname,ioSock.extName)
                except:
                    pass
 
        return {'FINISHED'}

def PBSTexMaterial_init(self,context):
    # create material
    newMat = bpy.data.materials.new(name="pbs_mat_" + self.name)
    print ("Create material:"+newMat.name)

def PBSTexEnvMap_init(self,context):
    print ("CREATED IMAGE")
    self.tex = bpy.data.textures.new(self.name,type="IMAGE")
    print ("CREATED IMAGE")


def PBSTexEnvMap_draw_buttons(self,context,layout):
    if self.envMap!="":
        try:
            tex = bpy.data.textures[0]
            #image = bpy.data.images[self.envMap]
            layout.template_preview(tex)
        except:
            print("Unknown image:" + self.envMap)
            print(traceback.format_exc())
            pass;
 
def PBSTexEnvMap_draw_buttons_ext(self,context,layout):
    if self.envMap!="":
        try:
            tex = bpy.data.textures[0]
            #image = bpy.data.images[self.envMap]
            layout.template_preview(tex)
        except:
            print("Unknown image:" + self.envMap)
            pass;

def PBSTex_Albedo_ALBEDO_MAP_update(self,context):
    print("ALBEDO UPDATE")
    if self.ALBEDO_MAP!="" and self.ALBEDO_MAP in bpy.data.images:
        image = bpy.data.images[self.ALBEDO_MAP]
        self.PATH = image.filepath
    else:
        print("NO")
        
def PBSTex_Normal_NORMAL_MAP_update(self,context):
    print("ALBEDO UPDATE")
    if self.NORMAL_MAP!="" and self.NORMAL_MAP in bpy.data.images:
        image = bpy.data.images[self.NORMAL_MAP]
        self.PATH = image.filepath
    else: 
        print("NO")     
           
def PBSTex_F0_F0_MAP_update(self,context):
    if self.F0_MAP!="" and self.F0_MAP in bpy.data.images:
        image = bpy.data.images[self.F0_MAP]
        self.PATH = image.filepath 
    else:
        print("NO")   
        
def PBSTex_Env_ENV_MAP_update(self,context):
    print("ALBEDO UPDATE")
    if self.ENV_MAP!="" and self.ENV_MAP in bpy.data.images:
        image = bpy.data.images[self.ENV_MAP]
        self.PATH = image.filepath
    else:
        print("NO")           
        


################### STATE REF - LOGIC #############################

def StateRef_draw_buttons(self,context,layout):
    if bpy.data.worlds[0].gk_nodetree_debugmode:
        row = layout.row()
        row.enabled=False
        row.prop(self,"STM",text="STM:")
        row = layout.row()
        row.enabled=False
        row.prop(self,"STATE",text="State:")  
    else:
        layout.prop(self,"enumSTM",text="STM:")
        layout.prop(self,"enumSTATE",text="State:")  


def StateMachine_draw_buttons(self,context,layout):
    if bpy.data.worlds[0].gk_nodetree_debugmode:
        row = layout.row()
        row.enabled = False
        row.prop(self,"INITIAL_STATE",text="Initial State:")  
    else:
        layout.prop(self,"enumSTATE",text="Initial State:")  
 
 
def PropertyGet_draw_buttons(self,context,layout):
    if bpy.data.worlds[0].gk_nodetree_debugmode:
        row = layout.row()
        row.enabled = False
        row.prop(self,"NAME",text="Initial State:")  
    else:
        if len(self.inputs["TARGET_OBJ"].links)>0 or self.MANUAL:
            layout.prop(self,"NAME",text="prop:")
        else:
            layout.prop(self,"enumNAME",text="prop:")   

def StateTransition_draw_label(self):
    return self.inputs["FROM"].links[0].from_node.STATE + "=>" +self.outputs["TO"].links[0].to_node.STATE

def StateRef_draw_label(self):
    return self.STM+"."+self.STATE

def AnimationNode_draw_label(self):
    return self.ANIM_NAME;

def AnimationDefinition_draw_buttons(self,context,layout):
    layout.prop(self,"FRAME_CUSTOM")
    if self.FRAME_CUSTOM:
        layout.prop(self,"FRAME_START")
        layout.prop(self,"FRAME_END")

def AnimationDefinition_draw_buttons_ext(self,context,layout):
    layout.prop(self,"FRAME_CUSTOM")
    if self.FRAME_CUSTOM:
        layout.prop(self,"FRAME_START")
        layout.prop(self,"FRAME_END")

def AnimationPlayer_draw_buttons(self,context,layout):
    if bpy.data.worlds[0].gk_nodetree_debugmode:
        row = layout.row()
        row.enabled = False
        row.prop(self,"NAME",text="Initial State:")  
    else:
        layout.prop(self,"enumNAME",text="Anim:")     

def ObjectNode_draw_label(sefl):
    return self.Obj;



def customRegister():
    try:
        bpy.utils.unregister_class(bpy.types.GamekitNodeTreeNodeOperator)    
    except:
        pass    
    bpy.utils.register_class(GamekitNodeTreeNodeOperator)
    
    runtimedata.Clazzes.classes["NodeTreeNode"].extInputs=bpy.props.CollectionProperty(type=SocketGroup)
    runtimedata.Clazzes.classes["NodeTreeNode"].extOutputs=bpy.props.CollectionProperty(type=SocketGroup)
    runtimedata.Clazzes.classes["NodeTreeNode"].currentTree=bpy.props.StringProperty()
    
    print("REGISTERED")

def customUnregister():
    bpy.utils.unregister_class(bpy.types.GamekitNodeTreeNodeOperator)