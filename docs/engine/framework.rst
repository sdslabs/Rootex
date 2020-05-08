Framework
=========

Rootex has certain funcitionalities built-in to support an ECS implementation. The main aim of this kind of a architecture is to break down the game into a collection of behaviors and perceiving the game as an interplay of behaviors, rather than hardcoded functionalities inside each game object through object oriented programming. It originates from a place in the programming world that suggests "Composition is better than Inheritance". 

ECS just implements a dynamic composition, in the sense that behaviors can be added to game objects at runtime.

One of the popular side benefits of ECS is increased cache coherency.

Rootex' ECS architecture has 3 main parts.

Component
=========

:ref:`Class Component`

A Component is a collection of data for the game to use. Components are analogous to behaviors and components store some peculiar data to maintain their behavior. Component do not do anything else. They may allow changing the data in a certain manner from their public API.

Entity
======

:ref:`Class Entity`

An Entity in Rootex is a collection of components. The entity will have a name additionally but all data being used in the game will be stored in one of the components of an entity. Entities provide the component with an identity so that components can be theorized to "belong" to a thing in the game.

System
======

:ref:`Class System`

A System in Rootex is containing all the logic/algorithms that are needs to make sense of the data that is stored inside a specific type of component. Systems only interact with a certain type of components. In Rootex, all components of similar type are stored in an array and all these arrays containing different types of components are stored in a hash map so that the array having an component type can be indexed and used for processing by a :ref:`_exhale_class_class_system`.

----

***************
Entity Curation
***************

Rootex uses JSON style serializations to store and load entities. Entities are created from either entity class files, or simply entity files. Both the types of files are JSON files that contain the data needed to reconstruct that entity in the same exact configuration as seen earlier or in the editor.

Entity files and entity class files are recognized by .entity.json file extensions.

Entity construction and component assignment is handled by :ref:`Class EntityFactory`

Each :ref:`Class Component` needs to implement 2 static functions called: ``Create`` and ``CreateDefault`` to be registered as a component and get assigned to an entity.
