Trigger = class("Trigger")

function Trigger:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
    self.enterCount = 0
end

function Trigger:enterTrigger(entity, trigger)
    if self.enterCount == 0 then
        entity.shortMusic:play()
    end
    self.enterCount = self.enterCount + 1
    entity.textUI:setText(trigger:getScene():getFullName() .. ": " .. entity:getScene():getFullName() .. " entered (" .. tostring(self.enterCount) .. ")")
end

function Trigger:exitTrigger(entity, trigger)
    entity.shortMusic:play()
    entity.textUI:setText(trigger:getScene():getFullName() .. ": " .. entity:getScene():getFullName() .. " exited")
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
