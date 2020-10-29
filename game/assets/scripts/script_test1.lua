exports = {
    movingEntity = "",
}

leftEntity = 0.0
topEntity = 0.0
totalTime = 0.0

function onBegin(entity)
    print("right moving entity initial x: " .. exports.movingEntity.script.exports.x)

    leftEntity = RTX.EntityFactory.CreateFromClass(RTX.ResourceLoader.CreateText('game/assets/classes/Cube Script/Cube Script.entity.json'))
    leftEntity.script.exports.x = -1.2
    print("left static entity initial x: " .. leftEntity.script.exports.x)

    leftEntity = RTX.EntityFactory.CreateFromClass(RTX.ResourceLoader.CreateText('game/assets/classes/Cube/Cube.entity.json'))
    leftEntity:setScript("game/assets/scripts/script_test2.lua")
    leftEntity.script.exports.y = 1.2
    print("top static entity initial y: " .. leftEntity.script.exports.y .. "    " .. leftEntity:getID())
end

function onUpdate(entity, delta)
    totalTime = totalTime + delta
    leftEntity.script.exports.y = 1.2
    print(totalTime)
    -- print(leftEntity:getName())
    if totalTime < 5000.0 then
        exports.movingEntity.script.exports.x = 0.0003 * delta + exports.movingEntity.script.exports.x
    end
    if totalTime > 5000.0 then
        exports.movingEntity:setScript("")
    end
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
