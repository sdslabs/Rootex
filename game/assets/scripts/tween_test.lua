local flux = require("flux")

TweenTest = class("TweenTest")

function TweenTest:enterScene(entity)
    flux.to(entity.transform, 4, { position = { x = 3 } })
end

function TweenTest:update(entity, delta)
    print(entity.transform.position.x)
end

return TweenTest
