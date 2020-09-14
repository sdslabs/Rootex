function onBegin(entity)
    ball = entity:getPhysicsCollider()
end

function onUpdate(entity, delta)
    if RTX.InputManager.Get():isPressed("Forward") then
       ball:applyForce(RTX.Vector3.new(0, 0, 0.002))
    end
    if RTX.InputManager.Get():isPressed("Backward") then
        ball:applyForce(RTX.Vector3.new(0, 0, -0.002)) 
    end
    if RTX.InputManager.Get():isPressed("Left") then
        ball:applyForce(RTX.Vector3.new(-0.002, 0.0, 0.0)) 
    end
    if RTX.InputManager.Get():isPressed("Right") then
        ball:applyForce(RTX.Vector3.new(0.002, 0, 0)) 
    end
    if RTX.InputManager.Get():isPressed("Jump") then
        ball:setVelocity(ball:getVelocity() + RTX.Vector3.new(0, 0.02, 0)) 
    end
end

function onEnd(entity)
end
