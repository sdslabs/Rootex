#include "grid_model_component.h"

#include "resource_loader.h"
#include "framework/systems/render_system.h"

DEFINE_COMPONENT(GridModelComponent);

GridModelComponent::GridModelComponent(Entity& owner, const JSON::json& data)
    : ModelComponent(owner, data)
    , m_CellCount(data.value("cellCount", 100))
    , m_CellSize(data.value("cellSize", Vector2 { 1.0f, 1.0f }))
    , m_ColorMaterial(ResourceLoader::CreateBasicMaterialResourceFile("rootex/assets/materials/grid.basic.rmat"))
{
}

void GridModelComponent::refreshVertexBuffers()
{
	const Vector3& origin = getTransformComponent()->getAbsoluteTransform().Translation();

	Vector<float> vertices;
	Vector<unsigned short> indices;

	// Push x = k && y = k lines
	for (int i = -m_CellCount / 2.0f; i <= m_CellCount / 2.0f; i++)
	{
		float x = origin.x + (i * m_CellSize.x);
		float y = origin.y;

		{
			float zDeep = origin.z + (m_CellCount * m_CellSize.y / 2.0f);
			// Push line's deep end
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(zDeep);
		}
		{
			float zShallow = origin.z - (m_CellCount * m_CellSize.y / 2.0f);
			// Push line's shallow end
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(zShallow);
		}
	}

	// Push z = k && y = k lines
	for (int i = -m_CellCount / 2.0f; i <= m_CellCount / 2.0f; i++)
	{
		float y = origin.y;
		float z = origin.z + (i * m_CellSize.y);

		{
			float xLeft = origin.x - (m_CellCount * m_CellSize.x / 2.0f);
			// Push line's left end
			vertices.push_back(xLeft);
			vertices.push_back(y);
			vertices.push_back(z);
		}
		{
			float xRight = origin.x + (m_CellCount * m_CellSize.x / 2.0f);
			// Push line's right end
			vertices.push_back(xRight);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	// Y axis line
	{
		vertices.push_back(0.0f);
		vertices.push_back(10000.0f);
		vertices.push_back(0.0f);
	}
	{
		vertices.push_back(0.0f);
		vertices.push_back(-10000.0f);
		vertices.push_back(0.0f);
	}

	// +6 = 4 boundary vertices + the 2 vertices on the Y axis line
	for (int i = 0; i < 4 * m_CellCount + 6; i++)
	{
		indices.push_back(i);
	}

	m_VertexBuffer.reset(new VertexBuffer((const char*)vertices.data(), vertices.size() / 3, sizeof(float) * 3, D3D11_USAGE_IMMUTABLE, 0));
	m_IndexBuffer.reset(new IndexBuffer(indices));
}

bool GridModelComponent::setupData()
{
	bool status = ModelComponent::setupData();

	if (status)
	{
		refreshVertexBuffers();
	}

	return status;
}

void GridModelComponent::render(float viewDistance)
{
	ZoneNamedN(componentRender, "Grid Render", true);

	RenderSystem::GetSingleton()->enableLineRenderMode();
	RenderSystem::GetSingleton()->getRenderer()->bind(m_ColorMaterial.get());
	RenderSystem::GetSingleton()->getRenderer()->draw(m_VertexBuffer.get(), m_IndexBuffer.get());
	RenderSystem::GetSingleton()->resetRenderMode();
}

JSON::json GridModelComponent::getJSON() const
{
	JSON::json& j = ModelComponent::getJSON();

	j["cellSize"] = m_CellSize;
	j["cellCount"] = m_CellCount;

	return j;
}

void GridModelComponent::draw()
{
	m_ColorMaterial->draw();

	if (ImGui::InputInt("Cell Count", &m_CellCount))
	{
		refreshVertexBuffers();
	}
	if (ImGui::InputFloat2("Cell Size", &m_CellSize.x))
	{
		refreshVertexBuffers();
	}
}
