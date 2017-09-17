LanguageManager = Class()

function LanguageManager:constructor(input)
   input = input or {}
   self.currentLang = nil
   self.langCache = {}
   -- path with ending /
   self.langPath = input.path or "media/lang/"
   
   LanguageManager.instance = self
end

function LanguageManager:loadLang(name)
   local newLang = self.langCache[name]
   
   if (newLang) then
      self.currentLang = newLang
      Gk.log("Ok. Set Lang to "..name)         
   end
   
   -- TODO: only let valid files be loaded
   Gk.import(self.langPath..name..".lua")

   newLang = self.langCache[name]
   if (not newLang) then
      error("Want to load unknown lang:"..name)
   end
   
   self.currentLang = newLang
   Gk.log("Ok. Set Lang to "..name.." elems:"..table.length(self.currentLang))         
end

function LanguageManager:get(path,strict)
   local splits = path:split(".")

   local temp = self.currentLang 
   
   for i=1,#splits do
      local elemName = splits[i]
      temp = temp[elemName]
      if (not temp) then
         if (strict) then
            error("ERROR: langmanager couldn't find "..path.."! stopped at "..elemName)
         else
            Gk.log("Unknown language-element: "..path.."! (path stopped at:"..elemName)
            return path
         end
      end  
   end
   
   return temp
end

