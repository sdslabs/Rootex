local other

function onBegin(entity)
    other = RTX.EntityFactory.Find(3)
    print(other:script().number)
    print("Nothing is true")
end

function onUpdate(entity, delta)
    other.script.number = 0.01 + other.script.number
    print(other.script.number)
end

function onHit(entity, hit, other)
end

function onEnd(entity)
    print("Everything is permitted")
end
