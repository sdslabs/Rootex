Pause = class("Pause")

function Pause:initialize(entity)
    self.exports = {
        pauseScene = "Enter scene to pause"
    }
    self.running = false
    self.timeSincePause = 0
end

function Pause:enterScene(entity)
    entity:bind("esc", Pause.onPauseLevel)
end

function Pause:onPauseLevel(entity, event)
    if event:getData().y == 1 and self.running == false then
        RTX.GetCurrentScene():addChild(RTX.Scene.CreateFromFile("game/assets/scenes/pauseUI.scene.json"))
        self.running = true
    end
    return true
end

function Pause:destroy()
end

return Pause
