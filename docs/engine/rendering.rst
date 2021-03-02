Rendering
=========

Rootex uses DirectX 11 to render graphics.

Rendering in Rootex has been implemented with special attention so that it behaves properly with our ECS architecture. The :ref:`Class RenderSystem` uses the :ref:`Class RenderableComponent` to share common funcitonalities across different components which add to the visuals of the scene.

The :ref:`Class RenderSystem` uses the owning :ref:`Class Scene` of the renderable component to recursively traverse the object hierarchy, starting from the root entity (which is persistent across levels). Every time the render system recognizes a parent, before processing its children, the render system takes note of the transform (a representation of position, rotation and scale all at once) of the parent and appends it to the transformation stack. The transformation stack is an implementation for inheriting transforms from the parent entity of a child entity, used while performing a Depth-First-Search on the component hierarchy established by hierarchy component instances.

The transformation stack of UI components is kept separate from the transformation stack of 3D world visual components.

Once all transformations are updated, :ref:`Class RenderSystem` loops over all the renderable components and does the rendering required to show them.

Rootex also performs sky, fog and related rendering effects and post processing effects.
