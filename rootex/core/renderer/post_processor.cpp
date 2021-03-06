#include "post_processor.h"

#include "application.h"
#include "core/renderer/rendering_device.h"
#include "framework/components/visual/camera_component.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "material.h"
#include "shader.h"
#include "shader_library.h"

#include "Tracy.hpp"

class ASSAOPostProcess : public PostProcess
{
	ASSAO_Effect* m_ASSAO = nullptr;

public:
	ASSAOPostProcess()
	{
		const FileBuffer& assaoShader = OS::LoadFileContents("rootex/vendor/ASSAO/ASSAO.hlsl");
		ASSAO_CreateDescDX11 assaoDesc(RenderingDevice::GetSingleton()->getDevice(), assaoShader.data(), assaoShader.size());
		m_ASSAO = ASSAO_Effect::CreateInstance(&assaoDesc);
	}

	~ASSAOPostProcess()
	{
		ASSAO_Effect::DestroyInstance(m_ASSAO);
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isASSAO)
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setOffScreenRTVOnly();

			ASSAO_InputsDX11 assaoInputs;
			assaoInputs.ViewportX = 0;
			assaoInputs.ViewportY = 0;
			assaoInputs.ViewportHeight = Application::GetSingleton()->getWindow()->getHeight();
			assaoInputs.ViewportWidth = Application::GetSingleton()->getWindow()->getWidth();
			assaoInputs.ProjectionMatrix = *reinterpret_cast<ASSAO_Float4x4*>(&camera->getProjectionMatrix());
			assaoInputs.MatricesRowMajorOrder = true;
			assaoInputs.DrawOpaque = false;
			assaoInputs.DeviceContext = RenderingDevice::GetSingleton()->getContext();
			assaoInputs.DepthSRV = RenderingDevice::GetSingleton()->getDepthSSRV().Get();
			assaoInputs.NormalSRV = nullptr;
			assaoInputs.OverrideOutputRTV = nullptr;
			ASSAO_Settings assaoSettings;
			assaoSettings.Radius = postProcessingDetails.assaoRadius;
			assaoSettings.DetailShadowStrength = postProcessingDetails.assaoDetailShadowStrength;
			assaoSettings.BlurPassCount = postProcessingDetails.assaoBlurPassCount;
			assaoSettings.FadeOutFrom = postProcessingDetails.assaoFadeOutFrom;
			assaoSettings.FadeOutTo = postProcessingDetails.assaoFadeOutTo;
			assaoSettings.HorizonAngleThreshold = postProcessingDetails.assaoHorizonAngleThreshold;
			assaoSettings.QualityLevel = postProcessingDetails.assaoQualityLevel;
			assaoSettings.ShadowClamp = postProcessingDetails.assaoShadowClamp;
			assaoSettings.ShadowMultiplier = postProcessingDetails.assaoShadowMultiplier;
			assaoSettings.ShadowPower = postProcessingDetails.assaoShadowPower;
			assaoSettings.Sharpness = postProcessingDetails.assaoSharpness;
			assaoSettings.AdaptiveQualityLimit = postProcessingDetails.assaoAdaptiveQualityLimit;
			m_ASSAO->Draw(assaoSettings, &assaoInputs);

			RenderingDevice::GetSingleton()->setOffScreenRTVDSV();

			nextSource = nextSource;
		}
	}
};

class GaussianPostProcess : public PostProcess
{
	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Variant loadRTVAndSRV(const Event* event)
	{
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
		return true;
	}

public:
	GaussianPostProcess()
	{
		BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, GaussianPostProcess::loadRTVAndSRV);

		loadRTVAndSRV(nullptr);

		m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isGaussianBlur)
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::GaussianBlur_5x5);
			m_BasicPostProcess->SetSourceTexture(nextSource);
			m_BasicPostProcess->SetGaussianParameter(postProcessingDetails.gaussianBlurMultiplier);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			nextSource = m_CacheSRV.Get();
		}
	}
};

class MonochromePostProcess : public PostProcess
{
	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Variant loadRTVAndSRV(const Event* event)
	{
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
		return true;
	}

public:
	MonochromePostProcess()
	{
		BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, MonochromePostProcess::loadRTVAndSRV);

		loadRTVAndSRV(nullptr);

		m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isMonochrome)
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Monochrome);
			m_BasicPostProcess->SetSourceTexture(nextSource);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			nextSource = m_CacheSRV.Get();
		}
	}
};

class SepiaPostProcess : public PostProcess
{
	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Variant loadRTVAndSRV(const Event* event)
	{
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
		return true;
	}

public:
	SepiaPostProcess()
	{
		BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, SepiaPostProcess::loadRTVAndSRV);

		loadRTVAndSRV(nullptr);

		m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isSepia)
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Sepia);
			m_BasicPostProcess->SetSourceTexture(nextSource);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			nextSource = m_CacheSRV.Get();
		}
	}
};

class BloomPostProcess : public PostProcess
{
	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;
	Ptr<DirectX::DualPostProcess> m_DualPostProcess;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomExtractRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomExtractSRV;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomHorizontalBlurRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomHorizontalBlurSRV;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BloomVerticalBlurRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BloomVerticalBlurSRV;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Variant loadRTVAndSRV(const Event* event)
	{
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomExtractRTV, m_BloomExtractSRV);
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomHorizontalBlurRTV, m_BloomHorizontalBlurSRV);
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_BloomVerticalBlurRTV, m_BloomVerticalBlurSRV);
		return true;
	}

public:
	BloomPostProcess()
	{
		BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, BloomPostProcess::loadRTVAndSRV);

		loadRTVAndSRV(nullptr);

		m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));
		m_DualPostProcess.reset(new DirectX::DualPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isBloom)
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomExtractRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::BloomExtract);
			m_BasicPostProcess->SetBloomExtractParameter(postProcessingDetails.bloomThreshold);
			m_BasicPostProcess->SetSourceTexture(nextSource);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomHorizontalBlurRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::BloomBlur);
			m_BasicPostProcess->SetBloomBlurParameters(true, postProcessingDetails.bloomSize, postProcessingDetails.bloomBrightness);
			m_BasicPostProcess->SetSourceTexture(m_BloomExtractSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_BloomVerticalBlurRTV);

			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::BloomBlur);
			m_BasicPostProcess->SetBloomBlurParameters(false, postProcessingDetails.bloomSize, postProcessingDetails.bloomBrightness);
			m_BasicPostProcess->SetSourceTexture(m_BloomHorizontalBlurSRV.Get());
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

			m_DualPostProcess->SetSourceTexture(m_BloomVerticalBlurSRV.Get());
			m_DualPostProcess->SetSourceTexture2(nextSource);
			m_DualPostProcess->SetBloomCombineParameters(postProcessingDetails.bloomValue, postProcessingDetails.bloomBase, postProcessingDetails.bloomSaturation, postProcessingDetails.bloomBaseSaturation);
			m_DualPostProcess->SetEffect(DirectX::DualPostProcess::Effect::BloomCombine);
			m_DualPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			nextSource = m_CacheSRV.Get();
		}
	}
};

class ToneMapPostProcess : public PostProcess
{
	Ptr<DirectX::ToneMapPostProcess> m_ToneMapPostProcess;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Variant loadRTVAndSRV(const Event* event)
	{
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
		return true;
	}

public:
	ToneMapPostProcess()
	{
		BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, ToneMapPostProcess::loadRTVAndSRV);

		loadRTVAndSRV(nullptr);

		m_ToneMapPostProcess.reset(new DirectX::ToneMapPostProcess(RenderingDevice::GetSingleton()->getDevice()));
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isToneMap)
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

			m_ToneMapPostProcess->SetOperator((DirectX::ToneMapPostProcess::Operator)postProcessingDetails.toneMapOperator);
			m_ToneMapPostProcess->SetHDRSourceTexture(nextSource);
			m_ToneMapPostProcess->SetExposure(postProcessingDetails.toneMapExposure);
			m_ToneMapPostProcess->SetTransferFunction((DirectX::ToneMapPostProcess::TransferFunction)postProcessingDetails.toneMapTransferFunction);
			m_ToneMapPostProcess->SetST2084Parameter(postProcessingDetails.toneMapWhiteNits);
			m_ToneMapPostProcess->Process(RenderingDevice::GetSingleton()->getContext());

			nextSource = m_CacheSRV.Get();
		}
	}
};

class FXAAPostProcess : public PostProcess
{
	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;

	FXAAShader* m_FXAAShader;
	LumaShader* m_LumaShader;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_LumaCacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_LumaCacheSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Ptr<VertexBuffer> m_FrameVertexBuffer;
	Ptr<IndexBuffer> m_FrameIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_FXAAPSCB;

	Variant loadRTVAndSRV(const Event* event)
	{
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_CacheRTV, m_CacheSRV);
		RenderingDevice::GetSingleton()->createRTVAndSRV(m_LumaCacheRTV, m_LumaCacheSRV);
		return true;
	}

public:
	FXAAPostProcess()
	    : m_FXAAShader(ShaderLibrary::GetFXAAShader())
	    , m_LumaShader(ShaderLibrary::GetLumaShader())
	{
		BIND_EVENT_MEMBER_FUNCTION(RootexEvents::WindowResized, FXAAPostProcess::loadRTVAndSRV);

		loadRTVAndSRV(nullptr);

		m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));

		m_FrameVertexBuffer.reset(new VertexBuffer(Vector<FXAAData> {
		    // Position                    // Texcoord
		    { Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) },
		    { Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) },
		    { Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f) },
		    { Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f) } }));
		m_FrameIndexBuffer.reset(new IndexBuffer(Vector<unsigned int> {
		    0, 2, 1,
		    0, 3, 2 }));
	}

	void draw(CameraComponent* camera, ID3D11ShaderResourceView*& nextSource) override
	{
		const PostProcessingDetails& postProcessingDetails = camera->getPostProcessingDetails();
		if (postProcessingDetails.isFXAA)
		{
			// Convert RGBA to RGBL
			{
				RenderingDevice::GetSingleton()->unbindSRVs();
				RenderingDevice::GetSingleton()->setRTV(m_LumaCacheRTV.Get());
				// Copy source to working buffer
				{
					m_BasicPostProcess->SetSourceTexture(nextSource);
					m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
					m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
				}
				// Perform RGBA to RGBL step
				m_FrameVertexBuffer->bind();
				m_FrameIndexBuffer->bind();
				m_LumaShader->bind();

				m_LumaShader->set(nextSource);
				RenderingDevice::GetSingleton()->drawIndexed(m_FrameIndexBuffer->getCount());
			}
			// Apply FXAA
			{
				RenderingDevice::GetSingleton()->unbindSRVs();
				RenderingDevice::GetSingleton()->setRTV(m_CacheRTV.Get());

				m_FrameVertexBuffer->bind();
				m_FrameIndexBuffer->bind();
				m_FXAAShader->bind();

				PSFXAACB cb;
				Window* window = Application::GetSingleton()->getWindow();
				cb.rcpFrame = Vector4(1.0f / window->getWidth(), 1.0f / window->getHeight(), 0.0f, 0.0f);
				Material::SetPSConstantBuffer(cb, m_FXAAPSCB, 0);
				m_FXAAShader->set(m_LumaCacheSRV.Get());
				RenderingDevice::GetSingleton()->drawIndexed(m_FrameIndexBuffer->getCount());
			}

			nextSource = m_CacheSRV.Get();
		}
	}
};

PostProcessor::PostProcessor()
{
	m_BasicPostProcess.reset(new DirectX::BasicPostProcess(RenderingDevice::GetSingleton()->getDevice()));

	m_PostProcesses.emplace_back(new ASSAOPostProcess());
	m_PostProcesses.emplace_back(new GaussianPostProcess());
	m_PostProcesses.emplace_back(new MonochromePostProcess());
	m_PostProcesses.emplace_back(new SepiaPostProcess());
	m_PostProcesses.emplace_back(new BloomPostProcess());
	m_PostProcesses.emplace_back(new ToneMapPostProcess());
	m_PostProcesses.emplace_back(new FXAAPostProcess());
}

void PostProcessor::draw(CameraComponent* camera)
{
	ZoneNamedN(postProcessing, "Post Processing", true);

	if (camera->getPostProcessingDetails().isPostProcessing)
	{
		ID3D11ShaderResourceView* source = RenderingDevice::GetSingleton()->getOffScreenSRV().Get();
		for (auto& postProcess : m_PostProcesses)
		{
			postProcess->draw(camera, source);
		}

		// If any post processing happened outside the current texture, copy the result of the last run post process into current texture.
		if (source != RenderingDevice::GetSingleton()->getOffScreenSRV().Get())
		{
			RenderingDevice::GetSingleton()->unbindSRVs();
			RenderingDevice::GetSingleton()->setOffScreenRTVDSV();
			m_BasicPostProcess->SetSourceTexture(source);
			m_BasicPostProcess->SetEffect(DirectX::BasicPostProcess::Effect::Copy);
			m_BasicPostProcess->Process(RenderingDevice::GetSingleton()->getContext());
		}
	}
}
