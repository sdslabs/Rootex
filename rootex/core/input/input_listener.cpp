#include "input_listener.h"

InputListener::InputListener(InputListenerFunction listener)
    : m_ListenerFunction(listener)
    , m_ID(-1)
{
}

bool InputListener::OnUserButtonBool(gainput::UserButtonId userButton, bool oldValue, bool newValue)
{
	return m_ListenerFunction(userButton, oldValue, newValue);
}

bool InputListener::OnUserButtonFloat(gainput::UserButtonId userButton, float oldValue, float newValue)
{
	return m_ListenerFunction(userButton, oldValue, newValue);
}
