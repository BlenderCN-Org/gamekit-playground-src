uiHelper = {}

-- function to return the space left without this element (only for last elements)
-- Caution: this is far for being optimal.Especial
function uiHelper.freeSpace(table)
   table = table or {}
   local elem = table.elem
   if (not elem) then
      error("freeSpace needs at least a elem-value")
   end
   
   local parent = elem:getParent()
   -- seems that padding is kept on the parents-element!?
   local pParent = parent:getParent()
   
   local id = parent:getId()
   local pX = parent:getAbsX()
   local pY = parent:getAbsY()
   local pH = parent:getHeight()
   local pW = parent:getWidth()
   
   local pPadX = 0; 
   local pPadY = 0;   
   if (parent) then
      pPadX = uiHelper.getProperty(parent,"padding-left") + uiHelper.getProperty(parent,"padding-right") 
      pPadY = uiHelper.getProperty(parent,"padding-top") + uiHelper.getProperty(parent,"padding-bottom")   
   end

   local offsetX = 0
   local offsetY = 0
   local fontSize = elem:resolveProperty("font-size") 
   
   if (table.x and type(table.x)=="table") then
      local value = tonumber(table.x[1])
      local unit = table.x[2]
      if (unit=="px") then
         offsetX = value
      elseif (unit=="%") then
         offsetX = pW / 100 * value
      elseif (unit=="em") then
         offsetX = fontSize * value
      else 
         error("Unknown x-unit: "..unit.." for elem:"..elem:getId().." ("..elem:getRML()..")")
      end
   end

   if (table.y and type(table.y)=="table") then
      local value = tonumber(table.y[1])
      local unit = table.y[2]
      if (unit=="px") then
         offsetY = value
      elseif (unit=="%") then
         offsetY = pH / 100 * value
      elseif (unit=="em") then
         offsetY = fontSize * value
      else 
         error("Unknown y-unit: "..unit.." for elem:"..elem:getId().." ("..elem:getRML()..")")
      end
   end


   local elemX = elem:getAbsX()
   local elemY = elem:getAbsY()
   local elemMX = uiHelper.getProperty(elem,"margin-left") + uiHelper.getProperty(elem,"margin-right")
   local elemMY = uiHelper.getProperty(elem,"margin-top") + uiHelper.getProperty(elem,"margin-bottom")
   local elemPadX = uiHelper.getProperty(elem,"padding-left") + uiHelper.getProperty(elem,"padding-right")
   local elemPadY = uiHelper.getProperty(elem,"padding-top") + uiHelper.getProperty(elem,"padding-bottom")
--   local elemMX = uiHelper.getProperty(elem,"margin-right")
--   local elemMY = uiHelper.getProperty(elem,"margin-bottom")
--   local elemPadX = uiHelper.getProperty(elem,"padding-right")
--   local elemPadY = uiHelper.getProperty(elem,"padding-bottom")
   
--   local elemBorderX = uiHelper.getProperty(elem,"border-left-width") + uiHelper.getProperty(elem,"border-right-width")
--   local elemBorderY = uiHelper.getProperty(elem,"border-top-width") + uiHelper.getProperty(elem,"border-bottom-width")   
   local elemBorderX = uiHelper.getProperty(elem,"border-right-width")
   local elemBorderY = uiHelper.getProperty(elem,"border-bottom-width")   

   local localStartX = (elemX- pX) 
   local localStartY = (elemY- pY)
   
   local canvasX = pW - pPadX - elemMX - elemPadX;
   local canvasY = pH - pPadY - elemMY - elemPadY;
   
   local topleftX = pX + uiHelper.getProperty(elem,"margin-left")   
   local topleftY = pY + uiHelper.getProperty(elem,"margin-top")  
   
   local innerCanvasDeltaX = elemX - topleftX;
   local innerCanvasDeltaY = elemY - topleftY;
   
   local wX = canvasX - innerCanvasDeltaX + offsetX;
   local wY = canvasY - innerCanvasDeltaY + offsetY;
   
   local newW_percent = wX / pW * 100;
   local newH_percent = wY / pH * 100;

 
   
--   local wX = pW - pPadX - localStartX - elemMX  - elemBorderX - elemPadX + offsetX
--   local wY = pH - pPadY - localStartY - elemMY  - elemBorderY - elemPadY + offsetY
--   local wX = pW - elemMX - pPadX - elemBorderX - elemPadX + offsetX
--   local wY = pH - elemMY - pPadY - elemBorderY - elemPadY + offsetY
   
   return wX,wY,newW_percent,newH_percent,elem;
end

function string:endsWith(piece)
  local result = string.sub(self, #self - #piece+1, #self)
  return  result == piece
end
--rawset(_G.string, "startsWith", endsWith)


function uiHelper.setFreeSpace(table)
   local xPx,yPx,xPercent,yPercent,elem = uiHelper.freeSpace(table)


   if (table.x) then
      elem:setProperty("width",xPercent.."%")
   end
   if (table.y) then
     elem:setProperty("height",yPercent.."%")
   end
end

function uiHelper.getProperty(elem,prop,type)
   type = type or "n"
   local result = nil
   if (type=="n") then
 --     result = elem:getPropertyInt(prop)
      result = elem:resolveProperty(prop)
   elseif (type=="s") then
      result = elem:getProperty(prop)
   end
      
   return result
end