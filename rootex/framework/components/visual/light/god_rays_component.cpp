#include "god_rays_component.h"

#include "application.h"
#include "entity.h"
#include "systems/render_system.h"

DEFINE_COMPONENT(GodRaysComponent);

GodRaysComponent::GodRaysComponent(Entity& owner, const JSON::json& data)
    : Component(owner)
    , m_NumSamples(data.value("numSamples", 100))
    , m_Density(data.value("density", 1.0f))
    , m_Weight(data.value("weight", 0.01f))
    , m_Decay(data.value("decay", 1.0f))
    , m_Exposure(data.value("exposure", 1.0f))
    , m_DependencyOnTransformComponent(this)
{
	m_GodRaysMidPostProcessor = new GodRaysMidPostProcessor();
}

void GodRaysComponent::render()
{
	CameraComponent* renderCamera = RenderSystem::GetSingleton()->getCamera();

	const Matrix& view = renderCamera->getViewMatrix();
	const Matrix& proj = renderCamera->getProjectionMatrix();
	const Matrix& model = getTransformComponent()->getAbsoluteTransform();

	Matrix mvp = model * view * proj;
	Vector4 dc = Vector4::Transform(Vector4(0.0f, 0.0f, 0.0f, 1.0f), mvp);
	Vector3 ndc = Vector3(dc.x, -dc.y, dc.z) / dc.w;
	Vector3 screenSpacePos = ndc / 2.0f + Vector3(0.5f, 0.5f, 0.5f);

	PSGodRaysCB cb;
	cb.sunScreenSpacePos = screenSpacePos;
	cb.numSamples = m_NumSamples;
	cb.density = m_Density;
	cb.weight = m_Weight;
	cb.decay = m_Decay;
	cb.exposure = m_Exposure;

	m_GodRaysMidPostProcessor->preDraw(cb);
	m_GodRaysMidPostProcessor->draw();
	m_GodRaysMidPostProcessor->postDraw();
}

JSON::json GodRaysComponent::getJSON() const
{
	JSON::json j;

	j["numSamples"] = m_NumSamples;
	j["density"] = m_Density;
	j["weight"] = m_Weight;
	j["decay"] = m_Decay;
	j["exposure"] = m_Exposure;

	return j;
}

void GodRaysComponent::draw()
{
	int minNumSamples = 0;
	int maxNumSamples = 100;
	ImGui::DragInt("Num Samples", &m_NumSamples, 1.0f, minNumSamples, maxNumSamples);

	float minDensity = 0.0f;
	float maxDensity = 2.0f;
	ImGui::DragFloat("Density", &m_Density, 0.01f, minDensity, maxDensity);

	float minWeight = 0.0f;
	float maxWeight = 0.1f;
	ImGui::DragFloat("Weight", &m_Weight, 0.001f, minWeight, maxWeight);

	float minDecay = 0.95f;
	float maxDecay = 1.05f;
	ImGui::DragFloat("Decay", &m_Decay, 0.001f, minDecay, maxDecay);

	float minExposure = 0.0f;
	float maxExposure = 2.0f;
	ImGui::DragFloat("Exposure", &m_Exposure, 0.02f, minExposure, maxExposure);
}

GodRaysMidPostProcessor::GodRaysMidPostProcessor()
{
	m_Binder.bind(RootexEvents::WindowResized, this, &GodRaysMidPostProcessor::loadRTVAndSRV);

	BufferFormat godRaysFormat;
	godRaysFormat.push(VertexBufferElement::Type::FloatFloatFloat, "POSITION", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	godRaysFormat.push(VertexBufferElement::Type::FloatFloat, "TEXCOORD", D3D11_INPUT_PER_VERTEX_DATA, 0, false, 0);
	m_GodRaysShader = Shader("rootex/core/renderer/shaders/god_rays_vertex_shader.hlsl", "rootex/core/renderer/shaders/god_rays_pixel_shader.hlsl", godRaysFormat);

	m_GodRaysSS = RenderingDevice::GetSingleton()->createSS(RenderingDevice::SamplerState::Default);

	loadRTVAndSRV(nullptr);

	m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));

	Vector<GodRaysData> godRaysData = {
		// Position                    // Texcoord
		{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f) }
	};
	m_FrameVertexBuffer.reset(new VertexBuffer((const char*)godRaysData.data(), godRaysData.size(), sizeof(GodRaysData), D3D11_USAGE_IMMUTABLE, 0));

	Vector<unsigned int> godRaysDataIndices = {
		0, 2, 1,
		0, 3, 2
	};
	m_FrameIndexBuffer.reset(new IndexBuffer(godRaysDataIndices));

	m_GodRaysPSCB = RenderingDevice::GetSingleton()->createBuffer<PSGodRaysCB>(PSGodRaysCB(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

Variant GodRaysMidPostProcessor::loadRTVAndSRV(const Event* event)
{
	RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
	return true;
}

void GodRaysMidPostProcessor::preDraw(const PSGodRaysCB& cb)
{
	m_SourceSRV = RenderingDevice::GetSingleton()->getOffScreenSRV().Get();

	RenderingDevice::GetSingleton()->unbindSRVs();
	RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

	// Copy source to working buffer
	{
		m_BasicPostProcess->SetSourceTexture(m_SourceSRV);
		m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
		m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
	}

	m_FrameVertexBuffer->bind();
	m_FrameIndexBuffer->bind();
	m_GodRaysShader.bind();

	RenderingDevice::GetSingleton()->setPSSS(0, 1, m_GodRaysSS.GetAddressOf());

	RenderingDevice::GetSingleton()->editBuffer<PSGodRaysCB>(cb, m_GodRaysPSCB.Get());
	RenderingDevice::GetSingleton()->setPSCB(0, 1, m_GodRaysPSCB.GetAddressOf());

	RenderingDevice::GetSingleton()->setPSSRV(0, 1, &m_SourceSRV);
}

void GodRaysMidPostProcessor::draw()
{
	RenderingDevice::GetSingleton()->drawIndexed(m_FrameIndexBuffer->getCount());
}

void GodRaysMidPostProcessor::postDraw()
{
	RenderingDevice::GetSingleton()->unbindSRVs();
	RenderingDevice::GetSingleton()->setOffScreenRTVDSV();
	m_BasicPostProcess->SetSourceTexture(m_CacheSRV.Get());
	m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
	m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
}
