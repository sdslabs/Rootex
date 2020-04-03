function onBegin(entity)
end

function onUpdate(delta, entity)
    number = number + 1
    text:setText(tostring(number))
end

function newFunction(event)
    print(event:getData())
end

function onEnd(entity)
end
