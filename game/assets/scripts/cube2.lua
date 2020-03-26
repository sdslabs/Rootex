function onBegin(entity)
    transform = entity:getTransform()
    number = 0
end

function onUpdate(delta, entity)
    local pos = transform:getPosition()
    number = number + 0.1
    pos.x = math.sin(number)
    transform:setPosition(pos)
end

function onEnd(entity)
end
