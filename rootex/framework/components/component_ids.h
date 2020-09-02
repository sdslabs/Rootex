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
	AudioListenerComponent,
	TransformAnimationComponent,
	TriggerComponent,
	SkyComponent,
	FogComponent
};
