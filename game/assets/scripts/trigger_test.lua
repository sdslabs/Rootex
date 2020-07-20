function onBegin(entity)
    m_Entity = entity

    Connect(onJump, "Jump")
end

function onUpdate(delta, entity)
end

function onJump(event)
    m_Entity:trigger()
    print("Triggered")
    return true
end

function onEnd(entity)
end
