#pragma once

#include "common/common.h"
#include "event_manager.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

/// Provides an interface to pass user input into RmlUi.
class InputInterface
{
	EventBinder<InputInterface> m_Binder;

	InputInterface();

public:
	static InputInterface* GetSingleton();

	bool m_IsMouseOver = false;
	bool m_IsEnabled = true;
	float m_ScaleX = 1.0f;
	float m_ScaleY = 1.0f;
	int m_Left = 0;
	int m_Right = 0;
	int m_Top = 0;
	int m_Bottom = 0;

	bool initialise();

	/// Process the Windows message.
	void processWindowsEvent(UINT message, WPARAM wParam, LPARAM lParam);

	void setContext(Rml::Context* context);
	Rml::Character getCharacterCode(Rml::Input::KeyIdentifier keyIdentifier, int keyModifier_state);

	Variant windowResized(const Event* event);

protected:
	Rml::Context* m_Context = nullptr;
};
