function onBegin(entity)
    transform = entity:getTransform()
    number = 0
end

function onUpdate(delta, entity)
    number = number + 0.01
    transform:setRotation(number, 0, 0)
end

function onEnd(entity)
end
