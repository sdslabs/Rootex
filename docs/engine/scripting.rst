Scripting
=========

Rootex Engine has a fully scriptable interface implemented using Lua and the Sol3 (https://sol2.readthedocs.io/en/latest/) library for creating bindings.

Scripts i.e. Lua files, can be attached to entities and can define functions upon those entities.

See Middleclass (https://github.com/kikito/middleclass) for details on the ``class()`` based Lua OOP support.

.. code-block:: javascript

    PlayerControl = class("PlayerControl")

    function PlayerControl:initialize(entity)
        self.exports = {
            variable = "Hello Rootex!"
        }
        self.collider = entity:getSphereCollider() or entity:getBoxCollider() or entity:getCapsuleCollider()
    end

    function PlayerControl:begin(entity)
    end

    function PlayerControl:update(entity, delta)
        if RTX.Input.IsPressed("Forward") then
            self.collider:applyForce(RTX.Vector3.new(0, 0, -0.1))
        end
        if RTX.Input.IsPressed("Backward") then
            self.collider:applyForce(RTX.Vector3.new(0, 0, 0.1))
        end
        if RTX.Input.IsPressed("Left") then
            self.collider:applyForce(RTX.Vector3.new(-0.1, 0, 0))
        end
        if RTX.Input.IsPressed("Right") then
            self.collider:applyForce(RTX.Vector3.new(0.1, 0, 0))
        end
    end

    function PlayerControl:destroy(entity)
    end

    return PlayerControl

The functions are called into Lua from Rootex on the command of the :ref:`Class ScriptSystem`.

The script files are run in a Lua VM and the Rootex functions available are registed by the :ref:`Class LuaInterpreter`'s implementation. The Lua scripting interface for Rootex mostly looks the same as the Rootex engine API that the engine uses internally to provide as vast a scripting environment as possible. All Rootex class names and functions are hidden under the ``RTX`` global Lua variable.
