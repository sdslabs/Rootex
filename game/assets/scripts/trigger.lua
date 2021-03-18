Trigger = class("Trigger")

function Trigger:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
end

function Trigger:enter(trigger)
    print("Enter was called: " .. tostring(trigger))
end

function Trigger:exit(trigger)
    print("Exit was called: " .. tostring(trigger))
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
