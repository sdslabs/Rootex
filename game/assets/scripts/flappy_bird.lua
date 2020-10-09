function onJump(event)
    velocity = collider:getVelocity()
    velocity.y = 5
    collider:setVelocity(velocity)

    return true
end

function onBegin(entity)
    collider = entity:getPhysicsCollider()
    collider:setVelocity(Vector3.new(0.1, 0, 0))

    Connect(onJump, "Jump")
end

function onUpdate(delta, entity)
end

function onHit(entity, other)
    failEvent = Event.new("FlabbyBird", "GameEnd", 0)
    CallEvent(failEvent)
end

function onEnd(entity)
end
