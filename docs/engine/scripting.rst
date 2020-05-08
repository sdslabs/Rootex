Scripting
=========

Rootex Engine has a fully scriptable interface implemented using Lua and the Sol3 (https://sol2.readthedocs.io/en/latest/) library for creating bindings.

Scripting is implemented as a script file in :ref:`Class ScriptComponent` which has the following skeleton:

.. code-block:: javascript

    // empty.lua is the default script for every ScriptComponent
    function onBegin(entity)
        print("Nothing is true")
    end

    function onUpdate(delta, entity)
    end

    function onHit(entity, hit, other)
    end

    function onEnd(entity)
        print("Everything is permitted")
    end

The functions are called into Lua from Rootex on the command of the :ref:`Class ScriptSystem` and some other systems who have business with the ScriptComponent.

The script files are run in a Lua VM and the Rootex functions available are registed by the :ref:`Class LuaInterpreter`'s implementation. The Lua scripting interface for Rootex mostly looks the same as the Rootex engine API that the engine uses internally to provide as vast a scripting environment as possible. All Rootex class names and functions are hidden under the ``Rootex`` global Lua variable.
