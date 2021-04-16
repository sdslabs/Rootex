LevelChange = class("LevelChange")

function LevelChange:initialize(entity)
    self.exports = {
        loadScene = "Enter scene to load"
    }
    self.running = false
    self.timeSinceLoad = 0
    self.transform = entity:getTransform()
    self.text = entity:getTextUI()
end

function LevelChange:enter(entity)
    entity:bind("A", LevelChange.onLoadLevel)
end

function LevelChange:update(entity, delta)
    if self.running == true then
        self.timeSinceLoad = self.timeSinceLoad + delta
        if self.timeSinceLoad > 2000 then
            local arguments = {}
            arguments[1] = self.exports.loadScene
            print(arguments)
            RTX.LoadScene("game/assets/scenes/loading_screen.scene.json", arguments)
            self.running = false
        end
    end
end

function LevelChange:onLoadLevel(entity, event)
    if event:getData().y == 1 and self.running == false then
        RTX.GetCurrentScene():addChild(RTX.Scene.CreateFromFile("game/assets/scenes/fade_to_black.scene.json"))
        self.running = true
    end
    return true
end

function LevelChange:destroy()
end

return LevelChange
