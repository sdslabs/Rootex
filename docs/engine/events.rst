Event Manager
=============

Events are a way to solve the problem of sphagetti code, and an efficient way to implement the publisher-subscriber design pattern in a considerably large codebase.

Events (:ref:`Class Event`) in Rootex are the equivalent of broadcast messages of a particular channel and Rootex' Event Manager (:ref:`Class EventManager`) is the equivalent of a broadcast company managing multiple channels. Functions can be registered as callbacks to events. When an event is called, all the functions registered to that event are called with the corresponding data related to the cause of origin of that event. Rootex' event manager has the ability to call both global functions and member functions (with the corresponding object that registered its member function, as a parameter into the member function, which is how C++ implements member functions).

:ref:`Class EventManager` is a singleton, and all engine level events are passed by it. User events can also be channeled through with no issues. E.g. Input events that are configured by the user are sent through the engine level event manager.

See :ref:`Struct RootexEvents`, :ref:`Struct EditorEvents`, :ref:`Class Event`
