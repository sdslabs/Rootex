#include "dxgiDebugInterface.h"
#include <dxgidebug.h>
#include <memory>


//TODO- Remove this pragma and see for an alternative of the library load in constructor
#pragma comment(lib, "dxguid.lib")


DxgiDebugInterface::DxgiDebugInterface()
{
	typedef HRESULT(WINAPI * DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		ERR("dxgidebug.dll not loaded");
	}

	// get address of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
	    reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
	if (DxgiGetDebugInterface == nullptr)
	{
		ERR("DXGIGetDebugInterface not loaded");
	}

	//HRESULT hr;
	//TODO- below call returns an HRESULT, validate it to see if an error is raised
	DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void **>(&pDxgiInfoQueue));
}

DxgiDebugInterface::~DxgiDebugInterface()
{
	SafeRelease(&pDxgiInfoQueue);
}

void DxgiDebugInterface::Set() noexcept
{
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

void DxgiDebugInterface::GetMessages(String file, String func) const
{
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength = 0;
		//TODO- below 2 GetMessage call returns an HRESULT, validate it to see if an error is raised
		pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
		ERR_CUSTOM(file, func, pMessage->pDescription);
	}
}