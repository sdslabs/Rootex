function onBegin(entity)
    text = entity:getTextVisual2D()
    print(text)
    number = 0
end

function onUpdate(delta, entity)
    number = number + 1
    text:setText(tostring(number))
end

function onEnd(entity)
end
