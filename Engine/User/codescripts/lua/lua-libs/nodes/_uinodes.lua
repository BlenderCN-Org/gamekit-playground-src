Gk.import(LUA_LIB_DIR.."/templater/_templater.lua")

TemplaterNode = BaseClass(NodeLogic)
TemplaterNode.docs={}
TemplaterNode.find = function(name)
  return TemplaterNode.docs[name]
end
function TemplaterNode:constructor()
  Gk.log("TemplaterNOde!")
  self.docname = self.node:getProperty("DOCNAME"):getString()
  self.templateFile = self.node:getProperty("TEMPLATE"):getString()

  if not self.docname or self.docname=="" or self.docname=="null" then
    error("ERROR TemplaterNode: Invalid docname")
  end  
  if TemplaterNode.docs[self.docname] then
--    error("ERROR "..self.node:getName()..": docname '"..self.docname.."' already in use!");
    Gk.log("ERROR "..self.node:getName()..": docname '"..self.docname.."' already in use!");
  end
  if not self.templateFile or self.templateFile=="" or self.templateFile=="null" then
    error("ERROR TemplaterNode: Invalid templateFile")
  end
  
  self.templater = Templater{html=self.templateFile}
  Gk.log("Add Template("..self.node:getName().."):"..self.docname)
  TemplaterNode.docs[self.docname]=self.templater
end

function TemplaterNode:afterInit()
  -- loadfonts
  local fontSocket = self.node:getOutSocket("FONTS")
  for i=0,fontSocket:getOutSocketAmount()-1 do
    local outsockNode = fontSocket:getOutSocketAt(i):getNode()
    local fileLogic = NodeTreeLogic.getLogic(outsockNode)
    
    Gk.log("Font Font:"..fileLogic.filename)

    self.templater.gui:loadFont(fileLogic.filename)
  end
  
--  self.templater:asyncParse{}
  self.templater:parse{}
  Gk.log("PARESED")
  Gk.log("PARESED")
  Gk.log("PARESED")
  Gk.log("PARESED")
  Gk.log("PARESED")
end

NodeTreeLogic.addLogic("Gamekit","TemplaterNode",TemplaterNode)

ScreenAction = BaseClass(NodeLogic)

function ScreenAction:constructor()
  self.running = false
  self.IN_ENABLED = self.node:getInSocket("ENABLED"):getVar()
  self.OUT_VALUE = self.node:getOutSocket("VALUE"):getVar()
  
  self.TYPE = self.node:getProperty("TYPE"):getInt()
  self.DOC = self.node:getProperty("DOC"):getString()
  self.PARAM = self.node:getProperty("PARAM"):getString()

  if (not self.DOC or self.DOC=="") then
    self:error("invalid DOC-Value")
  end
  
end

function ScreenAction:checkForParam()
  if (not self.PARAM or self.PARAM=="") then
    self:error("invalid PARAM-Value")
  end
end

function ScreenAction:getElementFromParam()
    self:checkForParam()
    self.ELEM = self.DOC:get(self.PARAM)
    if not self.ELEM then
      error("Unknown PARAM:"..self.PARAM)
    end
end

function ScreenAction:init()
  self.DOC = TemplaterNode.find(self.DOC)
  if not self.DOC then
    error("Unknown DOC:"..self.DOC)
  end
  
  if self.TYPE > 3 then
    self.ELEM = self:getElementFromParam()
  end
end

function ScreenAction:evaluate()
    local result = self.IN_ENABLED:getBool()

    if result and not self.running then
      if self.TYPE == 0 then
        uiScreenManager:show(self.PARAM,true) -- call as coroutine and wait for the screen to show up
      elseif self.TYPE == 1 then
        uiScreenManager:hide(self.PARAM)
      elseif self.TYPE == 2 then
        self.DOC:show()
      elseif self.TYPE == 3 then
        local me = self
        game:addAlwaysQueue(function() 
          if me.DOC.doc then 
            me.DOC:hide() 
            return false 
          else 
            return true 
          end 
        end)
        self.IN_ENABLED:setBool(false)
      elseif self.TYPE == 7 then
        self.ELEM:bindEvent("click",function() 
          Gk.log("set true")
          self.OUT_VALUE:setBool(true)
          game = game or DefaultGame()
          -- one frame later reset the state again
          game:addOnceQueue(function() 
            Gk.log("set false again")
            self.OUT_VALUE:setBool(false)
          end)
        end)
        self.running = true
      end
    end
    
    if not result and self.running then
      if self.TYPE == 5 then
        self.ELEM:unbindEvent("click")
      end
    end

  return result
end

function ScreenAction:update(tick)
-- nothing to do yet
end
NodeTreeLogic.addLogic("Gamekit","ScreenAction",ScreenAction)





