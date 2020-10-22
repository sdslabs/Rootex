exports = {
    movingEntity = ""
}

local leftEntity, topEntity
totalTime = 0.0

function onBegin(entity)
    print("right moving entity initial x: " .. exports.movingEntity.script.exports.x)

    leftEntity = RTX.EntityFactory.CreateFromClass(RTX.ResourceLoader.CreateText('game/assets/classes/Cube Script/Cube Script.entity.json'))
    leftEntity.script.exports.x = -1.2
    print("left static entity initial x: " .. leftEntity.script.exports.x)

    topEntity = RTX.EntityFactory.CreateFromClass(RTX.ResourceLoader.CreateText('game/assets/classes/Cube/Cube.entity.json'))
    topEntity:setScript("game/assets/scripts/script_test2.lua")
    topEntity.script.exports.y = 1.2
    print("top static entity initial y: " .. leftEntity.script.exports.y)
end

function onUpdate(entity, delta)
    totalTime = totalTime + delta
    print(totalTime)
    exports.movingEntity.script.exports.x = 0.0003 * delta + exports.movingEntity.script.exports.x
    if totalTime > 5000.0 then
        exports.movingEntity:setScript("")
    end
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
