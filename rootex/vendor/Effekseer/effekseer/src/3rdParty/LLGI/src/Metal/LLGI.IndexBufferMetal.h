#pragma once

#include "../LLGI.IndexBuffer.h"

namespace LLGI
{

struct Buffer_Impl;

class IndexBufferMetal : public IndexBuffer
{
private:
	Buffer_Impl* impl = nullptr;
	int32_t stride_ = 0;
	int32_t count_ = 0;

public:
	IndexBufferMetal();
	~IndexBufferMetal() override;

	bool Initialize(Graphics* graphics, int32_t stride, int32_t count);

	void* Lock() override;
	void* Lock(int32_t offset, int32_t size) override;
	void Unlock() override;
	int32_t GetStride() override;
	int32_t GetCount() override;

	Buffer_Impl* GetImpl() const { return impl; }
};

} // namespace LLGI
