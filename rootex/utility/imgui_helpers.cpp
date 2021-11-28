#include "imgui_helpers.h"

#include "resource_loader.h"
#include "event_manager.h"
#include "scene.h"
#include "scene_loader.h"

void RootexFPSGraph(const char* name, Vector<float>& fpsRecords, float lastFPS)
{
	fpsRecords.erase(fpsRecords.begin());
	fpsRecords.push_back(lastFPS);

	float averageFPS = 0.0f;
	for (auto& fps : fpsRecords)
	{
		averageFPS += fps;
	}
	averageFPS /= fpsRecords.size();

	static int targetFPSChoice = 1;
	static float targetFPS[4] = { 30.0f, 60.0f, 120.0f, 144.0f };

	const Color& fpsColor = Color::Lerp((Color)ColorPresets::Red, (Color)ColorPresets::ForestGreen, (lastFPS / targetFPS[targetFPSChoice]));
	ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4 { fpsColor.x, fpsColor.y, fpsColor.z, fpsColor.w });
	ImGui::PlotLines("FPS", fpsRecords.data(), fpsRecords.size(), 0, std::to_string(averageFPS).c_str(), 0, 200.0f, ImVec2(ImGui::GetContentRegionAvailWidth(), 100));
	ImGui::PopStyleColor();

	ImGui::Combo("Target FPS", &targetFPSChoice, "30\0 60\0 120\0 144\0");
}

void RootexSelectableImage(const char* name, Ref<ImageResourceFile> image, Function<void(const String&)> onSelected)
{
	if (image)
	{
		if (ImGui::ImageButton(image->getGPUTexture()->getTextureResourceView(), { 50, 50 }))
		{
			EventManager::GetSingleton()->deferredCall(EditorEvents::EditorOpenFile, VariantVector { image->getPath().generic_string(), (int)image->getType() });
		}
		ImGui::SameLine();
	}

	ImGui::BeginGroup();
	if (ImGui::Button((name + String("##Button")).c_str()))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
		{
			onSelected(*result);
		}
	}
	if (image)
	{
		ImGui::Text(image->getPath().string().c_str());
	}
	ImGui::EndGroup();
}

void RootexSelectableImageCube(const char* name, Ref<ImageCubeResourceFile> image, Function<void(const String&)> onSelected)
{
	if (image)
	{
		if (ImGui::ImageButton(image->getTexture()->getTextureResourceView(), { 50, 50 }))
		{
			EventManager::GetSingleton()->deferredCall(EditorEvents::EditorOpenFile, VariantVector { image->getPath().generic_string(), (int)image->getType() });
		}
		ImGui::SameLine();
	}

	ImGui::BeginGroup();
	if (ImGui::Button((name + String("##Button")).c_str()))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
		{
			onSelected(*result);
		}
	}
	if (image)
	{
		ImGui::Text(image->getPath().string().c_str());
	}
	ImGui::EndGroup();
}
