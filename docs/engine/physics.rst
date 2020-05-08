Physics
=======

Physics in Rootex has been implemented using the Bullet Collision Detection and Physics library (https://pybullet.org/Bullet/BulletFull/index.html)

Rootex uses the concept of physics colliders, adopted from the Bullet Physics library, to represent objects responding to physics and being controlled by it.

:ref:`Class PhysicsSystem` is responsible for providing physics to the Rootex engine. The physics system uses :ref:`Class PhysicsColliderComponent` instances to perform physics based calculations on them. Rootex engine currently supports all the collision shapes provides by Bullet. There are also extra features available like ray casting a ray into the world and reporting the colliders that the ray touched.

The Rootex Editor also exposes a functionality that displays all the colliders present in the world.
