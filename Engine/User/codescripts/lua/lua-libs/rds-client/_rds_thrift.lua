Gk.import(LUA_LIB_DIR.."/misc/_AsyncDispatcher.lua")
Gk.import(LUA_LIB_DIR.."/thrift/Thrift.lua")
Gk.import(LUA_LIB_DIR.."/thrift/TMemoryBuffer.lua")
Gk.import(LUA_LIB_DIR.."/thrift/TBinaryProtocol.lua")

Gk.import(LUA_LIB_DIR.."/rds-client/_rds_protocol.lua")
Gk.import(LUA_LIB_DIR.."/rds-client/_rds_client.lua")

TRDSTransport = TMemoryBuffer:new{
	__type = "TRDSTransport",
	inputQueue = {},
	host = "localhost",
	port = 62964,
	timeout = 12000,
	timeoutActive = false,
	idleCheck = 60000,
	waitingForData = false,
	dispatcher = AsyncDispatcher("rds-transport"),
	loop = nil,
	receiveMessages = false,
	connected = false,
}

function TRDSTransport:init()
	local me = self
	self:resetBuffer();
	
	self.loop = self.loop or DefaultGame()
	
	local callback = RDSCallback{
		onMessage=function(input)
			input = input or {}
			local data = input.data
			local rdsclient = input.client
			local type = input.type
			local channel = input.channel

			-- write incoming data from rds to the buffer
			if (data:byte(1)==95 and data:byte(2)==96 and
				data:byte(3)==95) then
				
				if (data:byte(4)==97) then
				--Gk.log("non thrift-msg: ignore (data)")
				   Gk.log("#ClientMsg!")
					me:onClientThriftMsg(data:sub(5))
				elseif (data:byte(4)==96) then
					me:onNonThriftMsg(data:sub(5))
				else
					error("Unknown Message-Type:"..data:byte(4))
				end
			else
			   Gk.log("#Reply!")
				-- TODO: do a different handling for channel-sessions!?			
				me.__parent.__parent:write(data)
			end		
		end,
		onDisconnect=function(graceful,client)
		end,
		onEvent = function(eventName,data)
		   if eventName == "no_connection" then
		     if me.connected then
              me.connected = false
              if SEND_MSG then
                SEND_MSG{type="network",subtype="no_connection"}
              end
		     end
		   end
			Gk.log("RDSTransport: Event:"..eventName)
		end,
	}
	self.rdsClient = RDSClient(self.host,self.port,{callback=callback,timeout=self.timeout,idleCheck=self.idleCheck})
	
	self:startAsyncReceiving()
	
	self.loop:addAlwaysQueue(function() 
--	   Gk.log("RDSTransport-Loop")
		me.dispatcher:asyncDispatch()
		return true
	end)
end

function TRDSTransport:activateTimeout(b)
   self.rdsClient:activateTimeout(b)
end

function TRDSTransport:startAsyncReceiving()
   local me = self
   self.dispatcher:call{
      call = function()
         while true do
            if (not me.receiveMessages) then
               -- suspend this task until I should receiveMessages again
               coroutine.yield(function() 
                  return me.receiveMessages 
               end)
            end
--            local result = me.rdsClient:receiveMessage()
            local result,data = pcall(me.rdsClient.receiveMessage,me.rdsClient)
            if not result then
               me.receiveMessages = false
               me.connected = false
               SEND_MSG{type="network",subtype="no_connection"}
            end
            coroutine.yield()
         end
      end
   }
end

function TRDSTransport:onNonThriftMsg(data)
	Gk.log("Got a nonthrifht msg:"..data)
end

function TRDSTransport:setClientProcessor(cprocessor)
   self.clientProcessor = cprocessor;
end

function TRDSTransport:onClientThriftMsg(data)
   local inBuf = TMemoryBuffer:new{}
   inBuf:resetBuffer(data)
   local iprot = TBinaryProtocol:new{trans=inBuf}
   --local callId = inBuf:read(1)
   
   local outBuf = TMemoryBuffer:new{}
   outBuf:write(string.char(95,96,95,97))
   --outBuf:write(callId)
   local oprot = TBinaryProtocol:new{trans=outBuf}
   
   if (not self.clientProcessor) then
      error("You got a client-message but didn't set a client-processor via TRDSTransport:setClientProcessor(..)")
   end

   local status = self.clientProcessor:process(iprot,oprot)
   
   --self.rdsClient:sessionMsg(outBuf:getBuffer())
end

function TRDSTransport:stopReceiving()
   self.receiveMessages = false;
end

function TRDSTransport:startReceiving()
   self.receiveMessages = true;
end


function TRDSTransport:open(username,password)
	if (not username or not password) then
		error("TRDSTransport need username,password!")
	end
	
	self.username = username
	self.password = password

	--request login
   return self:reopen()
end

function TRDSTransport:reopen()
   local connectionSuccess,err = self.rdsClient:login(self.username,self.password)
   if (not connectionSuccess) then
     Gk.log("no connection:"..tostring(err))
     return false
   end
   self.connected = true
   self:startReceiving()
   -- wait until login is confirmed
   self:waitForLogin()
   Gk.log("Login: go on with coroutine")
   return true
end

function TRDSTransport:close()
   self.connected = false
   self.rdsClient:logout()
   self:stopReceiving()
   self:resetBuffer()
   Gk.log("rdsclient: LOGOUT")
end

function TRDSTransport:waitForLogin()
   local me = self
   coroutine.yield(function() return me.rdsClient:isLoggedIn() end)
end

function TRDSTransport:flush()
   local msg = self:getBuffer()
   self:resetBuffer()
   self.__parent.__parent:resetBuffer()
   self.rdsClient:sessionMsg(msg)
end

function TRDSTransport:readAll(len)
   if (not self.connected) then
      error("not connected")
   end
   
	local me = self
	if (me.__parent.__parent:available() < len) then
		coroutine.yield(function() return me.__parent.__parent:available() > len end)
	end
	
	return self.__parent.__parent:readAll(len)
end






--callback = RDSCallback{
--	onMessage = function(data,client)
--		dPrintf("Got Message:"..data)
--	end
--}

--client = RDSClient("localhost",62964,callback)
--client = RDSClient("localhost",8000)



--__RDSDispatcher:call{
--	call = function()
--		while true do
--			client:receiveMessage()
--		end
--	end
--}

--		client:login("tom2","dertom")

		
function sendMessage()
	__rdsTransport.dispatcher:call{
		call = function() 
			local echo = __rdsService:echo("__Echo:"..tostring(os.time()))
			dPrintf("%i: Got Echo:%s",os.time(),echo)
			local user = __rdsService:getUser()
			dPrintf("%i: getUser %s",os.time(),user.username)			
		end
	}
end

--__RDSDispatcher:call{
--	call = function() 
----		coroutine.yield(waitSecs(2))
----		client:login("tom2","dertom")
----
--		coroutine.yield(waitSecs(3))
--		client:sessionMsg("CLIENT 1")
--		coroutine.yield(waitSecs(3))
--		client:sessionMsg("CLIENT 2")
--		coroutine.yield(waitSecs(3))
--		client:sessionMsg("CLIENT 3")
--		return true
--	end,
--	onSuccess = function()
--		Gk.log("FINISHED LOGIN-STUFF")
--	end 
--}

--		client:login("tom2","dertom")
--		client:sessionMsg("From Client to server")

--LOOP:addAlwaysQueue(function() 
--	__RDSDispatcher:asyncDispatch()
--	return true
--end)

--client:reconnect()
dPrintf("DONE:%i",RDS_PROTOCOL.LOGIN_REQUEST)
