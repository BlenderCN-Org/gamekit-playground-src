SoundManager = Class()

function SoundManager.get()
   if not __soundmanager then
      __soundmanager = SoundManager()
   end
   return __soundmanager 
end

function SoundManager:constructor()
   if SoundManager.instance then
      error("ONLY ONE SOUNDMANAGER-Allowed! Do not use the constructor instead get it via SoundManager.get()")
   end
   
   self.sm = Gk.SoundManager()
   self.sounds = {}
   self.playing = {}
   self.isPlaying = false
end

function SoundManager:changeGlobalVolume(inc)
   local current = Gk.getGlobalVolume()
   current = current + inc
   if (current > 1) then
      current = 1
   elseif (current < 0) then
      current = 0
   end
   Gk.setGlobalVolume(current)
   Gk.log("New Global Vol:"..current)
   return current
end

function SoundManager:addSound(soundFile,soundName,options)
   soundName = soundName or soundFile
   options = options or {}
   if (not self.sounds[soundName]) then
      -- load sound in system
      if not self.sm:loadSound(soundFile,soundName) then
         error("Couldn't find soundFile:"..soundFile)
      end
      
      self.sounds[soundName] = {
         soundName=soundName,soundFile=soundFile,available={}, options=options
      }
   end
end

function SoundManager:playSound(soundName)
   local sound = self.sounds[soundName]
   
   if not sound then
      error("Did try to play unknown sound:"..soundName)
   end
   
   local source = table.remove(sound.available)
   if (not source) then
      source = self.sm:createSource(sound.soundName);
      
      if sound.options.pitch then
         source:setPitch(sound.options.pitch)
      end
      if sound.options.volume then
         source:setVolume(sound.options.volume)
      end
   end
   
   source:play();
   local playContext = {sound=sound,src=source}
   table.insert(__soundmanager.playing,playContext)   
   if not self.isPlaying then
      self.isPlaying = true
      AsyncDispatcher.getLast():call({
         call = SoundManager.asyncplay
      })
   end
   return playContext
end

function SoundManager:stop(playContext)
   playContext.src:stop()
   playContext.__forcedStop = true
end

function SoundManager:stopAll()
   for _,playCtx in ipairs(self.playing) do
      self:stop(playCtx)
   end
end

function SoundManager.asyncplay()
   local playing = __soundmanager.playing 
   while #playing > 0 do
      for i=#playing,1,-1 do
         local s = playing[i]
         if s.src:isPlaying() == false then
            if (s.sound.options.loop and not s.__forcedStop) then
               yield();
               s.src:play();
               Gk.log("Restart:"..s.sound.soundName)
            else
               -- remove it from the current playing list
               table.remove(playing,i);
               -- add it to the sound as available source
               table.insert(s.sound.available, s.src);
               Gk.log("Finished sound "..tostring(s.sound.soundName))
            end
         
         end      
      end
      -- TODO: Maybe it is sufficient to wait for some time
      yield();
   end
   __soundmanager.isPlaying = false
end