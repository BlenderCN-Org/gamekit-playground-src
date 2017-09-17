

-- Game is a subclass of Engine
DefaultGame = BaseClass(OgreKit.Engine)

function DefaultGame:constructor()
	-- will be executed forever
	self._alwaysExecution = {}
	-- will be executed once
	self._onceExecution = {}
	
	-- connect call-back that is called every tick
    self:connect(OgreKit.EVT_TICK,self,DefaultGame.update)
    self.gameTime = 0

end

function DefaultGame:addAlwaysQueue(_func,_obj)
	table.insert(self._alwaysExecution, {func=_func,obj=_obj})
end

function DefaultGame:addOnceQueue(_func,_obj)
	table.insert(self._onceExecution, {func=_func,obj=_obj})
end

-- the function that got connected
function DefaultGame:update(dt)
	self.gameTime = self.gameTime + dt
	__currentDt = dt
	-- execute once-queue	
	for i,v in ipairs(self._onceExecution) do
		if (v.obj~=nil) then
			v.func(v.obj,dt)
		else
			v.func(dt)
		end
	end
	self._onceExecution = {} -- delete queue, since it is the ONCE-queue
	
	-- execute always-queue
	local newQueue = {}
	
	for i,v in ipairs(self._alwaysExecution) do
		if (v.obj~=nil) then
			if (v.func(v.obj,dt)) then
				table.insert(newQueue,v)
			end
		else
			if (v.func(dt)) then
				table.insert(newQueue,v)
			end
		end
	end
	self._alwaysExecution = newQueue
end

