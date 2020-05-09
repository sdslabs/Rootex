function onBegin(entity)
    m_Entity = entity
end

function onUpdate(delta, entity)
end

function onJump(event)
    m_Entity:trigger()
    print("Triggered")
end

function onEnd(entity)
end
