Resources
=========

Resources are first class members in the Rootex Engine. By resources, we refer to any file loaded data, that is processed and ready to be used in the engine.

ResourceLoader
==============

Resources are created and owned by the :ref:`Class ResourceLoader` and distributed to the user and the engine as pointers to instances of the polymorphic :ref:`Class ResourceFile`. :ref:`Class ResourceFile` has been subclassed multiple times to store different kinds of data like sounds, music, images, fonts, 3D models, normal text files like Lua files or JSON files, etc. Look up the documentation on the resource loader for more information.

Resources are often the heaviest parts of a game in terms of actual memory that they occupy. :ref:`Class ResourceLoader` has been designed in such a manner that stores resources and distributes the earlier cached resource again instead of loading the same resource again to save memory, in case the same resource is instructed to be loaded more than once. Hence, the engine and the users need not worry about not loading the same resources multiple times. 

Resources may change in the file system after they have been loaded by the engine. To fix this, all resources can detect if they have been changed by the file system and have the ability to reload their contents and re-process it on demand.

Resource Loading can often be a bottleneck for complex scenes in a simple game engine. Rootex provides 2 mechanisms to make Resoource handling smooth:

* Multithreaded loading: ResourceFiles are loaded to memory in parallel and the create objects are allowed to reference each other once basic setup is done.
* Preloads: A scene can define a list of filepaths as preloads. These resources are loaded into Resource cache during the initial loading of the scene, even if they're not being directly referenced. By specifying preloads, We can prevent frame drops due to disk reads loading mid-gameplay.
