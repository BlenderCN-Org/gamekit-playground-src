import bpy

bpy.types.World.IDCOUNTER = bpy.props.IntProperty(default=1)

bpy.types.NodeTree.bpid = bpy.props.IntProperty(default=-1)
bpy.types.Node.bpid = bpy.props.IntProperty(default=-1)

class BPointer:
    BPOINTERS = {}
    
    @staticmethod
    def create(elem):
        if not hasattr(elem,"bpid"):
            raise ValueError( ("Type: %s doesn't have a property bpid!") % (str(type(elem))))
        
        try: # check if there is already one element there
            lookup = BPointer.getByID(elem.bpid)
            return lookup
        except:
            pass
        
        if isinstance(elem,bpy.types.Node):
            print("CREATE BPNode:"+elem.name)
#             result = BPNode(elem.bpid,elem.id_data.name)
            result = BPNode(elem)
        elif isinstance(elem,bpy.types.NodeTree):
            result = BPDatablock(elem,"node_groups")
            print("####CREATE BPDataBlock:"+elem.name);
        else:
            raise ValueError('BPointer.create: using unsupported btype:'+str(type(elem)))

        # create crosslink to this
        print("BPID:"+str(elem.bpid))
        return result
        
    @classmethod
    def getByID(cls,id):
        return cls.BPOINTERS[id]
    
    @classmethod
    def nextID(cls):
        id = bpy.data.worlds[0].IDCOUNTER
        bpy.data.worlds[0].IDCOUNTER = id + 1
        return id
    
    
    @classmethod
    def refreshAll(cls):
        for bptr in cls.BPOINTERS.values():
            bptr.refreshPointer()
        

    def __init__(self,elem):
        self.bobj=elem
        if self.bobj.bpid==-1:
            self.bobj.bpid = BPointer.nextID()

        self.ID = self.bobj.bpid
        self.refreshPointer()
        BPointer.BPOINTERS[self.ID]=self
        self.isValid = True
    

    def __del__(self):
        global BPOINTERS
        self.invalidate()
        print ("REMOVE POINTER")

    def invalidate(self):
        del BPointer.BPOINTERS[self.ID]
        self.ID = -1
        self.isValid = False
        self.bobj.bpid = -1
        self.bobj=None
    
    def refreshPointer(self):
        container = self.getContainer()
        
        #print ("GOT CONTAINER:"+str(container))
        #print("REFRESH POINTER:"+str(self.ID))

#         if (self.bobj and self.bobj.bpid==-1):
#             bobj = None
#             return
        
        newVersion = None
        
        try:
            for item in container:
                if item.bpid == self.ID:
                    newVersion = item;
                    break;
                 
            if newVersion:
                #print("FOUND")
                self.set(newVersion)
            else:
                print("NOT FOUND")
                self.set(None)
            #print("new prt:"+str(self.get().as_pointer())+" ID:"+str(self.ID))
        except:
            pass
        
    def getID(self):
        return self.ID
      
    def get(self):
        try:
            return self.bobj
        except:
            return None 
    
    def set(self,newObj):
        self.bobj=newObj
    
    def getContainer(self):
        # need to implement that
        return None

    @staticmethod
    def printPTRS():
        print("BPOINTERS:"+str(BPointer.BPOINTERS))
        
        

class BPDatablock(BPointer):
    def __init__(self,elem,containerName):
        self.containerName = containerName
        super().__init__(elem)

    def getContainer(self):
        return eval("bpy.data."+self.containerName)
    

class BPNode(BPointer):
    def __init__(self,elem):
#    def __init__(self,currentID,nodetreeName):
        nodetreeName = elem.id_data.name
        #elem.bpid,elem.id_data.name
        self.nodetreeName = nodetreeName
        super().__init__(elem)

    def getContainer(self):
        try:
            return eval( ("bpy.data.node_groups['%s'].nodes") % (self.nodetreeName) )
        except:
            return None
    