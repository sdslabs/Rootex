Inputs
======

User inputs in Rootex are handled with polling and/or callback based mechanisms using the event system. Rootex also abstracts the exact button press occurences from the user and forces the user to use keybinding names strings to query for input.

:ref:`Class InputManager` handles the input for the entire engine. The input manager is initially feeded with a collection of 'input schemes'. These input schemes are defined in the level JSON files likewise:

.. code-block:: JSON

    // flappy_bird.level.json
    {
        "camera": 23,
        "inputSchemes": [
            {
                "name": "FlappyBird",
                "bools": [
                    {
                        "inputEvent": "Jump",
                        "device": 1,
                        "button": 99
                    }
                ],
                "floats": {}
            }
        ],
        "startScheme": "FlappyBird"
    }

The field ``inputSchemes`` is the collection of input schemes that the input manager will recognize. In this example, there is only 1 scheme called "FlappyBird", but a game can have multiple input schemes and only one of those input schemes can be active at a time. The field ``startScheme`` tells the Rootex engine which input scheme should be selected as soon as the level is loaded up.

Each input scheme has fields:

* ``name``: Name of the input scheme
* ``bools``: Array of inputs that are represented better as boolean values. Used for buttons that are either held down or not held down.
* "floats": Array of inputs that are not digital in nature and rather are analogous, like mouse positions and joystick movements.

Inside each input keybinding, there are fields:

* ``inputEvent``: The event name that gets emitted as soon as the input changes state. This need not be unique amongst other keybindings.
* ``device``: The device enum value that this input keybinding is present on.
* ``button``: The button value that is mapped to the keybinding
