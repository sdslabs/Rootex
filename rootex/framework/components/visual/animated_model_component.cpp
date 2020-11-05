#include "animated_model_component.h"

#include "core/resource_loader.h"
#include "framework/entity.h"
#include "renderer/render_pass.h"
#include "framework/systems/render_system.h"
#include "renderer/materials/animated_material.h"
#include "renderer/material_library.h"

AnimatedModelComponent::AnimatedModelComponent(unsigned int renderPass, const HashMap<String, String>& materialOverrides, AnimatedModelResourceFile* resFile, bool isVisible, const Vector<EntityID>& affectingStaticLightEntityIDs)
    : RenderableComponent(renderPass, materialOverrides, isVisible, affectingStaticLightEntityIDs)
    , m_CurrentTimePosition(0.0f)
    , m_IsPlaying(false)
{
	setAnimatedResourceFile(resFile, materialOverrides);
	m_CurrentAnimationName = m_AnimatedModelResourceFile->getAnimations().begin()->first;
	m_FinalTransforms.resize(m_AnimatedModelResourceFile->getBoneCount());
}

Component* AnimatedModelComponent::Create(const JSON::json& componentData)
{
	HashMap<String, String> materialOverrides;
	if (componentData.find("materialOverrides") != componentData.end())
	{
		for (auto& element : JSON::json::iterator_wrapper(componentData))
		{
			materialOverrides[element.key()] = element.value();
		}
	}
	Vector<EntityID> affectingStaticLights;
	if (componentData.find("affectingStaticLights") != componentData.end())
	{
		for (int lightEntityID : componentData["affectingStaticLights"])
		{
			affectingStaticLights.push_back(lightEntityID);
		}
	}
	AnimatedModelComponent* animatedModelComponent = new AnimatedModelComponent(
	    componentData["renderPass"],
		materialOverrides,
	    ResourceLoader::CreateAnimatedModelResourceFile(componentData["resFile"]),
	    componentData["isVisible"],
		affectingStaticLights);

	return animatedModelComponent;
}

Component* AnimatedModelComponent::CreateDefault()
{
	AnimatedModelComponent* animatedModelComponent = new AnimatedModelComponent(
	    unsigned int(RenderPass::Basic),
	    {},
	    ResourceLoader::CreateAnimatedModelResourceFile("rootex/assets/animation.dae"),
	    true,
	    {});
	
	return animatedModelComponent;
}

void AnimatedModelComponent::RegisterAPI(sol::table& rootex)
{
	sol::usertype<AnimatedModelComponent> animatedModelComponent = rootex.new_usertype<AnimatedModelComponent>(
	    "AnimatedModelComponent",
	    sol::base_classes, sol::bases<Component>());
	rootex["Entity"]["getAnimatedModel"] = &Entity::getComponent<AnimatedModelComponent>;
}

void AnimatedModelComponent::render()
{
	std::sort(m_AnimatedModelResourceFile->getMeshes().begin(), m_AnimatedModelResourceFile->getMeshes().end(), ModelComponent::compareMaterials);
	int i = 0;

	RenderableComponent::render();

	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{
		(std::dynamic_pointer_cast<AnimatedMaterial>(material))->setVSConstantBuffer(VSAnimationConstantBuffer(m_FinalTransforms));
		RenderSystem::GetSingleton()->getRenderer()->bind(m_MaterialOverrides[material].get());
		i++;

		for (auto& mesh : meshes)
		{
			RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.m_IndexBuffer.get());
		}
	}
}

void AnimatedModelComponent::setAnimatedResourceFile(AnimatedModelResourceFile* file, const HashMap<String, String>& materialOverrides)
{
	if (!file)
	{
		return;
	}
	
	m_AnimatedModelResourceFile = file;
	m_MaterialOverrides.clear();
	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{
		setMaterialOverride(material, material);
	}
	for (auto& [oldMaterial, newMaterial] : materialOverrides)
	{
		MaterialLibrary::CreateNewMaterialFile(newMaterial, MaterialLibrary::GetMaterial(oldMaterial)->getTypeName());
		setMaterialOverride(MaterialLibrary::GetMaterial(oldMaterial), MaterialLibrary::GetMaterial(newMaterial));
	}
}

JSON::json AnimatedModelComponent::getJSON() const
{
	JSON::json j = RenderableComponent::getJSON();

	j["resFile"] = m_AnimatedModelResourceFile->getPath().string();

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void AnimatedModelComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	ImGui::BeginGroup();

	String inputPath = m_AnimatedModelResourceFile->getPath().generic_string();
	ImGui::InputText("##Path", &inputPath);
	ImGui::SameLine();
	if (ImGui::Button("Create Animated Model"))
	{
		if (!ResourceLoader::CreateAnimatedModelResourceFile(inputPath))
		{
			WARN("Could not create Animated Model");
		}
		else
		{
			inputPath = "";
		}
	}

	ImGui::SameLine();

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
				setAnimatedResourceFile(ResourceLoader::CreateAnimatedModelResourceFile(payloadPath.string()), {});
			}
			else
			{
				WARN("Unsupported file format for Model");
			}
		}
		ImGui::EndDragDropTarget();
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

	if (ImGui::BeginCombo("Animation Name", m_CurrentAnimationName.c_str()))
	{
		for (auto& animationName : m_AnimatedModelResourceFile->getAnimationNames())
		{
			if (ImGui::Selectable(animationName.c_str()))
			{
				m_CurrentAnimationName = animationName;
			}
		}
		ImGui::EndCombo();
	}

	RenderableComponent::draw();

	int i = 0;
	for (auto& [material, meshes] : m_AnimatedModelResourceFile->getMeshes())
	{

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
}
#endif //ROOTEX_EDITOR
