#pragma once

enum class ComponentIDs : unsigned int
{
	Component,
	TestComponent,
	DebugComponent,

	VisualComponent,
	GridVisualComponent = VisualComponent,
	ModelVisualComponent = VisualComponent,
	TexturedVisualComponent = VisualComponent,
	CPUParticlesVisualComponent = VisualComponent,
	AnimationComponent = VisualComponent,

	Visual2DComponent = VisualComponent,
	TextVisual2DComponent = Visual2DComponent,
	
	TransformAnimationComponent,
	CameraComponent,
	TransformComponent,
	HierarchyComponent,
	PointLightComponent,
	DirectionalLightComponent,
	SpotLightComponent,
	
	PhysicsColliderComponent,
	SphereColliderComponent = PhysicsColliderComponent,
	BoxColliderComponent = PhysicsColliderComponent,
	
	ScriptComponent,
	
	AudioComponent,
	MusicComponent = AudioComponent,
	ShortMusicComponent = AudioComponent,
};
