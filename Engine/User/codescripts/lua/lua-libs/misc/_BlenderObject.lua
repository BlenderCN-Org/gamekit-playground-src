--! @file 
--! @brief Overview of BlenderObject and BObjectManager for Brick-Based LUA-Logic
--! @brief

BlenderObject = Class()

--! @brief Blender-Object the base-class for brick-based lua-logic.
-- It is **important** to call:  
-- `self:setMe(self) inside of your constructor`  
-- Otherwise your object's init,afterinit,afterinit2-methods will **not** be called  
-- You can set a blender game-property '*bgroup*' to set this object in a special group that you can get with the BObjectManager
function BlenderObject:constructor()
   self.__type="BlenderObject"
   -- do this to be able to have something like a ref to the uppest inheritance-layer which can be ref'd by self.me
   self:setMe(self)
   self:resetAfterInit()
   -- this is called from expression-logic in blender and the calling object is connected to this object
   expRegister(self)
   local bgroup = self:getProperty("bgroup",true)
   Gk.log("BObject:"..self.obj:getName().." grp:"..tostring(bgroup))
   BOManager:add(self,bgroup)
end



-- this is called after all logic-bricks are processed and therefore it is sure that all
-- BlenderObjects are mapped to their logic

--! @brief Overwrite this for the init-stage.
function BlenderObject:init()
end

--! @brief Overwrite this for the afterInit-stage.
function BlenderObject:afterInit()
end

--! @brief Overwrite this for the afterInit2-stage.
function BlenderObject:afterInit2()
end

--! @brief Helper-Call which have to be called from within your constructor in order to make init,afterinit,afterinit2 available for the BlenderObject-LifeCycle.
-- *TODO: This have to be done somehow else* 
function BlenderObject:setMe(me)
   local start = me
   start.me = me
   while (start.base) do
      start.base.me = me
      start = start.base
   end
end

function BlenderObject:resetAfterInit()
   self.me.__afterinitdone=false
   self.me.__afterinitdone2=false
end

--! @brief if this object is part of a group-instance you can get other objects of this groupinstance by name.
--! @param name : the name of the other object of this group-instance you want to get
function BlenderObject:getGroupElem(name)
   i = i or 0
   if self.obj:isGroupInstance() then
      return self.obj:getGroupInstance():getElementByName(name)
   end
   return nil
end

--! @brief get property from the object itself (optional also check the group-instance root for this property).
--! @param name the name of the property to get
--! @param includeGroupInstanceRoot first check if the group-instance-root have this property set if not check the object's properties 
function BlenderObject:getProperty(name,includeGroupInstanceRoot)
   local result = nil
   local grpInst = self.obj:getGroupInstance()
   if (includeGroupInstanceRoot 
         and self.obj:isGroupInstance()) then
      result = self.obj:getGroupInstance():getRoot():getProperty(name)
   end
   
   return result or self.obj:getProperty(name)
end


Gk.import(LUA_LIB_DIR .. "/misc/_utils.lua")


--! @brief Managment for BlenderObjects-Instances.
-- @section BObjectManagerSection
BObjectManager = Class()

function BObjectManager:constructor()
   -- todo: make table weak-ref:
   self.objects={}
   self.groups={}
   local mmgr = Gk.MessageManager()
   local me = self
--   mmgr:addListener("internal","","__logicmanager_afterfirst",function()
--      me:processAfterInit()
--   end)
end

function BObjectManager:add(bobj,group)
   self.objects[bobj.obj:getName()]=bobj.me or bobj
   
   self:requestAfterInitProcess()
   
   if (group) then
      local grp = self.groups[group] 
      if not grp then
         grp = {}
         self.groups[group] = grp
      end
   
      grp[bobj.obj:getName()]=bobj.me or bobj      
   end
end

function BObjectManager:requestAfterInitProcess()
   local me = self
   if not self.isAfterInitRequested then
      self.isAfterInitRequested = true
      game = game or DefaultGame()
      game:addOnceQueue(function()
         me:processAfterInit()
         me.isAfterInitRequested=false
      end)
   end
end

--! @brief get object-group.
-- Get a group of objects that had their blender game property *bgroup* set to this groupName
--! @param groupName : get group of objects that are registered under this groupName
function BObjectManager:getGroup(groupName)
   return self.groups[groupName]
end

--! @brief Start the BlenderObject lifecycle for all registered objects or optional only objects registered in a group.
--! @param group : the name of the group or nothing
function BObjectManager:processInit(group)
   local grp = self.groups[group] or self.objects
   for k,bobj in pairs(grp) do
      if (bobj.me.init and type(bobj.me.init)=="function") then
         bobj.me:init()
      end
   end
end

function BObjectManager:processAfterInit(group)
   local grp = self.groups[group] or self.objects
   
   for k,bobj in pairs(grp) do
      if (
            bobj.obj:isInstanced() and 
            not bobj.me.__afterinitdone and bobj.me.afterInit and type(bobj.me.afterInit)=="function") then
         bobj.me:afterInit()
         bobj.me.__afterinitdone = true
      end
   end
   for k,bobj in pairs(grp) do
      if (
         bobj.obj:isInstanced() and not bobj.me.__afterinitdone2 and bobj.me.afterInit2 and type(bobj.me.afterInit2)=="function") then
         bobj.me:afterInit2()
         bobj.me.__afterinitdone2 = true
      end
   end   
end

--! <HR/>

BOManager = BObjectManager()

--! @brief Helper-Function to traverse of the scenegraph
--! @param startnode : the object to start the traversal with. (starting with the children)
--! @param func : the function the traversed objects will be called with
--! @param ctx : a context-object that can be bypassed and can carry results
--  
function boVisitChildren(startnode,func,ctx)
   for i=0,startnode:getChildCount()-1 do
      local child = startnode:getChildAt(i)
      func(child,ctx)
      boVisitChildren(child,func,ctx)
   end
end
--! @brief Helper-Function to find children with a certain BlenderObject-Type
--! @param startNode : the object to start with (the children)
--! @param type : the BlenderObject-Type
function boFindType(startNode,type)
   local ctx={}
   boVisitChildren(startNode,function(obj,ctx) 
      Gk.log("Check:"..obj:getName())
      local logic = _obj[obj]
      if (logic and logic.me and logic.me.__type and logic.me.__type==type) then
         table.insert(ctx,{obj=obj,logic=logic.me})
      end
   end,ctx)
   return ctx
end

-------------------- Brick helpers----------------------

-- _obj is the mastermap of all BlenderObject<->BlenderBrick mappings ! TODO: Change the name
_obj = {}
_config = { debug = {
    error = true,
    warning = true,
    info  = true
  }
}
function expCreateIfNew(clazz)
  -- check if there is an instance for this object already in the object-mapping
  local obj = expObj()
  if expObj()==nil then
    obj = clazz() 
  end
  return obj
end

function expRegister(clazz)
    clazz.cont = Gk.getCurrentController()
    local owner = clazz.cont:getOwner()    
    -- write the owner in the clazz
    clazz.obj = owner
    _obj[owner] = clazz
     --info("Registered %s",clazz.obj:getName())
end

function expUnregister(obj)
  if (obj==nil) then
    if (Gk.getCurrentController()==nil) then
      error("Tried to unregister, but there is neither an object as attribute nor that we are inside!")
    else
      obj=Gk.getCurrentController():getOwner()
    end
  end
  
  --info("Try to unregister! Before #%i",table.length(_obj))
  _obj[obj]=nil
  --info("Try to unregister! After #%i",table.length(_obj))
end



function expObj(clazz,...)
    local owner = Gk.getCurrentController():getOwner()
  local result =  _obj[owner];
    if (clazz and not result) then
      result = clazz(...)
    end
    return result
end

function expObjInit(clazz,...)
   local result = expObj(clazz,...)
   if (result.me and result.me.init) then
      Gk.log("init:"..result.obj:getName())
      result.me:init()
   elseif (result and result.init) then
      Gk.log("init:"..result.obj:getName())
      result:init()
   end
end


function expParent()
    local owner = Gk.getCurrentController():getOwner()
  if (owner~=nil and owner:getParent()~=nil) then
      return _obj[owner:getParent()];
  end
  return nil;
end

function expChild(name)
    local owner = Gk.getCurrentController():getOwner()
  if (owner~=nil and owner:getChildByName(name)~=nil) then
      return _obj[owner:getChildByName(name)];
  end
  return nil;
end

function expGrpElem(name)
    local owner = Gk.getCurrentController():getOwner()
  if (owner:isGroupInstance()) then
    local grpElem = owner:getGroupInstance():getElementByName(name)
    if (grpElem~=nil) then
      return _obj[grpElem];
    end
  end
  return nil;
end

function expIsPositive(sensorName)
    return Gk.getCurrentController():getSensor(sensorName):isPositive()
end

function expIsNotPositive(sensorName)
    return Gk.getCurrentController():getSensor(sensorName):isPositive()==false
end

function expSensor(sensorName)
  return Gk.getCurrentController():getSensor(sensorName)
end

function expActuator(actuatorName)
  return Gk.getCurrentController():getActuator(actuatorName)
end

function expGetObj(gameobject)
  return _obj[gameobject]
end