
#include "LLGI.VertexBuffer.h"

namespace LLGI
{
void* VertexBuffer::Lock() { return nullptr; }

void* VertexBuffer::Lock(int32_t offset, int32_t size) { return nullptr; }

void VertexBuffer::Unlock() {}

int32_t VertexBuffer::GetSize() { return 0; }

} // namespace LLGI
