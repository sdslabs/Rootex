local transform

exports = {
    a = 1,
    number = 0.1
}

function onBegin(entity)
    print("Nothing is true")
    transform = entity:getTransform()
    print("exports.a: " .. exports.a)
end

function onUpdate(entity, delta)
    transform:setPosition(RTX.Vector3.new(exports.number, 0, -5))
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
