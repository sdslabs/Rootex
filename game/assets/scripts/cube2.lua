function onBegin(entity)
    transform = entity:getTransform()
    number = 0
end

function onUpdate(entity, delta)
    local pos = transform:getPosition()
    number = number + 0.01 * delta
    pos.y = 5 * math.sin(number)
    transform:setPosition(pos)
end

function onEnd(entity)
end
