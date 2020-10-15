exports = {
    move_entity = ""
}

function onBegin(entity)
    -- other = RTX.EntityFactory.Find(3)
    print(exports.move_entity:script().number)
    print("Nothing is true")
end

function onUpdate(entity, delta)
    exports.move_entity.script.number = 0.01 + exports.move_entity.script.number
    -- print(other.script.number)
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
