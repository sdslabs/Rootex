#pragma once

enum class ComponentIDs : unsigned int
{
	Component,
	TestComponent,
	DebugComponent,
	VisualComponent,
	DiffuseVisualComponent = VisualComponent,
	TransformComponent,
	HierarchyComponent,
	RootHierarchyComponent = HierarchyComponent,
	PointLightComponent,
	DirectionalLightComponent,
	SpotLightComponent,
	PhysicsComponent,
	SphereComponent
};
