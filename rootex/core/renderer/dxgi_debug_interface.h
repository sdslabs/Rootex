#pragma once

#include <dxgidebug.h>
#include <vector>

#include "common.h"

class DxgiDebugInterface
{
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_DxgiInfoQueue;

	DxgiDebugInterface();
	DxgiDebugInterface(const DxgiDebugInterface&) = delete;
	DxgiDebugInterface& operator=(const DxgiDebugInterface&) = delete;
	~DxgiDebugInterface();

public:
	static DxgiDebugInterface* GetSingleton();

	void set() noexcept;
	void getMessages(String file, String func) const;
};

/// Used to get DIrectX errors/warnings from its debug interface
#define GFX_ERR_CHECK(hr)                      \
	DxgiDebugInterface::GetSingleton()->set(); \
	if (FAILED(hr))                            \
	DxgiDebugInterface::GetSingleton()->getMessages(__FILE__, __func__)
