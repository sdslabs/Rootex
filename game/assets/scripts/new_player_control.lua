PlayerControl = class("PlayerControl")

function PlayerControl:initialize(entity)
    self.exports = {
        walkAnim = "",
        runAnim = "",
        idleAnim = "",
        jumpAnim = "",
        walkSpeed = 1.0,
        runSpeed = 1.0,
    }
    self.collider = entity:getSphereCollider() or entity:getBoxCollider() or entity:getCapsuleCollider()
    self.transform = entity:getTransform()
    self.anim = entity:getAnimatedModel()
end

function PlayerControl:begin(entity)
    self.anim:play()
    self.angle = 0
end

function PlayerControl:update(entity, delta)
    local changed = false
    
    if RTX.Input.IsPressed("Forward") then
        self.anim:transition(self.exports.runAnim, 2.0)
        self.collider:translate(RTX.Vector3.new(0, 0, -self.exports.runSpeed * delta))
        self.angle = 0
        changed = true
    end
    if RTX.Input.IsPressed("Left") then
        self.anim:transition(self.exports.walkAnim, 2.0)
        self.collider:translate(RTX.Vector3.new(-self.exports.walkSpeed * delta, 0, 0))
        self.angle = math.pi / 2
        changed = true
    end
    if RTX.Input.IsPressed("Right") then
        self.anim:transition(self.exports.walkAnim, 2.0)
        self.collider:translate(RTX.Vector3.new(self.exports.walkSpeed * delta, 0, 0))
        self.angle = -math.pi / 2
        changed = true
    end
    if RTX.Input.IsPressed("Backward") then
        self.anim:transition(self.exports.walkAnim, 2.0)
        self.collider:translate(RTX.Vector3.new(0, 0, self.exports.walkSpeed * delta))
        self.angle = math.pi
        changed = true
    end
    if RTX.Input.IsPressed("Punch") then
        self.anim:transition(self.exports.jumpAnim, 2.0)
        self.collider:setVelocity(RTX.Vector3.new(0, 0, 0))
        changed = true
    end

    if RTX.Input.IsPressed("Forward") and RTX.Input.IsPressed("Right") then
        self.angle = -math.pi / 4.0
    end
    if RTX.Input.IsPressed("Forward") and RTX.Input.IsPressed("Left") then
        self.angle = math.pi / 4.0
    end
    if RTX.Input.IsPressed("Backward") and RTX.Input.IsPressed("Right") then
        self.angle = -math.pi * 3.0 / 4.0
    end
    if RTX.Input.IsPressed("Backward") and RTX.Input.IsPressed("Left") then
        self.angle = math.pi * 3.0 / 4.0
    end

    self.transform:setRotation(self.angle, 0, 0)
    if changed == true then
    else
        self.anim:setAnimation(self.exports.idleAnim)
        self.collider:setVelocity(RTX.Vector3.new(0, 0, 0))
    end
end

function PlayerControl:destroy(entity)
end

return PlayerControl
