=================
Graphics Tutorial
=================

Create a scene
***************

To create a scene 

* Go to `file->CreateScene`.

* Name the scene and click create.

.. image:: images/graphics_tutorial/create_scene.png
	:alt: Create Scene
	:align: center

|


Create Empty scene
******************

To create an empty scene.

* Right-click the root scene.
* Click create an empty scene

.. image:: images/graphics_tutorial/add_empty_scene.png
	:alt: Create Empty Scene
	:align: center

.. image:: images/graphics_tutorial/empty_scene_created.png
	:alt: Empty Scene Created
	:align: center

|

Give appropriate components. Transform component is a must.

***************
Model Component
***************

* Right-click the empty scene.
* Click Edit Components.
* Check model component.
* Open inspector
* Go to the model component in the inspector.
* Click the folder icon next to model.

.. image:: images/graphics_tutorial/add_model_component_and_transform_component.png
	:alt: Components Added
	:align: center

.. image:: images/graphics_tutorial/select_model_file.png
	:alt: Selecting File
	:align: center

|


For sponza initially, it would look like this:

.. image:: images/graphics_tutorial/model_file_just_added.png
	:alt: Model File Just Added
	:align: center

|

set scale to (0.0031, 0.031, 0.031) and set LOD distance to 123:

.. image:: images/graphics_tutorial/change_model_scale.png
	:alt: Change Model Scale and LOD Distance
	:align: center

|

***************
Light Component
***************

To add light we now create an empty scene.

* Name the scene.
* Add transform and directional light components.

.. image:: images/graphics_tutorial/add_sun_scene.png
	:alt: Directional Light Added
	:align: center
.. image:: images/graphics_tutorial/edit_sun_components.png
	:alt: Added Components
	:align: center

|

*************
Editor Camera
*************

To have full control of movement you can use an editor camera

* Click the image icon.
* Go to editor camera.
* To move you have to hold the right mouse button and then use WASD space and shift keys to move. The cursor for direction. Space to move up and shift to move down.
.. image:: images/graphics_tutorial/change_to_editor_camera.png
	:alt: Change To Editor Camera
	:align: center

.. image:: images/graphics_tutorial/navigate_with_editor_camera.png
	:alt: Navigate With Editor Camera
	:align: center


***********
Point Light
***********

* Add an empty scene and give it a point light component.
* You can tweak its transformation value by either inputting it or dragging it left or right. 

.. image:: images/graphics_tutorial/add_point_light_texture.png
	:alt: Add Point Light
	:align: center


If you press ‘q’ a transform gizmo will appear on the object you have selected. You can adjust light location through it.
For rotation and scaling gizmo press ‘w’ and ‘e’ respectively.

.. image:: images/graphics_tutorial/translate_point_light.png
	:alt: Translate Point Light 1
	:align: center
.. image:: images/graphics_tutorial/translate_point_light_2.png
	:alt: Translate Point Light 2
	:align: center
.. image:: images/graphics_tutorial/translate_point_light_3.png
	:alt: Translate Point Light 3
	:align: center


*********************
Overriding a material
*********************

* Create a new material by going to file -> Create Resource.
* Name the material.
* Go to the `inspector-> model component->Materials`.
* click on the folder icon on the corresponding overriding material.

.. image:: images/graphics_tutorial/create_basic_material.png
	:alt: Create Basic Material
	:align: center
.. image:: images/graphics_tutorial/select_the_newly_created_basic_material.png
	:alt: Select Basic Material
	:align: center

Now you can change its basic textures by clicking on the pencil icon.

.. image:: images/graphics_tutorial/change_basic_material_texture.png
	:alt: Change Basic Material Texture
	:align: center

***************
Custom Material
***************

* Go to create resource -> Custom Material.
* Enter material name.
* Click on the folder icon and choose the material.

.. image:: images/graphics_tutorial/create_custom_material.png
	:alt: Create Custom Material
	:align: center
.. image:: images/graphics_tutorial/select_the_newly_created_custom_material.png
	:alt: Select Custom Material
	:align: center
.. image:: images/graphics_tutorial/change_custom_material_pixel_shader.png
	:alt: Change Custom Material Pixel Shader
	:align: center

***************
Adding a shader
***************

* Click on the pencil icon on the overriding custom material.
* Now you’ll get options to add vertex and pixel shaders.

You can use fire_pixel_shader from rootex/core/renderer/shaders

Clicking on the pencil icon opens an editor from which you can customise it.

.. image:: images/graphics_tutorial/changed_the_custom_material_pixel_shader_to_fire_shader.png
	:alt: Change Shader To Fire Shader
	:align: center
.. image:: images/graphics_tutorial/edit_the_default_fire_pixel_shader_2.png
	:alt: Edit Fire Pixel Shader
	:align: center

.. image:: images/graphics_tutorial/edited_the_default_fire_pixel_shader.png
	:alt: Final Fire Pixel Shader
	:align: center

***************
Decal Component
***************

To add a decal component

* Make a component DECAL and give it transform and Decal Component.
* Now go to the inspector and click DecalComponent. 
* Click on the pencil icon.
* Select a decal texture.
* Shift its position by manipulating the transform component.

By default the decal shader projects on the negative z-axis. You can rotate it till you get the desired result.


.. image:: images/graphics_tutorial/add_decal_component.png
	:alt: Add Decal Component
	:align: center
.. image:: images/graphics_tutorial/create_decal_material.png
	:alt: Create Decal Material
	:align: center
.. image:: images/graphics_tutorial/select_the_newly_created_decal_material.png
	:alt: Select Decal Material
	:align: center
.. image:: images/graphics_tutorial/change_decal_material_texture.png
	:alt: Change Decal Material
	:align: center