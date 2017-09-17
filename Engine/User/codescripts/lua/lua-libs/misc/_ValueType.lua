ValueType = Class()

function ValueType:constructor(input)
	self.type=ValueType
	input = input or {}
	self.value = input.value
	self.callbacks = {}
	if (input.callback) then
		self:addListener(input.callback)
	end
end

function ValueType:set(value,changeInfo)
	local old = self.value
	self.value = value

	-- only call the callbacks if the new value differs from the old one
	-- overide the equals-method for all subtypes otherwise it will be fired on
	-- every set()-call
	if (not self:equals(old,value)) then
	  self:_fire(old,value,changeInfo)
	end
	return value
end

function ValueType:_fire(old,newValue,changeInfo)
   for _,callback in ipairs(self.callbacks) do
      if (callback.obj==nil) then
         callback.func(old,newValue,changeInfo)
      else
         callback.func(callback.obj,old,newValue,changeInfo)
      end
   end
end

-- check if two value-types are the same
-- this is used to know if we fire the callback or not
function ValueType:equals(x,y)
	-- always fire
	return false
end

function ValueType:get()
	return self.value
end

function ValueType:addCallback(func,obj)
	table.insert(self.callbacks,{func=func,obj=obj})
end

function ValueType:removeCallback(callback)
	table.removeValue(callback);
end

function ValueType:clear()
	self:set(nil)
end
