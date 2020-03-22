function onBegin(entity)
    print(Rootex.Vector2.new(2, 2).x)
    print(Rootex.Vector3.new(3, 1, 1).x)
    print(Rootex.Vector4.new(4, 1, 1, 1).x)
    print(Rootex.Matrix.new().Identity.x)
    print(Rootex.ResourceLoader.CreateAudio("game/assets/jungle.wav"):getPath())
    text = Rootex.ResourceLoader.CreateText("game/assets/classes/cube.entity.json")
    transform = entity:getTransform()
    pos = transform:getPosition()
    print(transform)
    number = 0
end

function onUpdate(delta, entity)
    pos.x = math.sin(number)
    number = number + 0.05
    transform:setPosition(pos)
end

function onEnd(entity)
end
