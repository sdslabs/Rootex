#include "dxgi_debug_interface.h"
#include <dxgidebug.h>

//TODO- Remove this pragma and see for an alternative of the library load in constructor
#pragma comment(lib, "dxguid.lib")

DxgiDebugInterface::DxgiDebugInterface()
{
	typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void**);

	const HMODULE hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		ERR("dxgidebug.dll not loaded");
	}

	DXGIGetDebugInterface dxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
	    reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
	if (dxgiGetDebugInterface == nullptr)
	{
		ERR("DXGIGetDebugInterface not loaded");
	}
	
	if (FAILED(dxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(m_DxgiInfoQueue.ReleaseAndGetAddressOf()))))
	{
		ERR("DXGI Info Queue could not be loaded");
	}
}

DxgiDebugInterface::~DxgiDebugInterface()
{
}

DxgiDebugInterface* DxgiDebugInterface::GetSingleton()
{
	static DxgiDebugInterface singleton;
	return &singleton;
}

void DxgiDebugInterface::set() noexcept
{
	next = m_DxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

void DxgiDebugInterface::getMessages(String file, String func) const
{
	const unsigned int end = m_DxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength = 0;
		//TODO- below 2 GetMessage call returns an HRESULT, validate it to see if an error is raised
		m_DxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		m_DxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
		ERR_CUSTOM(file, func, pMessage->pDescription);
	}
}
