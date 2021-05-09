
#include "PrecompiledHeader.h"

#include "InputEvent.h"

// =============================================================================
// Xinput
// =============================================================================

InputEvent_Xinput::InputEvent_Xinput(const DWORD xinputId, const u16 buttonMask)
{
	this->inputEventType = InputEventType::XINPUT;
	this->xinputControlType = XinputControlType::STANDARD_BUTTON;
	this->xinputControlType = xinputControlType;
	this->value = buttonMask;
	this->analogType = XinputAnalogType::NONE;
	this->triggerType = XinputTriggerType::NONE;
}

InputEvent_Xinput::InputEvent_Xinput(const DWORD xinputId, const XinputAnalogType analogType, const u16 value)
{
	this->inputEventType = InputEventType::XINPUT;
	this->xinputControlType = XinputControlType::ANALOG_STICK;
	this->xinputControlType = xinputControlType;
	this->value = value;
	this->analogType = analogType;
	this->triggerType = XinputTriggerType::NONE;
}

InputEvent_Xinput::InputEvent_Xinput(const DWORD xinputId, const XinputTriggerType triggerType, const u16 value)
{
	this->inputEventType = InputEventType::XINPUT;
	this->xinputControlType = XinputControlType::TRIGGER;
	this->value = value;
	this->analogType = XinputAnalogType::NONE;
	this->triggerType = triggerType;
}

// =============================================================================
// Windows Keyboard
// =============================================================================

InputEvent_WindowsKeyboard::InputEvent_WindowsKeyboard(const u8 vKey, const bool state)
{
	this->vKey = vKey;
	this->state = state;
}
