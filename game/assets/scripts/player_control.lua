function onBegin(entity)
end

function onUpdate(entity, delta)
    collider = entity:getSphereCollider()
    if RTX.Input.Get():isPressed("Forward") then
        collider:applyForce(RTX.Vector3.new(0, 0, -0.1))
    end
    if RTX.Input.Get():isPressed("Backward") then
        collider:applyForce(RTX.Vector3.new(0, 0, 0.1))
    end
    if RTX.Input.Get():isPressed("Left") then
        collider:applyForce(RTX.Vector3.new(-0.1, 0, 0))
    end
    if RTX.Input.Get():isPressed("Right") then
        collider:applyForce(RTX.Vector3.new(0.1, 0, 0))
    end
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
