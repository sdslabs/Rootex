#include "input_listener.h"

InputListener::InputListener(InputBoolListenerFunction boolListener, InputFloatListenerFunction floatListener)
    : m_BoolListenerFunction(boolListener)
    , m_FloatListenerFunction(floatListener)
    , m_ID(-1)
{
}

bool InputListener::OnUserButtonBool(gainput::UserButtonId userButton, bool oldValue, bool newValue)
{
	return m_BoolListenerFunction(userButton, oldValue, newValue);
}

bool InputListener::OnUserButtonFloat(gainput::UserButtonId userButton, float oldValue, float newValue)
{
	return m_FloatListenerFunction(userButton, oldValue, newValue);
}
