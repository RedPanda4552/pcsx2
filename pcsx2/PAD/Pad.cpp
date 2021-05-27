
#include "PrecompiledHeader.h"

#include "Pad.h"

#include "Utilities/pxStreams.h"
#include "App.h"

#define currentPs2Controller ps2Controllers.at(padState.currentPort).at(padState.currentSlot)

Pad::Pad()
{
	ps2Controllers.at(0).at(0).Debug_SetBindings();
}

Pad::~Pad()
{
	
}

void Pad::UpdateBoundInputs()
{
	InputMain* inputMain = wxGetApp().inputMain;

#ifdef _WIN32
	// Xinput
	const size_t xinputSize = currentPs2Controller.xinputBindings.size();

	for (size_t i = 0; i < xinputSize; i++)
	{
		Binding_Xinput* binding = currentPs2Controller.xinputBindings.at(i);

		// If the Xinput control we are mapping is a standard button
		if (binding->GetButtonMask() != 0)
		{
			bool buttonValue = inputMain->inputInterface_Xinput->GetButtonValue(binding->GetXinputId(), binding->GetButtonMask());

			if (PS2Controller::IsPS2ControlButton(binding->GetPS2Control()))
			{
				currentPs2Controller.SetButton(binding->GetPS2Control(), buttonValue ? 0xff : 0x00);
			}
			else if (PS2Controller::IsPS2ControlAnalog(binding->GetPS2Control()))
			{
				currentPs2Controller.SetAnalog(binding->GetPS2Control(), buttonValue ? 0xff : 0x00);
			}
		}
		// If the Xinput control we are mapping is a trigger
		else if (binding->GetTriggerType() != XinputTriggerType::NONE)
		{
			BYTE triggerValue = inputMain->inputInterface_Xinput->GetTriggerValue(binding->GetXinputId(), binding->GetTriggerType());

			if (PS2Controller::IsPS2ControlButton(binding->GetPS2Control()))
			{
				currentPs2Controller.SetButton(binding->GetPS2Control(), triggerValue);
			}
			else if (PS2Controller::IsPS2ControlAnalog(binding->GetPS2Control()))
			{
				currentPs2Controller.SetAnalog(binding->GetPS2Control(), triggerValue);
			}
		}
		// If the Xinput control we are mapping is an analog
		else if (binding->GetAnalogType() != XinputAnalogType::NONE)
		{
			SHORT analogValue = inputMain->inputInterface_Xinput->GetAnalogValue(binding->GetXinputId(), binding->GetAnalogType());
			float f = (float)analogValue / 0x7fff;

			// If negative, invert so our scaling factor is still positive/usable.
			if (f < 0)
			{
				f *= -1;
			}

			// If greater than 1 (should only occur on negative component, since it reaches -32768 as opposed to the max positive 32767), ceiling at 1.
			if (f > 1)
			{
				f = 1;
			}

			// Compare to deadzone, drop if lower.
			if (f <= binding->GetDeadzone())
			{
				f = 0;
			}

			u8 analogValueNormalized = f * 0xff;

			if (PS2Controller::IsPS2ControlButton(binding->GetPS2Control()))
			{
				currentPs2Controller.SetButton(binding->GetPS2Control(), analogValueNormalized);
			}
			else if (PS2Controller::IsPS2ControlAnalog(binding->GetPS2Control()))
			{
				currentPs2Controller.SetAnalog(binding->GetPS2Control(), analogValueNormalized);
			}
		}
		// If the Xinput control we are mapping is a vibration motor
		else if (binding->GetXinputVibrationMotor() != XinputVibrationMotor::NONE)
		{
			if (binding->GetPS2VibrationMotor() == XinputVibrationMotor::SMALL)
			{
				float f = (float)currentPs2Controller.controllerState.vibrationStates.smallMotor / 0xff;
				WORD vibrationNormalized = f * 0xffff;
				inputMain->inputInterface_Xinput->StageVibration(binding->GetXinputId(), binding->GetXinputVibrationMotor(), vibrationNormalized);
			}
			else if (binding->GetPS2VibrationMotor() == XinputVibrationMotor::LARGE)
			{
				float f = (float)currentPs2Controller.controllerState.vibrationStates.largeMotor / 0xff;
				WORD vibrationNormalized = f * 0xffff;
				inputMain->inputInterface_Xinput->StageVibration(binding->GetXinputId(), binding->GetXinputVibrationMotor(), vibrationNormalized);
			}
		}
	}
	
	// Lastly for Xinput, send all staged vibrations at once (as opposed to sending them immediately while processing;
	// this ensures both motors kick on at once instead of staggered depending on the order of the bindings vector).
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		inputMain->inputInterface_Xinput->SendVibration(i);
	}

	// Windows Keyboard
	const size_t windowsKeyboardSize = currentPs2Controller.windowsKeyboardBindings.size();

	for (size_t i = 0; i < windowsKeyboardSize; i++)
	{
		Binding_WindowsKeyboard* binding = currentPs2Controller.windowsKeyboardBindings.at(i);
		SHORT keyState = inputMain->inputInterface_WindowsKeyboard->keyStates[binding->GetVkeyId()];

		if (PS2Controller::IsPS2ControlButton(binding->GetPS2Control()))
		{
			currentPs2Controller.SetButton(binding->GetPS2Control(), (keyState & 0x80) ? 0xff : 0x00);
		}
		else if (PS2Controller::IsPS2ControlAnalog(binding->GetPS2Control()))
		{
			currentPs2Controller.SetAnalog(binding->GetPS2Control(), (keyState & 0x80) ? 0xff : 0x00);
		}
	}
	
	// TODO: Nefarius' PS3 driver?
#endif
#ifdef __linux__
	// TODO: evdev
	// TODO: Does a POSIX keyboard lib exist?
#endif
#ifdef __APPLE__
	// TODO: Does a POSIX keyboard lib exist?
#endif
	// TODO: HID PS4
	// TODO: HID Switch Pro
}

u8 Pad::PadCommandInit(u8 port, u8 slot)
{
	this->padState.cmdBytesReceived = 1;
	this->padState.padCommandType = PadCommandType::NOT_SET;
	this->padState.currentPort = port;
	this->padState.currentSlot = slot;
	this->UpdateBoundInputs();
	return 0xff;
}

u8 Pad::PadCommandExec(u8 cmdByte)
{
	this->padState.cmdBytesReceived++;

	if (this->padState.cmdBytesReceived == 2)
	{
		this->padState.padCommandType = static_cast<PadCommandType>(cmdByte);
		return static_cast<u8>(this->currentPs2Controller.controllerState.currentPadMode);
	}
	// The third byte of a command is always 0x00, and always gets a 0x5a response.
	else if (this->padState.cmdBytesReceived == 3)
	{
		return 0x5a;
	}
	else
	{
		switch (this->padState.padCommandType)
		{
			case PadCommandType::MYSTERY:
				return Mystery(cmdByte);
			case PadCommandType::BUTTON_QUERY:
				return ButtonQuery(cmdByte); // TODO: Some non-return paths
			case PadCommandType::POLL:
				return Poll(cmdByte);
			case PadCommandType::CONFIG:
				return Config(cmdByte);
			case PadCommandType::MODE_SWITCH:
				return ModeSwitch(cmdByte); // TODO: Some non-return paths
			case PadCommandType::STATUS_INFO:
				return StatusInfo(cmdByte);
			case PadCommandType::CONST_1:
				return Constant1(cmdByte);
			case PadCommandType::CONST_2:
				return Constant2(cmdByte);
			case PadCommandType::CONST_3:
				return Constant3(cmdByte);
			case PadCommandType::VIBRATION_MAP:
				return VibrationMap(cmdByte);
			case PadCommandType::ANALOG_EDIT:
				return ResponseBytes(cmdByte);
			default:
				DevCon.Warning("(PAD) Second byte in PAD command (%02X) did not match any known PAD modes!", this->padState.padCommandType);
				return 0xff;
		}
	}
}

u8 Pad::Mystery(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
		case 5:
			return 0x00;
		case 6:
			return 0x02;
		case 7:
		case 8:
			return 0x00;
		case 9:
			return 0x5a;
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->padState.cmdBytesReceived);
			return 0xff;
	}
}

u8 Pad::ButtonQuery(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	// last (9th) byte returned is always 0x5a
	if (this->padState.cmdBytesReceived == 9)
	{
		return 0x5a;
	}

	if (this->currentPs2Controller.controllerState.targetPadMode == PadMode::DIGITAL)
	{
		return 0x00;
	}
	else if (this->currentPs2Controller.controllerState.targetPadMode == PadMode::ANALOG)
	{
		switch (this->padState.cmdBytesReceived)
		{
			case 4:
				return 0x3f;
			default:
				return 0x00;
		}
	}
	else if (this->currentPs2Controller.controllerState.targetPadMode == PadMode::DUALSHOCK2)
	{
		switch (this->padState.cmdBytesReceived)
		{
			case 4:
			case 5:
				return 0xff;
			case 6:
				return 0x03;
			default:
				return 0x00;
		}
	}
}

u8 Pad::Poll(u8 cmdByte, bool skipVibration)
{
	// Sanity check
	if (this->padState.cmdBytesReceived < 4)
	{
		pxAssert(false, "Pad::Poll invoked prematurely before 4th command byte received");
	}
	// Digital Byte 1
	else if (this->padState.cmdBytesReceived == 4)
	{
		if (!skipVibration)
		{
			// If less than 0xff, flush to zero (only 0xff enables small motor)
			this->currentPs2Controller.SetVibration(VibrationMotor::SMALL, cmdByte < 0xff ? 0 : cmdByte);
		}

		return this->currentPs2Controller.GetFirstDigitalByte();
	}
	// Digital Byte 2
	else if (this->padState.cmdBytesReceived == 5)
	{
		if (!skipVibration)
		{
			this->currentPs2Controller.SetVibration(VibrationMotor::LARGE, cmdByte);
		}

		return this->currentPs2Controller.GetSecondDigitalByte();
	}
	// Analog Axes (only requested in modes 0x73 (Analog) and 0x79 (Dualshock 2)
	else if (this->padState.cmdBytesReceived <= 9)
	{
		return this->currentPs2Controller.GetAnalog(static_cast<PS2Control>(this->padState.cmdBytesReceived));
	}
	// Pressures (only requested in mode 0x79 (Dualshock 2)
	else if (this->padState.cmdBytesReceived <= 21)
	{
		return this->currentPs2Controller.GetButton(static_cast<PS2Control>(this->padState.cmdBytesReceived));
	}
	else
	{
		DevCon.Warning("%s(%02X) overran max expected length (%d > 21)", __FUNCTION__, cmdByte, this->padState.cmdBytesReceived);
		return 0x00;
	}
}

u8 Pad::Config(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			if (cmdByte == 0x00)
			{
				if (this->currentPs2Controller.controllerState.currentPadMode == PadMode::CONFIG)
				{
					this->currentPs2Controller.controllerState.currentPadMode = this->currentPs2Controller.controllerState.targetPadMode;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unexpected exit while not in config mode", __FUNCTION__, cmdByte);
				}
			}
			else if (cmdByte == 0x01)
			{
				if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
				{
					this->currentPs2Controller.controllerState.currentPadMode = PadMode::CONFIG;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unexpected enter while already in config mode", __FUNCTION__, cmdByte);
				}
			}
			else
			{
				DevCon.Warning("%s(%02X) Unexpected enter/exit byte (%d > 1)", __FUNCTION__, cmdByte, cmdByte);
			}
			break;
		default:
			// Technically, the controller is not supposed to respond with pad states while in config mode. Specifically:
			// The first 0x43 command, with byte 4 = 0x01 to signal it is entering config mode, will still return states but
			// not set vibration. Any subsequent 0x43 commands will not return any pad states. However, there is no harm in
			// returning them anyways, the game will simply not use them.
			return this->Poll(cmdByte, true);
	}

	return 0xff;
}

u8 Pad::ModeSwitch(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			if (cmdByte == 0x01)
			{
				this->currentPs2Controller.controllerState.analogLight = AnalogLight::ON;
				this->currentPs2Controller.controllerState.targetPadMode = PadMode::ANALOG;
			}
			else if (cmdByte == 0x00)
			{
				this->currentPs2Controller.controllerState.analogLight = AnalogLight::OFF;
				this->currentPs2Controller.controllerState.targetPadMode = PadMode::DIGITAL;
			}
			else
			{
				DevCon.Warning("%s(%02X) Unexpected 4th byte (%d > 1)", __FUNCTION__, cmdByte, cmdByte);
			}
			break;
		case 5:
			this->currentPs2Controller.controllerState.analogLightLocked = (cmdByte == 0x03);
			break;
		default:
			return 0x00;
	}
}

u8 Pad::StatusInfo(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			return static_cast<u8>(currentPs2Controller.controllerState.physicalType);
		case 5:
			return 0x02; // magic!
		case 6:
			return static_cast<u8>(currentPs2Controller.controllerState.analogLight);
		case 7:
			return 0x02;
		case 8:
			return 0x01;
		case 9:
			return 0x00;
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->padState.cmdBytesReceived);
			return 0xff;
	}
}

u8 Pad::Constant1(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			this->currentPs2Controller.controllerState.constantStage = cmdByte;
		case 5:
			return 0x00;
		case 6:
			if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::STANDARD)
			{
				return 0x00;
			}
			else
			{
				return 0x01;
			}
		case 7:
			if (!this->currentPs2Controller.controllerState.constantStage)
			{
				return 0x02;
			}
			else
			{
				if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::STANDARD)
				{
					return 0x00;
				}
				else if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::GUITAR)
				{
					return 0x01;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unrecognized physical type (%02X)", __FUNCTION__, cmdByte, this->currentPs2Controller.controllerState.physicalType);
					return 0x00;
				}
			}
		case 8:
			if (!this->currentPs2Controller.controllerState.constantStage)
			{
				return 0x00;
			}
			else
			{
				if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::STANDARD)
				{
					return 0x00;
				}
				else if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::GUITAR)
				{
					return 0x01;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unrecognized physical type (%02X)", __FUNCTION__, cmdByte, this->currentPs2Controller.controllerState.physicalType);
					return 0x00;
				}
			}
		case 9:
			if (!this->currentPs2Controller.controllerState.constantStage)
			{
				return 0x0A;
			}
			else
			{
				return 0x14;
			}
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->padState.cmdBytesReceived);
			return 0x00;
	}
}

u8 Pad::Constant2(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
		case 5:
			return 0x00;
		case 6:
			return 0x02;
		case 7:
			return 0x00;
		case 8:
			if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::STANDARD)
			{
				return 0x00;
			}
			else if (this->currentPs2Controller.controllerState.physicalType == PhysicalType::GUITAR)
			{
				return 0x01;
			}
			else
			{
				DevCon.Warning("%s(%02X) Unrecognized physical type (%02X)", __FUNCTION__, cmdByte, this->currentPs2Controller.controllerState.physicalType);
				return 0x00;
			}
		case 9:
			return 0x00;
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->padState.cmdBytesReceived);
			return 0x00;
	}
}

u8 Pad::Constant3(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPs2Controller.controllerState.currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			this->currentPs2Controller.controllerState.constantStage = cmdByte;
		case 5:
		case 6:
			return 0x00;
		case 7:
			// Since documentation doesn't bother explaining this one...
			// (thanks padtest_ps2.elf for actually sheding some light on this!)
			// This byte, on each run of the command, specifies one of the controller's operating modes.
			// So far we know that (of the ones that actually matter) 0x04 = digital, 0x07 = analog.
			// This seems to correspond with assertions which are made about the "pad modes" being
			// 0x41 = digital, 0x73 = analog, 0x79 = dualshock 2. It does leave the question of if these
			// "constant" commands should also have a value somewhere for the second nibble of the pad mode...
			if (!this->currentPs2Controller.controllerState.constantStage)
			{
				return 0x04;
			}
			else
			{
				return 0x07;
			}
		case 8:
		case 9:
			return 0x00;
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->padState.cmdBytesReceived);
			return 0x00;
	}
}
u8 Pad::VibrationMap(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			return 0x00;
		case 5:
			return 0x01;
		default:
			return 0xff;
	}
}

u8 Pad::ResponseBytes(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	switch (this->padState.cmdBytesReceived)
	{
		case 4:
			this->currentPs2Controller.controllerState.buttonMask = cmdByte;
			break;
		case 5:
			this->currentPs2Controller.controllerState.buttonMask |= (cmdByte << 8);
			break;
		case 6:
			this->currentPs2Controller.controllerState.buttonMask |= (cmdByte << 16);
			break;
		case 9:
			switch (this->currentPs2Controller.controllerState.buttonMask)
			{
				case 0x0003ffff:
					this->currentPs2Controller.controllerState.analogLight = AnalogLight::ON;
					this->currentPs2Controller.controllerState.targetPadMode = PadMode::DUALSHOCK2;
					break;
				case 0x3f:
					this->currentPs2Controller.controllerState.analogLight = AnalogLight::ON;
					this->currentPs2Controller.controllerState.targetPadMode = PadMode::ANALOG;
					break;
				case 0x03:
					this->currentPs2Controller.controllerState.analogLight = AnalogLight::OFF;
					this->currentPs2Controller.controllerState.targetPadMode = PadMode::DIGITAL;
					break;
			}
			return 0x5a;
		default:
			break;
	}

	return 0x00;
}

void Pad::SaveState(void* dest)
{
	Console.Indent().WriteLn("Saving PAD");
	PadSaveState padSaveState;
	padSaveState.version = PAD_STATE_VERSION;
	padSaveState.padState = this->padState;
	
	for (size_t i = 0; i < MAX_PORTS; i++)
	{
		for (size_t j = 0; j < MAX_SLOTS; j++)
		{
			padSaveState.controllerStates[i][j] = this->ps2Controllers.at(i).at(j).controllerState;
		}
	}

	memcpy(dest, &padSaveState, sizeof(PadState));
}

void Pad::LoadState(pxInputStream& reader)
{
	Console.Indent().WriteLn("Loading PAD");

	if (!reader.IsOk() || !reader.Length())
	{
		Console.Warning("(PAD) Something went wrong when trying to read the save state. Ignoring it and running with whatever the previous state was...");
		return;
	}

	PadSaveState padSaveState;
	reader.Read(&padSaveState, sizeof(PadSaveState));

	if (padSaveState.version != PAD_STATE_VERSION)
	{
		Console.Indent().Warning("PAD version mismatch! Expected 0x%08X, state contains 0x%08X. PAD will not be loaded.", PAD_STATE_VERSION, padState.version);
		return;
	}

	this->padState = padSaveState.padState;

	for (size_t i = 0; i < MAX_PORTS; i++)
	{
		for (size_t j = 0; j < MAX_SLOTS; j++)
		{
			this->ps2Controllers.at(i).at(j).controllerState = padSaveState.controllerStates[i][j];
		}
	}
}

#undef currentPs2Controller
