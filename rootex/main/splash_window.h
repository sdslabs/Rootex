#pragma once

#include "common/types.h"

class SplashWindow
{
	HWND m_SplashWindow;

public:
	SplashWindow(const String& title, const String& icon, const String& image, int width, int height);
	SplashWindow(const SplashWindow&) = delete;
	~SplashWindow();
};
