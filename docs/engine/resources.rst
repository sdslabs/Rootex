Resources
=========

Resources are first class members in the Rootex Engine. By resources, we refer to any file loaded data, that is processed and ready to be used in the engine.

ResourceLoader
==============

Resources are created by the :ref:`Class ResourceLoader` and distributed to the user and the engine as pointers to instances of the polymorphic :ref:`Class ResourceFile`. :ref:`Class ResourceFile` has been subclassed multiple times to store different kinds of data like sounds, music, images, fonts, 3D models, normal text files like Lua files or JSON files, etc. Look up the documentation on the resource loader for more information.

Resources are often the heaviest parts of a game, in terms of actual memory that they occupy. :ref:`Class ResourceLoader` has been designed in such a manner that stores resources and distributes the earlier cached resource again instead of loading the same resource again to save memory, in case the same resource is instructed to be loaded more than once.
