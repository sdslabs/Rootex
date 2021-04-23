ScriptTest = class("ScriptTest")

function ScriptTest:initialize(entity)
    self.exports = {
        variable = "Hello Rootex!",
        entityName = entity:getName()
    }
end

function ScriptTest:makeBoxScene()
    local box = RTX.Scene.CreateEmpty()
    box.entity:addDefaultComponent("TransformComponent")
    box.entity:addDefaultComponent("ModelComponent")
    return box
end

function ScriptTest:begin(entity)
    self.box1 = self.makeBoxScene()
    self.box2 = self.makeBoxScene()
    self.box3 = self.makeBoxScene()

    self.box1.entity.transform.position = RTX.Vector3.new(-2, 0, -4)
    self.box2.entity.transform.position = RTX.Vector3.new( 0, 0, -4)
    self.box3.entity.transform.position = RTX.Vector3.new( 2, 0, -4)

    entity.scene:addChild(self.box1)
    entity.scene:addChild(self.box2)
    entity.scene:addChild(self.box3)
    
    collectgarbage("collect")
    
    print(self.box1)
    print(self.box2)
    print(self.box3)
end

function ScriptTest:update(entity, delta)
    self.box1.entity:getName()
    self.box2.entity:getName()
    self.box3.entity:getName()
end

function ScriptTest:destroy(entity)
    print("Everything is permitted")
end

return ScriptTest
