
#pragma once

#include "PadTypes.h"
#include "PS2Controller.h"
#include "Input/InputInterface_Xinput.h"
#include "Input/InputInterface_WindowsKeyboard.h"

#include <array>

static constexpr u8 MAX_PORTS = 2;
static constexpr u8 MAX_SLOTS = 4;

class PadState
{
public:
	u32 version;
	PadCommandType padCommandType = PadCommandType::NOT_SET;
	// Number of command bytes received. This value is always current; PadCommandInit and PadCommandExec
	// can read this value while executing and always expect this value to reflect the number of command
	// bytes which have been received so far, including the command byte they are currently working on.
	// Running PadCommandInit will flush this to 1, and PadCommandExec will increment this by 1 before
	// doing anything else.
	u8 cmdBytesReceived = 0;
	u8 currentPort = 0, currentSlot = 0;
};

class Pad
{
private:
	PadState padState;
	std::array<std::array<PS2Controller, MAX_SLOTS>, MAX_PORTS> ps2Controllers;

public:
	Pad();
	~Pad();
	void UpdateBoundInputs();
	// Initiate a PAD command sequence. Should be called only once, on the first byte of a PAD command.
	// Specifies which PAD port/slot will be used, and resets most relevant fields.
	u8 PadCommandInit(u8 port, u8 slot);
	// Send a byte as part of a PAD command sequence. Used to handle each command byte after the first.
	// This will track the specific command type and execute the appropriate response payload.
	u8 PadCommandExec(u8 cmdByte);

	u8 Mystery(u8 cmdByte);
	u8 ButtonQuery(u8 cmdByte);
	u8 Poll(u8 cmdByte, bool skipVibration = false);
	u8 Config(u8 cmdByte);
	u8 ModeSwitch(u8 cmdByte);
	u8 StatusInfo(u8 cmdByte);
	u8 Constant1(u8 cmdByte);
	u8 Constant2(u8 cmdByte);
	u8 Constant3(u8 cmdByte);
	u8 VibrationMap(u8 cmdByte);
	u8 ResponseBytes(u8 cmdByte);

	void SaveState(void* dest);
	void LoadState(pxInputStream& reader);
};

static const u32 PAD_STATE_VERSION = 0x00000007;

class PadSaveState
{
public:
	u32 version;
	PadState padState;
	ControllerState controllerStates[MAX_PORTS][MAX_SLOTS];
};

