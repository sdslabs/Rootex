function makeBoxScene()
    local box = RTX.Scene.CreateEmpty()
    box:getEntity():addDefaultComponent("TransformComponent")
    box:getEntity():addDefaultComponent("ModelComponent")
    return box
end

ScriptTest = class("ScriptTest")

function ScriptTest:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!",
        entityName = entity:getName()
    }
end

function ScriptTest:begin(entity)
    self.box1 = makeBoxScene()
    self.box2 = makeBoxScene()
    self.box3 = makeBoxScene()

    self.box1:getEntity():getTransform():setPosition(RTX.Vector3.new(-2, 0, -4))
    self.box2:getEntity():getTransform():setPosition(RTX.Vector3.new( 0, 0, -4))
    self.box3:getEntity():getTransform():setPosition(RTX.Vector3.new( 2, 0, -4))

    entity:getScene():addChild(self.box1)
    entity:getScene():addChild(self.box2)
    entity:getScene():addChild(self.box3)
    
    collectgarbage("collect")
    
    print(self.box1)
    print(self.box2)
    print(self.box3)
end

function ScriptTest:update(entity, delta)
    self.box1:getEntity():getName()
    self.box2:getEntity():getName()
    self.box3:getEntity():getName()
end

function ScriptTest:destroy(entity)
    print("Everything is permitted")
end

return ScriptTest
