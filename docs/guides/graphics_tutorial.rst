=============================================
Exploring the Graphical capabilities of Rootex
=============================================

This documentation aims to showcase the graphical capabilities of rootex and act as a tutorial for beginners to get started.

Let's start by creating a scene.

Create a scene
***************

To Create a Scene 

1) Go to `file->CreateScene`.

2) Name the scene and click create.

.. image:: images/graphics_tutorial/create_scene.png
	:alt: Create Scene
	:align: center

|

Now we Create an Empty Scene. An empty scene is nothing but objects. You can have different components in it, more on that later.

Create Empty scene
******************

To create an empty scene.

1) Right-click the root scene.
2) Click Add Empty Scene

.. image:: images/graphics_tutorial/add_empty_scene.png
	:alt: Create Empty Scene
	:align: center

.. image:: images/graphics_tutorial/empty_scene_created.png
	:alt: Empty Scene Created
	:align: center

|

*****************
Giving Components
*****************

Now we give components to the empty scene. 

1) Right-click the empty scene.
2) Click Edit Components.
3) Check the appropriate components, in this case, transform and Model. **Note**: Transform Component is a must.

.. image:: images/graphics_tutorial/add_model_component_and_transform_component.png
	:alt: Components Added
	:align: center
	
4) Open inspector.
5) Go to the model component in the inspector.
6) Click the folder icon next to Model.

.. image:: images/graphics_tutorial/select_model_file.png
	:alt: Selecting File
	:align: center

|


For sponza initially, it would look like this:

.. image:: images/graphics_tutorial/model_file_just_added.png
	:alt: Model File Just Added
	:align: center

|

This is due to the default settings of the sponza obj file. To get a better view, set the scale to (0.031, 0.031, 0.031) and set the LOD distance to 123:

.. image:: images/graphics_tutorial/change_model_scale.png
	:alt: Change Model Scale and LOD Distance
	:align: center

|

We need to create an empty scene and add a light component to it to add light.

***************
Light Component
***************

To add light, we now create an empty scene.

1) Name the scene.
2) Add transform and directional light components.

.. image:: images/graphics_tutorial/add_sun_scene.png
	:alt: Directional Light Added
	:align: center
.. image:: images/graphics_tutorial/edit_sun_components.png
	:alt: Added Components
	:align: center

|

To move freely, we can change our camera mode to Editor Camera. This allows us to move freely.

*************
Editor Camera
*************

To have complete control of movement, you can use an editor camera.

1) Click the image icon at the top left of the viewport.
2) Open dropdown of camera.
3) Select editor camera.

.. image:: images/graphics_tutorial/change_to_editor_camera.png
	:alt: Change To Editor Camera
	:align: center
	
To move, you have to hold the right mouse button and then use WASD space and shift keys to move. The cursor for direction. Space to move up and shift to move down.

.. image:: images/graphics_tutorial/navigate_with_editor_camera.png
	:alt: Navigate With Editor Camera
	:align: center
	


***********
Point Light
***********

A point light is helpful if you have a source of light, e.g. a candle, bulb etc. To add a point light, follow the given steps.

1) Add an empty scene and give it a point light component.
2) You can tweak its transformation value by either inputting it or dragging it left or right. 

.. image:: images/graphics_tutorial/add_point_light_texture.png
	:alt: Add Point Light
	:align: center


If you press 'q', a transform gizmo will appear on the object you have selected. You can adjust light location through it.
For rotation and scaling gizmo, press 'w' and 'e', respectively.

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

To change the properties of one object without changing the original material, we can use overriding materials. To override a material:

1) Create a new basic material by going to file -> Create Resource.
2) Name the material and click create.

.. image:: images/graphics_tutorial/create_basic_material.png
	:alt: Create Basic Material
	:align: center

3) Go to the `inspector-> model component->Materials`.
4) click on the folder icon on the corresponding overriding material.

.. image:: images/graphics_tutorial/select_the_newly_created_basic_material.png
	:alt: Select Basic Material
	:align: center

Now you can change its basic textures by 
1)clicking on the pencil icon 
2)In the file viewer now click on the diffuse texture and select the appropriate diffuse texture.

.. image:: images/graphics_tutorial/change_basic_material_texture.png
	:alt: Change Basic Material Texture
	:align: center

***************
Custom Material
***************



1) Go to create Resource -> Custom Material.

.. image:: images/graphics_tutorial/create_custom_material.png
	:alt: Create Custom Material
	:align: center

2) Enter material name.
3) Now go to Inspector -> ModelComponent and then to Materials.
4) Click on the folder icon and choose the material.

.. image:: images/graphics_tutorial/select_the_newly_created_custom_material.png
	:alt: Select Custom Material
	:align: center

***************
Adding a shader
***************

To Add shader:

1) Click on the pencil icon on the overriding custom material.
2) Now, in the file viewer you'll get options to add vertex and pixel shaders.
3) Click on the pixel shader. A dialog box will open now you can just select the shader.

.. image:: images/graphics_tutorial/change_custom_material_pixel_shader.png
	:alt: Change Custom Material Pixel Shader
	:align: center

You can use fire_pixel_shader from rootex/core/renderer/shaders

Clicking on the pencil icon opens an editor to customise the shader.

.. note:: 
	You can only add shaders to custom materials. If you want to use default material, override the original default material with custom material and then add a shader to the overriding material. The overriding material does inherit the textures of the original materials.

.. image:: images/graphics_tutorial/changed_the_custom_material_pixel_shader_to_fire_shader.png
	:alt: Change Shader To Fire Shader
	:align: center
.. image:: images/graphics_tutorial/edit_the_default_fire_pixel_shader_2.png
	:alt: Edit Fire Pixel Shader
	:align: center
.. image:: images/graphics_tutorial/edited_the_default_fire_pixel_shader.png
	:alt: Final Fire Pixel Shader
	:align: center
.. image:: images/graphics_tutorial/fire_shader_sample.gif
	:alt: Fire Shader Sample
	:align: center


***************
Decal Component
***************

To add a decal component. 

1) Make a scene DECAL and give it transform and Decal Component.

.. image:: images/graphics_tutorial/add_decal_component.png
	:alt: Add Decal Component
	:align: center
2) Create a decal material. By going to File -> CreateResource. And then slect Decal material in resource type dropdown.

.. image:: images/graphics_tutorial/create_decal_material.png
	:alt: Create Decal Material
	:align: center
	
3) Now go to the inspector and click DecalComponent. 
4) Click on the folder icon and select the decal material.

.. image:: images/graphics_tutorial/select_the_newly_created_decal_material.png
	:alt: Select Decal Material
	:align: center
	
5) Click on the pencil icon and the in the file viewer click on Decal Texture.

.. image:: images/graphics_tutorial/change_decal_material_texture.png
	:alt: Change Decal Material
	:align: center
	
6) Shift its position by manipulating the transform component.

By default, the decal shader projects on the negative z-axis. You can rotate it till you get the desired result.




.. image:: images/graphics_tutorial/decal_sample.gif
	:alt: Decal Sample
	:align: center

