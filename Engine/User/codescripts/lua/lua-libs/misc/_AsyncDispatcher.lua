Gk.import(LUA_LIB_DIR.."/misc/_utils.lua")

-- convenience
yield = coroutine.yield

AsyncDispatcher = Class()

--------------------------------------------------------------------------
function AsyncDispatcher.get(name)
   name = name or "Default"
   if (not AsyncDispatcher.__ALL) then
      AsyncDispatcher.__ALL = {}
   end
   
   local result = AsyncDispatcher.__ALL[name]
   if not result then
      result = AsyncDispatcher(name)
   end 
   return result
end

function AsyncDispatcher.getLast()
   return AsyncDispatcher.__LAST
end

function AsyncDispatcher.getLastUsed()
   return AsyncDispatcher.__LASTUSED
end
--------------------------------------------------------------------------
function AsyncDispatcher:constructor(name)
   if (not AsyncDispatcher.__ALL) then
      AsyncDispatcher.__ALL = {}
   end
   
   self.wrapped = {}
   self.inWrapMode = false
   self.name = name or "noname"
   
   if (name) then
      if (AsyncDispatcher.__ALL[name]) then
         error("Dispatcher-Name "..name.." already registered!");
      end
      
      AsyncDispatcher.__ALL[name] = self;
   end
   
	self.asyncPools = {}
	
	if (not AsyncDispatcher.__FIRST) then
      AsyncDispatcher.__FIRST = self
	end
	AsyncDispatcher.__LAST = self
end

function AsyncDispatcher:getName()
   return self.name
end

function AsyncDispatcher:asyncHasPools()
	return #self.asyncPools > 0 
end

function AsyncDispatcher:asyncNumPools()
	return #self.asyncPools
end

function AsyncDispatcher:asyncNumThreads(poolNr)
	local pool = self.asyncPools[1 + poolNr]
	
	if (pool) then
		return #pool
	else
		return 0
	end
end

function AsyncDispatcher:call(input)
   AsyncDispatcher.__LASTUSED = self

	input = input or error("async call needs an input-table with key:call(req),onSuccess(opt),onError(opt),poolNr(opt)")
	local call = input.call or error("async-call needs a call-function")
	local onSuccess = input.onSuccess
	local onError = input.onError
	local poolNr = input.poolNr or 0
	
	local co = coroutine.create(function()
		local result = call()
   	if (onSuccess) then
			onSuccess(result)
		end
	end)

	-- add the call to a specific pool
	local pool = nil
	if (type(poolNr)=="number") then
		pool = self.asyncPools[1 + poolNr]
	elseif (type(poolNr)=="string") then
		if (poolNr=="new") then
			-- do nothing, as it will automatically create a new pool
		end
	else
		pool = poolNr
	end
	
	if (pool == nil) then
		pool = {}
		table.insert(self.asyncPools,pool)
	end	
	table.insert(pool,{resumeCondition=nil,co=co, onError=onError,name=input.name})
	return co,pool
end

function AsyncDispatcher:futureCall(input)
   local future= input.future or Future()
   
   self:call{
      name = input.name,
      poolNr = input.poolNr,
      call = function()
               local result = input.call()
               if input.onFinish then
                  input.onFinish(result)
               end
               future.result = result
               Gk.log("Future Success finished")
               future:_setFinished()
             end,
      onError = function(err)
         if input.onError then
            input.onError(err)
         end 
         future.err = err 
         Gk.log("Future Fail finished")
         future:_setFinished()
         if input.onFinish then
            input.onFinish("err",err)
         end
      end
   }
   return future   
end

function AsyncDispatcher:wrap(obj)
   local me = self
   
   local newMt = {
      __index = function(tbl,key)
         local oldmt = rawget(tbl,"__oldmt")
         
         Gk.log("Check("..me.name.."|"..tostring(oldmt.__index).."):"..tostring(tbl).." | "..tostring(key))
         local func = nil
         if (type(oldmt.__index)=="table") then
            func = oldmt.__index[key]
         else
            func = oldmt.__index(tbl,key)
         end
         Gk.log("ok:   "..tostring(func).." type:"..type(func))
         if not func and key:startsWith("async_") then
            local normalName = key:sub(7)
            if (type(oldmt.__index)=="table") then
               func = oldmt.__index[normalName]
            else
               func = oldmt.__index(tbl,normalName)
            end
            if func then
               return function(...)
                  local args=...
                  return me:futureCall{
                     name="asyncwrap:"..normalName,
                     call=function()
                        return func(args)
                     end,
                     onError=function(err)
                        Gk.log(me.name..":WrappedAsyncCallProblem("..normalName.."):"..tostring(err))
                     end
                  }
               end
            else
               return nil
            end
         else
            return func
         end
      end
   }  
   obj.__oldmt=getmetatable(obj)
   setmetatable(obj,newMt); 
end

function AsyncDispatcher:asyncDispatch()

	local asyncThreads = self.asyncPools[1]
	
	if (not asyncThreads) then
		return
	end

	if (#asyncThreads==0) then
		Gk.log("Remove current async-pool")
		table.remove(self.asyncPools,1)
		self:asyncDispatch()
		return
	end
	
	if #asyncThreads > 0 then
		local n = table.getn(asyncThreads)
		local remove=nil
		for i=1,n do 
			local threadData = asyncThreads[i]
			local currentThread = threadData.co
			
			if (threadData.resumeCondition==nil or threadData.resumeCondition() ) then
				local clock = Clock()
				clockNr = (clockNr or 0) + 1
--				Gk.log("---- START CLOCK -----"..clockNr)
            __current_thread = threadData
            if __current_thread.name then
               Gk.log("Current Thread:"..__current_thread.name)
            end
				local status,result = coroutine.resume(currentThread)
				__current_thread=nil
			--local dif = clock:stop("AsyncDispatch: "..self.name.." took",true)
--				Gk.log("----------Stop Clock ----------"..clockNr)
				if (status) then
					if (result and type(result)=="function") then
						--print ("Status:"..tostring(coroutine.status(currentThread)))
						threadData.resumeCondition = result
					else
					   threadData.resumeCondition = nil
					end
				else
					if (threadData.onError) then
						threadData.onError(result)
					end
									
					if type(result)=="string" then
					 print ("error inside of thread:"..tostring(result))
				   else
				     print("error inside of thread:"..tostring(result.errorCode)..":"..tostring(result.errorMsg))
				   end
				   
				end
				
				if (not status or coroutine.status(currentThread)=="dead") then
					Gk.log("finished thread:"..i.." / "..#asyncThreads)
					if not remove then
					 remove = {}
					end
					table.insert(remove,threadData)
				end 
			end
			
		end	
		if remove then
		 for _,thread in ipairs(remove) do
		     table.removeValue(asyncThreads,thread)
		 end
		end
	end
		
end

function AsyncDispatcher:createDummyFuture(name)
   return self:futureCall{
      poolNr="new",
      name=name,
      call=function() return "dummyFtrCall_done" end,
      onError=defaultError((name or "").." dummycall error")
   }
end

Future = Class()

function Future:constructor()
   self.__type="future"
   self.result = nil
   self.finished = false
   self.err = nil
end

function Future:addOptionalCheck(func)
   self.optionalCheck = func
end

function Future:wait(comment)
   local me = self
   self.comment = comment
   coroutine.yield(function() 
      if self.optionalCheck then
         local optionalResult = self.optionalCheck()
         if optionalResult then
            me:_setFinished()
         end
      end
      
      return me.finished 
   end)
end

function Future:_setFinished()
   self.finished = true
end

function Future:getResult()
   return self.result
end



--helper to be used in coroutine.yield

function yWaitForFreeDispatcher(dispatcher)
   coroutine.yield(waitForFreeDispatcher(dispatcher))
end

function waitForFreeDispatcher(dispatcher)
   return function()
      return #dispatcher.asyncPools==0
   end
end

-- wait for secs
function yWaitMs(ms)
   coroutine.yield(waitMs(ms))
end

function waitMs(ms)
	local endTime = Gk.getSystemTime() + ms
	return function()
	   if Gk.getSystemTime() > endTime then
--	     Gk.log("FINISHED TIME!!! ###")
	   end
		return Gk.getSystemTime() > endTime 
	end
end

function waitForState(fsm,state)
   return function()
      Gk.log("fsm: "..fsm:getState().." == "..state.."?")
      return fsm:getState() == state
   end
end
-- wait until a variable is set. startpoint is global as default but can be all other local var
-- eg. waitUntilSet("x",vec1)
function waitUntilSet(varPath,startpoint)
   return waitUntilValue(varPath,startpoint,nil,true)
end


function waitUntilValue(varPath,startpoint,value,negate)
   startpoint = startpoint or _G
   negate = negate or false
   return function()
      local splits = varPath:split(".")
      local parentObject = startpoint 
      
      -- get the object where the var will be included in the future (or already) 
      for i=1,#splits do
         local elemName = splits[i]
         parentObject = parentObject[elemName]
         if (not parentObject) then
            break
         end
      end
      
      if negate then
         return parentObject ~= value
      else
         return parentObject == value
      end
   end
end