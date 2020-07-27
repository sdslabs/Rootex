function onBegin(entity)
    collider = entity:getPhysicsCollider()
    collider:setVelocity(Vector3.new(0.1, 0, 0))
end

function onUpdate(delta, entity)
end

function onJump()
    velocity = collider:getVelocity()
    velocity.y = 5
    collider:setVelocity(velocity)
end

function onHit(entity, hit, other)
    failEvent = Event.new("FlabbyBird", "GameEnd", 0)
    CallEvent(failEvent)
end

function onEnd(entity)
end
