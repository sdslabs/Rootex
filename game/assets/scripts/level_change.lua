running = false
timeSinceLoad = 0

function onBegin(entity)
    transform = entity:getTransform()
    text = entity:getTextUI()
    RTX.Connect(onLoadLevel, "A")
end

function onUpdate(entity, delta)
    if running == true then
        timeSinceLoad = timeSinceLoad + delta
        if timeSinceLoad > 2000 then
            RTX.LevelManager.Get():openLevel("game/assets/levels/loading")
        end
    end
end

function onLoadLevel(event)
    if event:getData().y == 1 and running == false then
        loadingScreen = RTX.ResourceLoader.CreateText("game/assets/classes/LoadingScreen/LoadingScreen.entity.json")
        loadingEntity = RTX.EntityFactory.CreateFromClass(loadingScreen)
        running = true
    end
    return true
end

function onEnd(entity)
end
