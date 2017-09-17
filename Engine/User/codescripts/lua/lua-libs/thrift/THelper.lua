ThriftHelper = {}

function ThriftHelper.obj2string(obj)
   local tmem = TMemoryBuffer:new{}
   tmem:resetBuffer()
   local protocol = TBinaryProtocol:new{trans=tmem}
   
   obj:write(protocol)
   
   local result = tmem:getBuffer()
   return result
end

function ThriftHelper.storeToFile(filename,outobject,compress)
   
   local result = ThriftHelper.obj2string(outobject)
   
   local file = io.open(filename,"w")
--   if compress then
--      local before = result:len()
--      result = Gk.compressString(result,result:len())
--      local re = Gk.uncompressString(result,result:len())
--      Gk.log("Compression: "..before.." --> "..result:len())
--   end
   file:write(result)
   file:close()
end

function ThriftHelper.string2obj(string,inObject)
   local tmem = TMemoryBuffer:new{}
   tmem:resetBuffer(string)
   local protocol = TBinaryProtocol:new{trans=tmem}
   
   inObject:read(protocol)
   
   local result = tmem:getBuffer()

   return inObject
end

function ThriftHelper.readFromFile(filename,inObject,compressed)
   local file = io.open(filename,"r")  
   local data = file:read("*all")
   
--   if compressed then
--      data = Gk.uncompressString(data,data:len())
--   end
   
   file:close()
   
   
   return ThriftHelper.string2obj(data,inObject)
end