
#pragma once

#include "LLGI.Base.h"

namespace LLGI
{

class ConstantBuffer : public ReferenceObject
{
private:
public:
	ConstantBuffer() = default;
	~ConstantBuffer() override = default;

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void* Lock();

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void* Lock(int32_t offset, int32_t size);

	/*[[deprecated("use CommandList::SetData.")]]*/ virtual void Unlock();

	virtual int32_t GetSize();
};

} // namespace LLGI
