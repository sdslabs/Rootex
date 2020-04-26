#include "grid_visual_component.h"

#include "framework/systems/render_system.h"

Component* GridVisualComponent::Create(const JSON::json& componentData)
{
	return new GridVisualComponent(
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

Component* GridVisualComponent::CreateDefault()
{
	return new GridVisualComponent(
	    { 1.0f, 1.0f },
		100,
		Color(ColorPresets::LightGray),
		RenderPass::RenderPassEditor,
	    true);
}

GridVisualComponent::GridVisualComponent(const Vector2& cellSize, const int& cellCount, const Color& gridColor, const unsigned int& renderPass, bool isVisible)
    : VisualComponent(renderPass, isVisible)
    , m_CellCount(cellCount)
    , m_CellSize(cellSize)
    , m_ColorMaterial(new ColorMaterial())
    , m_GridColor(gridColor)
{
}

void GridVisualComponent::refreshVertexBuffers()
{
	const Vector3& origin = m_TransformComponent->getAbsoluteTransform().Translation();

	Vector<float> vertices;
	Vector<unsigned short> indices;

	// Push x = k && y = k lines
	for (int i = 0; i < m_CellCount; i++)
	{
		float x = origin.x + (i * m_CellSize.x);
		float y = origin.y;
		float zDeep = origin.z + (m_CellCount * m_CellSize.y);
		float zShallow = origin.z - (m_CellCount * m_CellSize.y);

		{
			// Push line's deep end
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(zDeep);
		}
		{
			// Push line's shallow end
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(zShallow);
		}
		
		indices.push_back(i);
	}

	int totalXIndices = indices.size();
	
	// Push z = k && y = k lines
	for (int i = 0; i < m_CellCount; i++)
	{
		float xRight = origin.x + (m_CellCount * m_CellSize.x);
		float xLeft = origin.x - (m_CellCount * m_CellSize.x);
		float y = origin.y;
		float z = origin.z + (i * m_CellSize.y);

		{
			// Push line's left end
			vertices.push_back(xLeft);
			vertices.push_back(y);
			vertices.push_back(z);
		}
		totalXIndices++;
		{
			// Push line's right end
			vertices.push_back(xRight);
			vertices.push_back(y);
			vertices.push_back(z);
		}
			
		indices.push_back(totalXIndices + i);
	}

	m_VertexBuffer.reset(new VertexBuffer(vertices));
	m_IndexBuffer.reset(new IndexBuffer(indices));
}

bool GridVisualComponent::setup()
{
	bool status = VisualComponent::setup();

	if (status)
	{
		refreshVertexBuffers();
	}

	return status;
}

void GridVisualComponent::render(RenderPass renderPass)
{
	if (renderPass & m_RenderPass)
	{
		RenderSystem::GetSingleton()->enableLineRenderMode();
		m_ColorMaterial->setPSConstantBuffer({ m_GridColor });
		RenderSystem::GetSingleton()->getRenderer()->draw(m_VertexBuffer.get(), m_IndexBuffer.get(), m_ColorMaterial.get());
		RenderSystem::GetSingleton()->resetRenderMode();
	}
}

JSON::json GridVisualComponent::getJSON() const
{
	JSON::json& j = VisualComponent::getJSON();

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
void GridVisualComponent::draw()
{
	VisualComponent::draw();

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
