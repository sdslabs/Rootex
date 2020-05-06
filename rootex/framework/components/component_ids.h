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
	Visual2DComponent = VisualComponent,
	TextVisual2DComponent = Visual2DComponent,
	AnimationComponent,
	
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
