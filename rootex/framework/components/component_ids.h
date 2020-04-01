#pragma once

enum class ComponentIDs : unsigned int
{
	Component,
	TestComponent,
	DebugComponent,
	VisualComponent,
	CPUParticlesVisualComponent = VisualComponent,
	ModelVisualComponent = VisualComponent,
	DiffuseVisualComponent = VisualComponent,
	Visual2DComponent = VisualComponent,
	TextVisual2DComponent = Visual2DComponent,
	TransformComponent,
	HierarchyComponent,
	PointLightComponent,
	DirectionalLightComponent,
	SpotLightComponent,
	PhysicsColliderComponent,
	SphereColliderComponent,
	ScriptComponent,
	AudioComponent,
	MusicComponent = AudioComponent,
	ShortMusicComponent = AudioComponent
};
