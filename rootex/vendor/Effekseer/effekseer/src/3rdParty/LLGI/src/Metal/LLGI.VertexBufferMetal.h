#pragma once

#include "../LLGI.VertexBuffer.h"

namespace LLGI
{

struct Buffer_Impl;

class VertexBufferMetal : public VertexBuffer
{
private:
	Buffer_Impl* impl = nullptr;

public:
	VertexBufferMetal();
	~VertexBufferMetal() override;

	bool Initialize(Graphics* graphics, int32_t size);

	void* Lock() override;
	void* Lock(int32_t offset, int32_t size) override;
	void Unlock() override;
	int32_t GetSize() override;

	Buffer_Impl* GetImpl() const { return impl; }
};

} // namespace LLGI
