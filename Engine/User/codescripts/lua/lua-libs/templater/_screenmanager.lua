Gk.import(LUA_LIB_DIR .. "/misc/_AsyncDispatcher.lua")

UIScreenManager = Class()
function UIScreenManager:constructor()
   if not dispatcher then
     self.dispatcher = AsyncDispatcher()
     game = game or DefaultGame()
     local me = self
     game:addAlwaysQueue(function() me.dispatcher:asyncDispatch() return true end) 
   else
     self.dispatcher = dispatcher
   end
   self.screens = {}
   self.currentScreen=nil
end

function UIScreenManager:_addScreen(id,screen)
   Gk.log("Add screen "..tostring(id).." data:"..tostring(screen))
   self.screens[id]=screen.me
end

function UIScreenManager:hideAll()
   for _,screen in pairs(self.screens) do
      screen.me:hide(true)
   end
end

function UIScreenManager:getScreen(id)
   local screen = self.screens[id]
   return screen
end

function UIScreenManager:show(id,wait)
  if (self.currentScreenId==id) then
    Gk.log("SCREEN ALREADY THERE")
    return
  end
  
  Gk.log("SHOW SCREEN:"..id)
  Gk.log("SHOW SCREEN:"..id)
   if not wait then
      self:immediateShow(id)
      return
   end
   
   local me = self
   self.dispatcher:call{
    call=function()
      coroutine.yield(function() return me:getScreen(id) end)
      me:immediateShow(id)
    end
   }
end

function UIScreenManager:immediateShow(id)
     self:hideAll()
     local screen = self:getScreen(id)
     if (self.currentScreen and self.currentScreen.onHide) then
        self.currentScreen:onHide(id)
     end
     self.currentScreen=screen
     self.currentScreenId=id
     screen:show(true)
end

-- create instance
uiScreenManager = UIScreenManager()

UIScreen = BaseClass(TemplaterElement)

function UIScreen:constructor()
  self:setMe(self)
end

function UIScreen:init()
   uiScreenManager:_addScreen(self.id,self)
   self.me:hide(true)
end
