#pragma once

#include <dxgidebug.h>
#include <vector>

#include "common.h"
#include "utils.h"

class DxgiDebugInterface
{
	unsigned long long next = 0u;
	IDXGIInfoQueue* pDxgiInfoQueue;

	DxgiDebugInterface();
	DxgiDebugInterface(const DxgiDebugInterface&) = delete;
	DxgiDebugInterface& operator=(const DxgiDebugInterface&) = delete;
	~DxgiDebugInterface();

public:
	static DxgiDebugInterface* GetSingleton();

	void set() noexcept;
	void getMessages(String file, String func) const;
};

#define GFX_ERR_CHECK(hr)                      \
	DxgiDebugInterface::GetSingleton()->set(); \
	if (FAILED(hr))                            \
	DxgiDebugInterface::GetSingleton()->getMessages(__FILE__, __func__)
