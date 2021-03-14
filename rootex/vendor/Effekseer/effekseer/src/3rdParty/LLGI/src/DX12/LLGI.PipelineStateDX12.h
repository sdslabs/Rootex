
#pragma once

#include "../LLGI.PipelineState.h"
#include "../LLGI.Shader.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

using namespace DirectX;

namespace LLGI
{

class PipelineStateDX12 : public PipelineState
{
private:
	std::shared_ptr<GraphicsDX12> graphics_;

	std::array<Shader*, static_cast<int>(ShaderStageType::Max)> shaders_;

	ID3D12PipelineState* pipelineState_ = nullptr;

	ID3DBlob* signature_ = nullptr;
	ID3D12RootSignature* rootSignature_ = nullptr;

	bool CreateRootSignature();

public:
	PipelineStateDX12() = default;
	PipelineStateDX12(GraphicsDX12* graphics);
	~PipelineStateDX12() override;

	void SetShader(ShaderStageType stage, Shader* shader) override;
	bool Compile() override;

	ID3D12PipelineState* GetPipelineState() { return pipelineState_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_; }
};

} // namespace LLGI
