function onBegin(entity)
    ball = entity:getPhysicsCollider()
end

function onUpdate(entity, delta)
    if InputManager.Get():isPressed("Forward") then
       ball:applyForce(Vector3.new(0, 0, 0.002))
    end
    if InputManager.Get():isPressed("Backward") then
        ball:applyForce(Vector3.new(0, 0, -0.002)) 
    end
    if InputManager.Get():isPressed("Left") then
        ball:applyForce(Vector3.new(-0.002, 0.0, 0.0)) 
    end
    if InputManager.Get():isPressed("Right") then
        ball:applyForce(Vector3.new(0.002, 0, 0)) 
    end
    if InputManager.Get():isPressed("Jump") then
        ball:setVelocity(ball:getVelocity() + Vector3.new(0, 0.02, 0)) 
    end
end

function onEnd(entity)
end
