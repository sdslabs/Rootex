player = nil
transform = nil

function onBegin(entity)
    player = RTX.EntityFactory.Find(6)
    transform = entity:getTransform()
end

function onUpdate(entity, delta)
    transform:setPosition(player:getTransform():getPosition() + RTX.Vector3.new(0, 0, 10))
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
