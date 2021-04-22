local flux = require("flux")

TweenTest = class("TweenTest")

function TweenTest:enterScene(entity)
    self.position = RTX.Vector3.new(0.0, 0.0, -4.0)
    flux.to(self.position, 1, { x = 1 }):ease("elasticout"):delay(1)
    :after(1, { y = 1 }):ease("backinout")
    :after(1, { x = -1 }):ease("sineinout")
    :after(1, { y = -1 }):ease("expoin")
    :after(1, { x = 1 }):ease("elasticinout")
end

function TweenTest:update(entity, delta)
    entity.transform.position = self.position
end

return TweenTest
