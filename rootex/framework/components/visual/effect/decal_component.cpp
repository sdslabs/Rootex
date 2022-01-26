#include "decal_component.h"

#include "event_manager.h"
#include "core/resource_loader.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(DecalComponent);

DecalComponent::DecalComponent(Entity& owner, const JSON::json& data)
    : ModelComponent(owner, data)
    , m_DecalMaterialResourceFile(ResourceLoader::CreateDecalMaterialResourceFile(data.value("decalResFile", "rootex/assets/materials/default.decal.rmat")))
{
	m_PerDecalCB = RenderingDevice::GetSingleton()->createBuffer<PerDecalPSCB>(PerDecalPSCB(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void DecalComponent::bindDepthTexture()
{
	ID3D11ShaderResourceView* textures[] = {
		RenderingDevice::GetSingleton()->getDepthSSRV().Get()
	};

	RenderingDevice::GetSingleton()->setPSSRV(DEPTH_TEXTURE_PS_CPP, 1, textures);
}

void DecalComponent::bindDecalPSCB()
{
	CameraComponent* renderCamera = RenderSystem::GetSingleton()->getCamera();

	const Matrix& view = renderCamera->getViewMatrix();
	const Matrix& model = getTransformComponent()->getAbsoluteTransform();
	Matrix modelView = model * view;

	Vector4 decalViewspacePosition = Vector4::Transform(Vector4(0.0f, 0.0f, 0.0f, 1.0f), modelView);

	Vector3 decalRight = Vector4::Transform(Vector4(1.0f, 0.0f, 0.0f, 1.0f), modelView) - decalViewspacePosition;
	Vector3 decalForward = Vector4::Transform(Vector4(0.0f, 0.0f, -1.0f, 1.0f), modelView) - decalViewspacePosition;
	Vector3 decalUp = Vector4::Transform(Vector4(0.0f, 1.0f, 0.0f, 1.0f), modelView) - decalViewspacePosition;
	Vector3 decalHalfScale = Vector3(decalRight.Length(), decalForward.Length(), decalUp.Length()) / 2.0f;

	decalRight.Normalize();
	decalForward.Normalize();
	decalUp.Normalize();

	PerDecalPSCB perDecal;
	perDecal.decalRight = decalRight;
	perDecal.decalForward = decalForward;
	perDecal.decalUp = decalUp;
	perDecal.decalHalfScale = decalHalfScale;
	perDecal.decalViewspacePosition = (Vector3)decalViewspacePosition;

	RenderingDevice::GetSingleton()->editBuffer(perDecal, m_PerDecalCB.Get());
	RenderingDevice::GetSingleton()->setPSCB(PER_DECAL_PS_CPP, 1, m_PerDecalCB.GetAddressOf());
}

void DecalComponent::render(float viewDistance)
{
	ZoneNamedN(decalRender, "DecalRender", true);

	RenderSystem::GetSingleton()->getRenderer()->bind(m_DecalMaterialResourceFile.get());
	bindDepthTexture();
	bindDecalPSCB();

	auto& [material, meshes] = m_ModelResourceFile->getMeshes()[0];
	auto& mesh = meshes[0];
	RenderSystem::GetSingleton()->getRenderer()->draw(mesh.m_VertexBuffer.get(), mesh.getLOD(getLODFactor(viewDistance)).get());
}

JSON::json DecalComponent::getJSON() const
{
	JSON::json j = ModelComponent::getJSON();

	j["decalResFile"] = m_DecalMaterialResourceFile->getPath().string();

	return j;
}

void DecalComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	ImGui::Text("Material");
	ImGui::Indent();
	ImGui::Image(m_DecalMaterialResourceFile->getPreview(), { 50, 50 });
	ImGui::SameLine();
	ImGui::BeginGroup();
	ImGui::Text("%s", m_DecalMaterialResourceFile->getPath().filename().generic_string().c_str());
	if (ImGui::Button((ICON_ROOTEX_PENCIL_SQUARE_O "##" + m_DecalMaterialResourceFile->getPath().generic_string()).c_str()))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_DecalMaterialResourceFile->getPath().generic_string(), (int)m_DecalMaterialResourceFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button((ICON_ROOTEX_FOLDER_OPEN "##" + m_DecalMaterialResourceFile->getPath().generic_string()).c_str()))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::DecalMaterial), "game/assets/materials/"))
		{
			m_DecalMaterialResourceFile = ResourceLoader::CreateDecalMaterialResourceFile(*result);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button((ICON_ROOTEX_REFRESH "##" + m_DecalMaterialResourceFile->getPath().generic_string()).c_str()))
	{
		m_DecalMaterialResourceFile = ResourceLoader::CreateDecalMaterialResourceFile("rootex/assets/materials/default.decal.rmat");
	}
	ImGui::EndGroup();
	ImGui::Unindent();
}
