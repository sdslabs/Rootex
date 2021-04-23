EmptyScript = class("EmptyScript")

function EmptyScript:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
end

function EmptyScript:enterScene(entity)
    print("Nothing is true")
end

function EmptyScript:begin(entity)
end

function EmptyScript:update(entity, delta)
end

function EmptyScript:destroy(entity)
    print("Everything is permitted")
end

function EmptyScript:enterTrigger(entity, trigger)
end

function EmptyScript:exitTrigger(entity, trigger)
end

return EmptyScript
