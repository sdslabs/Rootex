Pause = class("Pause")

function Pause:initialize(entity)
    self.exports = {
        pauseScene = "Enter scene to pause"
    }
    self.running = false
end

function Pause:enterScene(entity)
    entity:bind("esc", Pause.onPauseLevel)
end

function Pause:onPauseLevel(entity, event)
    if event:getData().y == 1 and self.running == false then
        RTX.GetCurrentScene():addChild(RTX.Scene.CreateFromFile("game/assets/scenes/pauseUI.scene.json"))
        self.running = true
        RTX.System.pause()
    elseif event:getData().y == 1 and self.running == true then
        RTX.GetCurrentScene():removeChild(RTX.Scene.FindScenesByName("pause")[1])
        self.running = false
        RTX.System.unpause()
    end 
    
    return true
end

function Pause:destroy()
end

return Pause
