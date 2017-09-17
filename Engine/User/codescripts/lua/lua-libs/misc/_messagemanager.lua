--[[
create the MessageManager in the OnInit-call:

mmgr = MessageManager()

send message via:
SEND_MSG({type="type",...})
--]]
MessageManager = Class()

function MessageManager:constructor(input)
   if __mmgr then
      error("ERROR: Multiple MessageManagers")
   end
   
   __mmgr = self
   
   input = input or {}
   self.listenTo = input.listenTo or ""
   self.DEBUG = input.debug or false
   -- map that stores a list of subscribers per subject
   self.MSG_MAP = {}
   
   self.mappings = 0
   self.type = input.type or "LuaBased" 
    --messageManager = self
    if self.type == "MessageBased" then
       local mmgr = Gk.MessageManager()
       self.callback = mmgr:addListener("__mm",self.listenTo,"MM",self,MessageManager.onMessage)
    elseif self.type == "LuaBased" then
       
    end
end

-- add a listener to a specific subject
function MessageManager:addMapping(_subject,_obj,_func)
   local map = self.MSG_MAP[_subject]
   -- first time this subject is subscribed?
   if (map == nil) then
      map = {}
      self.MSG_MAP[_subject]=map
   end
   
   map[_obj]={
      subject=_subject,
      obj=_obj,
      func=_func
   }
   self.mappings = self.mappings + 1      
end

function MessageManager:removeMapping(_subject,_obj)
   local map = self.MSG_MAP[_subject]
   if (map) then
      dPrintf("Before:%i",table.length(map))
      table.removeKey(map,_obj)
      dPrintf("After:%i",table.length(map))
      
      self.mappings = self.mappings - 1      
   else
      warning("Tried to unmap from subject %s! But this subject doesn't exist!",_subject)
   end
end

--[[
TODO:
   removeMapping(_subject,_obj)
   removeMappingAll(_obj)
   
--]]

-- this function get called in the logic-empty as soon as the message-listener-brick gets a signal
function MessageManager:onMessage(from,to,subject,body)
        if (self.DEBUG) then
         dPrintf("onMessage: %s - %s - %s - %s",from,to,subject,body)
        end
      -- body-driven. It is assumed that there is a pickle-table in the body of the message
      -- that have a field .type
      local msg = nil
      if self.type == "MessageBased" then
         msg = unpickle(body)
      else
         msg = body
      end
      --dPrintf("Got MSG:%s",currentBody)
      if (not msg) then
         dPrintf("No Message in body! Sub:%s Body:%s",subject,body)
        else
            if (not msg.type) then
            dPrintf("There is a message! But not .type in the table Sub:%s Body:%s",subject,body)
            else
                self:processMsg(msg)
            end
        end
end

function MessageManager:processMsg(msg)
         -- get map of all handlers that listen to this subject
         local map = self.MSG_MAP[msg.type]
         if (map==nil) then
            dPrintf("Got msg:%s but no receiver!",msg.type)
         else
            for i,handler in pairs(map) do
            -- call the handler for that msg-typ
               if handler==nil then
                  dPrintf("Unknown Message-Type:%s in Sub:%s Body:%s",msg.type,self.messageSensor:getMessageSubject(i),currentBody)
               else
                  if (_config.debug.showOutgoingMesssage) then
                     dPrintf("INCOMING MSG: %s",msg)
                  end

                  if (handler.func~=nil) then
                     -- call a class-function
                     --dPrintf("FUNC:%s",tostring(handler.func))                    
                     handler.func(handler.obj,msg)
                  else
                     -- call a global-function
                     handler.obj(msg)
                  end
               end
            end
         end
end

function MessageManager:luaSendMsg(msg,to)
  -- local pmsg = pickle(msg)
   self:onMessage("",to or "","MM",msg)
end


-- send a message to with subject MM
function SEND_MSG(msg,to)
   if __mmgr and __mmgr.type=="LuaBased" then
      __mmgr:luaSendMsg(msg,to)
      return
   end
   --dPrintf("IN MSG")
   local pmsg = pickle(msg)
   -- send message to MM-Subject
   Gk.sendMessage("__mm",to or "","MM",pmsg)
   --dPrintf("SEND MSG! %s",pmsg)
end