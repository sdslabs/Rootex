#include "camera_visual_component.h"
#include "systems/render_system.h"
Component* CameraVisualComponent::Create(const JSON::json& componentData)
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { componentData["position"]["x"], componentData["position"]["y"], componentData["position"]["z"] },
	    { componentData["direction"]["x"], componentData["direction"]["y"], componentData["direction"]["z"] });

	//Ref<CameraVisualComponent> cameraPointer(cameraVisualComponent);

	//RenderSystem::GetSingleton()->setCamera(cameraPointer);

	return cameraVisualComponent;
}

Component* CameraVisualComponent::CreateDefault()
{
	CameraVisualComponent* cameraVisualComponent = new CameraVisualComponent(
	    { 0.0f, 0.0f, 4.0f },
	    { 0.0f, 0.0f, -1.0f });

	Ref<CameraVisualComponent> cameraPointer(cameraVisualComponent);

	RenderSystem::GetSingleton()->setCamera(cameraPointer);

	return cameraVisualComponent;
}

CameraVisualComponent::CameraVisualComponent(const Vector3& position,const Vector3& direction)
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt(position, position + direction, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(true)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(position)
    , m_Direction(direction)
{
	
}

CameraVisualComponent::CameraVisualComponent()
    : VisualComponent(RenderPassMain, nullptr, nullptr, false)
    , m_DebugCamera(false)
    , m_ViewMatrix(Matrix::CreateLookAt({ 0.0f, 0.0f, 0.4f }, {0.0f, 0.0f, 0.3f}, { 0.0f, 1.0f, 0.0f }))
    , m_ProjectionMatrix(Matrix::CreatePerspective(1.0f, 1.0f * 480.0f / 640.0f, 0.5f, 100.0f))
    , m_Active(true)
    , m_CameraOffset(0.0f, 1.0f, -10.0f, 0.0f)
    , m_Position(0.0f, 0.0f, 4.0f)
    , m_Direction(0.0f, 0.0f, -1.0f)
{
	
}

CameraVisualComponent::~CameraVisualComponent()
{
}

bool CameraVisualComponent::preRender()
{
	//if (m_TransformComponent)
	//{
	//	m_Position = m_TransformComponent->getPosition();
	//	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, { 0.0f, 1.0f, 0.0f });
	//}
	//else
	//{
	//	std::cout << "No transform";
	//}
	return true;
}

void CameraVisualComponent::render()
{
	if (m_DebugCamera)
	{
		//render(m_Frustum);
	}
	//Adding next thngs.
}

bool CameraVisualComponent::reset(HierarchyGraph* scene, int windowWidth, int windowHeight)
{
	// TODO: Add window resize logic that set the perspective matrix and frustum correctly

	return false;
}

bool CameraVisualComponent::isVisible() const
{
	return m_IsVisible;
}

void CameraVisualComponent::postRender()
{
}

void CameraVisualComponent::setPosition(Vector3 position)
{
	m_Position = position;
	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, { 0.0f, 1.0f, 0.0f });
}

void CameraVisualComponent::updatePosition()
{
	if (m_Owner)
	{
		m_TransformComponent = m_Owner->getComponent<TransformComponent>();
		m_Position = m_TransformComponent->getPosition();
	}
	else
	{
		std::cout << "No owner";
	}
	m_ViewMatrix = Matrix::CreateLookAt(m_Position, m_Position + m_Direction, { 0.0f, 1.0f, 0.0f });
}

void CameraVisualComponent::setViewTransform(const Matrix& view)
{
	m_ViewMatrix = view;
}


JSON::json CameraVisualComponent::getJSON() const
{
	JSON::json j;

	j["position"]["x"] = m_Position.x;
	j["position"]["y"] = m_Position.y;
	j["position"]["z"] = m_Position.z;

	j["direction"]["x"] = m_Direction.x;
	j["direction"]["y"] = m_Direction.y;
	j["direction"]["z"] = m_Direction.z;

	return j;
}

#ifdef ROOTEX_EDITOR
#include "imgui.h"
#include "imgui_stdlib.h"
void CameraVisualComponent::draw()
{
	ImGui::Checkbox("Active", &m_Active);
}
#endif // ROOTEX_EDITOR
