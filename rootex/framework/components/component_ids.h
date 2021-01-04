#pragma once

enum class ComponentIDs : unsigned int
{
	Component,
	TestComponent,
	DebugComponent,
	ModelComponent,
	GridModelComponent = ModelComponent,
	CPUParticlesComponent = ModelComponent,
	RenderUIComponent,
	TextUIComponent = RenderUIComponent,
	UIComponent,
	CameraComponent,
	TransformComponent,
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
	AudioListenerComponent,
	TransformAnimationComponent,
	SkyComponent,
	FogComponent,
	StaticPointLightComponent,
	AnimatedModelComponent,
	RenderableComponent
};
