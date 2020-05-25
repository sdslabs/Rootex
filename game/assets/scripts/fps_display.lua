function onBegin(entity)
    text = entity:getTextUI()
    number = 0
end

function onUpdate(delta, entity)
    number = number + 1
    if number == 20 then
        number = 0
        text:setText(tostring(1000 / delta))
    end
end

function onEnd(entity)
end
