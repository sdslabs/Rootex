#pragma once

#include "common/common.h"

#undef interface
#include "RmlUi/Core.h"
#define interface __STRUCT__

class InputInterface
{
public:
	static bool Initialise();
	static void Shutdown();

	/// Process the Windows message.
	static void ProcessWindowsEvent(UINT message, WPARAM w_param, LPARAM l_param);

	static void SetContext(Rml::Core::Context* context);
	static Rml::Core::Character GetCharacterCode(Rml::Core::Input::KeyIdentifier key_identifier, int key_modifier_state);

protected:
	static Rml::Core::Context* s_Context;
};
