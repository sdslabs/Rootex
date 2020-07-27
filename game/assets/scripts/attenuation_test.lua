function onBegin(entity)
    transform = entity:getTransform()
    number = 0
end

function onUpdate(delta, entity)
    number = number + 0.01
    if number < 4.71 then
        transform:setPosition(Vector3.new(6 * math.sin(number), 0, -10))
    elseif number < 11 then
        transform:setPosition(Vector3.new(6 * math.sin(number), 6 * math.cos(number), -10))
    elseif number < 13.4 then
        transform:setPosition(Vector3.new(5 * number - 61, 0, -10))
    else
        transform:setPosition(Vector3.new(6, 0, 124 - 10*number))
    end
end

