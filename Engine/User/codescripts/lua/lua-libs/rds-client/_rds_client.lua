RDSMessage = Class()

function RDSMessage:constructor(opcode)
	self.msg = writeByte(opcode) 
end

function RDSMessage:addByte(b)
	self.msg = self.msg .. writeByte(b)
	return self;
end

function RDSMessage:addUShort(s)
	self.msg = self.msg .. writeUI16(s)
	return self;
end

function RDSMessage:addText(txt,withoutLen)
	if (withoutLen) then
		self.msg = self.msg .. txt
	else
		self.msg = self.msg .. writeUI16(#txt) .. txt
	end
	return self;
end

function RDSMessage:get()
	return self.msg
end

RDSCallback = Class()

function RDSCallback:constructor(table)
	table = table or {}
	self.funcOnMessage=table.onMessage
	self.funcOnDisconnect=table.onDisconnect
	self.funcOnEvent=table.onEvent
end

function RDSCallback:onMessage(input)
	input = input or {}
	local data = input.data
	local rdsclient = input.client
	local type = input.type or "message" -- possible message or channel
	local channel = input.channel
	
	if (self.funcOnMessage) then
		self.funcOnMessage(input)
	else
		Gk.log("RDSCallback: Got msg:"..data)
	end
end

function RDSCallback:onDisconnect(graceful,rdsclient)
	if (self.funcOnDisconnect) then
		self.funcOnDisconnect(graceful,rdsclient)
	else		
		Gk.log("RDSCallback: Disconnected! (grafully:"..graceful..")")
	end
end

function RDSCallback:onEvent(eventName,data)
	if (self.funcOnEvent) then
		self.funcOnEvent(eventName,data)
	else
		Gk.log("RDSCallback-Event:"..eventName.." : "..tostring(data))
	end
end

RDSClient = Class()

function RDSClient:constructor(host,port,options)
   options = options or {}
	self.host = host
	self.port = port
   self.timeout = options.timeout or 120000
   self.idleCheck = options.idleCheck or 10000
   self.timeoutActive = false
   
   self.reconnectKey = nil
   self.loggedIn = false
	self.callback = options.callback
   self.connected = false
   
	if (Gk.getPlatform()=="EMSCRIPTEN") then
    Gk.initCallbacks()
    Gk.setSocketCallback(function(type,a,b) 
       Gk.log("Type:"..tostring(type))
       if (type=="open") then
         self.connected=true
       elseif (type=="error") then
         Gk.log("Error:"..tostring(a).." | "..tostring(b))
       elseif (type=="message") then
         Gk.log("MSG:"..tostring(a).." "..tostring(b))
       end
    end)
   end


end

function RDSClient:activateTimeout(b)
   self.timeoutActive = b
   self.timeWithoutBuf = 0
end

function RDSClient:connect()
   self.socket = require "socket"
   self.tcp = assert(self.socket.tcp())
   self.tcp:settimeout(0)
   local res,err = self.tcp:connect(self.host,self.port)
   a = 0;
end

function RDSClient:login(username,password)
   self:connect()
   
   self.currentUserName = nil
   if (Gk.getPlatform() == "EMSCRIPTEN") then
      local me = self
      coroutine.yield(function() return me.connected end)
--      Gk.log("EM - Connected")
   end


--   Gk.log("Connected but WAITING Start waiting")
   coroutine.yield(waitMs(250))
--   Gk.log("....lets send")


   local msgLogin = RDSMessage(RDS_PROTOCOL.LOGIN_REQUEST)
   msgLogin:addByte(RDS_PROTOCOL.VERSION)
   msgLogin:addText(username)
   msgLogin:addText(password)

   local ok,err = self:sendMsg(msgLogin,true)
   a = 1

   if (not err) then
      -- the name of the user who TRIES to login
      self._loginUser = username
   else
      self._loginUser = nil
   end
   return err==nil, err
   
end

-- not implemented on server
function RDSClient:reconnect()
	local msgReconnect = RDSMessage(RDS_PROTOCOL.RECONNECT_REQUEST)
	msgReconnect:addByte(5)
	-- add without array len
	msgReconnect:addText(self.reconnectKey,true)
	
	self:sendMsg(msgReconnect,true)
end

function RDSClient:sessionMsg(data)
	local msgSend = RDSMessage(RDS_PROTOCOL.SESSION_MESSAGE)
	--msgSend:addByte(95):addByte(96):addByte(95):addByte(96)
	msgSend:addText(data,true)
	self:sendMsg(msgSend)
end

function RDSClient:isLoggedIn()
	return self.loggedIn
end

function RDSClient:getReconnectKey()
   return self.reconnectKey
end

function RDSClient:logout()
	local msgLogout = RDSMessage(RDS_PROTOCOL.LOGOUT_REQUEST)
	self:sendMsg(msgLogout,true)
end

function RDSClient:sendEvent(eventName,data)
	if (self.callback) then
		self.callback:onEvent(eventName,data or {},self)
	end
end

function RDSClient:receiveMessage()
--         Gk.log("Waiting for msg!")
			local dataLen = self:readUI16()
			if not dataLen then
			   return false
			end
			 
			local opCode = self:readUByte();       

--         Gk.log("RDSClient: INCOMING opcode:"..opCode.." len:"..dataLen)

			if (opCode == RDS_PROTOCOL.LOGIN_SUCCESS) then
				Gk.log("Login Success")
				self.loggedIn = true
				self.reconnectKey = self:readString(dataLen-1)
				self.currentUserName = self._loginUser
				self:sendEvent("login_success",{reconnectKey=self.reconnectKey})
            return true
			elseif (opCode == RDS_PROTOCOL.LOGIN_FAILURE) then
				local reason = self:readString()
				self.currentUserName = nil
				Gk.log("Login-Failed:"..reason)
				self.loggedIn = false
				self:sendEvent("login_failed")
            return true
			elseif (opCode == RDS_PROTOCOL.LOGIN_REDIRECT) then
				local redirectHost = self:readString()
				local redirectPort = self:readUI32()
            return true
			elseif (opCode == RDS_PROTOCOL.RECONNECT_SUCCESS) then
				self.reconnectKey = self:readString()
				Gk.log("Reconnect success!")				
            return true
			elseif (opCode == RDS_PROTOCOL.RECONNECT_FAILURE) then
				local reason = self:readString()
				Gk.log("Reconnect failed:"..reason)
            return true
			elseif (opCode == RDS_PROTOCOL.SESSION_MESSAGE) then
				local data = self:readString(dataLen-1)
				Gk.log("got session-message:("..#data..")"..data)
				if (self.callback) then
					self.callback:onMessage{data=data,client=self}
				end
            return true
			elseif (opCode == RDS_PROTOCOL.LOGOUT_SUCCESS) then
				Gk.log("logout success")
				self.loggedIn = false
				self.reconnectKey = nil
				self:sendEvent("logout_success")
            return true
			elseif (opCode == RDS_PROTOCOL.CHANNEL_JOIN) then
				local channelName = self:readString()
				local channelId = self:readString(dataLen - #channelName - 2 -1)
				Gk.log("Joined Channel:"..channelName.."("..channelId..")")
				self:sendEvent("joined_channel",{name=channelName,id=channelId})
            return true
  			elseif (opCode == RDS_PROTOCOL.CHANNEL_LEAVE) then
				local channelId = self:readString(dataLen - 1)
				self:sendEvent("left_channel",{id=channelId})
				Gk.log("Left Channel:"..channelId)
            return true
			elseif (opCode == RDS_PROTOCOL.CHANNEL_MESSAGE) then
				local data = self:readString(dataLen-1)
				Gk.log("got session-message:"..data)
				if (self.callback) then
					self.callback:onChannelMessage(data,self)
				end
            return true
			else
				Gk.log("WARNING: Unknown OPCODE:"..opCode)
			   return false
			end
			

end

function RDSClient:sendMsg(msg,skipCache)
	local raw = msg:get()
	local binData = writeUI16(#raw) .. raw
   
   local ok,err= self.tcp:send(binData)

   if not ok then 
      self:sendEvent("no_connection")
   end
   
	return ok,err
end

function writeByte(byte)
--   Gk.log("write:"..byte)
  local buff = libluabpack.bpack('C', byte)
  return buff
end

function writeI16(i16)
  local buff = libluabpack.bpack('s', i16)
  return buff
end

function writeUI16(i16)
  local buff = libluabpack.bpack('S', i16)
  return buff
end

function writeI32(i32)
  local buff = libluabpack.bpack('i', i32)
  self.trans:write(buff)
end

function writeI64(i64)
  local buff = libluabpack.bpack('l', i64)
  self.trans:write(buff)
end

function writeDouble(dub)
  local buff = libluabpack.bpack('d', dub)
  self.trans:write(buff)
end

function writeString(str)
  -- Should be utf-8
  self:writeUI16(string.len(str))
  self.trans:write(str)
end
function RDSClient:readByte()
  local buff,status = self:readBuff(1)
  local val = libluabpack.bunpack('c', buff)

  return val,status
end

function RDSClient:readUByte()
  local buff,status = self:readBuff(1)
  local val = libluabpack.bunpack('C', buff)

  return val,status
end

function RDSClient:readI16()
  local buff,status = self:readBuff(2)
  local val = libluabpack.bunpack('s', buff)


  return val,status
end

function RDSClient:readBuff(len)
--  if (not self.loggedIn) then
--     Gk.log("RDSClient:readBuff waiting for login!")
--     coroutine.yield(function() 
--                        return self.loggedIn 
--                     end)
--  end
  local buffer = nil
  local startLen = len
  self.timeWithoutBuf = 0
  local lastTimer = Gk.getSystemTime()
  
  while (len>0) do
	  local buf,status = self.tcp:receive(len)
	  
      if status == "closed" then
         if self.connected then
            self.connected = false
            self:sendEvent("no_connection")
         end
         return false
      end
	  
	  self.connected = true
	  
	  if (buf) then
	  	if (not buffer) then
	  		buffer = buf
	  	else
	  		buffer = buffer .. buf
	  	end
	  	len = len - #buf
	  	self.timeWithoutBuf = 0
	  	lastTimer = Gk.getSystemTime()
	  else
        local curTime = Gk.getSystemTime()
        local dt = curTime - lastTimer
        self.timeWithoutBuf = self.timeWithoutBuf + dt
        lastTimer = curTime
        
        if self.timeoutActive then
           dPrintf("Timeout:"..self.timeWithoutBuf.." > "..self.timeout)
        else
           dPrintf("IdleCheck:"..self.timeWithoutBuf.." > "..self.timeout)
        end
        if self.timeoutActive and self.timeWithoutBuf > self.timeout then
           Gk.log("Read-TIMEOUT:"..self.timeWithoutBuf..">"..self.timeout)
           error("read-timeout")
        elseif not self.timeoutActive and self.timeWithoutBuf > self.idleCheck then
           SEND_MSG{type="network",subtype="idle_check"}
           self.timeWithoutBuf = 0
        end
	  end
	  
	  if status == "timeout" then
	  	coroutine.yield(self)
	  end
  end
  
  local len = string.len(buffer)
  
  if not buffer or string.len(buffer) ~= startLen then
    error("rds-socket read-error")
  end
  
  return buffer
end

function RDSClient:readUI16()
  local buff,status = self:readBuff(2)
  if not buff then
   return false
  end
  
  local val = libluabpack.bunpack('S', buff)
  return val,status
end

function RDSClient:readI32()
  local buff,status = self:readBuff(4)
  local val = libluabpack.bunpack('i', buff)
  return val,status
end

function RDSClient:readI64()
  local buff,status = self:readBuff(8)
  -- not sure if this kills the benefit of long?! but makes it possible to compare i64 with other numbers
  local val = tonumber(tostring(libluabpack.bunpack('l', buff)))
  return val,status
end

function RDSClient:readDouble()
  local buff,status = self:readBuff(8)
  local val = libluabpack.bunpack('d', buff)
  return val,status
end

function RDSClient:readString(byteLen)
  local len = 0
  if (byteLen) then
  	if (type(byteLen)=="number") then
	  	len = byteLen
  	else
  		len = self:readUByte() - 1
  	end
  else
  	len = self:readUI16()
  end
  local buff,status = self:readBuff(len)
  return buff,status
end

count = 0



