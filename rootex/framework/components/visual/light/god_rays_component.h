#pragma once

#include "common/common.h"
#include "component.h"
#include "components/space/transform_component.h"
#include "core/renderer/rendering_device.h"
#include "core/renderer/renderer.h"
#include "PostProcess.h"
#include "core/renderer/vertex_buffer.h"
#include "core/renderer/index_buffer.h"

class GodRaysMidPostProcessor
{
	EventBinder<GodRaysMidPostProcessor> m_Binder;

	Ptr<DirectX::BasicPostProcess> m_BasicPostProcess;

	Shader m_GodRaysShader;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_CacheRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CacheSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_GodRaysSS;

	Ptr<VertexBuffer> m_FrameVertexBuffer;
	Ptr<IndexBuffer> m_FrameIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_GodRaysPSCB;

	Variant loadRTVAndSRV(const Event* event);

	ID3D11ShaderResourceView* m_SourceSRV;

public:
	GodRaysMidPostProcessor();
	GodRaysMidPostProcessor(GodRaysMidPostProcessor&) = delete;
	~GodRaysMidPostProcessor() = default;

	void preDraw(const PSGodRaysCB& cb);
	void draw();
	void postDraw();
};

class GodRaysComponent : public Component
{
	COMPONENT(GodRaysComponent, Category::Light);
	DEPENDS_ON(TransformComponent);

	int m_NumSamples;
	float m_Density;
	float m_Weight;
	float m_Decay;
	float m_Exposure;

	GodRaysMidPostProcessor* m_GodRaysMidPostProcessor;

public:
	GodRaysComponent::GodRaysComponent(Entity& owner, const JSON::json& data);
	~GodRaysComponent() = default;

	void render();

	JSON::json getJSON() const override;
	void draw() override;
};

DECLARE_COMPONENT(GodRaysComponent);
