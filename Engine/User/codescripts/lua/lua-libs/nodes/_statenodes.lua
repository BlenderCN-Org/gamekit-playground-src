StateMachine = BaseClass(NodeLogic)
StateMachine.machines = {}
StateMachine.find = function(stmName)
    return StateMachine.machines[stmName]
end
function StateMachine:constructor(data)
    if not self:checkNodeType("StateMachine") then
        error("Tried to create Statemachine with wrong type:"..self.node:getType())
    end
    self.GLOBAL = self.node:getProperty("GLOBAL"):getBool()
end

function StateMachine:init()
   -- dPrintf("INIT:%s",self.node:getName())
    local nodeName = self.node:getName()
    if (not self.GLOBAL) then
      nodeName = self.node:getGameObject():getName().."_"..nodeName
    end
    
    StateMachine.machines[nodeName]=self
    self.states = {}
    self.fsm = Gk.FSM()
    
    -- load states:
    local statesSockets = self.node:getOutSocket("STATES")
    for i=0,statesSockets:getOutSocketAmount()-1 do
        local socket = statesSockets:getOutSocketAt(i)
        local stateNode = NodeTreeLogic.getLogic(socket:getNode())
        if (stateNode) then
            stateNode.fsm = self
            self.states[stateNode.name]=stateNode.id
            self.states[stateNode.id]=stateNode.name
           -- dPrintf("StateNode:%s - %i",stateNode.name,stateNode.id)
        end
    end
    
    
    self.UPDATESOCKET = self.node:getInSocket("UPDATE"):getVar()
    
   -- dPrintf("3") 
end

function StateMachine:setInitalState()
    local initalStateName = self.node:getProperty("INITIAL_STATE"):getString()
    self.currentState = self.states[initalStateName]
    
    if self.currentState then
        self.fsm:setState(self.currentState)
    else
        error("not valid initialstate:"..initalStateName)
    end
end

function StateMachine:initialize()
  --  dPrintf("LIFECYCLE:init %s",self.node:getName())
    self:setInitalState();
end

function StateMachine:isActive()
  return self.UPDATESOCKET:getBool()
end

function StateMachine:evaluate(tick)
    local result = self:isActive()
    return result
end

function StateMachine:update(tick)
    self.fsm:update()
end

StateMachineRef = BaseClass(NodeLogic)

function StateMachineRef:constructor()
  self.STM = self.node:getProperty("STM"):getString()
  self.fsm = StateMachine.find(self.STM)
end



State = BaseClass(NodeLogic)

function State:constructor()
    self.name = self.node:getProperty("NAME"):getString()
    self.id = self.node:getProperty("ID"):getInt()
    self.fsm = nil -- is set by the statemachine itself
    self.active = false
    self.START_TRIGGER_SOCKET = self.node:getOutSocket("START_TRIGGER"):getVar()
    self.END_TRIGGER_SOCKET = self.node:getOutSocket("END_TRIGGER"):getVar()  
      --dPrintf("Got state:%s (%i)",self.name,self.id)
end



function State:update(tick)
    local result = self.fsm.fsm:getState() == self.id
    if (result==false and self.active==true) then
        self.END_TRIGGER_SOCKET:setBool(true)
        self.START_TRIGGER_SOCKET:setBool(false)
    elseif (result==true and self.active==false) then
        self.START_TRIGGER_SOCKET:setBool(true)
        self.END_TRIGGER_SOCKET:setBool(false)
    else
        self.START_TRIGGER_SOCKET:setBool(false)
        self.END_TRIGGER_SOCKET:setBool(false)
    end
    self.active = result
end

StateRef = BaseClass(NodeLogic)

function StateRef:constructor()
    self.type="stateref"
    
    self.stmName = self.node:getProperty("STM"):getString()
    self.stateName = self.node:getProperty("STATE"):getString()
    self.ACTIVE_SOCKET = self.node:getOutSocket("ACTIVE"):getVar()
    self.fsm = nil -- is set by the statemachine itself
    self.START_TRIGGER_SOCKET = self.node:getOutSocket("START_TRIGGER"):getVar()
    self.END_TRIGGER_SOCKET = self.node:getOutSocket("END_TRIGGER"):getVar()      

end

function StateRef:afterInit()
   -- dPrintf("STATEREF:%s",self.node:getName())
    self.fsmLogic = StateMachine.find(self.stmName)
    if not self.fsmLogic then
        self.stmName = self.node:getGameObject():getName().."_"..self.stmName
        self.fsmLogic = StateMachine.find(self.stmName)
        if not self.fsmLogic then
          error("Unknown statemachine:"..self.stmName.." in StateRef:"..self.node:getName())
        end
    end
    self.fsm = self.fsmLogic.fsm
    Gk.log("StateRef:fsm:"..tostring(self.fsm))
    self.stateId = self.fsmLogic.states[self.stateName]
    if not self.stateId then
        error("Unknown state:"..self.stateName.." in StateRef:"..self.node:getName())
    end

end

function StateRef:update(tick)
    local result = self.fsm:getState() == self.stateId 

    if (result==false and self.ACTIVE_SOCKET:getBool()==true) then
        self.END_TRIGGER_SOCKET:setBool(true)
        self.START_TRIGGER_SOCKET:setBool(false)
    elseif (result==true and self.ACTIVE_SOCKET:getBool()==false) then
        self.START_TRIGGER_SOCKET:setBool(true)
        self.END_TRIGGER_SOCKET:setBool(false)
    else
        self.START_TRIGGER_SOCKET:setBool(false)
        self.END_TRIGGER_SOCKET:setBool(false)
    end
    self.active = result

    self.ACTIVE_SOCKET:setBool(result)
   -- dPrintf("(%s): %s %i==%i",self.node:getName(),tostring(result),self.fsm:getState(),self.stateId)
end


StateManipulator = BaseClass(NodeLogic)

function StateManipulator:constructor()
    self.type = self.node:getProperty("TYPE"):getInt()
    self.stmName = self.node:getInSocket("STM"):getVar():getString()
    self.stateName = self.node:getInSocket("STATE"):getVar():getString()
    self.UPDATE_SOCKET = self.node:getInSocket("UPDATE"):getVar();
end

function StateManipulator:afterInit()
    self.fsmLogic = StateMachine.find(self.stmName)
    if not self.fsmLogic then
        error("Unknown statemachine:"..self.stmName.." in StateRef:"..self.node:getName())
    end
    self.fsm = self.fsmLogic.fsm
    self.stateId = self.fsmLogic.states[self.stateName]
    if not self.stateId then
        error("Unknown state:"..self.stateName.." in StateRef:"..self.node:getName())
    end   
end

StateTransition = BaseClass(NodeLogic)

function StateTransition:constructor()
end

function StateTransition:init()
    Gk.log("TransitionNode init:"..self.node:getName())
    self.from = self:getInSocketLogic("FROM")
    self.to = self:getFirstOutSocketLogic("TO")
    self.cond = self:getInSocket("CONDITION"):getVar()
end

function StateTransition:initialize()
    Gk.log("TransitionNode initalize:"..self.node:getName())
    if (self.from.fsm and self.from.fsm ~= self.to.fsm) then
        error("ERROR/TransitioNode: you cannot do a transition from states of different statemachines! "..self.node:getName())
    end
    self.fsm = self.from.fsm
    Gk.log("self.from :"..tostring(self.from.node:getName()).." "..self.from.type.." "..tostring(self.from.fsmLogic))
    Gk.log("self.to:"..tostring(self.to.node:getName()).." "..self.to.type)
    local me = self
    self.fsm:addTransition(self.from.stateId,self.to.stateId,0):when(function()
        return self.cond:getBool()
    end)
    Gk.log("Created Transition:"..self.from.stateId.."=>"..self.to.stateId);
end 

function StateTransition:evaluate()
    return false
end

function StateManipulator:evaluate(tick)
    --dPrintf("%s %i %i",tostring(self.UPDATE_SOCKET:getBool()),self.fsm:getState(),self.stateId)
    return self.UPDATE_SOCKET:getBool() and self.fsm:getState()~=self.stateId
end

function StateManipulator:update(tick)
    if (self.node:getProperty("TYPE"):getInt()==0) then
        self.fsm:setState(self.stateId)
    end
end
--NodeTreeLogic.addLogic("Gamekit","StateMachine",StateMachine)
--NodeTreeLogic.addLogic("Gamekit","State",State)
--NodeTreeLogic.addLogic("Gamekit","StateRef",StateRef)
--NodeTreeLogic.addLogic("Gamekit","StateManipulator",StateManipulator)
--NodeTreeLogic.addLogic("Gamekit","StateTransition",StateTransition)