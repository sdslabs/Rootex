progress = RTX.AtomicInt.new()
totalProgress = -1
sceneToLoad = ""
isPreLoaded = false
timeToSwitch = 0

function onBegin(entity)
    sceneToLoad = RTX.GetSceneArguments()[1]
    if sceneToLoad ~= nil then
        totalProgress = RTX.PreloadScene(sceneToLoad, progress)
    else
        print("Did not load any level")
    end
end

function onUpdate(entity, delta)
    if progress:load() == totalProgress then
        if isPreLoaded == false then
            RTX.CallEvent(RTX.Event.new("LoadingScript", "GameLevelSwitch", 0))
        end
        isPreLoaded = true
    end
    
    if isPreLoaded then
        timeToSwitch = timeToSwitch + delta
        if timeToSwitch > 2000 then
            RTX.LoadPreloadedScene(sceneToLoad, {})
        end
    end
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
