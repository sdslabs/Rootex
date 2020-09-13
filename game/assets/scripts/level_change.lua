progress = RTX.AtomicInt.new()
totalProgress = -1
running = false

function onBegin(entity)
    transform = entity:getTransform()
    text = entity:getTextUI()
    RTX.Connect(onLoadLevel, "A")
    print("Connected to input A")
end

function onUpdate(entity, delta)
    if running == true and progress:load() == totalProgress then
        text:setText("Finished")
        RTX.LevelManager.Get():openPreloadedLevel("game/assets/levels/model_test")
    end
end

function onLoadLevel(event)
    print("Called")
    print(event:getData())
    if event:getData().y == 1 and running == false then
        totalProgress = RTX.LevelManager.Get():preloadLevel("game/assets/levels/model_test", progress)
        loadingScreen = RTX.ResourceLoader.CreateText("game/assets/classes/LoadingScreen/LoadingScreen.entity.json")
        running = true
        RTX.EntityFactory.CreateFromClass(loadingScreen)
    end
    print("Finished")
    return true
end

function onEnd(entity)
end
