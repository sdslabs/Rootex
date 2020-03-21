function onBegin(entity)
    entity = Rootex.EntityFactory.GetSingleton():createEntity("game/assets/classes/cube.entity.json")
    print(entity)
end

function onUpdate(delta, entity)
end

function onEnd(entity)
end
