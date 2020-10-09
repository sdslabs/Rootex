number = 0.1
local transform

function onBegin(entity)
    print("Nothing is true")
    transform = entity:getTransform()
end

function onUpdate(entity, delta)
    transform:setPosition(RTX.Vector3.new(number, 0, -5))
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
