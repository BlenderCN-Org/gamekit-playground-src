Label = BaseClass(TemplaterElement)

function Label:constructor(input)
   self:setMe(self)
   self.type=Label
   self.__type="Label"
   self.text = input.text 
end

function Label:setValueType(valueType)
   local me = self
   if (valueType.type==ValueType) then
      valueType:addCallback(function(old,new) 
         me:setText(new)
         dPrintf("Set New Value")
      end)
   end
   self:setText(valueType:get())
end

function Label:setText(txt)
   self.text = txt
   self.elem:setInnerRML(txt)
end

function Label:setInfo(info,templater,init)
   self.base:setInfo(info,templater,init)
   --self.info.logic = self
   
   if (self.text) then
      info.elem:setInnerRML(self.text)
   end
   --self.parent:removeChild(info.elem)
end

function Label:clone(input)
   local clone = self.base.clone(self,input)

   return clone
end

EnableElement = BaseClass(TemplaterElement)

function EnableElement:constructor()
   self:setMe(self)
   self.__type="EnableElement"
   self.__isactive=true
end

function EnableElement:setEnabled(b)
   local changed = false
   if b~=self.me.__isactive then
      changed = true
      if b==true and self.me.onEnable then
         self.me:onEnable()
      end
      if b==false and self.me.onDisable then
         self.me:onDisable()
      end
   end
   self.me.__isactive=b
   self.me.elem:setPseudoClass("disabled",not self.me.__isactive)
   return changed
end

function EnableElement:isEnabled()
   return self.__isactive;
end

------------------------------------------------
Button = BaseClass(EnableElement)

function Button:constructor()
   self:setMe(self)
   self.__type="Button"
  -- self.hideOnCreation = true
end



function Button:setLabel(text)
   self.elem:setInnerRML(text)
end

function Button:bindButtonEvent(evtname,func)
   local me = self
   self:bindEvent(evtname,function()  
      if me:isEnabled() then
         func()
      else
         Gk.log("Sorry, button is not active!")
      end
   end)
end
----------------------------------------------------------------------------------
InputText = BaseClass(EnableElement)

function InputText:constructor()
   self:setMe(self)
   self.clearOnFocus=true
   self.__type="InputText"
end

function InputText:init()
   self.mode = (self.elem:getAttribute("clearonfocus") or "never"):lower()
   
   self.clearOnFocus = self.mode == "once" or self.mode == "always"
   
   local me = self
   
   self:bindEvent("focus",function()
      if (me.clearOnFocus) then
         me:setText("")
         if self.mode =="once" then
            me.clearOnFocus=false
         end
      end
   end)   
end

function InputText:setText(text)
   self.elem:setValue(text)
end

function InputText:getText()
   return self.elem:getValue()
end

function InputText:setEnabled(b)
   local changed = self.base:setEnabled(b)
   if changed then
      if b then
         self.elem:removeAttribute("disabled")
      else
         self.elem:setAttribute("disabled","disabled")
      end
   end
end

