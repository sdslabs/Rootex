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
    if RTX.Input.IsPressed("Forward") then
        self.collider:applyForce(RTX.Vector3.new(0, 0, -0.1))
    end
    if RTX.Input.IsPressed("Backward") then
        self.collider:applyForce(RTX.Vector3.new(0, 0, 0.1))
    end
    if RTX.Input.IsPressed("Left") then
        self.collider:applyForce(RTX.Vector3.new(-0.1, 0, 0))
    end
    if RTX.Input.IsPressed("Right") then
        self.collider:applyForce(RTX.Vector3.new(0.1, 0, 0))
    end
end

function PlayerControl:destroy(entity)
end

return PlayerControl
