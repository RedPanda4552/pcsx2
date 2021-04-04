
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
	// A 32 bit mask used for command 0x4f to track which bits have been set
	// on each byte that comes in. In practice we should only ever get
	// ff ff 03 (DS2), 3f (Analog) or 03 (Digital).
	u32 buttonMask = 0;
#ifdef _WINDOWS
	std::vector<Binding_Xinput*> xinputBindings;
	std::vector<Binding_WindowsKeyboard*> windowsKeyboardBindings;
#endif

	PS2Controller();
	~PS2Controller();
	void Debug_SetBindings();
	void SetButton(PS2Control ps2Control, u8 newValue);
	void SetAnalog(PS2Control ps2Control, u8 newValue);
	u8 GetFirstDigitalByte();
	u8 GetSecondDigitalByte();
	u8 GetAnalog(PS2Control ps2Control);
};
