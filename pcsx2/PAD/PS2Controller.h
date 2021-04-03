
#pragma once

#include "PadTypes.h"
#include "Binding_Xinput.h"
#include "Binding_WindowsKeyboard.h"

#include <vector>

class PS2Controller
{
public:
	PhysicalType physicalType = PhysicalType::STANDARD;
	AnalogLight analogLight = AnalogLight::OFF;
	PadMode targetPadMode = PadMode::DIGITAL;
	PadMode currentPadMode = PadMode::DIGITAL;
	ButtonStates buttonStates;
	AnalogStates analogStates;
	GuitarStates guitarStates;
	bool analogLightLocked = false;
	// Used by commands 0x46 and 0x4c to remember which stage the pad is in.
	// Each of these commands actually runs twice in sequence and returns a
	// different payload on each run. This is a cheap way to track which run
	// we are on without storing the entire command sequence, which would be
	// useless for anything else.
	bool constantStage = 0;
#ifdef _WINDOWS
	std::vector<Binding_Xinput*> xinputBindings;
	std::vector<Binding_WindowsKeyboard*> windowsKeyboardBindings;
#endif

	PS2Controller();
	~PS2Controller();
	void SetButton(PS2Control ps2Control, u8 newValue);
	void SetAnalog(PS2Control ps2Control, u8 newValue);
	u8 GetFirstDigitalByte();
	u8 GetSecondDigitalByte();
};
