Gk.import(LUA_LIB_DIR .. "/misc/_BlenderObject.lua")

Vehicle = Class()
--[[
   input.
      speed = currentSpeed, vehicle tries to steer to gain this speed
      maxspeed = maxspeed
      radius = radius of the object taken into acount for steering-calculations
      initObstacleCheck = create a swepttest for obstacle-avoidance [default:true]
      userObject = a back-link to the object that is using the vehicle
--]]
function Vehicle:constructor(input)
   self.__type="vehicle"
   input = input or {}
   self.obj = input.obj
   
   self.neighbourGroup = Gk.SteerGroup()
   self.neighbourList = {}  
   self.neighbourCheckRunning = false
   
   
      
   local speed = input.speed or 1.0
   local maxSpeed = input.maxspeed or 2.0
   if speed > maxSpeed then
      maxSpeed = speed
   end
   
   self.userObject = input.userObject
   
   self.sensors = input.sensors or {}
   
   self.radius = input.radius or 2.5

   self.goal = input.goal
   self.goalRadius = input.goalRadius or self.radius * 2

   self.mode = "unknown"

   self.os = Gk.SteerObject(self.obj,maxSpeed,speed)
   self.name = input.name or self.obj:getName()
   self.os:setRadius(self.radius)
   --self.os:setMaxSpeed(10)
   self:setSpeed(speed,true)
   
   if (self.sensors["obstacle"]) then
      self.obstacleCheck = Gk.SteerSweptObstacle(self.radius*3,self.radius,64,64)
      self.obstacleCheck:ignoreObject(self.obj)
   end
   
   VehicleContext.get():addVehicle(self)
end

function Vehicle:stopNeighbourThread()
   self.neighbourCheckRunning = false
end

function Vehicle:startNeighbourThread()
   if self.neighbourCheckRunning then
      return
   end
   
   self.neighbourCheckRunning = true
   
   local me = self
   self:coLogic(function()
      while me.neighbourCheckRunning do 
         local vehicles = VehicleContext.get():getVehGroup("guys")
         if not vehicles then
            break
         end
         me.neighbourList = {}
         for i,veh in ipairs(vehicles) do
            if veh ~= me then
               local distance = me.obj:getPosition():distance(veh.obj:getPosition())
               if (distance < config.guy.vehicle.maxDistance) then
                  table.insert(me.neighbourList,{veh=veh,distance=distance})
               end
--               coroutine.yield(waitMs( math.random() * 20 ))
               coroutine.yield()
            end
         end
         me.neighbourGroup:clear()
         for _,v in ipairs(me.neighbourList) do
            me.neighbourGroup:add(v.veh.os)
         end
         coroutine.yield()
     end
     Gk.log("Ended vehiclethread")
   end)
end

function Vehicle:getUserObject()
   return self.userObject
end

function Vehicle:setDrawCallback(obj,func)
   self.drawCallback ={obj=obj,func=func}
end

function Vehicle:setSpeed(s,force)
   force = force or false
   local maxSpeed = self.os:maxSpeed()
   if (s > maxSpeed) then
      if (force) then
         self.os:setMaxSpeed(s)
         self.os:setSpeed(s)
      else
         self.os:setSpeed(maxSpeed)
      end
   else
      self.os:setSpeed(s)
   end
   self.speed = s
end


function Vehicle:draw(ui)
    ui = ui or VehicleContext.get().UI
 --   UIDEBUG:drawObjectAxis(self.obj,10)
    ui:drawCircle(self.os:position(),self.os:radius())
    ui:drawArrow(self.os:position(),self.os:position()+self.os:velocity()*(self.os:speed()*5),Gk.Vector4(1.0,0.5,0.25,1.0))

    if (self.goal) then
      ui:drawCross(self.goal,1.0)
      ui:drawCircle(self.os:position(),self.goalRadius,5.0,Gk.Vector4(0,1,0.5,0.3))
    end
    
   if (not self.debugui) then
      self.debugui = {}
      self.debugui.panel = VehicleContext.get().GUI3D:createPanel(0,0,100,100,"debug-"..self.obj:getName(),"dejavu",false)
      
      local colors = self.debugui.panel:getSettings()
      colors.captionTextSize=9
      colors.captionBackgroundGradientStart=Gk.Vector4(1,1,1,0.2):toColor()
      colors.captionBackgroundGradientEnd=Gk.Vector4(1,1,1,0.3):toColor()
      
      self.debugui.info = self.debugui.panel:createCaption(0,0,100,10,"",8)
      self.debugui.speed = self.debugui.panel:createCaption(0,10,100,10,"",8)
      self.debugui.mode = self.debugui.panel:createCaption(0,20,100,10,"",8)
      self.debugui.oX = -50
      self.debugui.oY = 10
   end
   
   self.debugui.info:setText(self.obj:getName())
   self.debugui.speed:setText("Speed:"..math.round(self.os:speed(),2).."/"..self.os:maxSpeed())
   self.debugui.mode:setText("Mode:"..self.mode)
--   local vec2d = scene:getScreenPos(self.obj:getPosition())
   local vec2d = scene:getScreenPos(self.os:position())

   if (self.drawCallback) then
      self.drawCallback.func(self.drawCallback.obj,ui,self.debugui)
   end
   

   self.debugui.panel:setPosition(vec2d.x+self.debugui.oX,vec2d.y+self.debugui.oY,0)
--    UIDEBUG:drawLine(self.os:position(),self.os:position()+self.os:forward()*4,Gk.Vector3(0,0.5,0))
--    UIDEBUG:drawLine(self.os:position(),self.os:position()+self.os:up()*4,Gk.Vector3(0,0,0.5))
--    UIDEBUG:drawLine(self.os:position(),self.os:position()+self.os:side()*4,Gk.Vector3(0.5,0,0))
--   --UIDEBUG:drawLine(self.os:position(),self.os:position()+self.os:predictedPosition(1),Gk.Vector3(0,1,1))
--   
--   if (self.userObject and self.userObject.me.draw) then
--      self.userObject.me:draw()
--   end
end

function Vehicle:applyForce(force)
   if not force then
      error("you have to provide a force!")
   end
   
   local vel = self.os:velocity()
   
   local speedForce = self.os:steerForTargetSpeed(self.speed)

  -- Gk.log("SpeedForce:"..tostring(speedForce))

   local all = force + speedForce
   
   self.os:applySteeringForce(all,0.0166)
   
end

function Vehicle:getGoal()
   return self.goal
end

function Vehicle:setGoal(goal)
   self.goal = goal
end

function Vehicle:getGoalDistance()
   if (not self.goal) then
      return -1
   end
   
   local distance = self.obj:getPosition():distance(self.goal)
   return distance
end

function Vehicle:isInGoal()
   local distance = self:getGoalDistance()
   --Gk.log("Distance:"..distance.." Gr:"..self.goalRadius)
   return distance~=-1 and self:getGoalDistance() < self.goalRadius
end

function Vehicle:coLogic(func)
   VehicleContext.get():getDispatcher():call{
      call=func,
      onError=function(err)
         Gk.log("VehicleLogicError:"..self.obj:getName())
      end
   }
end
-----------------------------------------------------------------------------------
VehicleBase = BaseClass(BlenderObject)

function VehicleBase:constructor()
   self.__type="VehicleBase"

   --mandatory for extending-classes 
   self:setMe(self)
   
   
   local rootName = self:getProperty("rootName",true)
   
   
   local vehObj = nil
   
   if (rootName) then
      vehObj = self:getGroupElem(rootName) 
   
      if (not vehObj) then
         Gk.log("Couldnt find VehicleObject "..rootName.." for "..self.obj:getName())
         return
      end
   else
      vehObj = self.obj
   end
    
   -- TODO: Get rid of this. Too highlevel
   local sensors = {}
   local tempSensors = self:getProperty("sensors",true)
   if (tempSensors) then
      local vc = VehicleContext.get()
      local sensorNames = tempSensors:split(",")
      for i,sensorName in ipairs(sensorNames) do
         sensors[sensorName]=true
      end 
   end
   
   local radius = self:getProperty("radius",true) or 1
   local goalRadius = self:getProperty("goalradius",true) or radius * 2
   local speed = self:getProperty("speed",true) or 1
   local maxspeed = self:getProperty("maxspeed",true) or 2
   self.veh = Vehicle{radius=radius,obj=vehObj,userObject=self,
                      sensors=sensors,maxspeed=maxspeed,speed=speed,
                      goalRadius=goalRadius}

   self.targetName = self:getProperty("target",true)
   self.propSteerGroups = self:getProperty("steergroup",true)
   if (propSteerGroups) then
      local vc = VehicleContext.get()
      local groupNames = propSteerGroups:split(",")
      for i,groupName in ipairs(groupNames) do
         vc:addToSteerGroup(groupName,self.veh.os)
         Gk.log("added:"..self.obj:getName().." to group "..groupName)
      end 
   end
   
   self.propVehGroups = self:getProperty("vehgroup",true)
   if (self.propVehGroups) then
      local vc = VehicleContext.get()
      local groupNames = self.propVehGroups:split(",")
      for i,groupName in ipairs(groupNames) do
         vc:addToVehGroup(groupName,self.veh)
      end 
   end
   

end

--gets called when all logic is bound and starts the steering
--CAUTION: if overriding, do not to forget to call this via self.base:afterInit() or start by hand
function VehicleBase:afterInit()
   if self.targetName then
      self:setTarget(self.targetName)
   end
   Gk.log("AFTERINIT")
   
   self.vehGroup = VehicleContext.get()._allVeh
   -- start the coroutine which calls the steering
    self:start()
end

function VehicleBase:start()
   if self.running then
      return
   end

--   self.veh:startNeighbourThread()
   
   self.running = true
   local me = self

   -- one thread for ruuning
   self.veh:coLogic(function()
      Gk.log("In V-Update")
      while me.running do
         me.me:doSteering()
--         Gk.log("After steering!")
         coroutine.yield()
      end
      Gk.log("Ended steering!")
   end)
   
   self.veh:startNeighbourThread()
end

function VehicleBase:stop()
   self.running = false
end

function VehicleBase:setTarget(target)
   local obj = nil
   if type(target)=="string" then
      obj = scene:getObject(target)
   else
      obj = target
   end
   
   if (obj) then
      if(obj:isGroupInstance()) then
         local root = obj:getGroupInstance():getRoot()
         local logic = _obj[root]
         
         
         if logic then
            self.target = logic.me.veh.obj
            return
         else
         end
         local targetObj = obj:getGroupInstance():getElementByName("ai")
         if (targetObj) then
            self.target = targetObj
            if (_obj[obj]) then
               self.targetLogic = _obj[obj].me
            end
            Gk.log("("..target.." "..table.length(_obj)..")Found inner logic! "..obj:getName().." groupinstance:"..tostring(self.targetLogic))
         else
            self.target = nil
            Gk.log("ERROR couldn't find innerlogic!")
         end
      else
         self.target = obj
      end
   else
      error("Unknown seeker-target:"..tostring(target))
   end
end
-----------------------------------------------------------------------------------

Obstacle = BaseClass(BlenderObject)

function Obstacle:constructor()
   self.__type="Obstacle"
   self:setMe(self)
   self.name = self.obj:getName()
--   self.obs = Gk.SteerObstacle(self.obj)
   VehicleContext.get():addObstacle(self.name,self)
   Gk.log("Obstacle:"..self.name.." group:"..self.obj:getFilterGroup())
end


----------------------------------------------------------------
Path = BaseClass(BlenderObject)

function Path:constructor()
   self.navPath = Gk.NavPath()
   self.radius  = self:getProperty("radius",true) or 1
   self.name = self:getProperty("name",true) or self.obj:getName()
   Gk.log("Create Curve:"..self.name)
   self.navPath:createFromCurve(self.obj,self.radius)
   if (UIDEBUG) then
      UIDEBUG:drawCurve(self.obj)
   end
   self:addToContext()
end

function Path:addToContext()
   VehicleContext.get():addPath(self.name,self.navPath)
end

Marker = BaseClass(BlenderObject)

function Marker:constructor()
   self.name = self.obj:getName()
   VehicleContext.get():addMarker(self.name,self)
end


-----------------------------------------------------------------------------------------------------
PathWalker = BaseClass(VehicleBase)

function PathWalker:constructor()
   self.__type="PathWalker"
   local me = self
   self:setMe(self)
   self.lastVec = Gk.Vector3(0,0,0)
end

function PathWalker:afterInit()
   self.base:afterInit()
end

function PathWalker:afterInit2()
   local pathToWalk = self:getProperty("path",true)
   if (pathToWalk) then
      self.path = VehicleContext.get():getPath(pathToWalk)
   else
      self.path = Gk.NavPath()
   end
   
   local direction = tonumber(self:getProperty("direction",true) or 1)
   self:setDirection(direction)


end

function PathWalker:setDirection(dir)
   Gk.log("PathWalker:"..self.obj:getName().." PAth:"..self:getProperty("path",true))
   if (self.path:getPointAmount()==0) then
      return
   end
   
   if (dir == 1) then 
      self.veh:setGoal(self.path:getPoint(self.path:getPointAmount()-1))
      self.direction = 1
   else
      self.direction = -1
      self.veh:setGoal(self.path:getPoint(0))
   end
   Gk.log("Set direction:"..dir.." goal:"..tostring(self.veh:getGoal()))
end

function PathWalker:doSteering()
   if (self.path:getPointAmount()==0) then
      return
   end
--   Gk.log("__STARTED PATHWALKER")
   if (not self.direction) then
       self.direction = 1
   end

   if (self.veh:isInGoal()) then
      Gk.log("GOAL")
      if (self.direction == 1) then
         self:setDirection(-1)
      else
         self:setDirection(1)
      end
      
      --self.path:clear()
   end

   if (self.path:getPointAmount()==0) then
      Gk.log("Waiting for path!")
      coroutine.yield(function() return self.path:getPointAmount()>1 end)
      Gk.log("Had path:"..self.path:getPointAmount())
      
   end
   
   self:walkPath(self.path,self.direction,__currentDt)
--   Gk.log("__END PATHWALKER")
end

function PathWalker:walkPath(path,direction,dt)
   if (self.path:getPointAmount()==0) then
      return
   end
   
   if (math.random()>0.2) then
  --    Gk.log("LastVec:"..tostring(self.lastVec))
      
      self.veh:applyForce(self.lastVec)
      
      return self.lastVec
   end
   
   dt = dt or 0.01666
   direction = direction or 1
   local walkPath = self.veh.os:steerToFollowPath(direction,0.25,path)
   
   
   local avoidObstacle = nil
   local seperation = nil
   local avoid = nil
   local align = nil


   local all = walkPath
   
   if (self.veh.sensors["obstacle"]) then
      avoidObstacle = self.veh.os:steerToAvoidObstacle(self.veh.obstacleCheck,4)
   end
   
   if (self.veh.sensors["separation"]) then
      local vc = VehicleContext.get()
      seperation = self.veh.os:steerForSeparation(vc._allVeh,10)
   
      if  (seperation and not seperation:isNull(0.00001)) then
         all = all*0.35 + seperation * 0.65 
         Gk.log("SEPERATION")
      end
   end
   
   if (self.veh.sensors["avoid"]) then
      local vc = VehicleContext.get()
      avoid = self.veh.os:steerToAvoidNeighbours(vc._allVeh,1)
      if  (avoid and not avoid:isNull(0.00001)) then
         all = all*0.35 + avoid * 0.65 
         --Gk.log("AVOID")
      end      
   end
   
   if (self.veh.sensors["align"]) then
      local vc = VehicleContext.get()
      local alignGroup = vc:getSteerGroup("align")
      if alignGroup then
         align = self.veh.os:steerForAlignment(alignGroup,1)
         if  (avoid and not avoid:isNull(0.00001)) then
            all = all*0.35 + align * 0.65 
            Gk.log("ALIGN")
         end      
      end
   end
--   Gk.log("walkpath:"..tostring(all))





   if  (avoidObstacle and not avoidObstacle:isNull(0.00001)) then
      all:normalize()
      all = all*0.25 + avoidObstacle * 0.75 
   end
   
   self.lastVec = all
   
   self.veh:applyForce(all)
end

function PathWalker:draw(ui)
--   if (self.path:getPointAmount()>0) then
--      UIDEBUG:drawPath(self.path)
--   end
end

function PathWalker:gotoNavMesh(point,pathradius)
   pathradius = pathradius or self.veh.radius
   --wait for navmesh
   if (not scene:hasNavMesh()) then
      coroutine.yield(function() return scene:hasNavMesh() end)
   end
   
   --check for gkPath
   if (not self.path) then
      self.path = Gk.NavPath()
   end
   
   Gk.log("Try Radius:"..tostring(self.veh.radius))
   self.path:createFromRecast(scene,self.veh.obj:getPosition(),point,pathradius)
   if (self.path:getPointAmount()==0) then
      return
   end
   
   self.veh:setGoal(self.path:getLast())
   
   Gk.log("New Goal:"..tostring(self.goal))
   if rDebug then
      local p = self.path:getLast()
      rDebug:drawCircle(self.path:getLast(),1)
      rDebug:drawCircle(self.path:getPoint(0),1)
      rDebug:drawPath(self.path,Gk.Vector4(1,1,0,1))
   end
--   if UIDEBUG then
--      UIDEBUG:drawPath(self.path)
--   end
   
end
-------------------------------------------------------------------------------------------
--[[
The vehicle-context keeps track of all vehicle objects that automatically registers.
The VehicleBase automatically adds the vehicle to steergroup,vehgroup,adds markers and obstacles
All needs to be tested a bit more
--]]

VehicleContext = Class()

function VehicleContext:constructor(name)
   self.name=name or default
   self._allObs = Gk.SteerObstacleGroup()
   self._staticObs = Gk.SteerSweptObstacle(8.0,2.0,64,64)
   self._allObs:add(self._staticObs)
   
--   self._allObs:setPositiveList(t) -- only list elements are recognized   
   
   self._allVeh = Gk.SteerGroup()
   self.vehicles = {}
   self.paths = {}
   self.markers = {}
   self.steerGroups = {}
   self.vehGroups = {}
   self.obstacles = {}
   
   
   self.UI = Gk.gsRecastDebugger(scene)
   self.GUI3D = Gk.gsGUI3D()
end

function VehicleContext:addToVehGroup(name,veh)
   local group = self.vehGroups[name]
   if not group then
      group = {}
      self.vehGroups[name]=group
   end
   table.insert(group,veh);
end

function VehicleContext:getVehGroup(name)
   return self.vehGroups[name]
end

function VehicleContext:addMarker(name,marker)
   self.markers[name]=marker
end

function VehicleContext:getMarker(name)
   return self.markers[name]
end

function VehicleContext:addPath(name,path)
   self.paths[name]=path
end

function VehicleContext:getPath(path)
   if self.paths[path] then
      Gk.log("FOundPath")
   else
      Gk.log("NO PATH")
   end
   return self.paths[path] 
end

function VehicleContext:addVehicle(veh)
   self.vehicles[veh.name]=veh
   self._allVeh:add(veh.os)
end

function VehicleContext:addObstacle(name,obs)
   self.obstacles[name]=obs
end


function VehicleContext:draw()
   self.UI:clear()   
   for k,veh in pairs(self.vehicles) do
      veh:draw(self.ui)
   end
end

function VehicleContext:addToSteerGroup(groupname,veh)
   local group = self.steerGroups[groupname]
   if not group then
      group = Gk.SteerGroup()
      self.steerGroups[groupname]=group
   end   
   group:add(veh)
end

function VehicleContext:getSteerGroup(groupname)
   return self.steerGroups[groupname]
end

function VehicleContext:createVehicleGroup(func)
   local otherGroup = Gk.SteerGroup()
   for k,veh in pairs(self.vehicles) do
      if (func(veh)) then
         otherGroup:add(veh.os)
      end
   end  
   return otherGroup
end

function VehicleContext:stopAllVehicles()
   for _,veh in pairs(self.vehicles) do
      if (veh.stop) then
         veh:stop()
      elseif (veh.userObject.me.stop) then
         veh.userObject.me:stop()
      else
         Gk.log("VC: Couldn't stop veh:"..veh.name)
      end
   end
end


function VehicleContext:getDispatcher()
   return AsyncDispatcher.get(self.name)
end

function VehicleContext.get(name)
   name = name or __vehicleCtxName
   if not VEHICLECONTEXTS then
      VEHICLECONTEXTS={}
   end
   
   local vc = VEHICLECONTEXTS[name]
   if not vc then
      vc = VehicleContext(name)
      VEHICLECONTEXTS[name] = vc
   end
   
   return vc
end

__vehicleCtxName = "default"

function VehicleContext.setDefault(ctx)
   __vehicleCtxName = ctx
end