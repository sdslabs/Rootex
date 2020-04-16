#include "render_system.h"

#include "framework/components/visual/sky_box_helper.h"
#include "framework/components/visual/visual_component.h"

RenderSystem* RenderSystem::GetSingleton()
{
	static RenderSystem singleton;
	return &singleton;
}

RenderSystem::RenderSystem()
    : m_Renderer(new Renderer())
    , m_Camera(new CameraVisualComponent())
    , m_VSProjectionConstantBuffer(nullptr)
    , m_VSViewConstantBuffer(nullptr)
{
	m_TransformationStack.push_back(Matrix::Identity);
	m_UITransformationStack.push_back(Matrix::Identity);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::calculateTransforms(HierarchyComponent* hierarchyComponent)
{
	pushMatrix(hierarchyComponent->getOwner()->getComponent<TransformComponent>()->getLocalTransform());

	for (auto&& child : hierarchyComponent->getChildren())
	{
		child->getOwner()->getComponent<TransformComponent>()->m_TransformBuffer.m_AbsoluteTransform = getTopMatrix();
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
	RenderingDevice::GetSingleton()->setRasterizerState();
	RenderingDevice::GetSingleton()->setDepthStencilState();

	setViewProjectionConstantBuffers();

	Ref<VisualComponent> rootVC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<VisualComponent>();
#ifdef ROOTEX_EDITOR
	renderPassRender(rootVC.get(), RenderPassEditor);
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

void RenderSystem::setViewProjectionConstantBuffers()
{
	const Matrix& view = getCamera()->getView();
	const Matrix& projection = getCamera()->getProjection();
	if (m_VSProjectionConstantBuffer == nullptr || m_VSViewConstantBuffer == nullptr)
	{
		D3D11_BUFFER_DESC cbd = { 0 };
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(view);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = { 0 };
		csd.pSysMem = &view.Transpose();

		m_VSViewConstantBuffer = RenderingDevice::GetSingleton()->createVSViewConstantBuffer(&cbd, &csd);

		cbd.ByteWidth = sizeof(projection);
		csd.pSysMem = &projection.Transpose();

		m_VSProjectionConstantBuffer = RenderingDevice::GetSingleton()->createVSProjectionConstantBuffer(&cbd, &csd);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE subresource = { 0 };
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_VSViewConstantBuffer.Get(), subresource);
		memcpy(subresource.pData, &view.Transpose(), sizeof(view));
		RenderingDevice::GetSingleton()->unmapBuffer(m_VSViewConstantBuffer.Get());

		subresource = { 0 };
		RenderingDevice::GetSingleton()->getBufferMappedContext(m_VSProjectionConstantBuffer.Get(), subresource);
		memcpy(subresource.pData, &projection.Transpose(), sizeof(projection));
		RenderingDevice::GetSingleton()->unmapBuffer(m_VSProjectionConstantBuffer.Get());
	}
}

void RenderSystem::setCamera(Ref<CameraVisualComponent> camera)
{
	m_Camera = camera;
}

const Matrix& RenderSystem::getTopMatrix() const
{
	return m_TransformationStack.back();
}

Matrix& RenderSystem::getTopUIMatrix()
{
	return m_UITransformationStack.back();
}
