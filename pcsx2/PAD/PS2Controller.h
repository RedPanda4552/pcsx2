
#pragma once

#include "PadTypes.h"
#include "Binding_Xinput.h"
#include "Binding_WindowsKeyboard.h"

#include <vector>

class PS2Controller
{
public:
	PadMode targetPadMode;
	PadMode currentPadMode;
	ButtonStates buttonStates;
	AnalogStates analogStates;
	GuitarStates guitarStates;
	bool analogLightEnabled = true;
	bool analogLightLocked = false;
#ifdef _WINDOWS
	std::vector<Binding_Xinput> xinputBindings;
	std::vector<Binding_WindowsKeyboard> windowsKeyboardBindings;
#endif

	PS2Controller();
	~PS2Controller();
	void SetButton(PS2Control ps2Control, u8 newValue);
	void SetAnalog(PS2Control ps2Control, u8 newValue);
	u8 GetFirstDigitalByte();
	u8 GetSecondDigitalByte();
};
