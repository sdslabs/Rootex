PlayerControl = class("PlayerControl")

function PlayerControl:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!"
    }
    self.collider = entity:getSphereCollider() or entity:getBoxCollider() or entity:getCapsuleCollider()
    self.transform = entity:getTransform()
    self.anim = entity:getAnimatedModel()
end

function PlayerControl:begin(entity)
    self.anim:play()
end

function PlayerControl:update(entity, delta)
    local angle = ""
    if RTX.Input.IsPressed("Forward") then
        self.collider:applyForce(RTX.Vector3.new(0, 0, -0.01))
        angle = 0
    end
    if RTX.Input.IsPressed("Left") then
        self.collider:applyForce(RTX.Vector3.new(-0.01, 0, 0))
        angle = math.pi / 2
    end
    if RTX.Input.IsPressed("Right") then
        self.collider:applyForce(RTX.Vector3.new(0.01, 0, 0))
        angle = -math.pi / 2
    end
    if RTX.Input.IsPressed("Backward") then
        self.collider:applyForce(RTX.Vector3.new(0, 0, 0.01))
        angle = math.pi
    end
    if RTX.Input.IsPressed("Punch") then
        self.collider:setVelocity(RTX.Vector3.new(0, 0, 0))
        angle = 0
    end

    if angle ~= "" then
        self.transform:setRotation(angle, 0, 0)
    else
        self.collider:setVelocity(RTX.Vector3.new(0, 0, 0))
    end
end

function PlayerControl:destroy(entity)
end

return PlayerControl
