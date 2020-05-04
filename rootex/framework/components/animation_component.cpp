#include "animation_component.h"

#include "resource_loader.h"

Component* AnimationComponent::Create(const JSON::json& componentData)
{
	return new AnimationComponent(
	    componentData["renderPass"],
	    componentData["isVisible"],
		ResourceLoader::CreateSkeletalAnimationResourceFile(componentData["animationFile"]));
}

Component* AnimationComponent::CreateDefault()
{
	return new AnimationComponent(
		RenderPassMain,
		true,
		ResourceLoader::CreateSkeletalAnimationResourceFile("rootex/assets/gun/gun.dae"));
}

AnimationComponent::AnimationComponent(RenderPass renderPass, bool isVisible, SkeletalAnimationResourceFile* animationFile)
    : VisualComponent(renderPass, isVisible)
	, m_AnimationFile(animationFile)
{
}

JSON::json AnimationComponent::getJSON() const
{
	JSON::json j;

	j["animationFile"] = m_AnimationFile->getPath().generic_string();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void AnimationComponent::draw()
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsSupported(payloadPath.extension().string(), SupportedAnimatedModelExtensions))
			{
				setAnimationFile(ResourceLoader::CreateSkeletalAnimationResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Cannot assign a non-animation file to animation File");
			}
		}
		ImGui::EndDragDropTarget();
	}
}
#endif // ROOTEX_EDITOR
