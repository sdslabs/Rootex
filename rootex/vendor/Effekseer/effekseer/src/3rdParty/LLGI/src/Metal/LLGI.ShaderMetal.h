
#pragma once

#include "../LLGI.Shader.h"

namespace LLGI
{

class GraphicsMetal;
struct Shader_Impl;

class ShaderMetal : public Shader
{
private:
	GraphicsMetal* graphics_ = nullptr;
	Shader_Impl* impl = nullptr;

public:
	ShaderMetal();
	~ShaderMetal() override;
	bool Initialize(GraphicsMetal* graphics, DataStructure* data, int32_t count);

	Shader_Impl* GetImpl() { return impl; }
};

} // namespace LLGI
