EmptyScript = class("EmptyScript")

function EmptyScript:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
end

function EmptyScript:begin(entity)
    print("Nothing is true")
end

function EmptyScript:update(entity, delta)
end

function EmptyScript:destroy(entity)
    print("Everything is permitted")
end

return EmptyScript
