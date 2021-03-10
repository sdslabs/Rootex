Loading = class("Loading")

function Loading:initialize(entity)
    self.progress = RTX.AtomicInt.new()
    self.totalProgress = -1
    self.sceneToLoad = ""
    self.isPreLoaded = false
    self.timeToSwitch = 0
end

function Loading:begin(entity)
    self.sceneToLoad = RTX.GetSceneArguments()[1]
    if self.sceneToLoad ~= nil then
        self.totalProgress = RTX.PreloadScene(self.sceneToLoad, self.progress)
    else
        print("Did not load any level")
    end
end

function Loading:update(entity, delta)
    if self.progress:load() == self.totalProgress then
        if self.isPreLoaded == false then
            RTX.CallEvent(RTX.Event.new("GameLevelSwitch", 0))
        end
        self.isPreLoaded = true
    end
    
    if self.isPreLoaded then
        self.timeToSwitch = self.timeToSwitch + delta
        if self.timeToSwitch > 2000 then
            RTX.LoadPreloadedScene(self.sceneToLoad, {})
        end
    end
end

function Loading.destroy(entity)
end

return Loading
