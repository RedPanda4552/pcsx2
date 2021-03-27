
#pragma once

#include "PadTypes.h"
#include "PS2Controller.h"
#include "InputInterface_Xinput.h"
#include "InputInterface_WindowsKeyboard.h"

class Pad
{
private:
#ifdef _WINDOWS
	InputInterface_Xinput* inputInterface_Xinput;
	InputInterface_WindowsKeyboard* inputInterface_WindowsKeyboard;
#endif
	PS2Controller* ps2Controllers[2][4];
	PadCommandType padCommandType = PadCommandType::NOT_SET;
	// Number of command bytes received. This value is always current; PadCommandInit and PadCommandExec
	// can read this value while executing and always expect this value to reflect the number of command
	// bytes which have been received so far, including the command byte they are currently working on.
	// Running PadCommandInit will flush this to 1, and PadCommandExec will increment this by 1 before
	// doing anything else.
	u8 cmdBytesReceived;
	u8 currentPort, currentSlot;

public:
	Pad();
	~Pad();
	void UpdateBoundInputs(PS2Controller* ps2Controller);
	// Initiate a PAD command sequence. Should be called only once, on the first byte of a PAD command.
	// Specifies which PAD port/slot will be used, and resets most relevant fields.
	u8 PadCommandInit(u8 port, u8 slot) noexcept;
	// Send a byte as part of a PAD command sequence. Used to handle each command byte after the first.
	// This will track the specific command type and execute the appropriate response payload.
	u8 PadCommandExec(u8 cmdByte);

	u8 ButtonQuery(u8 cmdByte) noexcept;
	u8 Poll(u8 cmdByte) noexcept;
	u8 Config(u8 cmdByte) noexcept;
	u8 ModeSwitch(u8 cmdByte) noexcept;
	u8 StatusInfo(u8 cmdByte) noexcept;
	u8 Constant1(u8 cmdByte) noexcept;
	u8 Constant2(u8 cmdByte) noexcept;
	u8 Constant3(u8 cmdByte) noexcept;
	u8 VibrationMap(u8 cmdByte) noexcept;
	u8 AnalogEdit(u8 cmdByte) noexcept;
};
