Scripting
=========

Rootex Engine has a fully scriptable interface implemented using Lua and the Sol3 (https://sol2.readthedocs.io/en/latest/) library for creating bindings.

Scripts i.e. Lua files, can be attached to entities and can define functions upon those entities.

See Middleclass (https://github.com/kikito/middleclass) for details on the ``class()`` based Lua OOP support.

.. code-block:: javascript

    EmptyScript = class("EmptyScript")

    -- First method called after script initialisation
    -- not safe to refer other entity script tables here
    -- setup initial data members here that don't refer entities
    function EmptyScript:begin(entity)
    end

    -- Called after all `begin` for the frame have been called
    -- safe to assume that all scripts have `begin`ed and have 
    -- data members
    function EmptyScript:enterScene(entity)
        print("Nothing is true")
    end

    -- called once every frame
    function EmptyScript:update(entity, delta)
    end

    -- called during entity destruction
    function EmptyScript:destroy(entity)
        print("Everything is permitted")
    end

    -- called when Collider of the entity detects a hit
    function EmptyScript:hit(hit)
        print("Everything is permitted")
    end

    -- called when entity enters a TriggerComponent
    function EmptyScript:enterTrigger(entity, trigger)
    end

    -- called when entity exits a TriggerComponent
    function EmptyScript:exitTrigger(entity, trigger)
    end

    return EmptyScript

The functions are called into Lua from Rootex on the command of the :ref:`Class ScriptSystem`.

The script files are run in a Lua VM and the Rootex functions available are registed by the :ref:`Class LuaInterpreter`'s implementation. The Lua scripting interface for Rootex mostly looks the same as the Rootex engine API that the engine uses internally to provide as vast a scripting environment as possible. All Rootex class names and functions are hidden under the ``RTX`` global Lua variable. An object which has its constructor registered can be constructed from scripts as ``RTX.Type.new(args)``.

Scripting API
=============

You can find the scripting API and related docs in the editor itself.
