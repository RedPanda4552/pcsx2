#pragma once

#include "PadTypes.h"

class Binding_Xinput
{
private:
	DWORD xinputId;
	WORD buttonMask;
	TriggerType triggerType;
	AnalogType analogType;
	PS2Control ps2Control;

public:
	Binding_Xinput();
	~Binding_Xinput();
	DWORD GetXinputId() noexcept;
	WORD GetButtonMask() noexcept;
	TriggerType GetTriggerType() noexcept;
	AnalogType GetAnalogType() noexcept;
	PS2Control GetPS2Control() noexcept;
};
