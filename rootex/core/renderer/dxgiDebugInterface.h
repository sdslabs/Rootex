#pragma once
#include "common.h"
#include <dxgidebug.h>
#include <vector>
#include "d3d11utils.h"

class DxgiDebugInterface
{
public:
	DxgiDebugInterface();
	~DxgiDebugInterface();
	DxgiDebugInterface(const DxgiDebugInterface&) = delete;
	DxgiDebugInterface& operator=(const DxgiDebugInterface&) = delete;
	void Set() noexcept;
	Vector<String> GetMessages() const;

private:
	unsigned long long next = 0u;
	IDXGIInfoQueue* pDxgiInfoQueue;
};