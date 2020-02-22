#pragma once

#include "common/common.h"
#include "gainput/gainput.h"

#include <functional>
typedef std::function<bool(int userButton, float oldValue, float newValue)> InputListenerFunction;

class InputListener : public gainput::MappedInputListener
{
	InputListenerFunction m_ListenerFunction;
	int m_ID;

public:
	InputListener(InputListenerFunction listener);
	InputListener(InputListener&) = delete;
	~InputListener() = default;

	virtual bool OnUserButtonBool(gainput::UserButtonId userButton, bool oldValue, bool newValue) override;
	virtual bool OnUserButtonFloat(gainput::UserButtonId userButton, float oldValue, float newValue) override;

	void setID(int id) { m_ID = id; }
};
