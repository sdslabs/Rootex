#pragma once

#include "../LLGI.Graphics.h"
#import <MetalKit/MetalKit.h>
#include <functional>
#include <unordered_map>

namespace LLGI
{

struct Buffer_Impl;

class BufferMetal : public ReferenceObject
{
private:
	Buffer_Impl* impl = nullptr;

public:
	BufferMetal();
	~BufferMetal() override;

	bool Initialize(Graphics* graphics, int32_t size);

	void* GetBuffer();

	Buffer_Impl* GetImpl() const;
};

} // namespace LLGI
