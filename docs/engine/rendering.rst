Rendering
=========

Rootex uses DirectX 11 to render graphics.

Rendering in Rootex has been implemented with special attention so that it behaves properly with our ECS architecture. This is because object hierarchies are notorious for how difficult they are to model correctly in a codebase that uses ECS. The :ref:`Class RenderSystem` uses the :ref:`Class VisualComponent` to track renderable components.

Modeling hierarchy structure is important for rendering because each object needs to know where exactly its parent is so that it can be rendered in the proper orientation and position. 

For representing entity hierarchies Rootex has decided to represent hierarchies in a separate component. This component is called :ref:`Class HierarchyComponent`. More information is in the documentation for the component. Each hierarchy component object holds a pointer to the parent entity's hierarchy component, and a list of pointers to the hierarchy components of its children. A :ref:`Class System` which requires information about the hierarchy of the object for any processing, is required to fetch the hierarchy component from the owner of every component that it wishes to operate upon.

The :ref:`Class RenderSystem` uses the hierarchy component to recursively traverse the object hierarchy, starting from the root entity (which is persistent across levels). Every time the render system recognizes a parent, before processing its children, the render system takes note of the transform (a representation of position, rotation and scale all at once) of the parent and appends it to the transformation stack. The transformation stack is an implementation for inheriting transforms from the parent entity of a child entity, used while performing a Depth-First-Search on the component hierarchy established by hierarchy component instances.

The transformation stack of UI components is kept separate from the transformation stack of 3D world visual components.
