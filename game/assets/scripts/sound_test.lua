require("game/assets/scripts/state_machine")

Box = class()
function Box:open()
    print("Box was opened")
end

function Box:close()
    print("Box was closed")
end

local box = Box()

function onBegin(entity)
    transform = entity:getTransform()
    number = 0
end

function onUpdate(entity, delta)
    number = number + 0.01 * delta / 10.0
    if number < 4.71 then
        transform:setPosition(RTX.Vector3.new(6 * math.sin(number), 0, -10))
    elseif number < 11 then
        transform:setPosition(RTX.Vector3.new(6 * math.sin(number),
                                                 6 * math.cos(number), -10))
    elseif number < 13.4 then
        transform:setPosition(RTX.Vector3.new(5 * (number) - 61, 0, -10))
    else
        transform:setPosition(RTX.Vector3.new(6, 0, 124 - 10 * number))
    end
    box:open()
    box:close()
end

function onEnd()
end
