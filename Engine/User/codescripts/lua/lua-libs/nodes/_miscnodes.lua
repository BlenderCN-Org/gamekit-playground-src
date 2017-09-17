FileNode = BaseClass(NodeLogic)

function FileNode:constructor()
  self.filename = self.node:getProperty("File"):getString() or self.node:getProperty("FILE"):getString()

  if not self.filename or self.filename=="" or self.filename=="null" then
    error("ERROR FileNode: Invalid Filename:"..self.filename.." in node:"..self.node:getName())
  end
  Gk.log("FileName:"..self.filename)
  
  if (self.filename:startsWith("//")) then
    self.filename = self.filename:sub(3);
    Gk.log("New FontName:"..self.filename)
  end
end
NodeTreeLogic.addLogic("Gamekit","FileNode",FileNode)


-- 
-- DEACTIVATED! USing cpp
AnimationNode = BaseClass(NodeLogic)

function AnimationNode:constructor()
  self.IN_ENABLED = self.node:getInSocket("ENABLED"):getVar()
  self.IN_ENABLED_RAW = self.node:getInSocket("ENABLED")
  
  self.IN_OBJECT = self.node:getInSocket("OBJECT")
  self.OUT_ENABLED = self.node:getOutSocket("ENABLED"):getVar()
  self.OUT_DONE = self.node:getOutSocket("DONE"):getVar()
  self.OUT_DONE:setBool(false)
  self.OUT_TRIGGERS = self.node:getOutSocket("TRIGGERS"):getVar()
  
  self.ANIM_NAME = self.node:getProperty("ANIM_NAME"):getString()
  self.MODE = math.pow(2,self.node:getProperty("MODE"):getInt())
  self.SPEED = self.node:getProperty("SPEED"):getFloat()
  self.RESET_ON_STOP = self.node:getProperty("RESET_FRAME"):getBool()
  self.BLEND = self.node:getProperty("BLEND"):getInt()
  
  self.FRAME_CUSTOM = self.node:getProperty("FRAME_CUSTOM"):getBool()
  self.FRAME_START = self.node:getProperty("FRAME_START"):getInt()
  self.FRAME_END = self.node:getProperty("FRAME_END"):getInt()
  
  self.playing = false
end

function AnimationNode:animInit()
  self.animationObjectName = self.IN_OBJECT:getVar():getString()
  
  if self.IN_OBJECT:isConnected() then
    self.animationObject = self.IN_OBJECT:getGameObject()
  else
    self.animationObject = self.node:getGameObject()
  end
  
--  if self.IN_OBJECT:isConnected() then
--    if self.animationObjectName and self.animationObjectName~="" then
--      self.animationObject = self.IN_OBJECT:getGameObject()
--    else
--      self.animationObject = self.node:getGameObject()
--    end
--  else
--    if self.animationObjectName and self.animationObjectName~="" then
--      if self.node:getGameObject():isGroupInstance() then
--        self.animationObject = self.node:getGameObject():getGroupInstance():getElementByName(self.animationObjectName)
--      end
--      if not self.animationObject then
--        self.animationObject = scene:getObject(self.animationObjectName)
--      end
--    end    
--  end
  if not self.animationObject then
    self:error("Couldn't locate object:"..self.animationObjectName)
  end

  Gk.log("AnimObj:"..self.animationObject:getName())
  
  self.animation = self.animationObject:getAnimation(self.ANIM_NAME)
  if not self.animation then
    error("ERROR AnimationNode "..self.node:getName()..": couldn't find animation "..self.ANIM_NAME)
  end

  self.animation:setMode(self.MODE)
  self.animation:setSpeedFactor(self.SPEED)
  self.animation:setFrame(self.FRAME_START)
  self.animation:addEventListener(self,AnimationNode.onEvent)
  
  
  
end

function AnimationNode:onEvent(eventType,triggerNr)
  if (eventType==1) then
    self.OUT_DONE:setBool(true)
  elseif (eventType==0) then
    self.OUT_DONE:setBool(false)
  end
  Gk.log("AnimNode "..self.node:getName()..": Got animation event "..eventType.." triggerNr:"..tostring(triggerNr))
end

function AnimationNode:evaluate()
  if not self.isInitialized then
    self:animInit()
    self.isInitialized=true
  end
  
  if not self.animationObject then
    return false
  end
  
  if (self.IN_ENABLED_RAW:isDirty()) then
    print("DIRTY")
    print("DIRTY")
    print("DIRTY")
    print("DIRTY")
    print("DIRTY")
    print("DIRTY")
    self.IN_ENABLED_RAW:requestClearDirty()
  end
  
  local enabled = self.IN_ENABLED:getBool()
  --dPrintf("ANIMATION-NODE:"..self.ANIM_NAME.." enabled:"..tostring(enabled).." playing:"..tostring(self.playing))  
  if (enabled and not self.playing) then
    --Gk.log(self.animationObject:getName().." ANIMATION-NODE:"..self.ANIM_NAME.." enabled:"..tostring(enabled).." playing:"..tostring(self.playing))  
    self.playing = true
    self.animationObject:playAnimation(self.animation,self.BLEND,self.RESET_ON_STOP)
    --Gk.log("PLAY:"..self.ANIM_NAME)
    self.OUT_ENABLED:setBool(true)
    
  elseif (not enabled and self.playing) then
    self.animation:reset()
    game = game or DefaultGame()
    local me = self
    game:addOnceQueue(function() 
      me.animationObject:stopAnimation(me.ANIM_NAME)
      me.playing = false
      me.OUT_ENABLED:setBool(false)
    end)
  end
  
  if (enabled and self.playing and self.FRAME_CUSTOM) then
    local frame = self.animation:getFrame()
    if (frame >=self.FRAME_END) then
      if (self.MODE==1) then
        self.animation:setFrame(self.FRAME_START)
      elseif (self.MODE==2) then
        self.animation:setEnabled(false)
        self.playing = false
        self.OUT_ENABLED:setBool(false)
        self.OUT_DONE:setBool(true)
      elseif (self.MODE==4) then
        Gk.log("TODO: Reverse does not work yet")
      end
    end
  end
  
  return enabled 
end

function AnimationNode:update(tick)
  --Gk.log("Animation:"..self.ANIM_NAME.." "..self.animation:getFrame())

end
--NodeTreeLogic.addLogic("Gamekit","AnimationNode",AnimationNode)


DeltaNode = BaseClass(NodeLogic)

function DeltaNode:constructor()
  self.TYPE = self.node:getProperty("TYPE"):getInt()
  self.DAMPING = self.node:getProperty("DAMPING"):getFloat()
  self.ENABLED = self.node:getInSocket("ENABLED"):getVar()
  self.FOLLOW_OBJ_RAW = self.node:getInSocket("FOLLOW_OBJ")
  self.TARGET_OBJ_RAW = self.node:getInSocket("TARGET_OBJ")
end

function DeltaNode:updateInputData(clearCache)
  self.followObj = self.FOLLOW_OBJ_RAW:getGameObject()
  if (not self.followObj) then
    self:error("No valid follow-obj:"..tostring(self.FOLLOW_OBJ_RAW:getVar():getString()))
  end
  
  self.targetObj = self.TARGET_OBJ_RAW:getGameObject()
  if (not self.targetObj) then
    self:error("No valid follow-obj:"..tostring(self.TARGET_OBJ_RAW:getVar():getString()))
  end

  Gk.log("FollowObj:"..self.followObj:getName())
  Gk.log("TargetObj:"..self.targetObj:getName())
  if clearCache then
    if self.TYPE == 0 then
      self.lastFollowPos = self.followObj:getWorldPosition()
      self.targetPos = self.targetObj:getWorldPosition()
    end 
  end
end

function DeltaNode:initialize()
  self.initalized=false
end
function DeltaNode:evaluate()
  local result = self.ENABLED:getBool()
  return result 
end

function DeltaNode:update(tick)
  if not self.initialized then
    self:updateInputData(true)
    self.initialized=true
  end
  
  if (self.TYPE == 0) then
    local delta = self.followObj:getWorldPosition() - self.lastFollowPos
    self.targetPos = self.targetPos + delta
    self.currentPos = self.targetObj:getWorldPosition()
    self.newPos = Gk.lerp(self.currentPos,self.targetPos,self.DAMPING)    
--    Gk.log("DELTANODE:"..self.TYPE.." : "..self.DAMPING.." targetpos:"..tostring(self.newPos).." "..self.targetObj:getName())
    self.targetObj:setPosition(self.newPos)
    self.lastFollowPos = self.followObj:getWorldPosition()
  end
end
NodeTreeLogic.addLogic("Gamekit","DeltaNode",DeltaNode)


EngineNode = BaseClass(NodeLogic)

function EngineNode:constructor()
    self.IN_SHUTDOWN = self.node:getInSocket("SHUTDOWN")
    self.IN_DEBUG_SERVER = self.node:getInSocket("DEBUG_SERVER")
    self.IN_CONSOLE = self.node:getInSocket("SHOW_CONSOLE")
    self.IN_OSTEXT = self.node:getInSocket("SHOW_ONSCREENTEXT")
    
    self.engine = Gk.Engine()
    self.blenderServerRunning=false
    self.consoleVisible=false
    self.first = true
end

function EngineNode:initialize()
end


 

function EngineNode:update(tick)
    if (self.first) then
        self.first = false
        dPrintf("%s %s %s %s"   ,tostring(self.IN_SHUTDOWN:getVar():getBool())
                                ,tostring(self.IN_DEBUG_SERVER:getVar():getBool())
                                ,tostring(self.IN_CONSOLE:getVar():getBool())
                                ,tostring(self.IN_OSTEXT:getVar():getBool()))
    end
    
    if (self.IN_OSTEXT:isDirty()) then
        dPrintf("SHOW TEXT:"..tostring(self.IN_OSTEXT:getVar():getBool()))
        Gk.infoShow(self.IN_OSTEXT:getVar():getBool())
        self.IN_OSTEXT:requestClearDirty()
    end

    if (self.IN_SHUTDOWN:isDirty()) then
        if (self.IN_SHUTDOWN:getVar():getBool()) then
            Gk.log("REQUEST EXIT")
            self.engine:requestExit()            
        end
    end
    
    if (self.IN_DEBUG_SERVER:isDirty()) then
        dPrintf("DIRTY S")
        if (self.IN_DEBUG_SERVER:getVar():getBool()) then
            Gk.log("START SERVER")
            Gk.import(LUA_LIB_DIR .. "/debugger/_blenderServer.lua")
            startDebugServer()
            Gk.log("__END")
        else
            Gk.import(LUA_LIB_DIR .. "/debugger/_blenderServer.lua")
            stopDebugServer()
        end
        
        self.IN_DEBUG_SERVER:requestClearDirty()
    end
    
    if (self.IN_CONSOLE:isDirty()) then
        dPrintf("CONSOLE:"..tostring(self.IN_CONSOLE:getVar():getBool()))
        GSGUI = GSGUI or Gk.gsGUI3D()
        GSGUI:showConsole(self.IN_CONSOLE:getVar():getBool())        
        self.IN_CONSOLE:requestClearDirty()
    end
    

end
NodeTreeLogic.addLogic("Gamekit","EngineNode",EngineNode)
