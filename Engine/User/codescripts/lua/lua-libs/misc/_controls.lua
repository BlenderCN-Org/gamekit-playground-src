CMouse = Class()

function CMouse:constructor(tbl)
   self.mouse = Gk.Mouse()
   self.mouse:capture()
   self.buttons = {}
   self.buttons[Gk.LEFT]=ni
   self.buttons[Gk.MIDDLE]={}
   self.buttons[Gk.RIGHT]={}
   self.lastX = self.mouse.xpos
   self.lastY = self.mouse.ypos
   self.dragDeathzone = tbl.dragDeathZone or 10
   
   self.fireMove = tbl.fireMove or true
   self.fireDrag = tbl.fireDrag or true
end

function CMouse:update()
   self.mouse:capture()
   
   local anyButtonDown = false
   
   local result = {}
   result[Gk.LEFT] = self:checkButton(Gk.LEFT)
   result[Gk.RIGHT] = self:checkButton(Gk.RIGHT)
   result[Gk.MIDDLE] = self:checkButton(Gk.MIDDLE)
   
   anyButtonDown = result[Gk.LEFT] or result[Gk.RIGHT] or result[Gk.MIDDLE]
   
   if not self.fireMove and not anyButtonDown and self.mouse.moved then
      SEND_MSG{type="mouse",subtype="moved",lastX=self.lastX,lastY=self.lastY,currentX=self.mouse.xpos,currentY=self.mouse.ypos}
   end

   return result
end

function CMouse:checkButton(btn)
   local mouseDown = self.mouse:isButtonDown(btn)
   
   if mouseDown then
      local info = self.buttons[btn]
      if not info then
         self.buttons[btn] = { state="pressed", startX=self.mouse.xpos, startY=self.mouse.ypos}
         SEND_MSG{type="mouse",subtype="pressed",btn=btn,info=self.buttons[btn]}
         return true,info 
      else
         if not self.fireDrag then
            return true,"drag-info deactivated"
         end
         if self.mouse.moved then
            info.currentX = self.mouse.xpos
            info.currentY = self.mouse.ypos
            info.relX = self.mouse.xrel
            info.relY = self.mouse.yrel
            
            --Gk.log("dragDeathzone:"..math.abs(info.startX - info.currentX))
            if ( info.dragged 
                  or (math.abs(info.startX - info.currentX) > self.dragDeathzone)
                  or (math.abs(info.startY - info.currentY) > self.dragDeathzone) 
                  )
            then
              -- Gk.log("DRAG")
               info.state="dragged"
               info.dragged=true
               SEND_MSG{type="mouse",subtype="dragged",btn=btn,info=info}
               return true,info
            else
               Gk.log("no drag")
            end
         end
      end
   else
      local info = self.buttons[btn]
      if info then
         info.currentX = self.mouse.xpos
         info.currentY = self.mouse.ypos
         info.relX = self.mouse.xrel
         info.relY = self.mouse.yrel
         
         info.state = "released"
         SEND_MSG{type="mouse",subtype="released",btn=btn,info=info}
         self.buttons[btn]=nil
         return false,info
      end
   end
   return false 
end





CKeyboard = Class()

function CKeyboard:constructor()
   self.keyboard = Gk.Keyboard()
   self.fsm = Gk.FSM()
   
   self.checkKeys = {}
   self.PRESSED = {}

   self.callback = {obj=self,func=CKeyboard.onCall}
   

   
end


function CKeyboard:setCallback(obj,func)
   self.callback = {obj=obj,func=func}
end

function CKeyboard:onCall(key,type)
   Gk.log("Key:"..tostring(key).." Type:"..tostring(type))
end

function CKeyboard:addKeyCheck(ch,callback)
   self.checkKeys[ch] = {key=ch,callback=callback}
   return self
end

function CKeyboard:update()
   self.keyboard:capture()
   self:poll()
end

function CKeyboard:_fire(key,type,callback)
   callback = callback or self.callback
   if (callback) then
      if (callback.func and callback.obj) then
         callback.func(callback.obj,key,type)
      else
         callback.func(key,type)
      end
   end
end


function CKeyboard:poll()
   for ch,data in pairs(self.checkKeys) do
      if (self.keyboard:isKeyDown(ch)) then
         if (not self.PRESSED[ch]) then
            self:_fire(ch,"pressed",data.callback)
            self.PRESSED[ch]=true   
         else
            self:_fire(ch,"hold",data.callback)
         end
      else
         if (self.PRESSED[ch]) then
            self:_fire(ch,"released",data.callback)
            self.PRESSED[ch]=nil
         end
      end
   end
end

function showKeyboard()
   Gk.sendMessage("","","__showkeyboard","true")
end

function hideKeyboard()
   Gk.sendMessage("","","__showkeyboard","false")
end
----------------------------------------------------------------------

CAccelerometer = Class()

function CAccelerometer:constructor()
   gkMsg:addListener("android","","accel",self,CAccelerometer.onAccelMsg)
   self.enabled = false
   self.callbacks = {}
   Gk.log("Accelerometer-Control added")
end

function CAccelerometer:setEnabled(b)
   if Gk.getPlatform()=="ANDROID" then
      if b then
         Gk.sendMessage("internal","","start_sensor","accel")
      else
         Gk.sendMessage("internal","","stop_sensor","accel")
      end
      self.enabled = b
   else
      error("Accelerometer not supported on "..Gk.getPlatform())
   end
end

function CAccelerometer:addMapping(func,obj)
   table.insert(self.callbacks,{func=func,obj=obj})
end

function CAccelerometer:onAccelMsg(from,to,subject,body)
   local values = body:split(":")
   for _,callback in ipairs(self.callbacks) do
      if (callback.obj) then
         callback.func(callback.obj,values)
      else
         callback.func(values);
      end
   end   
end

--------------------------------------------------------
CTouch = Class()

function CTouch:constructor()
   self.callbacks = {}
   self.pointers={}
   self.pointerList={}
   self.touch = Gk.Touch()  
   self.touch:addListener(self,CTouch.onTouch)
   self.dirty=false
   Gk.log("INIT TOUCH DONW")
   self.dragDeathzone=10
end

function CTouch:onTouch(type,pid,x,y)
   local pointer = self.pointers[pid]
   if not pointer then
      pointer = { pid = pid, type="none",startX=x,startY=y,x=x,y=y}
      self.pointers[pid] = pointer
      table.insert(self.pointerList,pid)
      pointer.pidNr = #self.pointerList
   end
   if self.dirty then
      for _pid,_pointer in pairs(self.pointers) do
         for i,val in ipairs(self.pointerList) do
            if val==_pid then
               _pointer.pidNr = i
               break
            end
         end
      end
      self.dirty=false
   end
   pointer.relX = x - pointer.x;
   pointer.relY = y - pointer.y;
   pointer.x=x
   pointer.y=y
   
   pointer.type=type

   self:_fire(pointer)
   if type=="pressed" then
      SEND_MSG{type="touch",subtype="pressed",pointer=pointer,amount=#self.pointerList}
   elseif type=="moved" then
      if (math.abs(pointer.startX - x) > self.dragDeathzone or math.abs(pointer.startY - y) > self.dragDeathzone) then
         pointer.dragged=true
      end
      SEND_MSG{type="touch",subtype="moved",pointer=pointer,amount=#self.pointerList}
   elseif type=="released" then
      SEND_MSG{type="touch",subtype="released",pointer=pointer,amount=#self.pointerList}
      self.pointers[pid]=nil
      table.removeValue(self.pointerList,pid)
      self.dirty=true
   else
      Gk.log("COUNTROLLER:"..type)
   end
  -- Gk.log("Touch event:"..type.." pnt:"..pid)
end

function CTouch:addMapping(func,obj)
   table.insert(self.callbacks,{func=func,obj=obj})
end
--
function CTouch:_fire(data)
   for _,callback in ipairs(self.callbacks) do
      if (callback.obj) then
         callback.func(callback.obj,data)
      else
         callback.func(data);
      end
   end   
end

----------------------------------------

CGamePad = Class()

function CGamePad:constructor(input)
   local numJoys = Gk.getNumJoysticks()

--   if numJoys == 0 then
--      Gk.log("No gamepads present")
--      return
--   end
   
   local currentNr = input.padnr or 0
   
   if currentNr <= numJoys then
      self.joy = Gk.Joystick(currentNr)
      self.numaxis = self.joy:getNumAxes()
      self.numbtns = self.joy:getNumButtons()
      self.controlls={{0,1},{2,3}}
      self.deadzone = 2048
      self.btnMapping = {}
      self.PRESSED = {}
   end
   
   Gk.log("Joystick-Count:"..numJoys)
end

function CGamePad:setDigitalCallback(callback)
   self.digitalCallback = callback
end

function CGamePad:addButtonCheck(btnNr,callback)
   self.btnMapping[btnNr]={callback=callback}
end

function CGamePad:addControlCheck(controlNr,callback,options)
   
   local control = self.controlls[controlNr]
   if control then
      control.callback=callback
      control.options=options or {}
   else
      error("Unknown controller-nr:"..controlNr)
   end
end


function CGamePad:poll()
   self.joy:capture()

   local x1,y1 = self:handleControl(1)
   local x2,y2 = self:handleControl(2)

   local changeLR = nil
   local changeUD = nil
   
   if x1 > 0 then
      if self.PRESSED['lr1'] ~= "right" then
         Gk.log("on right")
         changeLR = "right"
      end
      self.PRESSED['lr1'] = "right"
   elseif x1 < 0 then
      if self.PRESSED['lr1'] ~= "left" then
         changeLR = "left"
         Gk.log("on left")
      end
      self.PRESSED['lr1'] = "left"
   else
      if (self.PRESSED['lr1']) then
         Gk.log("NO LEFTRIGHT")
         changeLR = "clear"
         
      end
      self.PRESSED['lr1'] = nil
   end

   if y1 < 0 then
      if self.PRESSED['ud1'] ~= "up" then
         Gk.log("on up")
         changeUD = "up"
      end
      self.PRESSED['ud1'] = "up"
   elseif y1 > 0 then
      if self.PRESSED['ud1'] ~= "down" then
         Gk.log("on down")
         changeUD = "down"
      end
      self.PRESSED['ud1'] = "down"
   else
      if (self.PRESSED['ud1']) then
         Gk.log("NO UPDOWN")
         changeUD = "clear"
      end
      self.PRESSED['ud1'] = nil
   end
   
   if (self.digitalCallback) then
      if (type(self.digitalCallback)=="table") then
         if self.digitalCallback.obj and self.digitalCallback.func then
            self.digitalCallback.func(self.digitalCallback.obj,changeLR,changeUD)
         else
            self.digitalCallback.func(changeLR,changeUD)
         end
      else
         self.digitalCallback(changeLR,changeUD)
      end
   end
   
   self:handleButtons()
--   Gk.log("Left:"..x1.."|"..y1.." right:"..x2.."|"..y2)
--   self:debugButtons()
end

function CGamePad:handleControl(nr)
   local control = self.controlls[nr]
   local x = self.joy:getAxis(control[1])
   local y = self.joy:getAxis(control[2])
   local mx = math.abs(x)
   
   if mx>0 and mx < self.deadzone then
      x = 0
   else
      x = x / 32768
   end
   if math.abs(y) < self.deadzone then
      y = 0
   else
      y = y / 32768
   end
   
   if control.callback and (x~=0 or y~=0 or control.options.always) then
      if control.callback.func and control.callback.obj  then
         control.callback.func(control.callback.obj,nr,x,y)
      else
         control.callback.func(nr,x,y)
      end 
   end
   
   return x,y
end

function CGamePad:handleButtons()
   for btnNr,data in pairs(self.btnMapping) do
      if (self.joy:isButtonDown(btnNr)) then
         if (not self.PRESSED[btnNr]) then
            self:_fireBtn(btnNr,"pressed",data.callback)
            self.PRESSED[btnNr]=true   
         else
            self:_fireBtn(btnNr,"hold",data.callback)
         end
      else
         if (self.PRESSED[btnNr]) then
            self:_fireBtn(btnNr,"released",data.callback)
            self.PRESSED[btnNr]=nil
         end
      end
   end
end

function CGamePad:_fireBtn(btn,type,callback)
   callback = callback or self.callback
   if (callback) then
      if (callback.func and callback.obj) then
         callback.func(callback.obj,btn,type)
      else
         callback.func(btn,type)
      end
   end
end

function CGamePad:debugButtons()
   local result = ""
   Gk.log("BtnCount"..self.joy:getNumButtons().." | "..self.joy:getButtonCount())
   for i=0,self.joy:getNumButtons()-1 do
      result = result .. "btn"..i..":"..tostring(self.joy:isButtonDown(i)).." "
   end
   Gk.log(result)
end
