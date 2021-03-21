Follow = class("Follow")

function Follow:initialize(entity)
    self.exports = {
        following = 22,
        tightness = 0.1
    }
end

function Follow:begin(entity)
    local following = RTX.Scene.FindSceneByID(self.exports.following)
    self.followingTransform = following:getEntity():getTransform()
end

function Follow:update(entity, delta)
    entity:getTransform():setAbsolutePosition(
        self.exports.tightness * self.followingTransform:getAbsolutePosition() + 
        (1.0 - self.exports.tightness) * entity:getTransform():getAbsolutePosition())
    entity:getTransform():setRotation(0, 0, 0)
end

function Follow:destroy(entity)
end

return Follow
