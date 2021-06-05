#pragma once

#include "PadTypes.h"

#include "Input/Types_Xinput.h"

class Binding_Xinput
{
private:
	DWORD xinputId;
	WORD buttonMask;
	XinputTriggerType triggerType;
	XinputAnalogType analogType;
	PS2Control ps2Control;
	XinputVibrationMotor ps2VibrationMotor;
	XinputVibrationMotor xinputVibrationMotor;
	float deadzone;

public:
	Binding_Xinput(DWORD xinputId, WORD buttonMask, PS2Control ps2Control);
	Binding_Xinput(DWORD xinputId, XinputAnalogType analogType, PS2Control ps2Control, float deadzone);
	Binding_Xinput(DWORD xinputId, XinputTriggerType triggerType, PS2Control ps2Control, float deadzone);
	Binding_Xinput(DWORD xinputId, XinputVibrationMotor ps2VibrationMotor, XinputVibrationMotor xinputVibrationMotor);
	DWORD GetXinputId() noexcept;
	WORD GetButtonMask() noexcept;
	XinputTriggerType GetTriggerType() noexcept;
	XinputAnalogType GetAnalogType() noexcept;
	PS2Control GetPS2Control() noexcept;
	XinputVibrationMotor GetPS2VibrationMotor() noexcept;
	XinputVibrationMotor GetXinputVibrationMotor() noexcept;
	float GetDeadzone() noexcept;
};
