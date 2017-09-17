scene = Gk.getActiveScene()

NodeTreeLogic = Class()

function NodeTreeLogic.addLogic(treetype,nodetype,logic)
    if not NodeTreeLogic.objs then
        NodeTreeLogic.objs={}
    end

    if not NodeTreeLogic.logic then
        NodeTreeLogic.logic={}
    end
    if not NodeTreeLogic.logic[treetype] then
        NodeTreeLogic.logic[treetype] = {}
    end
    if NodeTreeLogic.logic[treetype][nodetype] then
        Gk.log("WARNING: You are overwriting nodelogic for "..treetype.."/"..nodetype)
    end
    NodeTreeLogic.logic[treetype][nodetype] = logic
end

function NodeTreeLogic.getLogic(obj)
    return NodeTreeLogic.objs[obj]
end

function NodeTreeLogic.wrap(nodetree,treetype)
    if not nodetree then
        error("You need to specify a nodetree with the NodeTreeLogic-wrap!")
    end


    local treetype = treetype or nodetree:getType()    
    dPrintf("Wrapping Type:%s",treetype)
    if (not NodeTreeLogic.logic or not NodeTreeLogic.logic[treetype]) then
        Gk.log("WARNING: Tried to wrap nodetree:"..nodetree:getName().." but treetype:"..treetype.." has no logic attached!?")
        Gk.log("Skipping!")        
        return
    end

    local result = {
      initObjs = {},
      afterInitObjs = {}
    }

    -- get the logic-classes from here
    local treeLogic = NodeTreeLogic.logic[treetype]
    
    -- check all nodes and wrap them if there is NodeLogic for this Node-Type
    for i=0,nodetree:getNodeAmount()-1 do
        local node = nodetree:getNodeAt(i)
        local nodeType = node:getType()
        Gk.log("Check for Type:"..nodeType)
        if treeLogic[nodeType] then
            Gk.log("Found!")
            -- create new logic instance
            local newLogicInstance = treeLogic[nodeType]{node=node}
            -- store it in the instance-container
            NodeTreeLogic.objs[node]=newLogicInstance
            if (newLogicInstance.init and type(newLogicInstance.init)=="function") then
                table.insert(result.initObjs,newLogicInstance)
            end
            if (newLogicInstance.afterInit and type(newLogicInstance.afterInit)=="function") then
                table.insert(result.afterInitObjs,newLogicInstance)
            end
            newLogicInstance:attachToNodeLifeCycle()
        end  
    end    
    
    return result;
end

function NodeTreeLogic.init(treeResult)
    -- the logic that has an init-method will be called now
    for i,initObj in ipairs(treeResult.initObjs) do
        initObj:init()
    end
end

function NodeTreeLogic.afterInit(treeResult)
    -- the logic that has an afterInit-method will be called now
    for i,afterInitObj in ipairs(treeResult.afterInitObjs) do
        afterInitObj:afterInit()
    end
end

NodeLogic = Class()

function NodeLogic:constructor(data)
    if not data or type(data)~="table" or not data.node then
        error("you need to pass node as table-value node=...")
    end
    
    self.node = data.node
    dPrintf("1")
end

-- do not call this in the constructor, since self is inside not the full object,yet
function NodeLogic:attachToNodeLifeCycle()
    if self.initialize then
        dPrintf("add init:%s",self.node:getName())
        self.node:setLuaInit(self,self.initialize)
    end
    if self.evaluate then
        dPrintf("add eval")
        self.node:setLuaEval(self,self.evaluate)
    end    
    if self.update then
        dPrintf("add update!")
        self.node:setLuaUpdate(self,self.update)
    end
end

function NodeLogic:getType()
    return self.node:getType()
end

function NodeLogic:checkNodeType(type)
    return self.node:getType() == type
end

function NodeLogic:getInSocket(socketname)
    return self.node:getInSocket(socketname)
end

function NodeLogic:getOutSocket(socketname)
    return self.node:getOutSocket(socketname)
end

function NodeLogic:getGameObject()
  return self.node:getGameObject()
end

function NodeLogic:getInSocketLogic(socketname)
    local socket = self.node:getInSocket(socketname):getFrom()
    if not socket then
        error("ERROR/getInSocketObject:Unknown socket with name:"..socketname.." in node:"..self.node:getName())
    end
    return NodeTreeLogic.getLogic(socket:getNode())
end

function NodeLogic:getFirstOutSocketLogic(socketname)
    local socket = self.node:getOutSocket(socketname):getOutSocketAt(0)
    if not socket then
        error("ERROR/getOutSocketObject:Unknown socket with name:"..socketname.." in node:"..self.node:getName())
    end
    return NodeTreeLogic.getLogic(socket:getNode())
end

function NodeLogic:error(output)
  error("ERROR "..self.node:getName()..": "..output)
end


LuaNode = BaseClass(NodeLogic)

function LuaNode:constructor()
    self.TYPE = self.node:getProperty("TYPE"):getString()

    if _G[self.TYPE] then
      self.typeInstance = _G[self.TYPE](self)
    else
      error("ERROR "..self.node:getName()..": Unknown TYPE("..self.TYPE..") for LuaNode!")
    end
end

function LuaNode:initialize()
  if (self.typeInstance.initialize) then
    self.typeInstance:initialize()
  end
end

function LuaNode:evaluate(tick)
    if (self.typeInstance.evaluate) then
      return self.typeInstance:evaluate(tick)
    end
    return true
end

function LuaNode:update(tick)
    if (self.typeInstance.update) then
      self.typeInstance:update(tick)
      return
    end
end


-- base-class for logic-types created by LuaNodes
LuaNodeLogic = Class()

function LuaNodeLogic:constructor(node)
  self.nodeLogic=node
end

function LuaNodeLogic:getNode()
  return self.nodeLogic.node
end


NodeTreeLogic.addLogic("Gamekit","LuaNode",LuaNode)


Gk.import(LUA_LIB_DIR.."/nodes/_miscnodes.lua")
Gk.import(LUA_LIB_DIR.."/nodes/_statenodes.lua")
Gk.import(LUA_LIB_DIR.."/nodes/_uinodes.lua")


function wrapAllTreesWithLuaNodes()
  for i=0,Gk.getLogicTreeAmount()-1 do
    local tree = Gk.getLogicTreeAt(i)
    wrapAllLogicNodes(tree:getName())
  end
end

function wrapAllLogicNodes(treename)
  treename = treename or "Gamekit"
  local scene = Gk.getActiveScene()
  local objList = scene:getObjectList()
  local nodeTreeResults = {}
  for i=0,objList:size()-1 do 
    local obj = objList:at(i)
    local logicTree = obj:getLogicTree()
    if (logicTree) then
      Gk.log("Wrapping:"..obj:getName())
--      local result = NodeTreeLogic.wrap(logicTree,treename)
      local result = NodeTreeLogic.wrap(logicTree)
      table.insert(nodeTreeResults,result)
      Gk.log("done")
    end
  end
  
  -- call all init-methods from all trees
  for i,r in ipairs(nodeTreeResults) do
    NodeTreeLogic.init(r)
  end
  -- call all afterInit-Methods from all trees
  for i,r in ipairs(nodeTreeResults) do
    NodeTreeLogic.afterInit(r)
  end
  
  Gk.log("finished weapAllLogicNodes()")
end


