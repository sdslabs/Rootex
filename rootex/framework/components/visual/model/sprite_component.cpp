#include "sprite_component.h"

#include "common/common.h"

#include "event_manager.h"
#include "core/resource_loader.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(SpriteComponent);

SpriteComponent::SpriteComponent(Entity& owner, const JSON::json& data)
    : RenderableComponent(owner, data)
    , m_ImageMaterialResourceFile(ResourceLoader::CreateBasicMaterialResourceFile("rootex/assets/materials/sprite.basic.rmat"))
{
	configureBoundingRectangle();
}

void SpriteComponent::configureBoundingRectangle()
{
	float m_ImageScaleFactor = 0.001;

	Ref<Texture> imageTexture = m_ImageMaterialResourceFile->getDiffuseImageFile()->getTexture();
	float rectWidth = imageTexture->getWidth() * m_ImageScaleFactor;
	float rectHeight = imageTexture->getHeight() * m_ImageScaleFactor;

	Vector<VertexData> vertices;
	VertexData vertex;
	ZeroMemory(&vertex, sizeof(VertexData));
	for (int i = 0; i < 4; i++)
	{
		int u = (i == 1 || i == 2) ? 1 : 0;
		int v = (i == 2 || i == 3) ? 1 : 0;

		vertex.position.x = u * rectWidth;
		vertex.position.y = v * rectHeight;
		vertex.position.z = 0;

		vertex.normal.x = 0;
		vertex.normal.y = 0;
		vertex.normal.z = 1;

		vertex.textureCoord.x = u;
		vertex.textureCoord.y = v;

		vertices.push_back(vertex);
	}

	Vector<unsigned short> indices = { 0, 1, 2, 2, 3, 0 };

	m_VertexBuffer.reset(new VertexBuffer((const char*)vertices.data(), vertices.size(), sizeof(VertexData), D3D11_USAGE_IMMUTABLE, 0));
	m_IndexBuffer.reset(new IndexBuffer(indices));
}

bool SpriteComponent::preRender(float deltaMilliseconds)
{
	ZoneNamedN(componentPreRender, "Sprite Pre-Render", true);
	RenderableComponent::preRender(deltaMilliseconds);
	return true;
}

void SpriteComponent::render(float viewDistance)
{
	ZoneNamedN(componentRender, "Sprite Render", true);
	RenderableComponent::render(viewDistance);

	RenderSystem::GetSingleton()->getRenderer()->bind(m_ImageMaterialResourceFile.get());
	RenderSystem::GetSingleton()->getRenderer()->draw(m_VertexBuffer.get(), m_IndexBuffer.get());
}

void SpriteComponent::setImageMaterialResourceFile(Ref<BasicMaterialResourceFile> newMaterial)
{
	if (!newMaterial)
	{
		WARN("Tried to set a null material resource file");
		return;
	}

	m_ImageMaterialResourceFile = newMaterial;
	configureBoundingRectangle();
}

JSON::json SpriteComponent::getJSON() const
{
	JSON::json j = RenderableComponent::getJSON();

	j["resFile"] = m_ImageMaterialResourceFile->getPath().string();

	return j;
}

void SpriteComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	int renderPassUI = log2(m_RenderPass);
	if (ImGui::Combo("Renderpass", &renderPassUI, "Basic\0Editor\0Alpha"))
	{
		m_RenderPass = pow(2, renderPassUI);
	}

	ImGui::Text("Material");
	ImGui::Indent();

	ImGui::Image(m_ImageMaterialResourceFile->getPreview(), { 100, 100 });
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::Text("%s", m_ImageMaterialResourceFile->getPath().filename().generic_string().c_str());
	if (ImGui::Button((ICON_ROOTEX_PENCIL_SQUARE_O "##" + m_ImageMaterialResourceFile->getPath().generic_string()).c_str()))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_ImageMaterialResourceFile->getPath().generic_string(), (int)m_ImageMaterialResourceFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button((ICON_ROOTEX_FOLDER_OPEN "##" + m_ImageMaterialResourceFile->getPath().generic_string()).c_str()))
	{
		if (Optional<String> result = OS::SelectFile("Material(*.rmat)\0*.rmat\0", "game/assets/materials/"))
		{
			setImageMaterialResourceFile(ResourceLoader::CreateBasicMaterialResourceFile(*result));
		}
	}
	ImGui::EndGroup();

	ImGui::Unindent();
}
