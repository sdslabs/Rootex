local transform

exports = {
    msg = "hello from script",
    x = 0,
    y = 0
}

function onBegin(entity)
    transform = entity:getTransform()
    print("msg: " .. exports.msg)
end

function onUpdate(entity, delta)
    transform:setPosition(RTX.Vector3.new(exports.x, exports.y, -5))
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
