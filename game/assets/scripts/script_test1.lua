exports = {
    moveEntity = ""
}

local leftEntity

function onBegin(entity)
    print(exports.moveEntity.script.number)
    leftEntity = RTX.EntityFactory.CreateFromClass(RTX.ResourceLoader.CreateText('game/assets/classes/Cube/Cube.entity.json'))
    leftEntity:getTransform():setPosition(RTX.Vector3.new(-1.5, 0, -5))
    print("Nothing is true")
end

function onUpdate(entity, delta)
    exports.moveEntity.script.number = 0.01 + exports.moveEntity.script.number
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
