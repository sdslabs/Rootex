function onJump(event)
    velocity = collider:getVelocity()
    velocity.y = 5
    collider:setVelocity(velocity)

    return true
end

function onBegin(entity)
    collider = entity:getPhysicsCollider()
    collider:setVelocity(RTX.Vector3.new(0.1, 0, 0))
    local color = Colourb.new(1,2,3,4)
    dbg()
    RTX.Connect(onJump, "Jump")
end

function onUpdate(entity, delta)
end

function onHit(entity, hit, other)
    failEvent = RTX.Event.new("FlabbyBird", "GameEnd", 0)
    RTX.CallEvent(failEvent)
end

function onEnd(entity)
end
