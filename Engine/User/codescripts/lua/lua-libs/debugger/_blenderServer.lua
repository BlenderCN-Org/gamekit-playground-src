Gk.import(LUA_LIB_DIR.."/misc/_AsyncDispatcher.lua")

dispatch = AsyncDispatcher.get("server")

Gk.enableLogicTreeDebug(true)

serverGame = serverGame or DefaultGame()
serverGame:addAlwaysQueue(function() dispatch:asyncDispatch() return true end)
serverGame:setSystemMode(true)
--dispatch:call{
--  call=function()
--    while true do
--      coroutine.yield(function() return Gk.isLogicTreeDebugging() end)
--      Gk.log("CurrentDebug:"..Gk.getLogicTreeDebugOutput())
--    end
--  end
--}

function stopDebugServer()
  DEBUGSERVER_RUNNING=false
  Gk.enableLogicTreeDebug(false)
end

function readMessage(c)
    local length, e = c:receive(4)
    if e then 
      return nil,e
    end
    local len = libluabpack.bunpack('i', length)
    print ("gk:Length:"..len)
    local code, e = c:receive(1)
    if e then 
      return nil,e
    end
    print ("gk:CODE:"..tostring(tonumber(code)))
    local l,e=c:receive(len)
    return l,e,code
end

function startDebugServer()
  Gk.log("1")
  if DEBUGSERVER_RUNNING then
    Gk.log("SERVER ALREADY RUNNING!")
    return
  end
  Gk.enableLogicTreeDebug(true)
  Gk.log("2")
  DEBUGSERVER_RUNNING=true
  dispatch:call{
  call=function()
    print ("IN DISPATCHE")
    Gk.log("22")
    local socket = require 'socket'
    yWaitMs(1000)
  
    local server = socket.tcp()
    while true do
      yWaitMs(1000)
      local a = server:bind('*',9090)
      Gk.log("SUCCESS STARTING SERVER? " ..tostring(a==1))
      if (a==1) then
        break
      end
      server:close()
      server = socket.tcp()
    end
    
    Gk.log("24")
    
    yWaitMs(1000)
    server:listen(5)
    yWaitMs(1000)
    server:settimeout(0.05)
    Gk.log("26")
    
    while (DEBUGSERVER_RUNNING) do
      Gk.log("Waiting for client....")
      local c,e = server:accept()  
      if c then
        c:settimeout(0.05)
        print("gk:Connected. Here is the stuff:")
        local input,e,code = readMessage(c)
        Gk.log("from server:"..tostring(input).." e:"..tostring(e).." code:"..tostring(code))
        while DEBUGSERVER_RUNNING do

          if input then
            Gk.log("INPUT:"..input)
            local splits = input:split("|")
            Gk.log("Splits:"..#splits)
            local l = splits[1]
            
            local command = _G[l]
            
            if command and type(command)=="function" then
              command(l,c,splits)
            end
            
    --        c:send("TOM:"..l)
            
            if input then
              Gk.log("gk:INCOMING:"..tostring(input))
            end
          end
          input, e = readMessage(c)
          
          if e and e~="timeout" then
            c:close()
            break;
          end
          yWaitMs(50)
          --coroutine.yield()
  
        end
      else
        --print ("TIMEOUT")
        yWaitMs(1000)
      end
      coroutine.yield()
    end
  
  
  end
  }
end

GetDebug=function(l,c)
  if (not Gk.isLogicTreeDebugging()) then
    Gk.enableLogicTreeDebug(true)
    coroutine.yield()
  end
  local output = Gk.getLogicTreeDebugOutput()
  Gk.log("gk:SEND "..(#output).." bytes")
  --c:send(writeI32(#output)..output)
  Gk.log(output)
  
  sendToBlender(c,95,output)
end

GetDebugCompressed=function(l,c)
  if (not Gk.isLogicTreeDebugging()) then
    Gk.enableLogicTreeDebug(true)
    coroutine.yield()
  end
  local output = Gk.getLogicTreeDebugOutputCompressed()
  Gk.log("gk:SEND "..(#output).." bytes")
  --c:send(writeI32(#output)..output)
  --Gk.log(output)
  sendToBlender(c,96,output)
end

GetScreenshot=function(l,c,splits)
  Gk.setScreenSize(tonumber(splits[2]),tonumber(splits[3]))
  coroutine.yield()
  local screenShot = Gk.getScreenshotAsString()
  Gk.log("gk:SEND "..(#screenShot).." bytes")
  sendToBlender(c,97,screenShot)
end

LuaCall=function(l,c,splits)
  Gk.log("Execute script:" .. splits[2])
  Gk.executeScript(splits[2])  
  sendToBlender(c,98,"ok")
end

scene = Gk.getActiveScene()
--cam = scene:createCamera("BlenderCam")
cam = scene:getMainCamera()

-- helper function to set the viewport
function setData(vmat,proj,near,far,isortho)
  Gk.log("SET DATA:")
  cam:makeCurrent()
  
  if (vmat) then
    cam:setTransform(vmat)
  end
  
  
  print("pos:" .. tostring(pos))
  if (pos) then

    
--    pos.x = -pos.x
 --   pos.x = a
 --   pos.y = -pos.y
    pos.z=-pos.z
    cam:setPosition(pos)
  end
  
  if (rot) then
    cam:setOrientation(rot)
    
  end
  
  if (proj) then
    cam:setProjection(proj)
  end
end
--function sendScreenshot(client,data)
--  local screensize = Gk.getScreenSize()
--  local buf = "" .. libluabpack.bpack('i', #data)
--  buf = buf .. libluabpack.bpack('C', 97)
----  buf = buf .. libluabpack.bpack('i', screensize.x)  
----  buf = buf .. libluabpack.bpack('i', screensize.y)  
-- 
--  buf = buf .. data
--  
--  
--  local start = 0
--  local size = #buf   
--  
--  Gk.log("TRY TO SEND:"..size.." bytes");
--  while true do
--    local idx = client:send(buf,start,size)
--    Gk.log("IDX:"..tostring(idx))
--    if not idx or idx==1 or idx==size then
--      break
--    end
--    start=idx
--  end
--  io.flush()
--  Gk.log("gk:FLUSH")
--end

function sendToBlender(client,kind,output,additionalHeader)
  local buf = "" .. libluabpack.bpack('i', #output)
  buf = buf .. libluabpack.bpack('C', kind)
  
  -- inject additionalheader data
  if  additionalHeader then
    buf = buf .. additionalHeader()
  end
  
  buf = buf .. output
  
  Gk.log("Want to send:"..#buf.." bytes")   
  local sentBytes = client:send(buf)
  Gk.log("I DID SENT "..tostring(sentBytes))
  
  io.flush()
  Gk.log("gk:FLUSH")
end

function writeUI16(i16)
  local buff = libluabpack.bpack('S', i16)
  return buff
end

function writeI32(i32)
  local buff = libluabpack.bpack('i', i32)
  return buff
end

--local socket = require("socket")
--host = host or "127.0.0.1"
--port = port or 9090
--if arg then
--    host = arg[1] or host
--    port = arg[2] or port
--end
--print("Binding to host '" ..host.. "' and port " ..port.. "...")
--udp = assert(socket.tcp())
--assert(udp:setsockname(host, port))
--assert(udp:settimeout(5))
--ip, port = udp:getsockname()
--assert(ip, port)
--
--c = assert(udp:accept())
--print("Connected. Here is the stuff:")
--l, e = c:receive()
--while not e do
--  print(l)
--  l, e = c:receive()
--end
--print(e)
--
