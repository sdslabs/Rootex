
#pragma once

#include "LLGI.Base.h"

namespace LLGI
{

class VertexBuffer : public ReferenceObject
{
private:
public:
	VertexBuffer() = default;
	~VertexBuffer() override = default;

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void* Lock();

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void* Lock(int32_t offset, int32_t size);

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void Unlock();

	virtual int32_t GetSize();
};

} // namespace LLGI
