-- make the Gk-Filesystem available as 
function initPackageLoader()
  package.path = LUA_LIB_DIR..'/debugger/?.lua;?.lua;lua/lua-libs/?;' .. package.path
  
  local function load(modulename)
    dPrintf("TRY TO MODULE")
    local errmsg = ""
    -- Find source
    local modulepath = string.gsub(modulename, "%.", "/")
    for path in string.gmatch(package.path, "([^;]+)") do
      local filename = string.gsub(path, "%?", modulepath)
      dPrintf("Try to load filename:"..filename)
      if Gk.fileExists(filename) then
        -- Compile and return the module
        dPrintf("FOUND!")
        local result = assert(loadstring(assert(Gk.fileLoad(filename))))
        dPrintf("Ok:"..filename)
        return result
      end
      errmsg = errmsg.."\n\tno file '"..filename.."' (checked with custom loader)"
    end
    return errmsg
  end
  -- Install the loader so that it's called just before the normal Lua loader
  table.insert(package.loaders, 2, load)
end

-- get the first child that starts with "startName" of a gameobject
function getFirstChildStartingWith(me,startName)
	for i=0,me:getChildCount()-1 do
		local child = me:getChildAt(i);
		dPrintf("Check: %s",child:getName())
		if (child:getName():startsWith(startName)) then
			return child
		end
	end
	return nil
end

-- executes a couple of one-param-functions and combines them with or
-- e.g. unionOr(self,GameDB.isTrue,"a", self,GameDB.isPressed,"x")
function unionOr(...)
	local argos = {...}
	return function()
	  local i=0
	  while #argos>i do
	  	local aself = argos[i+1]
	  	local afunc = argos[i+2]
	  	local aparam = argos[i+3]
	  	
	  	if afunc(aself,aparam) then
	  		return true
	  	end
	  	
	  	i=i+3
	  end
      return false;
	end
	
end

function table.contains(tbl,elem)
   for _,k in ipairs(tbl) do
      if (k==elem) then
         return true
      end
   end
   return false
end

Clock = Class()

function Clock:constructor(checkFunc)
   self.checkFunc = checkFunc
   self:start()
end

function Clock:start()
   self.time = Gk.getSystemTime()
end

function Clock:stop(info,onScreen)
   local dif = Gk.getSystemTime() - self.time
   if onScreen then
      dPrintf("Time %s: %f",info or "",dif)
   end
      Gk.log("Time "..info..": "..dif)
   
   if (self.checkFunc) then
      self.checkFunc(self,dif)
   end
   
   return dif
end

-- executes a couple of one-param-functions and combines their result with AND-operation
-- e.g. unionAnd(self,GameDB.isTrue,"a", self,GameDB.isPressed,"x")
function unionAnd(...)
	local argos = {...}
	return function()
	  local i=0
	  
	  while #argos>i do
	  	local aself = argos[i+1]
	  	local afunc = argos[i+2]
	  	local aparam = argos[i+3]
	  	
	  	
	  	if not afunc(aself,aparam) then
	  		return false
	  	end
	  	
	  	i=i+3
	  end
      return true;
	end
end



--function warning(...)
--	if (_config.debug.warning) then
--		dPrintf(...)
--	end
--end
--
--function error(...)
--	if (_config.debug.error) then
--		dPrintf(...)
--	end
--end
--
--function info(...)
--	if (_config.debug.info) then
--		dPrintf(...)
--	end
--end

function table.removeKey(table, key)
    local element = table[key]
    table[key] = nil
    return element
end

function table.removeValueFromMap(tbl,value)
   local key = nil
   for k,v in pairs(tbl) do
      if v == value then
         key = k
         break
      end
   end
   return table.removeKey(tbl,key)
end

function table.getKeys(tbl)
   local keys = {}
   for k,_ in pairs(tbl) do 
      table.insert(keys,k)
   end
   return keys
end

function table.removeValue(tbl,obj)
	local pos = nil
	for i,v in ipairs(tbl) do
		if v==obj then
			pos = i
		end
	end
	if (pos) then
		table.remove(tbl,pos)
	end
end

function table.length(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

function table.contains(T,elem)
   for i,v in ipairs(T) do
      if v==elem then
         return true,i
      end
   end
   return false
end

function math.round(num, idp)
  local mult = 10^(idp or 0)
  return math.floor(num * mult + 0.5) / mult
end
--[[
function math.round(x)
  if x%2 ~= 0.5 then
    return math.floor(x+0.5)
  end
  return x-0.5
end
--]]
function math.makeFloat(x)
	return x + 0.0
end

function string:split(sep)
        local sep, fields = sep or ":", {}
        local pattern = string.format("([^%s]+)", sep)
        self:gsub(pattern, function(c) fields[#fields+1] = c end)
        return fields
end

startsWith = function(self, piece)
  return string.sub(self, 1, string.len(piece)) == piece
end
rawset(_G.string, "startsWith", startsWith)


function normalizePath(path)
    path = string.gsub(path, "\\", "/")
    local parts = path:split("/")
    
    for i,p in ipairs(parts) do
        if (p=='..') then
            if (i==1) then
                return "ERROR! Normpass cannot have ../ as first pathpart!"
            end
            
            parts[i]="X"
            
            local j=i;
            while (j>0) do
                if (parts[j]~="X") then
                    parts[j]="X"
                    j=0;
                else
                    j=j-1;
                end
            end
        end
    end

    local result = ""
    
    for i,p in ipairs(parts) do
        if (p~="X") then
            if (result~="") then
                result = result .. "/"
            end
            result = result..p
        end
    end
    return result
end

function stripControlCodes( str )
    local s = ""
    for i in str:gmatch( "%C+" ) do
    	s = s .. i
    end
    return s
end
 
function stripControlAndExtCodes( str )
    local s = ""
    for i = 1, str:len() do
	    if str:byte(i) >= 32 and str:byte(i) <= 126 then
	        s = s .. str:sub(i,i)
	    end
    end
    return s
end

Gk.log("Finished utils")
