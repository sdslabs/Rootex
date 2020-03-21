function onBegin(entity)
    textFile = Rootex.ResourceLoader.CreateTextResourceFile("LICENSE")
    print(textFile:getString())
end

function onUpdate(delta, entity)
end

function onEnd(entity)
end
