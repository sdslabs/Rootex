#include "grid_model_component.h"

#include "framework/systems/render_system.h"

Component* GridModelComponent::Create(const JSON::json& componentData)
{
	return new GridModelComponent(
	    componentData.value("cellSize", Vector2 { 1.0f, 1.0f }),
	    componentData.value("cellCount", 100),
	    componentData.value("renderPass", (unsigned int)RenderPass::Editor),
	    componentData.value("isVisible", true));
}

GridModelComponent::GridModelComponent(const Vector2& cellSize, const int& cellCount, const unsigned int& renderPass, bool isVisible)
    : ModelComponent(renderPass, nullptr, {}, isVisible, {})
    , m_CellCount(cellCount)
    , m_CellSize(cellSize)
    , m_ColorMaterial(MaterialLibrary::GetMaterial("rootex/assets/materials/grid.rmat"))
{
}

void GridModelComponent::refreshVertexBuffers()
{
	const Vector3& origin = m_TransformComponent->getAbsoluteTransform().Translation();

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

	m_VertexBuffer.reset(new VertexBuffer(vertices));
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

void GridModelComponent::render()
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
