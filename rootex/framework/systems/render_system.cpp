#include "render_system.h"

#include "components/visual/fog_component.h"
#include "renderer/shaders/register_locations_vertex_shader.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "light_system.h"
#include "renderer/material_library.h"
#include "components/visual/sky_component.h"
#include "application.h"

RenderSystem* RenderSystem::GetSingleton()
{
	static RenderSystem singleton;
	return &singleton;
}

RenderSystem::RenderSystem()
    : System("RenderSystem", UpdateOrder::Render, true)
	, m_Renderer(new Renderer())
    , m_VSProjectionConstantBuffer(nullptr)
    , m_VSPerFrameConstantBuffer(nullptr)
    , m_PSPerFrameConstantBuffer(nullptr)
    , m_IsEditorRenderPassEnabled(false)
{
	m_Camera = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<CameraComponent>().get();
	m_TransformationStack.push_back(Matrix::Identity);
	setProjectionConstantBuffers();
	
	m_LineMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial("rootex/assets/materials/line.rmat"));
	m_CurrentFrameLines.m_Endpoints.reserve(LINE_INITIAL_RENDER_CACHE * 2 * 3);
	m_CurrentFrameLines.m_Indices.reserve(LINE_INITIAL_RENDER_CACHE * 2);

	m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	m_DualPostProcess.reset(new DirectX::DualPostProcess(RenderingDevice::GetSingleton()->getDevice()));

	RenderingDevice::GetSingleton()->createRTVAndSRV(m_GaussianBlurRTV, m_GaussianBlurSRV);
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_MonochromeRTV, m_MonochromeSRV);
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_SepiaRTV, m_SepiaSRV);
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomExtractRTV, m_BloomExtractSRV);
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomHorizontalBlurRTV, m_BloomHorizontalBlurSRV);
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomVerticalBlurRTV, m_BloomVerticalBlurSRV);
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomRTV, m_BloomSRV);
}

void RenderSystem::calculateTransforms(HierarchyComponent* hierarchyComponent)
{
	pushMatrix(hierarchyComponent->getOwner()->getComponent<TransformComponent>()->getLocalTransform());
	for (auto&& child : hierarchyComponent->getChildren())
	{
		child->getOwner()->getComponent<TransformComponent>()->m_ParentAbsoluteTransform = getCurrentMatrix();
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

void RenderSystem::setConfig(const JSON::json& configData, bool openInEditor)
{
	if (configData.find("camera") != configData.end())
	{
		Ref<Entity> cameraEntity = EntityFactory::GetSingleton()->findEntity(configData["camera"]);
		if (cameraEntity)
		{
			setCamera(cameraEntity->getComponent<CameraComponent>().get());
			return;
		}
	}
}

void RenderSystem::update(float deltaMilliseconds)
{
	RenderingDevice::GetSingleton()->setOffScreenRT();

	Color clearColor = { 0.15f, 0.15f, 0.15f, 1.0f };
	float fogStart = 0.0f;
	float fogEnd = -1000.0f;

	// Fog
	if (!s_Components[FogComponent::s_ID].empty())
	{
		FogComponent* firstFog = (FogComponent*)s_Components[FogComponent::s_ID].front();
		clearColor = firstFog->getColor();

		for (auto& component : s_Components[FogComponent::s_ID])
		{
			FogComponent* fog = (FogComponent*)component;
			clearColor = Color::Lerp(clearColor, fog->getColor(), 0.5f);
			fogStart = fog->getNearDistance();
			fogEnd = fog->getFarDistance();
		}
	}
	Application::GetSingleton()->getWindow()->clearOffScreen(clearColor);

	// Pre-calculate absolute transforms
	Ref<HierarchyComponent> rootHC = HierarchySystem::GetSingleton()->getRootEntity()->getComponent<HierarchyComponent>();
	calculateTransforms(rootHC.get());

	// Render geometry
	RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RenderingDevice::GetSingleton()->setCurrentRS();
	RenderingDevice::GetSingleton()->setDSS();
	RenderingDevice::GetSingleton()->setAlphaBS();

	perFrameVSCBBinds(fogStart, fogEnd);
	const Color& fogColor = clearColor;
	perFramePSCBBinds(fogColor);

#ifdef ROOTEX_EDITOR
	if (m_IsEditorRenderPassEnabled)
	{
		renderPassRender(RenderPass::Editor);
		renderLines();
	}
#endif // ROOTEX_EDITOR
	renderPassRender(RenderPass::Basic);
	renderPassRender(RenderPass::Alpha);

	// Sky
	{
		RenderingDevice::GetSingleton()->enableSkyDSS();
		RenderingDevice::RasterizerState currentRS = RenderingDevice::GetSingleton()->getRSType();
		RenderingDevice::GetSingleton()->setRSType(RenderingDevice::RasterizerState::Sky);
		RenderingDevice::GetSingleton()->setCurrentRS();
		for (auto& component : s_Components[SkyComponent::s_ID])
		{
			SkyComponent* sky = (SkyComponent*)component;
			for (auto& [material, meshes] : sky->getSkySphere()->getMeshes())
			{
				m_Renderer->bind(sky->getSkyMaterial());
				for (auto& mesh : meshes)
				{
					m_Renderer->draw(mesh.m_VertexBuffer.get(), mesh.m_IndexBuffer.get());
				}
			}
		}
		RenderingDevice::GetSingleton()->setRSType(currentRS);
		RenderingDevice::GetSingleton()->disableSkyDSS();
	}

	// Post processes
	const PostProcessingDetails& postProcessingdetails = m_Camera->getPostProcessingDetails();
	if (postProcessingdetails.isPostProcessing)
	{
		if (postProcessingdetails.isGaussianBlur)
		{
			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_GaussianBlurRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::GaussianBlur_5x5);
			m_BasicPostProcess->SetSourceTexture(RenderingDevice::GetSingleton()->getOffScreenRTSRV().Get());
			m_BasicPostProcess->SetGaussianParameter(postProcessingdetails.gaussianBlurMultiplier);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setOffScreenRT();

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
			m_BasicPostProcess->SetSourceTexture(m_GaussianBlurSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
		}

		if (postProcessingdetails.isMonochrome)
		{
			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_MonochromeRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Monochrome);
			m_BasicPostProcess->SetSourceTexture(RenderingDevice::GetSingleton()->getOffScreenRTSRV().Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setOffScreenRT();

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
			m_BasicPostProcess->SetSourceTexture(m_MonochromeSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
		}

		if (postProcessingdetails.isSepia)
		{
			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_SepiaRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Sepia);
			m_BasicPostProcess->SetSourceTexture(RenderingDevice::GetSingleton()->getOffScreenRTSRV().Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setOffScreenRT();

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
			m_BasicPostProcess->SetSourceTexture(m_SepiaSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
		}

		if (postProcessingdetails.isBloom)
		{
			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomExtractRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::BloomExtract);
			m_BasicPostProcess->SetBloomExtractParameter(postProcessingdetails.bloomThreshold);
			m_BasicPostProcess->SetSourceTexture(RenderingDevice::GetSingleton()->getOffScreenRTSRV().Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomHorizontalBlurRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::BloomBlur);
			m_BasicPostProcess->SetBloomBlurParameters(true, postProcessingdetails.bloomSize, postProcessingdetails.bloomBrightness);
			m_BasicPostProcess->SetSourceTexture(m_BloomExtractSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomVerticalBlurRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::BloomBlur);
			m_BasicPostProcess->SetBloomBlurParameters(false, postProcessingdetails.bloomSize, postProcessingdetails.bloomBrightness);
			m_BasicPostProcess->SetSourceTexture(m_BloomHorizontalBlurSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomRTV.Get());

			m_DualPostProcess->SetSourceTexture(m_BloomVerticalBlurSRV.Get());
			m_DualPostProcess->SetSourceTexture2(RenderingDevice::GetSingleton()->getOffScreenRTSRV().Get());
			m_DualPostProcess->SetBloomCombineParameters(postProcessingdetails.bloomValue, postProcessingdetails.bloomBase, postProcessingdetails.bloomSaturation, postProcessingdetails.bloomBaseSaturation);
			m_DualPostProcess->SetEffect(DirectX::DualPostProcess::Effect::BloomCombine);
			m_DualPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindRTSRVs();
			RenderingDevice::GetSingleton()->setOffScreenRT();

			m_BasicPostProcess->SetSourceTexture(m_BloomSRV.Get());
			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
		}
	}
	
	RenderingDevice::GetSingleton()->unbindRTSRVs();
	RenderingDevice::GetSingleton()->setOffScreenRT();
}

void RenderSystem::renderLines()
{
	if (m_CurrentFrameLines.m_Endpoints.size())
	{
		m_Renderer->bind(m_LineMaterial.get());

		enableLineRenderMode();

		VertexBuffer vb(m_CurrentFrameLines.m_Endpoints);
		IndexBuffer ib(m_CurrentFrameLines.m_Indices);

		m_Renderer->draw(&vb, &ib);

		m_CurrentFrameLines.m_Endpoints.clear();
		m_CurrentFrameLines.m_Indices.clear();

		resetRenderMode();
	}
}

void RenderSystem::submitLine(const Vector3& from, const Vector3& to)
{
	m_CurrentFrameLines.m_Endpoints.push_back(from.x);
	m_CurrentFrameLines.m_Endpoints.push_back(from.y);
	m_CurrentFrameLines.m_Endpoints.push_back(from.z);

	m_CurrentFrameLines.m_Endpoints.push_back(to.x);
	m_CurrentFrameLines.m_Endpoints.push_back(to.y);
	m_CurrentFrameLines.m_Endpoints.push_back(to.z);

	m_CurrentFrameLines.m_Indices.push_back(m_CurrentFrameLines.m_Indices.size());
	m_CurrentFrameLines.m_Indices.push_back(m_CurrentFrameLines.m_Indices.size());
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
	RenderingDevice::GetSingleton()->setRSType(RenderingDevice::RasterizerState::Wireframe);
}

void RenderSystem::resetDefaultRasterizer()
{
	RenderingDevice::GetSingleton()->setRSType(RenderingDevice::RasterizerState::Default);
}

void RenderSystem::setProjectionConstantBuffers()
{
	const Matrix& projection = getCamera()->getProjectionMatrix();
	Material::SetVSConstantBuffer(projection.Transpose(), m_VSProjectionConstantBuffer, PER_CAMERA_CHANGE_VS_CPP);
}

void RenderSystem::perFrameVSCBBinds(float fogStart, float fogEnd)
{
	const Matrix& view = getCamera()->getViewMatrix();
	Material::SetVSConstantBuffer(PerFrameVSCB({ view.Transpose(), -fogStart, -fogEnd }), m_VSPerFrameConstantBuffer, PER_FRAME_VS_CPP);
}

void RenderSystem::perFramePSCBBinds(const Color& fogColor)
{
	PerFramePSCB perFrame;
	perFrame.lights = LightSystem::GetSingleton()->getLights();
	perFrame.fogColor = fogColor;
	Material::SetPSConstantBuffer(perFrame, m_PSPerFrameConstantBuffer, PER_FRAME_PS_CPP);
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

#ifdef ROOTEX_EDITOR
#include "imgui.h"
void RenderSystem::draw()
{
	System::draw();

	ImGui::Columns(2);

	ImGui::Text("Camera");
	ImGui::NextColumn();
	if (ImGui::BeginCombo("##Camera", RenderSystem::GetSingleton()->getCamera()->getOwner()->getFullName().c_str()))
	{
		for (auto&& camera : System::GetComponents(CameraComponent::s_ID))
		{
			if (ImGui::MenuItem(camera->getOwner()->getFullName().c_str()))
			{
				RenderSystem::GetSingleton()->setCamera((CameraComponent*)camera);
			}
		}

		ImGui::EndCombo();
	}
	ImGui::NextColumn();
	ImGui::Columns(1);
}
#endif
