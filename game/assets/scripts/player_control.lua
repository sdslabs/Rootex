function onBegin(entity)
end

function onUpdate(entity, delta)
    collider = entity:getSphereCollider()
    if RTX.InputManager.Get():isPressed("Forward") then
        collider:setVelocity(RTX.Vector3.new(0, 0, -2))
    end
    if RTX.InputManager.Get():isPressed("Backward") then
        collider:setVelocity(RTX.Vector3.new(0, 0, 2))
    end
    if RTX.InputManager.Get():isPressed("Left") then
        collider:setVelocity(RTX.Vector3.new(-2, 0, 0))
    end
    if RTX.InputManager.Get():isPressed("Right") then
        collider:setVelocity(RTX.Vector3.new(2, 0, 0))
    end
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
