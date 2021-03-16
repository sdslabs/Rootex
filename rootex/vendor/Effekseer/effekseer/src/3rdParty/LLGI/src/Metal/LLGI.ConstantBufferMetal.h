#pragma once

#include "../LLGI.ConstantBuffer.h"
#include "LLGI.BufferMetal.h"

namespace LLGI
{

struct Buffer_Impl;

class ConstantBufferMetal : public ConstantBuffer
{
private:
	BufferMetal* buffer_ = nullptr;
	int32_t size_ = 0;
	int32_t offset_ = 0;

public:
	ConstantBufferMetal();
	~ConstantBufferMetal() override;

	bool Initialize(Graphics* graphics, int32_t size);

	bool InitializeAsShortTime(BufferMetal* buffer, int32_t offset, int32_t size);

	void* Lock() override;

	void* Lock(int32_t offset, int32_t size) override;

	void Unlock() override;

	int32_t GetSize() override;

	Buffer_Impl* GetImpl() const;

	int32_t GetOffset() const { return offset_; }
};

} // namespace LLGI
