#pragma once

#include "PadTypes.h"

class Binding_WindowsKeyboard
{
private:
	u8 vkeyId;
	PS2Control ps2Control;

public:
	Binding_WindowsKeyboard(u8 vkeyId, PS2Control ps2Control);
	~Binding_WindowsKeyboard();
	int GetVkeyId();
	PS2Control GetPS2Control();
};
