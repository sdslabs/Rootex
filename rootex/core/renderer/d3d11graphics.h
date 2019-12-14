#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <array>
#include <iostream>
#include <DirectXMath.h>
#include "d3d11utils.h"
#include "dxgiDebugInterface.h"

class RootexGraphics
{
private:	
	// TODO: Implement this as a separate Rootex class
	std::chrono::time_point<std::chrono::system_clock> m_StartTime;
	
	unsigned int height;
	unsigned int width;
	float maxX = 1.0f;
	float minZ = 0.5f;
	float maxZ = 10.0f;

public:
	RootexGraphics(unsigned int w = 640, unsigned int h = 480 );
	RootexGraphics(const RootexGraphics&) = delete;
	RootexGraphics& operator=(const RootexGraphics&) = delete;  
	~RootexGraphics();

	void drawTest();
	void drawTestCube(float angle);
};
