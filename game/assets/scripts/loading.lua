progress = RTX.AtomicInt.new()
totalProgress = -1
levelToLoad = ""
isPreLoaded = false
timeToSwitch = 0

function onBegin(entity)
    levelToLoad = RTX.LevelManager.Get():getCurrentLevelArguments()[1]
    if levelToLoad ~= nil then
        totalProgress = RTX.LevelManager.Get():preloadLevel(levelToLoad, progress)
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
            RTX.LevelManager.Get():openPreloadedLevel(levelToLoad, {})
        end
    end
end

function onHit(entity, hit, other)
end

function onEnd(entity)
end
