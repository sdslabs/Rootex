#pragma once
#include <string.h>
#include <memory>
#include <iostream>
#include <unordered_map>

class Shader;
class GPUParticleEmitPass;
class GPUParticleRenderPass;
class GPUParticleContext;

struct Matrix44
{
public:
	float Values[4][4];


	Matrix44& LookAtRH(const LLGI::Vec3F& eye, const LLGI::Vec3F& at, const LLGI::Vec3F& up);

	Matrix44& PerspectiveFovRH(float ovY, float aspect, float zn, float zf);

	static Matrix44& Mul(Matrix44& o, const Matrix44& in1, const Matrix44& in2);
};

struct EmitDataVertex
{
	LLGI::Vec2F ParticleIdAndLifeTime;
	LLGI::Vec3F Position;
	LLGI::Vec3F Velocity;
};

struct RectangleVertex
{
	LLGI::Vec3F Position;
	LLGI::Vec2F UV;
};

struct alignas(16) GPUParticleTextureInfo
{
	float TextureSize[4];
	Matrix44 viewProjMatrix;
};

class Shader
{
public:
	Shader(
		LLGI::Graphics* graphics,
		LLGI::DeviceType deviceType,
		const char* vsBinaryPath,
		const char* psBinaryPath);

	LLGI::Shader* vertexShader() const { return m_vs.get(); }
	LLGI::Shader* pixelShader() const { return m_ps.get(); }

private:
	std::vector<uint8_t> LoadData(const char* path);

	std::shared_ptr<LLGI::Shader> m_vs;
	std::shared_ptr<LLGI::Shader> m_ps;
};

struct GPUParticleShaders
{
	std::shared_ptr<Shader> emitShader;
	std::shared_ptr<Shader> updateShader;
	std::shared_ptr<Shader> renderShader;
};

class GPUParticleBuffer
{
public:
	GPUParticleBuffer(GPUParticleContext* context);

	LLGI::Texture* GetPositionTexture() const { return positionTexture_.get(); }
	LLGI::Texture* GetVelocityTexture() const { return velocityTexture_.get(); }
	LLGI::RenderPass* GetRenderPass() const { return renderPass_.get(); }

private:
	std::shared_ptr<LLGI::Texture> positionTexture_;
	std::shared_ptr<LLGI::Texture> velocityTexture_;
	std::shared_ptr<LLGI::RenderPass> renderPass_;
};

class GPUParticleEmitPass
{
public:
	GPUParticleEmitPass(GPUParticleContext* context);

	void Render(LLGI::CommandList* commandList, const std::vector<EmitDataVertex>& emitData, int particleDataCount);

private:
	GPUParticleContext* context_;

	std::shared_ptr<Shader> shader_;

	// LLGI::CommandList::SetData() が未対応なので、Lock/Unlock で対応することになるが、そのため FrameCount 分必要/
	std::vector<std::shared_ptr<LLGI::VertexBuffer>> emitDataVertexBuffer_;

	// いまは DX12 バックエンドが IndexBuffer 必須となっているので用意する必要がある
	std::shared_ptr<LLGI::IndexBuffer> emitDataIndexBuffer_;

	std::array<std::shared_ptr<LLGI::RenderPass>, 2> emitParticleRenderPass_;

	std::array<std::shared_ptr<LLGI::PipelineState>, 2> pipelineState_;


	std::unordered_map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pipelineCache_;
};

class GPUParticleUpdatePass
{
public:
	GPUParticleUpdatePass(GPUParticleContext* context);

	void Render(LLGI::CommandList* commandList);

private:
	GPUParticleContext* context_;
	std::shared_ptr<Shader> shader_;
	std::shared_ptr<LLGI::VertexBuffer> vertexBuffer_;
	std::shared_ptr<LLGI::IndexBuffer> indexBuffer_;
	std::array<std::shared_ptr<LLGI::PipelineState>, 2> pipelineState_;
};


class GPUParticleRenderPass
{
public:
	struct SimpleVertex
	{
		LLGI::Vec3F Position;
		LLGI::Vec2F UV;
	};

	GPUParticleRenderPass(GPUParticleContext* context);

	void Render(LLGI::RenderPass* renderPass, LLGI::CommandList* commandList);

private:
	GPUParticleContext* context_;
	std::shared_ptr<Shader> shader_;
	std::shared_ptr<LLGI::VertexBuffer> vb_;
	std::shared_ptr<LLGI::IndexBuffer> ib_;
	std::unordered_map<std::shared_ptr<LLGI::RenderPassPipelineState>, std::shared_ptr<LLGI::PipelineState>> pipelineCache_;

};

class GPUParticleContext
{
public:
	static const int kMaxOneFrameEmitCount = 1024;

	GPUParticleContext(
		LLGI::Graphics* graphics,
		LLGI::DeviceType deviceType,
		int frameCount,
		int textureSize,
		LLGI::Texture* particleTexture,
		GPUParticleShaders shaders);

	LLGI::Graphics* GetGraphcis() const { return graphcis_; }

	LLGI::DeviceType GetDeviceType() const { return deviceType_; }

	int GetFrameIndex() const { return frameIndex_; }

	int GetMaxFrameCount() const { return maxFrameCount_; }

	int GetParticleCount() const { return emitedCount_; }

	int GetMaxParticles() const { return maxTexels_; }

	int GetBufferTextureWidth() const { return bufferTextureWidth_; }

	LLGI::Texture* GetParticleTexture() const { return particleTexture_; }

	const GPUParticleShaders& shaders() const { return shaders_; }

	/* Emit を書き込むところ & 今フレームの計算に使う情報の Fetch 元 */
	GPUParticleBuffer* GetPrimaryParticleBuffer() const { return particleBuffers_[primaryParticleBufferIndex_].get(); }

	GPUParticleBuffer* GetTargetParticleBuffer() const { return particleBuffers_[(primaryParticleBufferIndex_ + 1) % 2].get(); }

	LLGI::ConstantBuffer* GetTextureInfoConstantBuffer() const { return textureInfoConstantBuffer_.get(); }

	int GetPrimaryParticleBufferIndex() const { return primaryParticleBufferIndex_; }
	int GetTargetParticleBufferIndex() const { return (primaryParticleBufferIndex_ + 1) % 2; }

	GPUParticleBuffer* GetParticleBuffer(int index) const { return particleBuffers_[index].get(); }

	void Emit(float lifeTime, LLGI::Vec3F position, LLGI::Vec3F velocity);

	void NewFrame();

	void Render(LLGI::RenderPass* renderPass, LLGI::CommandList* commandList);

private:
	LLGI::Graphics* graphcis_;
	LLGI::DeviceType deviceType_;
	int frameIndex_;
	int maxFrameCount_;
	int bufferTextureWidth_;
	int maxTexels_;
	LLGI::Texture* particleTexture_;
	GPUParticleShaders shaders_;

	std::vector<EmitDataVertex> emitData_;
	int emitedCount_;
	int newParticleCountInFrame_;

	std::array<std::unique_ptr<GPUParticleBuffer>, 2> particleBuffers_;
	std::shared_ptr<LLGI::ConstantBuffer> textureInfoConstantBuffer_;
	int primaryParticleBufferIndex_;

	std::unique_ptr<GPUParticleEmitPass> particleEmitPass_;
	std::unique_ptr<GPUParticleUpdatePass> particleUpdatePass_;
	std::unique_ptr<GPUParticleRenderPass> particleRenderPass_;
};


