#include "animated_model_component.h"

#include "core/resource_loader.h"
#include "framework/entity.h"
#include "renderer/render_pass.h"
#include "framework/systems/render_system.h"
#include "renderer/materials/animated_material.h"

AnimatedModelComponent::AnimatedModelComponent(UINT renderPass, AnimatedModelResourceFile* resFile, bool isVisible)
    : m_RenderPass(renderPass)
	, m_AnimatedModelResourceFile(resFile)
    , m_IsVisible(isVisible)
    , m_CurrentAnimationName(m_AnimatedModelResourceFile->getAnimations().begin()->first)
    , m_CurrentTimePosition(0.0f)
    , m_IsPlaying(false)
    , m_TransformComponent(nullptr)
    , m_HierarchyComponent(nullptr)
{
	m_FinalTransforms.resize(m_AnimatedModelResourceFile->getBoneCount());
}

Component* AnimatedModelComponent::Create(const JSON::json& componentData)
{
	AnimatedModelComponent* animatedModelComponent = new AnimatedModelComponent(
	    componentData["renderPass"],
	    ResourceLoader::CreateAnimatedModelResourceFile(componentData["resFile"]),
	    componentData["isVisible"]);

	return animatedModelComponent;
}

Component* AnimatedModelComponent::CreateDefault()
{
	AnimatedModelComponent* animatedModelComponent = new AnimatedModelComponent(
	    UINT(RenderPass::Basic),
	    ResourceLoader::CreateAnimatedModelResourceFile("rootex/assets/gunbot.dae"),
	    true);
	
	return animatedModelComponent;
}

void AnimatedModelComponent::RegisterAPI(sol::state& rootex)
{
	sol::usertype<AnimatedModelComponent> animatedModelComponent = rootex.new_usertype<AnimatedModelComponent>(
	    "AnimatedModelComponent",
	    sol::base_classes, sol::bases<Component>());
	rootex["Entity"]["getAnimatedModel"] = &Entity::getComponent<AnimatedModelComponent>;
}

JSON::json AnimatedModelComponent::getJSON() const
{
	JSON::json j;
	
	j["resFile"] = m_AnimatedModelResourceFile->getPath().string();
	j["isVisible"] = m_IsVisible;
	j["renderPass"] = m_RenderPass;

	return j;
}

bool AnimatedModelComponent::setup()
{
	bool status = true;
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>().get();
		if (m_TransformComponent == nullptr)
		{
			WARN("Entity without transform component found.");
			status = false;
		}

		m_HierarchyComponent = m_Owner->getComponent<HierarchyComponent>().get();
		if (m_HierarchyComponent == nullptr)
		{
			WARN("Entity without hierarchy component found.");
			status = false;
		}
	}

	return status;
}

bool AnimatedModelComponent::preRender()
{
	if (m_TransformComponent)
	{
		RenderSystem::GetSingleton()->pushMatrixOverride(m_TransformComponent->getAbsoluteTransform());
	}
	else
	{
		RenderSystem::GetSingleton()->pushMatrixOverride(Matrix::Identity);
	}
	
	return true;
}

bool AnimatedModelComponent::isVisible() const
{
	return m_IsVisible;
}

void AnimatedModelComponent::render()
{
	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{
		(std::dynamic_pointer_cast<AnimatedMaterial>(material))->setVSConstantBuffer(VSAnimationConstantBuffer(m_FinalTransforms));
		RenderSystem::GetSingleton()->getRenderer()->bind(material.get());

		for (auto& mesh : meshes)
		{
			RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.m_IndexBuffer.get());
		}
	}
}

void AnimatedModelComponent::postRender()
{
	RenderSystem::GetSingleton()->popMatrix();
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void AnimatedModelComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	ImGui::BeginGroup();
	
	String path = m_AnimatedModelResourceFile->getPath().generic_string();
	ImGui::InputText("##Path", &path);
	ImGui::SameLine();
	if (ImGui::Button("Create Animated Model"))
	{
		if (!ResourceLoader::CreateAnimatedModelResourceFile(path))
		{
			WARN("Could not create Animated Model");
		}
		else
		{
			path = "";
		}
	}


	if (ImGui::Button("Model"))
	{
		EventManager::GetSingleton()->call("Open Script", "EditorOpenFile", m_AnimatedModelResourceFile->getPath().string());
	}
	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource Drop"))
		{
			const char* payloadFileName = (const char*)payload->Data;
			FilePath payloadPath(payloadFileName);
			if (IsFileSupported(payloadPath.extension().string(), ResourceFile::Type::AnimatedModel))
			{
				setAnimatedResourceFile(ResourceLoader::CreateAnimatedModelResourceFile(payloadPath.string()));
			}
			else
			{
				WARN("Unsupported file format for Model");
			}
		}
		ImGui::EndDragDropTarget();
	}

	int renderPassUI = log2(m_RenderPass);
	if (ImGui::Combo("RenderPass", &renderPassUI, "Basic\0Editor\0Alpha"))
	{
		m_RenderPass = pow(2, renderPassUI);
	}
	
	if (ImGui::Button("Start"))
	{
		m_IsPlaying = true;
	}
	ImGui::SameLine();
	ImGui::SliderFloat("", &m_CurrentTimePosition, 0.0f, m_AnimatedModelResourceFile->getAnimationEndTime(m_CurrentAnimationName));
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		m_IsPlaying = false;
	}

	if(ImGui::BeginCombo("Animation Name", m_CurrentAnimationName.c_str()))
	{
		WARN(m_CurrentAnimationName.c_str());
		for (auto& animationName : m_AnimatedModelResourceFile->getAnimationNames())
		{
			if (ImGui::Selectable(animationName.c_str()))
			{
				m_CurrentAnimationName = animationName;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::TreeNodeEx("Materials"))
	{
		int i = 0;
		for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
		{
			material->draw(std::to_string(i));

			if (ImGui::TreeNodeEx(("Meshes##" + std::to_string(i)).c_str()))
			{
				ImGui::Columns(3);

				ImGui::Text("Serial");
				ImGui::NextColumn();
				ImGui::Text("Vertices");
				ImGui::NextColumn();
				ImGui::Text("Indices");
				ImGui::NextColumn();

				int p = 0;
				for (auto& mesh : meshes)
				{
					p++;
					ImGui::Text("%d", p);
					ImGui::NextColumn();
					ImGui::Text("%d", mesh.m_VertexBuffer->getCount());
					ImGui::NextColumn();
					ImGui::Text("%d", mesh.m_IndexBuffer->getCount());
					ImGui::NextColumn();
				}

				ImGui::Columns(1);
				ImGui::TreePop();
			}
			ImGui::Separator();
			i++;
		}
		ImGui::TreePop();
	}
}
#endif //ROOTEX_EDITOR
