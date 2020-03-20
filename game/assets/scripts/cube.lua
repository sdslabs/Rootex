function onBegin()
end

function onUpdate(delta, entity)
    print(delta)
    print(Rootex.EntityFactory.GetSingleton():findEntity(entity))
end

function onEnd( )
end
