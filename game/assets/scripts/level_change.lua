require('mobdebug').start()
progress = RTX.AtomicInt.new()
totalProgress = -1
running = false

function onBegin(entity)
    transform = entity:getTransform()
    text = entity:getTextUI()
    model = entity:getModel()
    model:setIsVisible(false)
    number = 0
    current = 0
    RTX.Connect(onLoadLevel, "A")
    print("Connected to input A")
end

function onUpdate(entity, delta)
    if running then
        number = number + delta * 0.001
        transform:setRotation(number, 0, 0)
        velocity = progress:load() / totalProgress
        current = current + velocity * 0.01
        text:setText("Loading: " .. tostring(100 * current / totalProgress))
    end

    if progress:load() == totalProgress then
        text:setText("Finished")
        RTX.LevelManager.Get():openPreloadedLevel("game/assets/levels/model_test")
    end
end

function onLoadLevel(event)
    if event:getData().x == 1 then
        totalProgress = RTX.LevelManager.Get():preloadLevel("game/assets/levels/model_test", progress)
        model:setIsVisible(true)  
        running = true
    end
    return true
end

function onEnd(entity)
end
