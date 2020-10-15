exports = {
    moveEntity = ""
}

function onBegin(entity)
    print(exports.moveEntity:script().number)
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
