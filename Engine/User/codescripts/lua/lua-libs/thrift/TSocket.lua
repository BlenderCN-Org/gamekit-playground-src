---- Licensed to the Apache Software Foundation (ASF) under one
-- or more contributor license agreements. See the NOTICE file
-- distributed with this work for additional information
-- regarding copyright ownership. The ASF licenses this file
-- to you under the Apache License, Version 2.0 (the
-- "License"); you may not use this file except in compliance
-- with the License. You may obtain a copy of the License at
--
--   http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing,
-- software distributed under the License is distributed on an
-- "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
-- KIND, either express or implied. See the License for the
-- specific language governing permissions and limitations
-- under the License.
--

Gk.import(LUA_LIB_DIR.."/thrift/TTransport.lua") 
Gk.import(LUA_LIB_DIR.."/misc/_AsyncDispatcher.lua")
--Gk.import(LUA_LIB_DIR.."/thrift/libluasocket.lua") 

-- TSocketBase
TSocketBase = TTransportBase:new{
  __type = 'TSocketBase',
  timeout = 1000,
  host = 'localhost',
  port = 9090,
  handle
}

function TSocketBase:close()
  if self.handle then
    self.handle:close()
    self.handle = nil
  end
end

-- Returns a table with the fields host and port
function TSocketBase:getSocketInfo()
  if self.handle then
    return self.handle:getsockinfo()
  end
  terror(TTransportException:new{errorCode = TTransportException.NOT_OPEN})
end

function TSocketBase:setTimeout(timeout)
  if timeout and ttype(timeout) == 'number' then
    if self.handle then
      self.handle:settimeout(timeout)
    end
    self.timeout = timeout
  end
end



-- TSocket
TSocket = TSocketBase:new{
  __type = 'TSocket',
  host = 'localhost',
  port = 9090
}

function TSocket:isOpen()
  if self.handle then
    return true
  end
  return false
end

function TSocket:open()
  if self.handle then
    self:close()
  end

  local socket = require("socket")
  local tcp = assert(socket.tcp())
  local res,err = tcp:connect(self.host,self.port)
  
  if (res == nil) then
  	error({error="no_connection",host=self.host,port=self.port,msg=err})
  end
  
  tcp:settimeout(self.timeout)
  
  self.handle = tcp;
  
--  -- Create local handle
--  local sock, err = luasocket.create_and_connect(
--    self.host, self.port, self.timeout)
--  if err == nil then
--    self.handle = sock
--  end
--
--  if err then
--    terror(TTransportException:new{
--      message = 'Could not connect to ' .. self.host .. ':' .. self.port
--        .. ' (' .. err .. ')'
--    })
--  end
end

function TSocket:read(len)
--  local buf = self.handle:receive(self.handle, len)
  local buf = self.handle:receive(len)
  if not buf or string.len(buf) ~= len then
    terror(TTransportException:new{errorCode = TTransportException.UNKNOWN})
  end
  return buf
end

function TSocket:write(buf)
--  self.handle:send(self.handle, buf)
  self.handle:send(buf)
end

function TSocket:flush()
end

TAsyncSocket = TSocket:new{
	__type = "TAsyncSocket",
	host = 'localhost',
	port = 9090,
	timeout = 0,
}


function TAsyncSocket:read(len)

  local buffer = nil
  local startLen = len

  while (len>0) do
	  local buf,status = self.handle:receive(len)
	  
	  if status == "closed" then
	  	return
	  end
	  
	  if (buf) then
	  	if (not buffer) then
	  		buffer = buf
	  	else
	  		buffer = buffer .. buf
	  	end
	  	len = len - #buf
	  end
	  
	  if status == "timeout" then
	  	Gk.log("timeout left"..len)
	  	coroutine.yield(self)
	  end
  end
  
  local len = string.len(buffer)
  
  if not buffer or string.len(buffer) ~= startLen then
    terror(TTransportException:new{errorCode = TTransportException.UNKNOWN})
  end
  return buffer
end

-- TServerSocket
TServerSocket = TSocketBase:new{
  __type = 'TServerSocket',
  host = 'localhost',
  port = 9090
}

function TServerSocket:listen()
  if self.handle then
    self:close()
  end

  local sock, err = luasocket.create(self.host, self.port)
  if not err then
    self.handle = sock
  else
    terror(err)
  end
  self.handle:settimeout(self.timeout)
  self.handle:listen()
end

function TServerSocket:accept()
  local client, err = self.handle:accept()
  if err then
    terror(err)
  end
  return TSocket:new({handle = client})
end
