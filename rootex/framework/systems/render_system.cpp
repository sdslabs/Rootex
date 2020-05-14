#include "render_system.h"

#include "renderer/shaders/register_locations_vertex_shader.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "light_system.h"

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
	setProjectionConstantBuffers();
}

void RenderSystem::calculateTransforms(HierarchyComponent* hierarchyComponent)
{
	pushMatrix(hierarchyComponent->getOwner()->getComponent<TransformComponent>()->getLocalTransform());
	for (auto&& child : hierarchyComponent->getChildren())
	{
		child->getOwner()->getComponent<TransformComponent>()->m_TransformBuffer.m_ParentAbsoluteTransform = getCurrentMatrix();
		calculateTransforms(child);
	}
	popMatrix();
}

void RenderSystem::renderPassRender(RenderPass renderPass)
{
	ModelComponent* mc = nullptr;
	for (auto& component : s_Components[ModelComponent::s_ID])
	{
		mc = (ModelComponent*)component;
		if (mc->getRenderPass() & (unsigned int)renderPass)
		{
			mc->preRender();
			if (mc->isVisible())
			{
				mc->render();
			}
			mc->postRender();
		}
	}
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

#ifdef ROOTEX_EDITOR
	if (m_IsEditorRenderPassEnabled)
	{
		renderPassRender(RenderPass::Editor);
	}
#endif // ROOTEX_EDITOR
	renderPassRender(RenderPass::Basic);
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

const Matrix& RenderSystem::getCurrentMatrix() const
{
	return m_TransformationStack.back();
}
