Gk.import(LUA_LIB_DIR.."/misc/_pickletable.lua")

if Templater then
  Gk.log("tried to reapply templater! skipped")
  return
end

-- for debugging purpose
_pickleSerializer["gsGuiElement"]=function(elem)
	local id = elem:getId()
	return string.format("'%s'",id or "")
end

_pickleSerializer["gsGuiDocumentRocket"]=function(elem)
	return string.format("'%s'","doc")
end

IDCOUNTER = 0

function NEXT_ID()
	IDCOUNTER = IDCOUNTER + 1
	return "__tt_"..IDCOUNTER
end

TTCOUNTER = 0
function NEXT_TT_ID()
   TTCOUNTER = TTCOUNTER + 1
   return "__utt_"..TTCOUNTER
end

Gk.import(LUA_LIB_DIR.."/misc/_AsyncDispatcher.lua")
Gk.import(LUA_LIB_DIR.."/misc/_Languagemanager.lua")

Templater = Class()

function Templater:constructor(input)
	self.__type="Templater"
	self.elem2tt = {}
	self.tid2tt = {}
	self.ttAttributes = {}
	
	self.focus = {
	  current = {},
	  elements = {},
	  row = 1,
	  col = 1
	}
	
	self.rootElement = TemplaterElement{id="ROOT"}
	input = input or {}
	if (input.html) then
		self.html = input.html
	end
	self.parsed = false
	self.gui = input.gui or Gk.gsGUI()
   self.gui:showDebugger(input.showDebugger==true)
   
   self.currentFocusContainer = nil
--   self.gui:showDebugger(true)
	
	-- add the attributes defined in _templaterAttributes.lua
   self:addAttribute("tta_lang",TAttrLang())
   self:addAttribute("tta_boxsizing",TAttrBoxSizing())
	self:addAttribute("tta_rest",TAttrRest())
	
	self:addAttribute("tta_check",TAttrCheck())
	self:addAttribute("tta_focusname",TAttrFocusName())
	self:addAttribute("tta_focusorder",TAttrFocusOrder())
end

function Templater:addAttribute(name,attriblogic)
   self.ttAttributes[name] = attriblogic
end

function Templater:executeRemoval()
	for _,elem in ipairs(self.remove) do
		elem:remove()
	end
end

function Templater:addFocusElement(elem)
   if not self.focus.current then
      error("There is no currentFocus set for element:"..tostring(elem.id))
   end

   local absx = elem.elem:getAbsX()
   local absY = elem.elem:getAbsY()
   local pos = math.round(absY / 100)

   local posContainer = self.focus.current.elems[pos]
   if not posContainer then
      posContainer = {}
      self.focus.current.elems[pos] = posContainer
   end

   table.insert(posContainer,elem)
   table.sort(posContainer,function(a,b)
      local ax = a.elem:getAbsX()
      local bx = b.elem:getAbsX() 
      return ax < bx
   end)
   
   if (not table.contains(self.focus.current.ids,pos)) then
      table.insert(self.focus.current.ids,pos)
      table.sort(self.focus.current.ids)
   end
   return posContainer
end

function Templater:setCurrentFocus(name)
   local newfoc = self.focus.elements[name]
   if not newfoc then
      newfoc = { elems={},ids={}}
      self.focus.elements[name] = newfoc
   end
   self.focus.current = newfoc
   return newfoc
end

function Templater:activateFocus(name)
   local focus = self.focus.elements[name]
   if not focus then
      return
--      error("Unknown focus:"..name)
   end
   
   self.focusCtx = self:setCurrentFocus(name);
   
   self.focus.row = 1
   self.focus.col = 1
   self:setFocusElement(self.focus.row,self.focus.col)
end

function Templater:setFocusElement(row,col)
   if self.focus.focusElem then
      if self.focus.focusElem.onUnFocus then
         self.focus.focusElem.onUnFocus()
      else
         self.focus.focusElem.elem:setPseudoClass("focustest",false)
      end
   end
   self.focus.focusElem = self.focusCtx.elems[self.focusCtx.ids[row]][col]
   if self.focus.focusElem then
      if self.focus.focusElem.onFocus then
         self.focus.focusElem.onFocus()
      else
         self.focus.focusElem.elem:setPseudoClass("focustest",true)
      end
   end
   
   local a = 0
end

function Templater:changeFocus(x,y)
   self.focus.row = self.focus.row + y
   if (self.focus.row > table.length(self.focus.current.elems)) then
      self.focus.row = 1
   elseif (self.focus.row < 1) then
      self.focus.row = #self.focus.current.elems
   end
   local row = self.focus.current.elems[self.focus.current.ids[self.focus.row]]
   self.focus.col = self.focus.col + x
   if (self.focus.col < 1) then
      self.focus.col = #row
   elseif (self.focus.col > #row) then
      self.focus.col = 1
   end
   self:setFocusElement(self.focus.row,self.focus.col)   
end

function Templater:_asyncTimeCheck(func)
   if (self.__async and __current_thread) then
      local dif = Gk.getSystemTime() - self.__async.lastTime
      if (dif >= self.__async.time) then
         Gk.log("FIRED TIMECHECK")
         coroutine.yield(func)
         self.__async.lastTime = Gk.getSystemTime()
      else
         Gk.log("Templater-Async-Check:"..dif.." < "..self.__async.time)
      end
   end
end

function Templater:asyncParse(input)
   input = input or {}
   local dispatcher = input.dispatcher or AsyncDispatcher.get("templater")
   local game = input.game or game or DefaultGame()
   self.game = game
   local parserActive = true
   local parserTicks = 0
   game:addAlwaysQueue(function() 
      Gk.log("Template-Parser-Tick:"..parserTicks)
      parserTicks = parserTicks + 1
      dispatcher:asyncDispatch() 
      return parserActive 
   end)
   
   local me = self
   dispatcher:call{
      call = function()
         input.asyncTime = input.asyncTime or 50
         me:parse(input)
         parserActive = false
         if input.onFinish then
            input.onFinish()
         end
      end,
      onError = function(err)
         Gk.log("ERROR TEMPLATER-PARSER:"..tostring(err))
      end
   }
end

function Templater:show()
  self.doc:createInstance()
end

function Templater:hide()
  self.doc:destroyInstance()
end

function Templater:parse(input)
	input = input or {}
	local disable_remove = input.disable_remove or false
	local disable_autocreate = input.disable_autocreate or false
	
	if (input.asyncTime) then
      self.__async = {
        time = input.asyncTime, lastTime = Gk.getSystemTime()
      }
	end
	
	self.doc = self.gui:getDocument(self.html)
	
	if (not disable_autocreate) then
		self.doc:createInstance()
	end

	self.mapping = {}
	-- what elements to remove using "tt_remove-attribute"
	self.remove = {}
	for i=0,self.doc:getChildAmount()-1 do
		local elem = self.doc:getChild(i)
		self:checkElem(elem,self.mapping)
		-- apply tt-attributes
		self:applyAllAttributes(elem)
	end
	
	if (not disable_remove) then
		self:executeRemoval()
	end
	
	Gk.log("1 - INIT")
	
	-- wire types
	-- store elements that will be initalized after the first pass
	self.initElements = {}
	self:autowire(self,self.mapping)
	for i,initElem in ipairs(self.initElements) do
		if (initElem.init) then
			initElem:init()
		end
	end


   Gk.log("2 - afterinit")

	
   for _,initElem in ipairs(self.initElements) do
      if (initElem.me.afterInit) then
         initElem.me:afterInit()
      end
   end
   
   for i=0,self.doc:getChildAmount()-1 do
      local elem = self.doc:getChild(i)
      self:checkElem(elem,self.mapping)
      -- apply tt-attributes
      self:applyAllAttributes(elem,"afterInit")
   end
   
   Gk.log("3 - afterinit2")

   for _,initElem in ipairs(self.initElements) do
      if (initElem.me.afterInit2) then
         initElem.me:afterInit2()
      end
   end   

   self:updateLocalization()
	
	-- apply attributes
  
	
	dPrintf("AFTER PARSE:%s",self.doc:getRML())
	self.parsed = true	
end

function Templater:isParsed()
   return self.parsed
end

function Templater:updateLocalization()
   for key,initElem in ipairs(self.initElements) do
      if (initElem._setLocalization) then
         initElem:_setLocalization()
      end
   end
end

function Templater:checkElem(cElem,container)
   -- check if we are in time contigenent if using async-dispatch
   self:_asyncTimeCheck()
   
   local ttRemove = cElem:getAttribute("tt_remove")
   if (ttRemove=="1" or ttRemove=="true") then
      table.insert(self.remove,cElem)
   end
   
   local ttAttr = cElem:getAttribute("tt")
   local ttType = cElem:getAttribute("tt_type")
   
   Gk.log("Check:"..cElem:getTagName().." tt:"..tostring(ttAttr))

   local id = cElem:getId()
   if (id=="") then
      id = NEXT_ID()
      cElem:setId(id)
      cElem:setAttribute("autoid","1")
   end


   if (ttAttr~="") then
      dPrintf("FOUND")
      if ttAttr=="_" then
         ttAttr=NEXT_TT_ID()
      end
      
      local ttTable = {
         name = ttAttr,
         id = id,
         doc = self.doc,
         elem = cElem,
         elemId = cElem:getId(),
         container = {},
      }
      
      if (ttType and ttType~="") then
         local type = _G[ttType]
         if (not type) then
            error("unknown type:"..ttType.." for tt("..ttAttr..")\n"..cElem:getRML())
         end
         ttTable.type = type
      else
         ttTable.type = TemplaterElement
      end

      container[ttAttr] = ttTable
      container = ttTable.container
   end
      
-- check children 
   for i=0,cElem:getChildAmount()-1 do
      local elem = cElem:getChild(i)
      self:checkElem(elem,container)
   end
end



function Templater:applyAllAttributes(root,functionCall)
   local me = self
   Templater._traverseHTML(root,function(cElem)
      self:applyTTAttributes(cElem,functionCall)
   end)
end

function Templater:applyTTAttributes(elem,functionCall)
   functionCall = functionCall or "init"
   for aName,attr in pairs(self.ttAttributes) do
      local tmpValue = elem:getAttribute(aName)
      if (tmpValue and #tmpValue>0) then
         -- bypass the attributes value to the attribute logic
         --attr:init(elem,tmpValue)
         if (attr[functionCall]) then
            attr[functionCall](attr,elem,tmpValue,self.elem2tt[elem],self)
         end
      end
   end   
end


function Templater:autowire(currentBlock,mappingTable)

	for id,info in pairs(mappingTable) do
		Gk.log("Autowire:"..id)
		if (info.type) then
			local resultElem = nil;
			
			resultElem = info.type{id=info.name}
			resultElem:setInfo(info,self,false) 
		   if (resultElem.hideOnCreation) then
		      resultElem:hide(true)
--		      resultElem.elem:setProperty("display","none")
		   end
		   currentBlock:add(resultElem);
		   
			
			table.insert(self.initElements,resultElem)			
			
			-- rewire child-elements
         self:autowire(resultElem,info.container)  
		end
	end	
end


function Templater:add(tElem,initElem)
	if (initElem == nil) then
		initElem = true
	end
	
	self.rootElement:add(tElem)
	
	return tElem		
end

-- get elements byt tt-id dot-separated "page.elem.a"
function Templater:get(id)
   local result = self.rootElement:get(id)
   return result
end

function Templater._traverseInfo(info,visitor)
	if (visitor) then
		visitor(info)
	end

	for _,childInfo in pairs(info.container) do
		Templater._traverseInfo(childInfo,visitor)
	end	
end

function Templater._traverseHTML(elem,visitor)
   if (visitor) then
      visitor(elem)
   end
   
   for i=0,elem:getChildAmount()-1 do
      local child = elem:getChild(i);
      Templater._traverseHTML(child,visitor)
   end
end

function Templater:_traverseParents(elem,visitor)
   local goOn = true
   if (visitor) then
      goOn = visitor(elem)
   end
   
   if (goOn and elem:getParent()) then
      Templater._traverseParents(elem:getParent(),visitor)
   end
end


-- to be used from within inline-rcss-event
function Templater:current(elem) 
	local current = elem or Gk.getCurrentRCSSElement()
	local logic = nil
	repeat
		logic = self.elem2tt[current] 
		current = current:getParent()
	until(logic or current==nil)
	return logic
end

TemplaterElement = Class()

-- {id=ttAttrib,container=templaterParser-container}
function TemplaterElement:constructor(input)
	self.me = self
	self.id = input.id
	self.type = nil
	self.parentBlock = input.parentBlock
   self.hideOnCreation = input.hideOnCreation or self.hideOnCreation  
	
	self.meta = {}
	self.children = {}
	self.boundEvents = {}
   
	--dPrintf("Created element with id:%s",self.id)
	if (not self.id) then
		error("no id in templater-element")
	end
end



function TemplaterElement:add(child,init)
   self.children[child.id] = child
   child.parent = self.me
end

-- set reference to this element to all inhertitance-layers
function TemplaterElement:setMe(me)
   local start = me
   start.me = me
   while (start.base) do
      start.base.me = me
      start = start.base
   end
end

function TemplaterElement:init()
	-- this is more or less the constructor, since at this state the TemplaterElement
	-- is in the right context (as he is NOT in the real constructor)
	--Gk.log("DefaultInit")
end

function TemplaterElement:_setLocalization()
   local lang = self.elem:getAttribute("tt_lang")

   if (lang and #lang>0) then
      if not LanguageManager.instance then
         Gk.log("Error: No languagemanager! Couldn't use language-templater-attribute:"..tostring(lang))
      end
   -- set localized data here
   -- this function gets called when the language is changed and right after the first init
      local text = LanguageManager.instance:get(lang)
      if (text) then
         self.elem:setInnerRML(text)
      end
   end
end

function TemplaterElement:hasBoundEvent(evtName)
   return self.boundEvents[evtName]
end

function TemplaterElement:bindEvent(evtName,selfObj,Clazz)
   if (Clazz) then
      self.elem:addEventListener(evtName,selfObj,Clazz)
   else
      self.elem:addEventListener(evtName,selfObj)
   end
   self.boundEvents[evtName]=true
end

function TemplaterElement:unbindEvent(evtName)
   self.elem:removeEventListener(evtName)
   self.boundEvents[evtName]=nil
end

function TemplaterElement:unbindAll()
   for eName,_ in pairs(self.boundEvents) do
      self:unbindEvent(eName);
   end
end

function TemplaterElement:setInfo(info,templater,initElem)
   if (initElem==nil) then
      initElem = true
   end
   
   self.info = info
   self.parent = info.elem:getParent()
   self.parentId = self.parent:getId()
   self.templater = templater
   self.elem = info.elem
   self.parentBlock = nil
   
   self.logic = self.me 
   
   --[[ if (initElem and self.me.init) then
      self.me:init()
   end --]]

-- self.templater[initElem] = self.me
   self.templater.elem2tt[self.elem]=self.me
   self.templater.tid2tt[self.id] = self.me
end

function TemplaterElement:create(input)
   input = input or {}
   dPrintf("CREATE TemplaterElement")
   if (self.isClone) then
      error("Cant use create on cloned block")
   end
   input.cloneHTMLElement=true
   input.initElems = {}
   local clone = self:clone(input)
   
   -- initalize the new cloned objects
   for i,elem in ipairs(input.initElems) do
      elem:init()
      elem:_setLocalization()
      if (self.hideOnCreation) then
           elem.me:hide(true)
      end
   end
   
   for i,elem in ipairs(input.initElems) do
      if (elem.afterInit) then
         elem:afterInit()
      end
   end
   
   return clone
end

function TemplaterElement:hide(byprop)
   if self.hidden then
      return
   end
   
   if not byprop then
      self.me.hidden = true
      self.me.hiddenMode = "byclass"
      self.elem:setClass("hidden",true)
   else
      self.me.hiddenMode = "byprop"
      self.me.hidden = true
      self.me.hidden_lastDisplayProperty = self.elem:getProperty("display")
      self.elem:setProperty("display","none")
   end
end

function TemplaterElement:show()
   if not self.me.hidden then
      return
   end
   
   if not self.me.hiddenMode or self.me.hiddenMode=="byclass" then
      self.me.elem:setClass("hidden",false)
   else
      self.me.elem:setProperty("display",self.hidden_lastDisplayProperty or "block")
   end
   self.me.hidden = false
end

function TemplaterElement:clone(input)
   input = input or {}
   local currentBlock = input.currentBlock or self.parent
   local cloneHTMLElement = input.cloneHTMLElement 
   local appendTo = input.appendToElem
   local parentTo = input.parentToElem or self.elem;
   
   local initElems = input.initElems
   
-- if (not currentBlock) then
--    error("You cant clone label without setting a block")
-- end
   
 
   Gk.log("CLONE LABEL "..self.id)
   self.info = self.info or self.me.info
   local clone = self.info.type{id=self.id,text=self.text}
   clone.parent = currentBlock
   clone.info = self.info
   clone.parentId = self.parentId
   clone.templater = self.templater
   
   if (cloneHTMLElement) then
      clone.elem = self.info.elem:clone()
      
      if (appendTo) then
         appendTo:appendChild(clone.elem)
      else
         parentTo:getParent():insertBefore(parentTo,clone.elem)
      end
      self.templater:applyAllAttributes(clone.elem)
      currentBlock = clone.elem
   end   
   
   if (currentBlock) then
      clone.elem = currentBlock:getElementById(self.info.elemId)
   end
   
   for id,elem in pairs(self.children) do
      clone.children[id] = elem.me:clone{currentBlock=clone.elem,initElems = initElems}
   end
   
   table.insert(input.initElems,clone)
   
   self.templater.elem2tt[clone.elem]=clone.me
   self.templater.tid2tt[clone.id] = self.me
   
   return clone	
end

function TemplaterElement:get(id)
	local splits = id:split(".")

	local temp = self	
	
	for i=1,#splits do
		local elemName = splits[i]
		temp = temp.children[elemName]
	end

	if (temp) then
		if temp.me then
         return temp.me
		else
         return temp
		end
	else
		error("block.getInfo(..) :unknown element with id "..id.." in block:"..self.id)
	end	
end

function TemplaterElement:traversInfo(visitor)
	Templater._traverseInfo(self.info,visitor)	
end

function TemplaterElement:remove()
	self.me.templater.elem2tt[self.me.elem]=nil
	self.me.elem:remove()
	
	-- TODO: remove all that logic stuff	
end

function TemplaterElement:bindEvent(evtName,selfObj,Clazz)
   -- TODO: This is a hack. For some reason gamekit crashes
   --       if calling gkLuaEvents that are created from within
   --       a coroutine. therefore do this via game-queue outside
   
   if __current_thread then
      local me = self
      self.templater.game:addOnceQueue(function() 
         me:_bindEvent(evtName,selfObj,Clazz)
      end)
   else
      self:_bindEvent(evtName,selfObj,Clazz)
   end
end

function TemplaterElement:_bindEvent(evtName,selfObj,Clazz)
   if (Clazz) then
      self.elem:addEventListener(evtName,selfObj,Clazz)
   else
      self.elem:addEventListener(evtName,selfObj)
   end
   table.insert(self.boundEvents,evtName)
end

function TemplaterElement:unbindEvent(evtName)
   self.elem:removeEventListener(evtName)
   table.removeValue(self.boundEvents,evtName)
end

function TemplaterElement:unbindAll()
   for _,eName in ipairs(self.boundEvents) do
      self:unbindEvent(eName);
   end
end


TemplaterAttribute = Class()

function TemplaterAttribute:constructor(name)
end

function TemplaterAttribute:init(elem)
   Gk.log("TemplaterAttribute: Default");
end





Gk.import(LUA_LIB_DIR .. "/templater/_templaterElements.lua")
Gk.import(LUA_LIB_DIR .. "/templater/_templaterAttributes.lua")
Gk.import(LUA_LIB_DIR .. "/templater/_screenmanager.lua")
