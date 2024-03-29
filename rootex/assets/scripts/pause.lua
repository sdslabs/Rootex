Pause = class("Pause")

function Pause:initialize(entity)
    self.exports = {
        pauseScene = "Enter scene to pause"
    }
    self.running = false

end

function Pause:enterScene(entity)
    entity:bind("InputPause", Pause.onPauseLevel)
end

function Pause:Resume(entity)
    RTX.GetCurrentScene():removeChild(RTX.Scene.FindScenesByName("pause")[1])
    self.running = false
    RTX.System.unPause()
end

function Pause:onPauseLevel(entity, event)
    ::start::
    if event:getData().y == 1 and self.running == false then
        RTX.GetCurrentScene():addChild(RTX.Scene.CreateFromFile("rootex/assets/scenes/pauseUI.scene.json"))
        self.running = true
        RTX.System.pause()
    elseif event:getData().y == 1 and self.running == true then
        if((#RTX.Scene.FindScenesByName("pause"))==0) then
            self.running=false
            goto start
        end
        RTX.GetCurrentScene():removeChild(RTX.Scene.FindScenesByName("pause")[1])
        self.running = false
        RTX.System.unPause()
    end 

    return true
end

function Pause:destroy()
end

return Pause