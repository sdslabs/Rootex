#include "render_system.h"

#include "framework/components/visual/sky_box_helper.h"
#include "framework/components/visual/visual_component.h"
#include "framework/systems/light_system.h"
#include "core/renderer/constant_buffer.h"
#include "core/renderer/shaders/register_locations_pixel_shader.h"
#include "core/renderer/shaders/register_locations_vertex_shader.h"

RenderSystem* RenderSystem::GetSingleton()
{
	static RenderSystem singleton;
	return &singleton;
}

RenderSystem::RenderSystem()
    : m_Renderer(new Renderer())
    , m_VSProjectionConstantBuffer(nullptr)
    , m_VSPerFrameConstantBuffer(nullptr)
    , m_PSPerFrameConstantBuffer(nullptr)
    , m_IsEditorRenderPassEnabled(false)
{
	m_Camera = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<CameraComponent>().get();
	m_TransformationStack.push_back(Matrix::Identity);
	m_UITransformationStack.push_back(Matrix::Identity);
	setProjectionConstantBuffers();
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::calculateTransforms(HierarchyComponent* hierarchyComponent)
{
	pushMatrix(hierarchyComponent->getOwner()->getComponent<TransformComponent>()->getLocalTransform());

	for (auto&& child : hierarchyComponent->getChildren())
	{
		child->getOwner()->getComponent<TransformComponent>()->m_TransformBuffer.m_ParentAbsoluteTransform = getTopMatrix();
		calculateTransforms(child);
	}

	popMatrix();
}

void RenderSystem::renderPassRender(VisualComponent* vc, const RenderPass& renderPass)
{
	vc->preRender();
	if (vc->isVisible())
	{
		vc->render(renderPass);
	}
	vc->renderChildren(renderPass);
	vc->postRender();
}

void RenderSystem::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

void RenderSystem::render()
{
	Ref<HierarchyComponent> rootHC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<HierarchyComponent>();
	calculateTransforms(rootHC.get());

	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderingDevice::GetSingleton()->setCurrentRasterizerState();
	RenderingDevice::GetSingleton()->setDepthStencilState();
	RenderingDevice::GetSingleton()->setDefaultBlendState();

	perFrameVSCBBinds();
	perFramePSCBBinds();

	Ref<VisualComponent> rootVC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<VisualComponent>();

#ifdef ROOTEX_EDITOR
	if (m_IsEditorRenderPassEnabled)
	{
		renderPassRender(rootVC.get(), RenderPassEditor);
	}
#endif // ROOTEX_EDITOR
	renderPassRender(rootVC.get(), RenderPassMain);
	{
		SkyBoxHelper skyHelper;
		renderPassRender(rootVC.get(), RenderPassSky);
	}
	{
		RenderingDevice::GetSingleton()->beginDrawUI();
		renderPassRender(rootVC.get(), RenderPassUI);
		RenderingDevice::GetSingleton()->endDrawUI();
	}
}

void RenderSystem::pushMatrix(const Matrix& transform)
{
	m_TransformationStack.push_back(transform * m_TransformationStack.back());
}

void RenderSystem::pushMatrixOverride(const Matrix& transform)
{
	m_TransformationStack.push_back(transform);
}

void RenderSystem::popMatrix()
{
	m_TransformationStack.pop_back();
}

void RenderSystem::pushUIMatrix(const Matrix& transform)
{
	m_UITransformationStack.push_back(transform * m_TransformationStack.back());
}

void RenderSystem::popUIMatrix()
{
	m_UITransformationStack.pop_back();
}

void RenderSystem::enableWireframeRasterizer()
{
	RenderingDevice::GetSingleton()->setRasterizerState(RenderingDevice::RasterizerState::Wireframe);
}

void RenderSystem::resetDefaultRasterizer()
{
	RenderingDevice::GetSingleton()->setRasterizerState(RenderingDevice::RasterizerState::Default);
}

void RenderSystem::setProjectionConstantBuffers()
{
	const Matrix& projection = getCamera()->getProjectionMatrix();
	Material::setVSConstantBuffer(projection.Transpose(), m_VSProjectionConstantBuffer, PER_CAMERA_CHANGE_VS_CPP);
}

void RenderSystem::perFrameVSCBBinds()
{
	const Matrix& view = getCamera()->getViewMatrix();
	Material::setVSConstantBuffer(view.Transpose(), m_VSPerFrameConstantBuffer, PER_FRAME_VS_CPP);
}

void RenderSystem::perFramePSCBBinds()
{
	const PSDiffuseConstantBufferLights& lights = LightSystem::GetSingleton()->getLights();
	Material::setPSConstantBuffer(lights, m_PSPerFrameConstantBuffer, PER_FRAME_PS_CPP);
}

void RenderSystem::enableLineRenderMode()
{
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void RenderSystem::resetRenderMode()
{
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderSystem::setCamera(CameraComponent* camera)
{
	m_Camera = camera;
	if (m_Camera)
	{
		setProjectionConstantBuffers();
	}
}

void RenderSystem::restoreCamera()
{
	setCamera(HierarchySystem::GetSingleton()->getRootEntity()->getComponent<CameraComponent>().get());
}

const Matrix& RenderSystem::getTopMatrix() const
{
	return m_TransformationStack.back();
}

Matrix& RenderSystem::getTopUIMatrix()
{
	return m_UITransformationStack.back();
}
