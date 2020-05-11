#include "grid_model_component.h"

#include "framework/systems/render_system.h"

Component* GridModelComponent::Create(const JSON::json& componentData)
{
	return new GridModelComponent(
	    { 
			componentData["cellSize"]["x"],
	        componentData["cellSize"]["y"]
		}, 
		componentData["cellCount"],
	    { 
			componentData["color"]["r"],
	        componentData["color"]["g"],
	        componentData["color"]["b"],
	        componentData["color"]["a"],
		},
		componentData["renderPass"],
	    componentData["isVisible"]);
}

Component* GridModelComponent::CreateDefault()
{
	return new GridModelComponent(
	    { 1.0f, 1.0f },
		100,
		Color(ColorPresets::DarkGray),
		(unsigned int)RenderPass::Editor,
	    true);
}

GridModelComponent::GridModelComponent(const Vector2& cellSize, const int& cellCount, const Color& gridColor, const unsigned int& renderPass, bool isVisible)
    : ModelComponent(renderPass, MaterialLibrary::GetDefaultMaterial(), nullptr, isVisible)
    , m_CellCount(cellCount)
    , m_CellSize(cellSize)
    , m_ColorMaterial(MaterialLibrary::GetDefaultMaterial())
    , m_GridColor(gridColor)
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

bool GridModelComponent::setup()
{
	bool status = ModelComponent::setup();

	if (status)
	{
		refreshVertexBuffers();
	}

	return status;
}

void GridModelComponent::render()
{
	RenderSystem::GetSingleton()->enableLineRenderMode();
	RenderSystem::GetSingleton()->getRenderer()->draw(m_VertexBuffer.get(), m_IndexBuffer.get(), m_ColorMaterial.get());
	RenderSystem::GetSingleton()->resetRenderMode();
}

JSON::json GridModelComponent::getJSON() const
{
	JSON::json& j = ModelComponent::getJSON();

	j["cellSize"]["x"] = m_CellSize.x;
	j["cellSize"]["y"] = m_CellSize.y;
	j["cellCount"] = m_CellCount;
	j["color"]["r"] = m_GridColor.x;
	j["color"]["g"] = m_GridColor.y;
	j["color"]["b"] = m_GridColor.z;
	j["color"]["a"] = m_GridColor.w;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void GridModelComponent::draw()
{
	ModelComponent::draw();

	if (ImGui::InputInt("Cell Count", &m_CellCount))
	{
		refreshVertexBuffers();
	}
	if (ImGui::InputFloat2("Cell Size", &m_CellSize.x))
	{
		refreshVertexBuffers();
	}

	ImGui::ColorEdit4("Grid Color", &m_GridColor.x);
}
#endif // ROOTEX_EDITOR
