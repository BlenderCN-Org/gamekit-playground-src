from runtime import * 
from bpointers import *
from time import time
import runtimedata

class DefaultGroup(bpy.types.PropertyGroup):
    pass

try:
    bpy.utils.unregister_class(bpy.type.DefaultGroup)
except:
    pass
bpy.utils.register_class(DefaultGroup)




class StateMachine(LogicElement):

    def __init__(self,runtime,elem,typeName):
        super().__init__(runtime,elem,typeName)
        print("AFTER")
        
        self.isGlobal = False
        self.stateNodes = []
        self.dirty = False
        self.initialSet = False
        self.initialSTATE = self.get().INITIAL_STATE
        try:
            del runtimedata.RuntimeData.runtime.getCacheSTM()[elem.id_data.bpid]
        except:
            print(traceback.format_exc())
            pass
    
    def invalidate(self):
        try:
            id = self.getID()
            self.runtime.removeStatemachine(self)
            
            del runtimedata.RuntimeData.runtime.getCacheSTATE()[id]
            ntLogic = self.getData("nodetree")
            del runtimedata.RuntimeData.runtime.getCacheSTM()[ntLogic.getID()]
        except:
            print(traceback.format_exc())
            
        super().invalidate()
        
            
    def update_GLOBAL(self,elem,context):
        # add statemachine to list
        self.runtime.addStatemachine(self,self.get().GLOBAL);
        try:
            del runtimedata.RuntimeData.runtime.getCacheSTM()["global"]
            del runtimedata.RuntimeData.runtime.getCacheSTM()[elem.id_data.bpid]
        except:
            print(traceback.format_exc())
            pass

    def update_NAME(self,elem,context):
        try:
            nodetree = self.getData("nodetree")
            del runtimedata.RuntimeData.runtime.getCacheSTM()[elem.id_data.bpid]
        except:
            print(traceback.format_exc())
            pass
                    
    def updateValues(self):
        if not self.initialSet:
            try:
                if (self.initialSTATE!=""):
                    self.get().enumSTATE = self.initialSTATE
                print("INITIAL SET")
                self.initialSet = True
            except:
                pass
                #print(traceback.format_exc())
                        
            # TODO UPDATE 
            
        _stms = self.runtime.CUSTOMDATA['statemachines']
        
        ##print("TYPE:"+str(type(self)))
        #print("DATA:"+str(self.data))

        
        if (len(self.stateNodes) != len(self.get().outputs["STATES"].links)):
            self.stateNodes=[]
            try:
                del runtimedata.RuntimeData.runtime.getCacheSTATE()[self.getID()]
            except:
                print(traceback.format_exc())
                pass
            
            self.dirty=True
            for link in self.get().outputs["STATES"].links:
                to_node = link.to_node
                #print ("STATE TYPE:"+to_node.rna_type.identifier)
                toLogic = self.runtime.getLogic(to_node.bpid)
                if (to_node.rna_type.identifier=="State"):
                    self.stateNodes.append(toLogic)
                    #print("ADDED STATE LOGIC")                

                

    def validation(self,params):
        #print("VALIDATION")
        superResult = super().validation()

       # print("STATEMACHINE VALIDATION:")
        for link in self.get().outputs["STATES"].links:
            to_node = link.to_node
            if not (to_node.rna_type.identifier=="State" or to_node.rna_type.identifier=="StateRef"):
                nctx = nodeRuntime.getNodeCTX(to_node)
                nctx["validity"]="INVALID"
                to_node.custom_color=(1,0,0)
                to_node.use_custom_color=True
                print("INVALID")
            else:
                print("Node:"+to_node.name+" is valid")            

    def startup(self,params=None):
       # print("STATEMACHINE STARTUP:" + self.get().NAME)
        super().startup(params)
        
        self.update_GLOBAL(self.get(),None);
        
        self.updateValues()
                #print("ADDED valid state:" + to_node.name)
            
        self.color = None

        self.runtime.addToTickQueue(self)
    
     

    def update(self,params):
        #print("CHECK STM:"+self.get().NAME)
        self.updateValues()
            
        #print( ("UPDATE STATE: %s != %s") % (str(self.color),str(self.get().color)) )
        if self.color != self.get().color or self.dirty:
            self.color = self.get().color.copy()
            
            for state in self.stateNodes:
                #print ("SET COL:"+state.get().NAME)
                state.setColor(self.get().color)
        
        self.dirty = False


    def getState(self,stateName):
        for state in self.stateNodes:
            if state.get().NAME == stateName:
                return state
            
        return None
    
    @staticmethod
    def get_states(self,context):
        try:
            if self.bpid==-1:
                print(self.name+" has bpid -1")
                return []
            
            data = runtimedata.RuntimeData.runtime.getCacheSTATE()[self.bpid]
            #print("USING CHACHED STATES:"+str(data))
            return data
        except:
            stmLogic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
            items = [(stNode.get().NAME, stNode.get().NAME, "") for stNode in stmLogic.stateNodes ]
            #print("StateMachine States:"+str(items))
            runtimedata.RuntimeData.runtime.getCacheSTATE()[self.bpid] = items
            return items
    
    @staticmethod
    def set_state(self,context):
        self.INITIAL_STATE = self.enumSTATE
############################ STATE REF #######################################################################################
class State(LogicElement):
    
    def __init__(self,runtime,elem,typeName):
        super().__init__(runtime,elem,typeName)
        self.stateRefs = []
        self.parent = None
        try:
            stmBPID = elem.inputs["STATE_ID"].links[0].from_node.bpid
            del runtimedata.RuntimeData.runtime.getCacheSTATE()[stmBPID];
        except:
            print (traceback.format_exc()) 

    def setColor(self,col):
        if self.get().enabled__:
            self.get().color = col
            self.get().use_custom_color=True
            for stateRef in self.stateRefs:
                stateRef.setColor(col)

    def addStateRef(self,sref):
        if sref not in self.stateRefs:
            print ("ADDED STATEREF:"+sref.get().name)
            self.stateRefs.append(sref)
            sref.setColor(self.get().color)
            

    def removeStateRef(self,sref):
        try:
            self.stateRefs.remove(sref);
        except:
            print("couldnt remove sref:"+sref.get().name)
            pass
    
    def update_NAME(self,elem,context):
        try:
            stmBPID = elem.inputs["STATE_ID"].links[0].from_node.bpid
            del runtimedata.RuntimeData.runtime.getCacheSTATE()[stmBPID];
        except:
            print (traceback.format_exc()) 
###############################################################################
class StateRef(LogicElement):

    def __init__(self,runtime,elem,typeName):
        super().__init__(runtime,elem,typeName)
        self.stmLogic=None
        self.stateLogic=None
        
        self.initialSTATE = self.get().STATE
        self.initialSTM = self.get().STM
        self.initialSet = False

    def startup(self,params):
        print("STATEREF STM:"+self.get().STM)
        super().startup(params)
        
        stmname = self.get().STM
        nodetree = self.getData("nodetree")
        
        if self.get().GLOBAL:
            stm = runtimedata.RuntimeData.runtime.getGlobalStatemachine(stmname)
        else:
            stm = runtimedata.RuntimeData.runtime.getStatemachine(nodetree,stmname)
        
            
        StateRef.set_statemachine(self.get(),None)
        self.runtime.addToTickQueue(self)
    
    def update(self,params):
        if not self.initialSet:
            try:
                self.initialSet = True
                self.runtime.removeFromTickQueue(self)
                if (self.initialSTM!=""):
                    self.get().enumSTM = self.initialSTM
                if (self.initialSTATE!=""):
                    self.get().enumSTATE = self.initialSTATE
                print("INITIAL SET")
                # when I'm here it really worked!
            except:
                print(traceback.format_exc())
        
    
    def update_GLOBAL(self,elem,context):
        #print("UUUUUUUUUUUUUUPDATE GLOBAL!!!")
        StateRef.set_statemachine(elem,context)     
    
    def getSTM(self):
        return self.stmLogic        
        
    def setSTM(self,stmLogic):
        self.stmLogic = stmLogic
    
    def getSTATE(self):
        return self.stateLogic
    
    def setSTATE(self,stateLogic): 
        #print("SETSTATE!!!")
#         if stateLogic == self.stateLogic: #new == old state? do nothing
#             print("SS1")
#             return
        
        if self.stateLogic is not None: # disconnect from former statelogic
            self.stateLogic.removeStateRef(self)
            
        if stateLogic is not None: #connect to new
            print ("STATELOGIC:" + str(stateLogic))
            stateLogic.addStateRef(self)
            
        self.stateLogic = stateLogic
        
        if stateLogic:
            self.get().cover_mode = stateLogic.get().cover_mode
            if stateLogic.get().my_previews!="":
                self.get().my_previews = stateLogic.get().my_previews 
        
            
    def setColor(self,col):
        if self.get().enabled__:
            self.get().color=col
            self.get().use_custom_color = True
                
    def get_statemachines():
    #closure - keep a reference to the list
        items = None
        def func(self, context):
            
#             print("get statemachines:"+str(type(self))+" logic:"+str(type(self)))
            logic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
            nodetree = self.id_data

            if logic is None:
                #print("NO DATA ")
                return []

            
            try:
                ######## TEST ###############
                if (logic.get().GLOBAL):
                    data = runtimedata.RuntimeData.runtime.getCacheSTM()["global"]
                    #print("USE GLOBAL CACHE")
                    return data
                else:
                    data = runtimedata.RuntimeData.runtime.getCacheSTM()[nodetree.bpid]
                    #print("USER NTCACHE:"+str(data))
                    return data
            except:
                if (logic.get().GLOBAL):
                    key="global"
                    items = [(stm.get().NAME, stm.get().NAME, "") for stm in logic.runtime.CUSTOMDATA["statemachines"]["global"] ]
                else:
                    key=nodetree.bpid
                    try:
                        #print("STATEMACHINES:"+str(nodetree.bpid)+" "+str(logic.runtime.CUSTOMDATA["statemachines"]))
                        
                        items = [(stm.get().NAME, stm.get().NAME, "") for stm in logic.runtime.CUSTOMDATA["statemachines"][nodetree.bpid] ]
                    except:
                        print(traceback.format_exc())
                        return []
                    
                if len(items)==0:
                    #print("NONE LENGTH")
                    logic.setSTM(None)
                    logic.setSTATE(None)

                runtimedata.RuntimeData.runtime.getCacheSTM()[key]=items
                #print("ITEMS:"+str(items))
                return items
        return func

    @staticmethod
    def set_statemachine(self,context):
        logic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
        self.STM = self.enumSTM
        if self.STM is None or self.STM=="":
            logic.setSTM(None)
            StateRef.set_state(self,context)
            return
        
        nodetree = logic.getData("nodetree")
        #print("set_statemachine() NODETREE:"+str(type(nodetree)))
        
        if self.GLOBAL:
            STM = runtimedata.RuntimeData.runtime.getStatemachine("global",self.STM)
        else:
            STM = runtimedata.RuntimeData.runtime.getStatemachine(nodetree,self.STM)
        logic.setSTM(STM)
        StateRef.set_state(self,context)
            
            
    @staticmethod        
    def get_states():
        #closure - keep a reference to the list
        items = None
        
        def func(self, context):
            #print("GET STATES")
            
            if self.STM is None:
                return []
            nodetree = self.id_data
            logic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
            if logic is None:
                return []
            
            STM = logic.getSTM()
            
#             if self.GLOBAL:
#                 STM = runtimedata.RuntimeData.runtime.getStatemachine("global",self.STM)
#             else:
#                 STM = runtimedata.RuntimeData.runtime.getStatemachine(nodetree,self.STM)
            #print("TRY TO GET STATEMACHINE:"+str(self.STM)+" =>"+str(STM))
    
            if STM is None:

                return []
            else:
                logic.setData("STM",STM)
            #print("CALL GET STATE:"+str(STM.getID()));    
            items = StateMachine.get_states(STM.get(),None)
            return items
        return func    
    
    @staticmethod
    def set_state(self,context):
        self.STATE = self.enumSTATE
        logicStateRef = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
        stm = logicStateRef.getSTM()
        if stm is None:
            return
        stateLogic = stm.getState(self.STATE)
        if not stateLogic or stateLogic=="":
            return
        logicStateRef.setSTATE(stateLogic)
###################################################################################
class Property(LogicElement):

    def __init__(self,runtime,elem,typeName):
        super().__init__(runtime,elem,typeName)
        #print("000000000 PROPTERY "+str(self.get().MODE))
        try:
            #print("TRY TO DELETE:"+str(elem.id_data.bpid))
            del runtimedata.RuntimeData.runtime.getCachePROP()[elem.id_data.bpid];
            print("OK")
        except:
            print(traceback.format_exc())
            pass

#TODO: only init_property-types should be shown AND should be removed on mode-switches        
    def startup(self,params):
        super().startup(params)
        #print("PROP STARTUP")
        
#         if self.get().MODE=="init_property":
            #print("try to add")
        runtimedata.RuntimeData.runtime.addProperty(self)
            #print("ADD PROPERTY "+self.getName())

    def getName(self):
        return self.get().NAME

    def update_NAME(self,raw,context):
        try:
           # print("UPDATE NAME:"+self.getName())
            #print("TRY TO DELETE CACHE")
            del runtimedata.RuntimeData.runtime.getCachePROP()[raw.id_data.bpid];
            #print ("OK")
        except:
            print(traceback.format_exc())     

    
class AnimationDefinition(LogicElement):
    
    def startup(self,params):
        super().startup(params)
        runtimedata.RuntimeData.runtime.addAnimation(self)
        
    def update_NAME(self,raw,context):
        try:
            del runtimedata.RuntimeData.runtime.getCacheANIM()[raw.id_data.bpid];
        except:
            print(traceback.format_exc())     
         
    def close(self):
        runtimedata.RuntimeData.runtime.removeAnimation(self)
        print("REMOVED ANIMATION")
        try:
            nodetree = self.getData("nodetree")
            del runtimedata.RuntimeData.runtime.getCacheANIM()[nodetree.ID];
        except:
            print(traceback.format_exc())     
           
    @staticmethod
    def get_animations(self,context):
        
        nodetreeID = self.id_data.bpid
        
        try:
            data = runtimedata.RuntimeData.runtime.getCacheANIM()[nodetreeID]
            #print ("USING CACHED DATA")
            return data
        except:
            try:
                anims = runtimedata.RuntimeData.runtime.getAnimations(self.id_data)
                
                result = []
                for anim in anims:
                    animName = anim.get().NAME
                    if (animName,animName,animName) not in result:
                        result.append( (animName,animName,animName) )
                    
    #             result = [ (prop.getName(),prop.getName(),prop.getName()) for prop in props ]
                
                runtimedata.RuntimeData.runtime.getCacheANIM()[nodetreeID] = result
                return result
            except:
                print (traceback.format_exc())
                return []
        

class AnimationPlayer(LogicElement):
    def __init__(self,runtime,elem,typeName):
        super().__init__(runtime,elem,typeName)
        self.initialName = self.get().NAME
        if not self.initialName:
            self.initialName = self.get().enumNAME
            
        self.initialSet = False
        
    def startup(self,params):
       # print("PROPERTY GET STARTUP")
        super().startup(params)
        nodetree = self.getData("nodetree")
        self.runtime.addToTickQueue(self)     
           

    def update(self,params):
        #print("PROPERTY GET UPDATE")

        self.processInput(params)
        if not self.initialSet and self.initialName!="":
            try:
                self.initialSet=True
              #  print("INITALIZED!! before:"+str(self.get().enumNAME)+" now:"+self.initialName)
                self.get().enumNAME=self.initialName
                self.runtime.removeFromTickQueue(self)
            except:
                print(traceback.format_exc())
                pass            
            
    def setNAME(self,name):
        self.get().NAME = name  
       
    @staticmethod 
    def set_animation(self,context):
        logic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
        logic.setNAME(self.enumNAME)   

class PropertyGet(LogicElement):
    
    
    
    def __init__(self,runtime,elem,typeName):
        super().__init__(runtime,elem,typeName)
        self.initialName = self.get().NAME
        self.initialSet = False
       # print("PROPERTY GET INIT")

    def startup(self,params):
       # print("PROPERTY GET STARTUP")
        super().startup(params)
        nodetree = self.getData("nodetree")
        self.runtime.addToTickQueue(self)        

    def update(self,params):
        #print("PROPERTY GET UPDATE")

        self.processInput(params)
        if not self.initialSet and self.initialName!="":
            try:
                self.initialSet=True
              #  print("INITALIZED!! before:"+str(self.get().enumNAME)+" now:"+self.initialName)
                self.get().enumNAME=self.initialName
                self.runtime.removeFromTickQueue(self)
            except:
                print(traceback.format_exc())
                pass            
            
    def setNAME(self,name):
        self.get().NAME = name
        
    @staticmethod
    def get_properties(self,context):
        
        nodetreeID = self.id_data.bpid
        
        try:
            data = runtimedata.RuntimeData.runtime.getCachePROP()[nodetreeID]
            #print ("USING CACHED DATA")
            return data
        except:
            try:
                props = runtimedata.RuntimeData.runtime.getProperties(self.id_data)
                
                result = []
                for prop in props:
                    propName = prop.getName()
                    if (propName,propName,propName) not in result:
                        result.append( (propName,propName,propName) )
                    
    #             result = [ (prop.getName(),prop.getName(),prop.getName()) for prop in props ]
                
                runtimedata.RuntimeData.runtime.getCachePROP()[nodetreeID] = result
                return result
            except:
                print (traceback.format_exc())
                return []

    @staticmethod
    def set_property(self,context):
        logic = runtimedata.RuntimeData.runtime.getLogic(self.bpid)
        logic.setNAME(self.enumNAME)
        
        
####################### REGISTRY ######################################################################################        

def custom_register():
    print("START CUSTOM STUFFY")
    runtimedata.Clazzes.classes["StateRef"].enumSTM=bpy.props.EnumProperty(items=StateRef.get_statemachines(),update=StateRef.set_statemachine)
    runtimedata.Clazzes.classes["StateRef"].enumSTATE=bpy.props.EnumProperty(items=StateRef.get_states(),update=StateRef.set_state)
    runtimedata.Clazzes.classes["StateMachine"].enumSTATE=bpy.props.EnumProperty(items=StateMachine.get_states,update=StateMachine.set_state)


#    StateRef.enumSTM=bpy.props.EnumProperty(items=StateRef.get_statemachines(),update=StateRef.set_statemachine)
#    StateRef.enumSTATE=bpy.props.EnumProperty(items=StateRef.get_states(),update=StateRef.set_state)
#    StateMachine.enumSTATE=bpy.props.EnumProperty(items=StateMachine.get_states,update=StateMachine.set_state)

    runtimedata.Clazzes.classes["PropertyGet"].enumNAME = bpy.props.EnumProperty(items=PropertyGet.get_properties,update=PropertyGet.set_property)

    runtimedata.Clazzes.classes["AnimationPlayer"].enumNAME = bpy.props.EnumProperty(items=AnimationDefinition.get_animations,update=AnimationPlayer.set_animation)
   # bpy.types.AnimationPlayer.enumNAME = bpy.props.EnumProperty(items=[]);

    print("DONE RUNTIMEMAPPINGS REGISTER")
    
def custom_unregister():
    pass

# nr = runtimedata.RuntimeData.noderuntime = BlenderRuntime()
# nr.processNodetrees("Gamekit")
# 
# ## CHECK calls
# nr.callValidation()                
# nr.callStartup()
# 
# testSTM = bpy.data.node_groups["GuyLogic"].nodes["GUY_STM"]
# nctx = nr.getLogic(testSTM)
# print(str(nctx))
# print("GROUP GRUP")
# nr.callGroupFunc(["StateMachine"],"update")
# print("TICK")
