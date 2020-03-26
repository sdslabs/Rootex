function onBegin(entity)
    v1 = Rootex.Vector2.new(1,1)
    v2 = Rootex.Vector2.new(1,1)
    print((v1 + v2).x)
    print(v1.x)
    print(v2.x)
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
