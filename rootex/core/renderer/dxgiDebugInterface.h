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
	void GetMessages(String file, String func) const;

private:
	unsigned long long next = 0u;
	IDXGIInfoQueue* pDxgiInfoQueue;
};