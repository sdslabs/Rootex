#pragma once

enum class ComponentIDs : unsigned int
{
	Component,
	TestComponent,
	DebugComponent,
	VisualComponent,
	ModelVisualComponent = VisualComponent,
	TexturedVisualComponent = VisualComponent,
	Visual2DComponent = VisualComponent,
	TextVisual2DComponent = Visual2DComponent,
	TransformComponent,
	HierarchyComponent,
	PointLightComponent,
	DirectionalLightComponent,
	SpotLightComponent,
	PhysicsComponent,
	SphereComponent,
	ScriptComponent,
	AudioComponent,
	MusicComponent = AudioComponent,
	ShortMusicComponent = AudioComponent
};
