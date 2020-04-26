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
		componentData["renderPass"],
	    componentData["isVisible"]);
}

Component* GridVisualComponent::CreateDefault()
{
	return new GridVisualComponent(
	    { 1.0f, 1.0f },
		100l,
		RenderPass::RenderPassEditor,
	    true);
}

GridVisualComponent::GridVisualComponent(const Vector2& cellSize, const long& cellCount, const unsigned int& renderPass, bool isVisible)
    : VisualComponent(renderPass, isVisible)
    , m_CellCount(cellCount)
    , m_CellSize(cellSize)
    , m_GridMaterial(new GridMaterial())
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
		float x = origin.x + i * m_CellSize.x;
		float y = origin.y;
		float zDeep = origin.z + m_CellCount * m_CellSize.y;
		float zShallow = origin.z - m_CellCount * m_CellSize.y;

		{
			// Push line's deep end
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(zDeep);
			indices.push_back(i);
		}
		{
			// Push line's shallow end
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(zShallow);
			indices.push_back(i + 1);
		}
	}

	// Push z = k && y = k lines
	for (int i = 0; i < m_CellCount; i++)
	{
		float xRight = origin.x + m_CellCount * m_CellSize.x;
		float xLeft = origin.x - m_CellCount * m_CellSize.x;
		float y = origin.y;
		float z = origin.z + i * m_CellSize.y;

		int lastVertexIndex = vertices.size() - 1;
		{
			// Push line's left end
			vertices.push_back(xLeft);
			vertices.push_back(y);
			vertices.push_back(z);
			indices.push_back(lastVertexIndex + 1);
		}
		lastVertexIndex++;
		{
			// Push line's right end
			vertices.push_back(xRight);
			vertices.push_back(y);
			vertices.push_back(z);
			indices.push_back(lastVertexIndex + 1);
		}
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
		RenderSystem::GetSingleton()->getRenderer()->draw(m_VertexBuffer.get(), m_IndexBuffer.get(), m_GridMaterial.get());
		RenderSystem::GetSingleton()->resetRenderMode();
	}
}

JSON::json GridVisualComponent::getJSON() const
{
	JSON::json& j = VisualComponent::getJSON();

	j["cellSize"]["x"] = m_CellSize.x;
	j["cellSize"]["y"] = m_CellSize.y;
	j["cellCount"] = m_CellCount;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void GridVisualComponent::draw()
{
	VisualComponent::draw();

	ImGui::InputInt("Cell Count", &m_CellCount);
	ImGui::InputFloat2("Cell Size", &m_CellSize.x);
}
#endif // ROOTEX_EDITOR
