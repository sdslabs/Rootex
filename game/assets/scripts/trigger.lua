Trigger = class("Trigger")

function Trigger:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
    self.text = entity:getTextUI()
    self.sound = entity:getShortMusic()
    self.enterCount = 0
end

function Trigger:enter(entity, trigger)
    if self.enterCount == 0 then 
        self.sound:play() 
    end
    self.enterCount = self.enterCount + 1
    self.text:setText(trigger:getScene():getFullName() .. ": " .. entity:getScene():getFullName() .. " entered (" .. tostring(self.enterCount) .. ")")
end

function Trigger:exit(entity, trigger)
    self.sound:play()
    self.text:setText(trigger:getScene():getFullName() .. ": " .. entity:getScene():getFullName() .. " exited")
    self.enterCount = 0
end

function Trigger:begin(entity)
    print("Nothing is true")
end

function Trigger:update(entity, delta)
end

function Trigger:destroy(entity)
    print("Everything is permitted")
end

return Trigger
