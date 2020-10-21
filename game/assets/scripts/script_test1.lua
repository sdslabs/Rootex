exports = {
    moveEntity = ""
}

local leftEntity

function onBegin(entity)
    print("right moving entity number: " .. exports.moveEntity.script.exports.number)
    leftEntity = RTX.EntityFactory.CreateFromClass(RTX.ResourceLoader.CreateText('game/assets/classes/Cube Script/Cube Script.entity.json'))
    leftEntity:getTransform():setPosition(RTX.Vector3.new(-1.5, 0, -5))
    print("left static entity number: " .. leftEntity.script.exports.number)
end

function onUpdate(entity, delta)
    exports.moveEntity.script.exports.number = 0.01 + exports.moveEntity.script.exports.number
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
