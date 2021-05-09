
#pragma once

#include "Pcsx2Types.h"

#include "Types_Xinput.h"

enum class InputEventType
{
	NOT_SET,
	XINPUT,
	WINDOWS_KEYBOARD
};

class InputEvent
{
public:
	InputEventType inputEventType = InputEventType::NOT_SET;
	bool isConsumed = false;
};

// =============================================================================
// Xinput
// =============================================================================

class InputEvent_Xinput : public InputEvent
{
public:
	DWORD xinputId;
	XinputControlType xinputControlType;
	u16 value;
	XinputAnalogType analogType;
	XinputTriggerType triggerType;

	InputEvent_Xinput(const DWORD xinputId, const u16 buttonMask);
	InputEvent_Xinput(const DWORD xinputId, const XinputAnalogType analogType, const u16 value);
	InputEvent_Xinput(const DWORD xinputId, const XinputTriggerType triggerType, const u16 value);
};

// =============================================================================
// Windows Keyboard
// =============================================================================

class InputEvent_WindowsKeyboard : public InputEvent
{
public:
	u8 vKey;
	bool state;

	InputEvent_WindowsKeyboard(const u8 vKey, const bool state);
};