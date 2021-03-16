#pragma once

#include "../LLGI.PipelineState.h"

namespace LLGI
{

class GraphicsMetal;
struct PipelineState_Impl;

class PipelineStateMetal : public PipelineState
{
	friend struct PipelineState_Impl;

private:
	GraphicsMetal* graphics_ = nullptr;
	PipelineState_Impl* impl = nullptr;
	std::array<Shader*, static_cast<int>(ShaderStageType::Max)> shaders;

public:
	PipelineStateMetal();
	~PipelineStateMetal() override;

	bool Initialize(GraphicsMetal* graphics);
	void SetShader(ShaderStageType stage, Shader* shader) override;
	bool Compile() override;

	std::array<Shader*, static_cast<int>(ShaderStageType::Max)> GetShaders() const { return shaders; }

	RenderPassPipelineState* GetRenderPassPipelineState() const { return renderPassPipelineState_.get(); }

	PipelineState_Impl* GetImpl() { return impl; }
};

} // namespace LLGI
