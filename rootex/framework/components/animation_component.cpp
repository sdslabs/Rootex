#include "animation_component.h"

Component* AnimationComponent::Create(const JSON::json& componentData)
{
	return nullptr;
}

Component* AnimationComponent::CreateDefault()
{
	return nullptr;
}

AnimationComponent::AnimationComponent(SkeletalAnimationResourceFile* animationFile)
    : m_AnimationFile(animationFile)
{
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AnimationComponent::draw()
{
}
#endif // ROOTEX_EDITOR
