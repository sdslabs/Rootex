PlayerControl = class("PlayerControl")

function PlayerControl:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
    self.collider = entity:getSphereCollider() or entity:getBoxCollider() or entity:getCapsuleCollider()
end

function PlayerControl:begin(entity)
end

function PlayerControl:update(entity, delta)
    delta = delta * 0.025
    if RTX.Input.IsPressed("Forward") then
        self.collider:applyForce(RTX.Vector3.new(0, 0, -delta))
    end
    if RTX.Input.IsPressed("Backward") then
        self.collider:applyForce(RTX.Vector3.new(0, 0, delta))
    end
    if RTX.Input.IsPressed("Left") then
        self.collider:applyForce(RTX.Vector3.new(-delta, 0, 0))
    end
    if RTX.Input.IsPressed("Right") then
        self.collider:applyForce(RTX.Vector3.new(delta, 0, 0))
    end
end

function PlayerControl:destroy(entity)
end

return PlayerControl
