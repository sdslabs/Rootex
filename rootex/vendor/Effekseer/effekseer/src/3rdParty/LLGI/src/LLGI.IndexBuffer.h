
#pragma once

#include "LLGI.Base.h"

namespace LLGI
{

class IndexBuffer : public ReferenceObject
{
private:
public:
	IndexBuffer() = default;
	~IndexBuffer() override = default;

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void* Lock();

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void* Lock(int32_t offset, int32_t size);

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void Unlock();

	virtual int32_t GetStride();

	virtual int32_t GetCount();
};

} // namespace LLGI
