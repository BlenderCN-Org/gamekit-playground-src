DEBUG=true

function StringToColor(color,default)
    local s = color:split(",")
    if (#s==3) then
        return Gk.Vector4(tonumber(s[1]),tonumber(s[2]),tonumber(s[3]),1)
    elseif (#s==4) then
        return Gk.Vector4(tonumber(s[1]),tonumber(s[2]),tonumber(s[3]),tonumber(s[4]))
    end
    
    if (default) then
        return default
    else
        return Gk.Vector4(1,1,1,0.5)
    end
end

-- base-object for wrapping gui3d-elements
	-- type-mapping from enum to string
	_GUI3dElementTypeMapping = {"EButton", "ECaption", "ECheckbox", "ECombobox", "EListbox", "EScrollbar", "ETextzone", "EProgressBar"}

GUI3dElement = Class()
 
function GUI3dElement:constructor(elem)
	dPrintf("##GUI3D_BASE %s",tostring(elem))
	self.type = _GUI3dElementTypeMapping[elem:getType()] or "base-elem"
	self.name = elem:getName() or "noname"
	self.elem = elem
	
	dPrintf("##REGISTERED: %s / %s",tostring(self.name),tostring(self.type))
end
 
function GUI3dElement:onCallback(functionName)
	dPrintf("##WARNING: onCallback not overriden on %s / %s",tostring(self.name),tostring(self.type))
end

function GUI3dElement:setPosition(x,y)
	self.elem:setPosition(x,y)
end

function GUI3dElement:translate(x,y)
	local pos = self.elem:getPosition()
	self.elem:setPosition(pos.x + x,pos.y + y)
end

function GUI3dElement:getName()
	return self.elem:getName()
end

function GUI3dElement:getElement()
	return self.elem
end


ListBox = BaseClass(GUI3dElement)

function ListBox:constructor(elem,data)
	self.data = _registry[data]
	self:update()
end

function ListBox:setData(data)
	self.data = data
	self:update()
end

function ListBox:update()
     if (self.data) then
     	self.elem:clear()
        for i,j in ipairs(self.data) do
            self.elem:addValue(tostring(j))
        end
     end
end

function ListBox:addItem(name)
	self.elem:addItem(name)
end

TSGUI = Class()

function TSGUI:constructor(scene,atlas,convert)
    self.guiM = Gk.gsGUI3D(atlas,scene)
    self.guiScene = scene
    self.atlas = atlas
    
    self.panels = {}    

    self.destoryElems = {}
    self.uidCounter = 0
    if (convert~=false and scene:isInstanced()) then
    	self:convert(scene)
    end
end

function TSGUI:setVisible(show)
	self.panel:setVisible(show)
end


function TSGUI:destroyFromScene()

--	game.queue:add(
	--	function () 
			--dPrintf("destory start! %s",tostring(gui.tgui))
			for key, var in ipairs(self.destoryElems) do
				--dPrintf("%s",tostring(var))
				if (var.destroyInstance) then
                    var:destroyInstance()
                end
				--var:removeFromScene()				
			end
			--dPrintf("destory end!")
--			return false;
--		end
--	)
	dPrintf("DESTROY GUI!!!! IMPLEMENT SOMETHING")
end

-- check the objects and sort them on their zorder-parameter
function TSGUI:createZList(obj)
    local zlist = {}
    zlist.positions = {}
    zlist.data = {}
    for i=0,obj:getChildCount()-1 do    
        local child = obj:getChildAt(i)

        local z = 0
        local zorder = child:getProperty("zorder")
        if (zorder) then
            z = tonumber(zorder)
            dPrintf("FOUND ZORDER:%i",z)
        else
            dPrintf("%s no zorder",child:getName())
        end
        
        local zTable = zlist.data[z]
        
        if (not zTable) then
            zTable = {}
            zlist.data[z] = zTable
        end
        
        table.insert(zTable,child)
    end
    
    for k,v in pairs(zlist.data) do
        table.insert(zlist.positions,tonumber(k))
    end
    table.sort(zlist.positions)
    
    -- output:
    dPrintf("z-orders:")
    for i,k in ipairs(zlist.positions) do
        dPrintf("z:%i",k);
    end
    
    return zlist
end

function TSGUI:convert(scene)
    local winSize = Gk.getScreenSize()
    dPrintf("Screen: %f x %f",winSize.x,winSize.y)
    local scene = scene or self.guiScene
	--dPrintf("_GUIMAKER:Converter!")
    local iter = scene:getObjectList():iterator()
    while (iter:hasMoreElements()) do
        local obj = iter:getNext()
        if (obj:getProperty("gk_gui_panel")) then
            dPrintf("Create Panel!")
            local pos = obj:getPosition()
            local scl = obj:getScale()
            local rot = obj:getRotation()

            dPrintf("pos:%s scl:%s rot:%s",tostring(pos),tostring(scl),tostring(rot))

            local parent = obj:getParent()
            
            dPrintf("parent:%s",tostring(parent))
            
            local withBackground = obj:getProperty("no_background") == nil
            
--            self.panel = self.guiM:createPanel3D(scl.x,scl.y,obj:getName(),self.atlas,-1,withBackground)
            
            local has3dTag = obj:getProperty("3d")
            
            local atlas = obj:getProperty("atlas") or self.atlas
            
            dPrintf("USE ATLAS:%s",atlas)
            
            if (not has3dTag) then
                self.panel = self.guiM:createPanel(
                                            winSize.x / 2 + (pos.x*100 - scl.x*50),
                                            winSize.y / 2 + (-pos.y*100 - scl.y*50),
                                            scl.x*100,scl.y*100,obj:getName(),atlas,withBackground)
            else
                self.panel = self.guiM:createPanel3D(scl.x*100,scl.y*100,obj:getName(),atlas,-1,withBackground)
                if (parent) then
                    self.panel:attachToObject(parent)
                    --dPrintf("parented...")
                end
                self.panel:setPosition(pos.x,pos.y,pos.z);
                self.panel:setRotation(rot.x,rot.y,rot.z);
            end

            local currentPanelName = obj:getProperty("name") or obj:getName()
            self.panels[currentPanelName]={
            							panel=self.panel,
            							elements={}
            						   };
            
            Gk.log("Register Panel:"..currentPanelName)
            
            local border = obj:getProperty("border")
            local borderColor = obj:getProperty("border-color")
            
            if (border) then
            	local bcolor;
            	if (borderColor) then
					bcolor = StringToColor(borderColor)
            	else
            		bcolor = Gk.Vector4(0.0,0.0,0.0,0.75)
            	end
            	
            	self.panel:setBackgroundBorder(tonumber(border),bcolor)
            end
            
            if (obj:getProperty("invisible")) then
	            self.panel:setVisible(false)
            end
            
            local backgroundColor = obj:getProperty("background-color")
            if (backgroundColor) then
            	dPrintf("SET NEW COLOR:%s",backgroundColor)
				self.panel:setBackgroundColor(StringToColor(backgroundColor));            	
            else
            	dPrintf("#NO PANEL COLOR!")
            end


            
            table.insert(self.destoryElems,obj)

            local zlist = self:createZList(obj)
            
            for i,z in ipairs(zlist.positions) do
               dPrintf("Process Z:%s",tostring(z))
               for i,child in ipairs(zlist.data[z]) do
                
                local cpos = child:getPosition()
                

                local cscl = child:getScale()
                local crot = child:getRotation()
                --dPrintf("Child:%s Pos:%s",child:getName(),tostring(cpos))

                cpos.x = scl.x*100/2+cpos.x*scl.x*100
                cpos.y = scl.y*100/2-cpos.y*scl.y*100
                --dPrintf("Child:%s Pos:%s",child:getName(),tostring(cpos))


                if (child:getProperty("gk_gui_caption")) then
-- caption
                     align = child:getProperty("align")
                     valign = child:getProperty("valign")
                     font = child:getProperty("font")
                     color = child:getProperty("color")
                      
                     if (align==nil) then
                        align=1
                     end         
                     
                     if (valign==nil) then
                       valign=0
                     end           
            
         
                     local settings = self.guiM:getPanelColors()
                     saveCaptionTextSize = settings.captionTextSize            
    
                     if (font) then
                         settings.captionTextSize = font  
                     end

                     caption = self.panel:createCaption(cpos.x,
                                          cpos.y,
                                          cscl.x,
                                          cscl.y,
                                          child:getProperty("gk_gui_caption"),
                                          align,
                                          valign
                                          )

                     settings.captionTextSize = saveCaptionTextSize  

                     caption:setName(child:getName())  
                    
                     
                     if (color) then
                     	caption:setTextColor(StringToColor(color))
                     else
                        caption:setTextColor(1,1,1,1)
                     end
                     
                                         
                     self.panels[currentPanelName].elements[child:getName()]=caption; 
            		 table.insert(self.destoryElems,child)
                     
-- button                                                            
                elseif (child:getProperty("gk_gui_button")) then
                     dPrintf("Create button:%s %f %f %f %f",tostring(child:getProperty("gk_gui_button")),cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y)

					local oldTextColor = nil
					if (child:getProperty('color')) then
						local colors = child:getProperty('color'):split(',')
						oldTextColor = self.guiM:getPanelColors().buttonText
						self.guiM:getPanelColors().buttonText = Gk.ColourValue(
							tonumber(colors[1]),
							tonumber(colors[2]),
							tonumber(colors[3]),
							tonumber(colors[4]) or 1
						)
						
						
					end
					

					local fontsize = child:getProperty('fontsize') or -1 
                    local button = self.panel:createButton(cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y,
                                          child:getProperty("gk_gui_button"),
                                          fontsize
                                          )
                    local sprites = child:getProperty('sprites')
                    if (sprites) then
                        local sp = sprites:split(",")
                        
                        if (#sp==4) then
                            button:setBackgroundImage(sp[1],sp[2],sp[3],sp[4])                        
                        else
                            dPrintf("button:setBackgroundImage needs 4 comma seperated spritenames")
                        end
                    end


                     if (child:getProperty('on_callback')) then
                        local func = _registry[child:getProperty('on_callback')];
                        local data = child:getProperty('callback_data') or "";
                        dPrintf("btn: on_callback %s",tostring(func))
                        if (func) then
                            button:onClicked(func,data)
                        else
                        	dPrintf("COULD FIND BTN FUNC:%s",tostring(func))
                        end
                     end                                          
                     
                     if (oldTextColor) then
                     	self.guiM:getPanelColors().buttonText = oldTextColor
                     end
                     
                     button:setName(child:getName())                      
                     self.panels[currentPanelName].elements[child:getName()]=button;                         
                     table.insert(self.destoryElems,child)                                         
-- progressbar                
                elseif (child:getProperty("gk_gui_progressbar")) then
                     local progressbar = self.panel:createProgressbar(cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y
                                          )
                     progressbar:setValue(0.3)
                     progressbar:setName(child:getName())
                     self.panels[currentPanelName].elements[child:getName()]=progressbar;                         
                     table.insert(self.destoryElems,child)                                         
-- checkbox
                elseif (child:getProperty("gk_gui_checkbox")) then
                     local checkbox = self.panel:createCheckbox(cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y
                                          )
                     checkbox:setName(child:getName())                     
                     self.panels[currentPanelName].elements[child:getName()]=checkbox;
                     if (child:getProperty("checked")) then
                         checkbox:setChecked(child:getProperty("checked"))
                     end
                     table.insert(self.destoryElems,child)  

-- scrollbar
                elseif (child:getProperty("gk_gui_scrollbar")) then
                     local min = child:getProperty("max") or 95
                     local max = child:getProperty("min") or 0
                     local value = child:getProperty("value") or 0
                     local step = child:getProperty("step") or 1
                     
                     
                     local scrollbar = self.panel:createScrollbar(cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y,
                                          min,
                                          max
                                          )
                     if (value) then
                        scrollbar:setValue(value)
                     end
                    
                    
                     if (step) then
                        scrollbar:setStep(step)
                        --dPrintf("Step: %f",step);
                     else
                        scrollbar:setStep(0.01)
                     end
                     
                     local callback = child:getProperty("on_callback")
                     if (callback) then
                        local func = _registry[callback];
                        --dPrintf("btn: on_callback %s",tostring(func))
                        if (func) then
                            scrollbar:onChanged(func)
                        end
                     end
                     scrollbar:setName(child:getName())                                                                    
                     self.panels[currentPanelName].elements[child:getName()]=scrollbar;


                     table.insert(self.destoryElems,child)  
                                          
-- textfield
                elseif (child:getProperty("gk_gui_textfield")) then

                     text = child:getProperty("gk_gui_textfield") or ""
                     
                     local textfield = self.panel:createTextfield(cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y,
                                          text
                                          )
                                         
                     local font = child:getProperty("fontsize")
                     if (font) then
                        textfield:setFont(tonumber(font))
                     end                                         
                                         
                     self.panels[currentPanelName].elements[child:getName()]=textfield;
                     textfield:setName(child:getName())
                     table.insert(self.destoryElems,child)
                elseif (child:getProperty("gk_gui_rect")) then
                   dPrintf("Create rect: %f %f %f %f",cpos.x,
                          cpos.y,
                          cscl.x*100*scl.x,
                          cscl.y*100*scl.y)
  
                     local rect = self.panel:createRectangle(cpos.x,
                                                         cpos.y,
                                                         cscl.x*100*scl.x,
                                                         cscl.y*100*scl.y
                                                         )
                     local color = child:getProperty("color")
                     local spriteName = child:getProperty("sprite")
                     if (spriteName) then
                        rect:setBackground(spriteName)
                     elseif (color) then
                        local col = StringToColor(color)
                        rect:setBackground(col)
                     end
                     
                     local border = child:getProperty("border")
                     local borderColor = child:getProperty("border_color")
                     
                     if (border and borderColor) then
                         rect:setBorder(tonumber(border),StringToColor(borderColor))
                     end
                                      
                     self.panels[currentPanelName].elements[child:getName()]=rect;
                     table.insert(self.destoryElems,child)
-- listbox                                     
                elseif (child:getProperty("gk_gui_list")) then
                     local showElements = child:getProperty("amount") or 3

                     local listbox = self.panel:createListbox(cpos.x,
                                          cpos.y,
                                          cscl.x*100*scl.x,
                                          cscl.y*100*scl.y,
                                          showElements
                                          )
                     for i=0,20 do
                        if (child:getProperty(tostring(i))) then
                            listbox:addValue(child:getProperty(tostring(i)))
                        else
                          --dPrintf("%i not drin",i);
                          break;
                        end
                     end
                     
                     if (child:getProperty('on_callback')) then
                        func = _registry[child:getProperty('on_callback')];
                        --dPrintf("btn: on_callback %s",tostring(func))
                        if (func) then
                            listbox:onValueChanged(func)
                        end
                     end   
                     
                        
                     listbox:setName(child:getName()) 
                     
                     local objWrapper = ListBox(listbox,child:getProperty('lua_list'))
                     self.panels[currentPanelName].elements[child:getName()]=objWrapper;                         
                     table.insert(self.destoryElems,child)                                         
                
                end

               end
            end


        --dPrintf("%s, %s", obj:getName(), tostring(obj:getWorldPosition()))
        end
    end
    self:destroyFromScene()
	
	dPrintf("OVERRIDEN GUIMAKER!!!")
	
end

function TSGUI:destroyPanel(panelName)
	
end
