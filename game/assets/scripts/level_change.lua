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
            local arguments = {}
            arguments[1] = "game/assets/scenes/model.scene.json"
            print(arguments)
            RTX.LoadScene("game/assets/scenes/loading_screen.scene.json", arguments)
            running = false
        end
    end
end

function onLoadLevel(event)
    if event:getData().y == 1 and running == false then
        local scene = RTX.Scene.CreateFromFile("game/assets/scenes/fade_to_black.scene.json")
        RTX.GetCurrentScene():addChild(scene)
        running = true
    end
    return true
end

function onEnd(entity)
end
