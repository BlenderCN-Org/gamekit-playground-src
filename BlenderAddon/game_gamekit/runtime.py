import bpy
import runtimedata
import traceback
from bpointers import *

# organisation class to add logic-classes to all kinds of data
class Runtime():
    def __init__(self):
        print ("NEW RUNTIME!!!")
        self.ELEMENTS={} # keep elements by object
        self.GROUPS={} # group elements in lists together
        
        self.UPDATE_ELEMENTS = { "tick" : [], "low_priority" : []}
        
        self.CUSTOMDATA = {}
        self.STARTUPS = []
    def dispose(self):
        #nothing to do,yet. just let it garbage-collected
        self.ELEMENTS={} # keep elements by object
        self.GROUPS={} # group elements in lists together
        
        self.UPDATE_ELEMENTS = { "tick" : [], "low_priority" : []}
        
        self.CUSTOMDATA = {}
        
        pass
    
    def deleteCaches(self):
        # IMPLEMENT THIS IS SUBCLASSES
        pass
    
    def getStartups(self):
        return self.STARTUPS
    def clearStartups(self):
        self.STARTUPS=[]
        
    def getLogic(self,element):
        #print("GET CONTEXT:"+str(element))
        if element not in self.ELEMENTS:
            return None
        return self.ELEMENTS[element]

    def addToTickQueue(self, logic):
        if logic not in self.UPDATE_ELEMENTS["tick"]:
            self.UPDATE_ELEMENTS["tick"].append(logic)

    def removeFromTickQueue(self,logic):
        try:
            self.UPDATE_ELEMENTS["tick"].remove(logic)
        except:
            print(traceback.format_exc())
            
    def addToLowQueue(self,logic):
        if logic not in self.UPDATE_ELEMENTS["low_priority"]:
            self.UPDATE_ELEMENTS["low_priority"].append(logic)


    def tick(self,params=None):
        if bpy.data.worlds[0].gk_nodetree_debugmode:
            print("NO TICK CAUSE DEBUG-MODE")
            return
        #print("TICK..."+str(len(self.UPDATE_ELEMENTS)))
        # first execute the logic of the tick-queue
        for logic in self.UPDATE_ELEMENTS["tick"]:
            if not params:
                params={}
                
            logic.update(params)
        #print("...TACK")
        #todo lowpriority coroutine
            

    # iterate over all nodes and check if there is a startup-hook-set
    def callStartup(self,paramMap=None,elements=None):
        print("CALL STARTUP")
        self.callFunc("startup",paramMap,elements)
        
        
        
    def callValidation(self,paramMap=None):
        print("CALL VAL")
        self.callFunc("validation",paramMap)

    def callGroupFunc(self,groupNames,funcName,paramMap=None):
        for groupName in groupNames:
            for elem in self.GROUPS[groupName]:
                print("ELEM:"+str(elem))
                try:
                    if hasattr(elem,funcName):
                        func = getattr(elem,funcName)
                        func(paramMap)
                    else:
                        print("NO FUNC:"+funcName)
                except:
                    print(traceback.format_exc())

                
    def callFunc(self,funcName,paramMap=None,elements=None):
        if not elements:
            elements = self.ELEMENTS.values()
            
        print("CALL FUNC! "+str(self.ELEMENTS.keys()))
        for elem in elements:
            print("ELEM:"+str(elem))
            try:
                if hasattr(elem,funcName):
                    func = getattr(elem,funcName)
                    func(paramMap)
                else:
                    print("NO FUNC:"+funcName)
            except:
                print(traceback.format_exc())

    # adds an element and returns its context
    # if the element already is present it returns also the context (TODO: is this the behaviour what i want?)                    
    def addElement(self,elem,typeName,group=None):
        try:
            if elem.bpid in self.ELEMENTS:
                return self.ELEMENTS[elem.bpid]
            # get the class
            type = eval(typeName)
            #print("FOUND TYPE:" + typeName +" => "+str(type))
            # create an object
            logic = type(self,elem,typeName)
            print("!")
            self.ELEMENTS[logic.getID()]=logic
            if group is not None:
                self.group = group
                if group not in self.GROUPS:
                    grp = []
                    self.GROUPS[group]=grp
                else:
                    grp = self.GROUPS[group]
                
                if logic not in grp:
                    grp.append(logic)
                    
            self.STARTUPS.append(logic)
            return logic            
        except:
            print(traceback.format_exc())
            pass
        
    def removeElement(self,elem):
        logic = self.getLogic(elem.bpid)
        if not logic:
            print("COULDNT FIND LOGIC:"+str(elem))
            
        if logic.getID() in self.ELEMENTS:
            del self.ELEMENTS[logic.getID()]
            print ("REMOVED FROM ELEMENTS")
        try:
            self.GROUPS[logic.typeName].remove(logic)    
        except:
            pass
                
        try:
            self.UPDATE_ELEMENTS["tick"].remove(logic)
        except:
            pass

        try:
            self.UPDATE_ELEMENTS["low_priority"].remove(logic)
        except:
            pass        
        
        logic.close()    
        logic.invalidate()        
            
        
class BlenderRuntime(Runtime):
    
    def __init__(self):
        super().__init__()
        self.CUSTOMDATA["statemachines"]={'global':[]}
        self.CUSTOMDATA["properties"]={} # sorted by nodetree
        self.CUSTOMDATA["animations"]={}
        self.initCache()
        
    def deleteCaches(self):
        self.initCache()
        
    def initCache(self):
        self.CUSTOMDATA["cacheSTM"]={}
        self.CUSTOMDATA["cacheSTATE"]={}
        self.CUSTOMDATA["cacheProp"]={}
        self.CUSTOMDATA["cacheAnim"]={}
        
    def getCacheSTM(self):
        return self.CUSTOMDATA["cacheSTM"]
    
    def getCacheSTATE(self):
        return self.CUSTOMDATA["cacheSTATE"]    

    def getCachePROP(self):
        return self.CUSTOMDATA["cacheProp"]
    
    def getCacheANIM(self):
        return self.CUSTOMDATA["cacheAnim"]
    
    def addAnimation(self,anim):
        nodetree = anim.getData("nodetree")
        if not nodetree.ID in self.CUSTOMDATA["animations"]:
            self.CUSTOMDATA["animations"][nodetree.ID]=[]
            
        treeAnims = self.CUSTOMDATA["animations"][nodetree.ID]
        if anim not in treeAnims:
            treeAnims.append(anim)
            print("APPEND DATA "+str(anim)+" to nt:"+str(nodetree.ID))
            try:
                del self.getCacheANIM()[nodetree.ID]
            except:
                print(traceback.format_exc())
        else:
            print("PROPBLE;!")
    
    def removeAnimation(self,anim):
        try:
            nodetree = anim.getData("nodetree")
            treeAnims = self.CUSTOMDATA["animations"][nodetree.ID]
            treeAnims.remove(anim);
        except:
            print(traceback.format_exc())

    def getAnimations(self,rawNodetree):
        #print("GET PROPS FROM NODETREE:"+rawNodetree.name)
        return self.CUSTOMDATA["animations"][rawNodetree.bpid]    
    
    def addProperty(self,prop):
        nodetree = prop.getData("nodetree")
        if not nodetree.ID in self.CUSTOMDATA["properties"]:
            self.CUSTOMDATA["properties"][nodetree.ID]=[]
            
        treeProps = self.CUSTOMDATA["properties"][nodetree.ID]
        if prop not in treeProps:
            treeProps.append(prop)
            print("APPEND DATA "+str(prop)+" to nt:"+str(nodetree.ID))
        else:
            print("PROPBLE;!")

    def getProperties(self,rawNodetree):
        #print("GET PROPS FROM NODETREE:"+rawNodetree.name)
        return self.CUSTOMDATA["properties"][rawNodetree.bpid]
    
    def getStatemachine(self,tree,stmname):
        stms = self.CUSTOMDATA["statemachines"]
        
        
        if tree=="global":
            treeID = "global"
        elif tree.ID not in stms:
            return None
        else:
            treeID = tree.ID
        
#        print("getstatemachine tree-id:"+str(treeID)+" elem:"+str(type(tree.get().name)))
        
        treestms = stms[treeID]
        print("MACHINES:"+str(treestms))
        print("SEARCH:"+str(stmname))
        for tsm in treestms:
            if tsm.elem.get().NAME == stmname:
                print("FOUND STM:"+str(tsm))
                return tsm
        print("FAIL")    
        return None

    def getGlobalStatemachine(self,stmname):
        return self.getStatemachine("global",stmname)

    def removeStatemachine(self,stmLogic):
        nodetree = stmLogic.getData("nodetree")
        try:
            self.CUSTOMDATA["statemachines"][nodetree.getID()].remove(stmLogic)
        except:
            try:
                self.CUSTOMDATA["statemachines"]["global"].remove(stmLogic)
            except:
                print(traceback.format_exc())
        
    def addStatemachine(self,stmLogic,is_global):
        print("ADD STATEMACHINE WITH BPID:"+str(stmLogic.getID()))

        nodetree=stmLogic.getData("nodetree")
        nodetreeID = nodetree.getID()
        
        if is_global:
            where="global"
        else:
            where = nodetreeID
            
        stms = self.CUSTOMDATA["statemachines"]
        if where not in stms:
            stms[where]=[]
        
        if stmLogic not in stms[where]:
            stms[where].append(stmLogic)
            print("ADDED STM")
        #check if we need to delete from the (non)global list
        try:
            if is_global and nodetreeID in stms and stmLogic in stms[nodetreeID]:
                stms[nodetreeID].remove(stmLogic)
                print("REMOVED FROM NODETREE")
            elif not is_global and stmLogic in stms["global"]:
                stms["global"].remove(stmLogic);
                print("REMOVED FROM GLOBAL")
        except:
            print(traceback.format_exc())
        
    
    def processNodetrees(self,treetype=None):    
        for nodetree in bpy.data.node_groups:
            if not treetype or nodetree.rna_type.identifier==treetype:
                print("Process NODETREE:" + nodetree.name)
                
                if nodetree.name[:6]=="debug_" or nodetree.name[:6]=="zz____": # skip debug-trees
                    continue
                
                bpNodetree = BPointer.create(nodetree) 
                for node in nodetree.nodes:
                    bpNode = BPointer.create(node) 
                    
                    nodetype = node.rna_type.identifier
                    
                    try:
                        logic =self.addElement(node,nodetype,nodetype) # add element to Runtime=> give it NodeLogic from the typename and groups it using the nodetype
                        if logic:
                            logic.setData("nodetree",bpNodetree)
                    except:
                        print(traceback.format_exc())
        
            
                        
                        

class LogicElement():
    
    def __init__(self,runtime,elem,typeName):
        print("CREATE LOGIC:"+str(elem))
        self.runtime = runtime
        try:
            self.elem = BPointer.create(elem)
            print("LOGICELEMENT  CREATED POINTER")
        except:
            print("LOGICELEMENT ERROR POINTER")
            self.elem = None
            print(traceback.format_exc())
            return
        
        self.elem.refreshPointer()
        self.typeName = typeName
        self.status = []
        self.data = {}
    
    def get(self,forceRefresh=True):
        if not self.elem:
            raise ValueError("NO ELEMENT SET!!!")
        
        if forceRefresh:
            #self.elem.storeName()
            self.elem.refreshPointer()
            
        return self.elem.get()
    
    def getID(self):
        return self.elem.getID()
      
    def setData(self,key,value):
        self.data[key]=value
        #print( ("SET DATA %s => %s") % (str(key),str(value)) )
        
    def getData(self,key):
        try:
            return self.data[key]
        except:
            return None
        
    def validation(self):
        print ("DEFAULT VALIDATION")
        return True
    
    def invalidate(self):
        try:
            self.elem.invalidate()
        except:
            print(traceback.format_exc())
        pass
    
    def processInput(self,data):
        if not data:
            return
        
        print("PROCESS INPUT:")
        for k,v in data.items():
            print( ("%s => %s") % (str(k),str(v)))
            self.setData(k,v) 

    def startup(self,paramMap):
        self.processInput(paramMap)
    
    def close(self):
        pass
    
    def update(self,paramMap):
        pass
    
    def onIssue(comment):
        status.append(comment)



class Cache():
    def __init__(self,cacheTime):
        self.time = 0
        self.data = None
        self.cacheTime = cacheTime
        
    def isValid(self):
        return self.data!=None and time() < self.time
    
    def setData(self,data):
        self.data = data
        self.time = self.time() + self.cacheTime
        
    def getData(self):
        return self.data
#######################################################################################################################
#        MAPPINGS
#######################################################################################################################
from runtimeMappings import *

# nr.tick()