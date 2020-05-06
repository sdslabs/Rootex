#include "animation_component.h"

#include "resource_loader.h"
#include "core/renderer/material_library.h"
#include "framework/systems/render_system.h"

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
		ResourceLoader::CreateSkeletalAnimationResourceFile("rootex/assets/models/character.dae"));
}

AnimationComponent::AnimationComponent(RenderPass renderPass, bool isVisible, SkeletalAnimationResourceFile* animationFile)
    : VisualComponent(renderPass, isVisible)
	, m_AnimationFile(animationFile)
    , m_Material(std::dynamic_pointer_cast<AnimationMaterial>(MaterialLibrary::GetMaterial("animation_material.rmat")))
    , m_CurrentTimePosition(0)
{
	m_FinalTransforms.resize(m_AnimationFile->getBoneCount(), Matrix::Identity);
}

bool AnimationComponent::preRender()
{
	if (m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrix(getTransform());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrix(Matrix::Identity);
	}
	return true;
}

void AnimationComponent::render(RenderPass renderPass)
{
	if (renderPass & m_RenderPass)
	{
		m_Material->setVSConstantBuffer(m_FinalTransforms);
		RenderSystem::GetSingleton()->getRenderer()->draw(m_AnimationFile->getVertexBuffer(), m_AnimationFile->getIndexBuffer(), m_Material.get());
	}
}

void AnimationComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

JSON::json AnimationComponent::getJSON() const
{
	JSON::json& j = VisualComponent::getJSON();

	j["animationFile"] = m_AnimationFile->getPath().generic_string();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void AnimationComponent::draw()
{
	VisualComponent::draw();

	ImGui::InputFloat("Time", &m_CurrentTimePosition);

	String m_AnimationFileUI = m_AnimationFile->getPath().string();
	if(ImGui::InputText("Animation", &m_AnimationFileUI, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		SkeletalAnimationResourceFile* anim = ResourceLoader::CreateSkeletalAnimationResourceFile(m_AnimationFileUI);
		if (anim)
		{
			setAnimationFile(anim);
		}
		else
		{
			WARN("Could not assign animation file");
		}
	}

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
