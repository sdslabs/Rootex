function onBegin(entity)
    text = entity:getTextVisual2D()
    print(text)
    number = 0
    Rootex.CallEvent("MyEvent", "CubeEvent")
    Rootex.CallEvent("MyEvent", "CubeEvent")
    Rootex.CallEvent("MyEvent", "CubeEvent")
    Rootex.CallEvent("MyEvent", "CubeEvent")
end

function onUpdate(delta, entity)
    number = number + 1
    text:setText(tostring(number))
end

function newFunction()
    print("newFunction was called!")
end

function onEnd(entity)
end
