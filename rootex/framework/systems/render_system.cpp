#include "render_system.h"

#include "components/visual/fog_component.h"
#include "components/visual/sky_component.h"
#include "components/visual/grid_model_component.h"
#include "components/visual/cpu_particles_component.h"
#include "renderer/shaders/register_locations_vertex_shader.h"
#include "renderer/shaders/register_locations_pixel_shader.h"
#include "light_system.h"
#include "renderer/material_library.h"
#include "application.h"
#include "scene_loader.h"

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
    , m_PSPerLevelConstantBuffer(nullptr)
    , m_IsEditorRenderPassEnabled(false)
{
	BIND_EVENT_MEMBER_FUNCTION("OpenedScene", onOpenedScene);

	m_Camera = SceneLoader::GetSingleton()->getRootScene()->getEntity()->getComponent<CameraComponent>();
	m_TransformationStack.push_back(Matrix::Identity);
	setProjectionConstantBuffers();

	m_LineMaterial = std::dynamic_pointer_cast<BasicMaterial>(MaterialLibrary::GetMaterial("rootex/assets/materials/line.rmat"));
	m_CurrentFrameLines.m_Endpoints.reserve(LINE_INITIAL_RENDER_CACHE * 2 * 3);
	m_CurrentFrameLines.m_Indices.reserve(LINE_INITIAL_RENDER_CACHE * 2);

	PostProcessor::GetSingleton();
}

void RenderSystem::recoverLostDevice()
{
	ERR("Fatal error: D3D Device lost");
}

void RenderSystem::setConfig(const SceneSettings& sceneSettings)
{
	Scene* cameraScene = SceneLoader::GetSingleton()->getRootScene()->findScene(sceneSettings.camera);
	if (!cameraScene)
	{
		ERR("Camera scene not found with ID " + std::to_string(sceneSettings.camera));
		restoreCamera();
		return;
	}

	if (!cameraScene->getEntity())
	{
		ERR("Entity not found in camera scene " + cameraScene->getFullName());
		restoreCamera();
		return;
	}

	CameraComponent* camera = cameraScene->getEntity()->getComponent<CameraComponent>();
	if (!camera)
	{
		ERR("CameraComponent not found on entity " + cameraScene->getFullName());
		restoreCamera();
		return;
	}

	setCamera(camera);
}

void RenderSystem::calculateTransforms(Scene* scene)
{
	if (Entity* entity = scene->getEntity())
	{
		if (TransformComponent* transform = entity->getComponent<TransformComponent>())
		{
			pushMatrix(entity->getComponent<TransformComponent>()->getLocalTransform());
		}
		else
		{
			pushMatrix(Matrix::Identity);
		}

		for (auto& child : scene->getChildren())
		{
			if (Entity* childEntity = child->getEntity())
			{
				if (TransformComponent* childTransform = childEntity->getComponent<TransformComponent>())
				{
					childTransform->setParentAbsoluteTransform(getCurrentMatrix());
				}
			}

			calculateTransforms(child.get());
		}
		popMatrix();
	}
}

void RenderSystem::renderPassRender(float deltaMilliseconds, RenderPass renderPass)
{
	renderComponents<ModelComponent>(deltaMilliseconds, renderPass);
	renderComponents<GridModelComponent>(deltaMilliseconds, renderPass);
	renderComponents<CPUParticlesComponent>(deltaMilliseconds, renderPass);
	renderComponents<AnimatedModelComponent>(deltaMilliseconds, renderPass);
}

void RenderSystem::update(float deltaMilliseconds)
{
	ZoneScoped;
	RenderingDevice::GetSingleton()->unbindSRVs();
	RenderingDevice::GetSingleton()->setOffScreenRTVDSV();
	m_Renderer->resetCurrentShader();

	Color clearColor = { 0.15f, 0.15f, 0.15f, 1.0f };
	float fogStart = 0.0f;
	float fogEnd = -1000.0f;
	{
		ZoneNamedN(fogCalculation, "Fog", true);
		if (!ECSFactory::GetComponents<FogComponent>().empty())
		{
			FogComponent* firstFog = (FogComponent*)ECSFactory::GetComponents<FogComponent>().front();
			clearColor = firstFog->getColor();

			for (auto& c : ECSFactory::GetComponents<FogComponent>())
			{
				FogComponent* fog = (FogComponent*)c;
				clearColor = Color::Lerp(clearColor, fog->getColor(), 0.5f);
				fogStart = fog->getNearDistance();
				fogEnd = fog->getFarDistance();
			}
		}
		Application::GetSingleton()->getWindow()->clearOffScreen(clearColor);
	}
	{
		ZoneNamedN(absoluteTransform, "Absolute Transformations", true);
		calculateTransforms(SceneLoader::GetSingleton()->getRootScene());
	}
	{
		ZoneNamedN(stateSet, "Render State Reset", true);
		// Render geometry
		RenderingDevice::GetSingleton()->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderingDevice::GetSingleton()->setCurrentRS();
		RenderingDevice::GetSingleton()->setDSS();
		RenderingDevice::GetSingleton()->setAlphaBS();

		perFrameVSCBBinds(fogStart, fogEnd);
		const Color& fogColor = clearColor;
		perFramePSCBBinds(fogColor);
	}
	{
		ZoneNamedN(renderPasses, "Render Passes", true);
		if (m_IsEditorRenderPassEnabled)
		{
			ZoneNamedN(editorRenderPass, "Editor Render Pass", true);
			{
				renderPassRender(deltaMilliseconds, RenderPass::Editor);
			}
		}
		{
			ZoneNamedN(basicRenderPass, "Basic Render Pass", true);
			renderPassRender(deltaMilliseconds, RenderPass::Basic);
		}
		{
			ZoneNamedN(alphaRenderPass, "Alpha Render Pass", true);
			renderPassRender(deltaMilliseconds, RenderPass::Alpha);
		}
		renderLines();
	}
	{
		ZoneNamedN(skyRendering, "Sky Rendering", true);
		RenderingDevice::GetSingleton()->enableSkyDSS();
		RenderingDevice::RasterizerState currentRS = RenderingDevice::GetSingleton()->getRSType();
		RenderingDevice::GetSingleton()->setRSType(RenderingDevice::RasterizerState::Sky);
		RenderingDevice::GetSingleton()->setCurrentRS();
		for (auto& c : ECSFactory::GetComponents<SkyComponent>())
		{
			SkyComponent* sky = (SkyComponent*)c;
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
	PostProcessor::GetSingleton()->draw(m_Camera);

	RenderingDevice::GetSingleton()->unbindSRVs();
	RenderingDevice::GetSingleton()->setOffScreenRTVDSV();
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

void RenderSystem::submitBox(const Vector3& min, const Vector3& max)
{
	Vector3 d = max - min;
	Vector3 x = Vector3(d.x, 0.0f, 0.0f);
	Vector3 y = Vector3(0.0f, d.y, 0.0f);
	Vector3 z = Vector3(0.0f, 0.0f, d.z);

	/// Representation of bottom/top vertices
	///   [3/7]-------[2/6]
	///    /           /
	/// [0/4]-------[1/5]
	Vector3 corners[8];
	corners[0] = min;
	corners[1] = min + x;
	corners[2] = min + x + z;
	corners[3] = min + z;

	corners[4] = min + y;
	corners[5] = min + y + x;
	corners[6] = max;
	corners[7] = min + y + z;

	submitLine(corners[0], corners[1]);
	submitLine(corners[1], corners[2]);
	submitLine(corners[2], corners[3]);
	submitLine(corners[3], corners[0]);
	submitLine(corners[4], corners[5]);
	submitLine(corners[5], corners[6]);
	submitLine(corners[6], corners[7]);
	submitLine(corners[7], corners[4]);
	submitLine(corners[0], corners[4]);
	submitLine(corners[1], corners[5]);
	submitLine(corners[2], corners[6]);
	submitLine(corners[3], corners[7]);
}

void RenderSystem::submitSphere(const Vector3& center, const float& radius)
{
	Vector3 points[6];
	Vector3 x = { radius, 0.0f, 0.0f };
	Vector3 y = { 0.0f, radius, 0.0f };
	Vector3 z = { 0.0f, 0.0f, radius };

	points[0] = center + x;
	points[1] = center - x;
	points[2] = center + y;
	points[3] = center - y;
	points[4] = center + z;
	points[5] = center - z;

	for (int i = 0; i < 6; i++)
	{
		for (int j = i; j < 6; j++)
		{
			submitLine(points[i], points[j]);
		}
	}
}

void RenderSystem::submitCone(const Matrix& transform, const float& height, const float& radius)
{
	Vector3 direction;
	transform.Forward().Normalize(direction);
	Vector3 up;
	transform.Up().Normalize(up);
	up *= radius;
	Vector3 right;
	transform.Right().Normalize(right);
	right *= radius;
	Vector3 center = transform.Translation();
	Vector3 end = center + height * direction;
	submitLine(transform.Translation(), end);
	submitLine(end - up, end + up);
	submitLine(end - right, end + right);
	submitLine(end - right, end + up);
	submitLine(end - right, end - up);
	submitLine(end + right, end + up);
	submitLine(end + right, end - up);
	submitLine(center, end + up);
	submitLine(center, end - up);
	submitLine(center, end + right);
	submitLine(center, end - right);
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
	perFrame.lights = LightSystem::GetSingleton()->getDynamicLights();
	perFrame.fogColor = fogColor;
	Material::SetPSConstantBuffer(perFrame, m_PSPerFrameConstantBuffer, PER_FRAME_PS_CPP);
}

void RenderSystem::perScenePSCBBinds()
{
	calculateTransforms(SceneLoader::GetSingleton()->getRootScene());
	PerLevelPSCB perScene;
	perScene.staticLights = LightSystem::GetSingleton()->getStaticPointLights();
	Material::SetPSConstantBuffer(perScene, m_PSPerLevelConstantBuffer, PER_SCENE_PS_CPP);
}

void RenderSystem::updatePerSceneBinds()
{
	perScenePSCBBinds();
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
	if (camera)
	{
		m_Camera = camera;
		setProjectionConstantBuffers();
	}
}

void RenderSystem::restoreCamera()
{
	if (SceneLoader::GetSingleton()->getRootScene())
	{
		setCamera(SceneLoader::GetSingleton()->getRootScene()->getEntity()->getComponent<CameraComponent>());
	}
}

const Matrix& RenderSystem::getCurrentMatrix() const
{
	return m_TransformationStack.back();
}

Variant RenderSystem::onOpenedScene(const Event* event)
{
	updatePerSceneBinds();
	return true;
}

void RenderSystem::draw()
{
	System::draw();

	ImGui::Columns(2);

	ImGui::Text("Camera");
	ImGui::NextColumn();
	if (ImGui::BeginCombo("##Camera", RenderSystem::GetSingleton()->getCamera()->getOwner()->getFullName().c_str()))
	{
		for (auto& c : ECSFactory::GetComponents<CameraComponent>())
		{
			CameraComponent* camera = (CameraComponent*)c;
			if (ImGui::MenuItem(camera->getOwner()->getFullName().c_str()))
			{
				RenderSystem::GetSingleton()->setCamera(camera);
			}
		}

		ImGui::EndCombo();
	}
	ImGui::NextColumn();
	ImGui::Columns(1);

	if (ImGui::Button("Update Static Lights"))
	{
		updatePerSceneBinds();
	}
}
