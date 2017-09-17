Gk.import(LUA_LIB_DIR .. "/misc/_ui_helper.lua")

function __csvToMap(csv)
   local input = {}
   for i,split in ipairs(csv:split(",")) do
      local key,value = __keyvalue(split) 
      input[key]=value or true
   end 
   return input
end 

-- splits 'key=value'-strings
function __keyvalue(st)
   local splits = st:split("=")
   
   if (#splits == 2) then
      local valueSplit = splits[2]:split(":")
      if #valueSplit==2 then
         return splits[1],{valueSplit[1],valueSplit[2]}
      end
   end
   
   return splits[1],splits[2]
end


TAttrFocusName = BaseClass(TemplaterAttribute)
function TAttrFocusName:constructor()
end
function TAttrFocusName:afterInit(elem,params,logic,templater)
   templater:setCurrentFocus(params)
end

TAttrFocusOrder = BaseClass(TemplaterAttribute)
function TAttrFocusOrder:constructor()
end
function TAttrFocusOrder:afterInit(elem,params,logic,templater)
   local b=0;
   local focusParent = nil
   templater:addFocusElement(logic)
   Gk.log(logic.id.." "..elem:getAbsX().."|"..elem:getAbsY())
end


TAttrLang = BaseClass(TemplaterAttribute)

function TAttrLang:constructor()
end

function TAttrLang:init(elem,params)
   local lang = params

   if not LanguageManager.instance then
      Gk.log("Error: No languagemanager! Couldn't use language-templater-attribute:"..tostring(lang))
   end
   
   if (lang and #lang>0) then
      local text = LanguageManager.instance:get(lang)
      if (text) then
         elem:setInnerRML(text)
      end
   end   
end


TAttrRest = BaseClass(TemplaterAttribute)

function TAttrRest:constructor()
end

function TAttrRest:init(elem,params)
   Gk.log("TAttrRest:"..params)
   elem:test()
   local input = __csvToMap(params)
   input.elem = elem
   uiHelper.setFreeSpace(input)
end


--for testing purpose
TAttrCheck = BaseClass(TemplaterAttribute)
function TAttrCheck:constructor()
end
function TAttrCheck:init(elem,params)
   elem:test();
   local height = elem:resolveProperty("height",0)
   local wHeight = elem:getProperty("height")
   local b=2
end

TAttrBoxSizing = BaseClass(TemplaterAttribute)

function TAttrBoxSizing:constructor()
   self.elements = {}
   local messagemanager = Gk.MessageManager()
   --messagemanager:addListener("internal","","__windowresized",self,TAttrBoxSizing.onResize)
end
function TAttrBoxSizing:init(elem,csv,res)
   local input = __csvToMap(csv)

   local pWidth = elem:getParent():getWidth()
   local pHeight = elem:getParent():getHeight()

   local offsetX = 0
   local offsetY = 0

   local fontSize = elem:resolveProperty("font-size") 

   if (input.x and type(input.x)=="table") then
      local value = tonumber(input.x[1])
      local unit = input.x[2]
      if (unit=="px") then
         offsetX = value
      elseif (unit=="%") then
         offsetX = pWidth / 100 * value
      elseif (unit=="em") then
         offsetX = fontSize * value
      else 
         error("Unknown x-unit: "..unit.." for elem:"..elem:getId().." ("..elem:getRML()..")")
      end
   end

   if (input.y and type(input.y)=="table") then
      local value = tonumber(input.y[1])
      local unit = input.y[2]
      if (unit=="px") then
         offsetY = value
      elseif (unit=="%") then
         offsetY = pHeight / 100 * value
      elseif (unit=="em") then
         offsetY = fontSize * value
      else 
         error("Unknown y-unit: "..unit.." for elem:"..elem:getId().." ("..elem:getRML()..")")
      end
   end

   local width = elem:getWidth()
   local height = elem:getHeight()
   

   if res then
      width = res.width
      height = res.height
   end
   
   local elemMX = uiHelper.getProperty(elem,"margin-left") + uiHelper.getProperty(elem,"margin-right")
   local elemMY = uiHelper.getProperty(elem,"margin-top") + uiHelper.getProperty(elem,"margin-bottom")
   local elemPadX = uiHelper.getProperty(elem,"padding-left") + uiHelper.getProperty(elem,"padding-right")
   local elemPadY = uiHelper.getProperty(elem,"padding-top") + uiHelper.getProperty(elem,"padding-bottom")
   local elemBorderX = uiHelper.getProperty(elem,"border-left-width") + uiHelper.getProperty(elem,"border-right-width")
   local elemBorderY = uiHelper.getProperty(elem,"border-top-width") + uiHelper.getProperty(elem,"border-bottom-width")   

   -- should margin be included into calculation?
   if (input.nomargin) then
      elemMX = 0;
      elemMY = 0;
   end

   local newWidth  = width - elemPadX + elemBorderX - elemMX + offsetX
   local newHeight = height - elemPadY + elemBorderY - elemMY  + offsetY

   local newW_percent = newWidth / pWidth * 100;
   local newH_percent = newHeight / pHeight * 100;

   local newW_em = newWidth / fontSize;
   local newH_em = newHeight / fontSize;

   if (input.fix) then
      elem:setProperty("width",newWidth.."px")
      elem:setProperty("height",newHeight.."px")
   else
      elem:setProperty("width",newW_percent.."%")
      elem:setProperty("height",newH_percent.."%")
   end
   
   self.elements[elem]=csv
end

function TAttrBoxSizing:onResize()
   for elem,csv in pairs(self.elements) do
      self:init(elem,csv)
   end
end