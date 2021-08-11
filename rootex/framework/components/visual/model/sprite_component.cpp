#include "sprite_component.h"

#include "common/common.h"

#include "event_manager.h"
#include "core/resource_loader.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(SpriteComponent);

SpriteComponent::SpriteComponent(Entity& owner, const JSON::json& data)
    : RenderableComponent(owner, data)
    , m_ImageResourceFile(ResourceLoader::CreateImageResourceFile(data.value("resFile", "rootex/assets/white.png")))
    , m_ImageMaterial(ResourceLoader::CreateBasicMaterialResourceFile("rootex/assets/materials/default.basic.rmat"))
{
	configureImageMaterial();
	configureBoundingRectangle();
}

void SpriteComponent::configureImageMaterial()
{
	m_ImageMaterial->setDiffuse(m_ImageResourceFile);
}

void SpriteComponent::configureBoundingRectangle()
{
	float m_ImageScaleFactor = 0.001;

	Ref<Texture> imageTexture = m_ImageResourceFile->getTexture();
	float rectWidth = imageTexture->getWidth() * m_ImageScaleFactor;
	float rectHeight = imageTexture->getHeight() * m_ImageScaleFactor;

	PRINT("Rect Width: " + std::to_string(rectWidth));
	PRINT("Rect Height: " + std::to_string(rectHeight));

	Vector<VertexData> vertices;
	VertexData vertex;
	ZeroMemory(&vertex, sizeof(VertexData));
	for (int i = 0; i < 4; i++)
	{
		int u = (i == 1 || i == 2) ? 1 : 0;
		int v = (i == 2 || i == 3) ? 1 : 0; 

		vertex.position.x = u * rectWidth / 2;
		vertex.position.y = v * rectHeight / 2;
		vertex.position.z = 0;

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

	RenderSystem::GetSingleton()->getRenderer()->bind(m_ImageMaterial.get());
	RenderSystem::GetSingleton()->getRenderer()->draw(m_VertexBuffer.get(), m_IndexBuffer.get());
}

void SpriteComponent::setImageResourceFile(Ref<ImageResourceFile> newImage)
{
	if (!newImage)
	{
		WARN("Tried to set a null sprite image resource file");
		return;
	}

	m_ImageResourceFile = newImage;
	configureImageMaterial();
	configureBoundingRectangle();
}

JSON::json SpriteComponent::getJSON() const
{
	JSON::json j = RenderableComponent::getJSON();

	j["resFile"] = m_ImageResourceFile->getPath().string();

	return j;
}

void SpriteComponent::draw()
{
	ImGui::Checkbox("Visible", &m_IsVisible);

	String filePath = m_ImageResourceFile->getPath().generic_string();
	ImGui::Text("%s", filePath.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Image"))
	{
		EventManager::GetSingleton()->call(EditorEvents::EditorOpenFile, VariantVector { m_ImageResourceFile->getPath().generic_string(), (int)m_ImageResourceFile->getType() });
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_ROOTEX_FOLDER_OPEN "##Image File"))
	{
		if (Optional<String> result = OS::SelectFile(SupportedFiles.at(ResourceFile::Type::Image), "game/assets/"))
		{
			setImageResourceFile(ResourceLoader::CreateImageResourceFile(*result));
		}
	}

	RenderableComponent::draw();
}
