GroupPool = Class()

function GroupPool:constructor(scene)
	self.scene = scene or Gk.getActiveScene()
	pool = {}
end

function GroupPool:get(groupName,pos,rot,scl)
	if (pool[groupName]==nil) then
		pool[groupName]={}
	end
	
	local groupInst = nil
	if (#pool[groupName]>0) then
		groupInst = table.remove(pool[groupName])
		groupInst:createInstance()
		--info("recycled %s",tostring(groupName))
	else
		--info("new %s",tostring(groupName))
		groupInst = Gk.createGroupInstance(self.scene,groupName)
	end

	if (pos) then
		groupInst:getRoot():setPosition(pos)
	end
	if (rot) then
		groupInst:getRoot():setRotation(rot)
	end
	if (scl) then
		groupInst:getRoot():setScale(scl)
	end

	return groupInst
end

function GroupPool:release(obj,mapTo)
	local groupName = obj:getGroupName()
	if (pool[groupName]==nil) then
		pool[groupName]={}
	end
	
	obj:destroyInstance()
	table.insert(pool[groupName],obj)
	--dPrintf("released...")
end

