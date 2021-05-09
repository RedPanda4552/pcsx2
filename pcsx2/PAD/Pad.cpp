
#include "PrecompiledHeader.h"

#include "Pad.h"


#include "Utilities/pxStreams.h"
#include "App.h"

Pad::Pad()
{
	for (size_t i = 0; i < MAX_PORTS; i++)
	{
		for (size_t j = 0; j < MAX_SLOTS; j++)
		{
			ps2Controllers[i][j] = new PS2Controller();
		}
	}

	ps2Controllers[0][0]->Debug_SetBindings();
}

Pad::~Pad()
{
	for (size_t i = 0; i < MAX_PORTS; i++)
	{
		for (size_t j = 0; j < MAX_SLOTS; j++)
		{
			delete ps2Controllers[i][j];
		}
	}
}

void Pad::UpdateBoundInputs(PS2Controller* ps2Controller)
{
	InputMain* inputMain = wxGetApp().inputMain;

#ifdef _WIN32
	const size_t xinputSize = ps2Controller->xinputBindings.size();

	for (size_t i = 0; i < xinputSize; i++)
	{
		Binding_Xinput* binding = ps2Controller->xinputBindings.at(i);

		if (binding->GetButtonMask() != 0)
		{
			bool buttonValue = inputMain->inputInterface_Xinput->GetButtonValue(binding->GetXinputId(), binding->GetButtonMask());
			ps2Controller->SetButton(binding->GetPS2Control(), buttonValue ? 0xff : 0x00);
		}
		else if (binding->GetTriggerType() != XinputTriggerType::NONE)
		{
			BYTE triggerValue = inputMain->inputInterface_Xinput->GetTriggerValue(binding->GetXinputId(), binding->GetTriggerType());
			ps2Controller->SetButton(binding->GetPS2Control(), triggerValue);
		}
		else if (binding->GetAnalogType() != XinputAnalogType::NONE)
		{
			SHORT analogValue = inputMain->inputInterface_Xinput->GetAnalogValue(binding->GetXinputId(), binding->GetAnalogType());
			s32 larger = analogValue + 0x8000;
			float f = (float)larger / 0xffff;
			u8 analogValueNormalized = f * 0xff;

			if (binding->GetPS2Control() == PS2Control::LEFT_Y || binding->GetPS2Control() == PS2Control::RIGHT_Y)
			{
				analogValueNormalized = 0xff - analogValueNormalized;
			}

			ps2Controller->SetAnalog(binding->GetPS2Control(), analogValueNormalized);
		}
		else if (binding->GetPS2VibrationMotor() != XinputVibrationMotor::NONE)
		{
			if (binding->GetPS2VibrationMotor() == XinputVibrationMotor::SMALL)
			{
				float f = (float)ps2Controller->vibrationStates.smallMotor / 0xff;
				WORD vibrationNormalized = f * 0xffff;
				inputMain->inputInterface_Xinput->StageVibration(binding->GetXinputId(), binding->GetXinputVibrationMotor(), vibrationNormalized);
			}
			else if (binding->GetPS2VibrationMotor() == XinputVibrationMotor::LARGE)
			{
				float f = (float)ps2Controller->vibrationStates.largeMotor / 0xff;
				WORD vibrationNormalized = f * 0xffff;
				inputMain->inputInterface_Xinput->StageVibration(binding->GetXinputId(), binding->GetXinputVibrationMotor(), vibrationNormalized);
			}
		}
	}

	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		inputMain->inputInterface_Xinput->SendVibration(i);
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
	this->cmdBytesReceived = 1;
	this->padCommandType = PadCommandType::NOT_SET;
	this->currentPort = port;
	this->currentSlot = slot;
	this->currentPS2Controller = ps2Controllers[currentPort][currentSlot];
	this->UpdateBoundInputs(currentPS2Controller);
	return 0xff;
}

u8 Pad::PadCommandExec(u8 cmdByte)
{
	cmdBytesReceived++;

	if (cmdBytesReceived == 2)
	{
		padCommandType = static_cast<PadCommandType>(cmdByte);
		return static_cast<u8>(this->currentPS2Controller->currentPadMode);
	}
	// The third byte of a command is always 0x00, and always gets a 0x5a response.
	else if (cmdBytesReceived == 3)
	{
		return 0x5a;
	}
	else
	{
		switch (padCommandType)
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
				DevCon.Warning("(PAD) Second byte in PAD command (%02X) did not match any known PAD modes!", padCommandType);
				return 0xff;
		}
	}
}

u8 Pad::Mystery(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->cmdBytesReceived)
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
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, cmdBytesReceived);
			return 0xff;
	}
}

u8 Pad::ButtonQuery(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	// last (9th) byte returned is always 0x5a
	if (this->cmdBytesReceived == 9)
	{
		return 0x5a;
	}

	if (this->currentPS2Controller->targetPadMode == PadMode::DIGITAL)
	{
		return 0x00;
	}
	else if (this->currentPS2Controller->targetPadMode == PadMode::ANALOG)
	{
		switch (this->cmdBytesReceived)
		{
			case 4:
				return 0x3f;
			default:
				return 0x00;
		}
	}
	else if (this->currentPS2Controller->targetPadMode == PadMode::DUALSHOCK2)
	{
		switch (this->cmdBytesReceived)
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
	switch (this->cmdBytesReceived)
	{
		// Digital Bytes
		case 4:
			if (!skipVibration)
			{
				// If less than 0xff, flush to zero (only 0xff enables small motor)
				this->currentPS2Controller->SetVibration(VibrationMotor::SMALL, cmdByte < 0xff ? 0 : cmdByte);
			}

			return this->currentPS2Controller->GetFirstDigitalByte();
		case 5:
			if (!skipVibration)
			{
				this->currentPS2Controller->SetVibration(VibrationMotor::LARGE, cmdByte);
			}

			return this->currentPS2Controller->GetSecondDigitalByte();
		// Analog Axes (only requested in modes 0x73 (Analog) and 0x79 (Dualshock 2)
		case 6:
			return this->currentPS2Controller->GetAnalog(PS2Control::RIGHT_X);
		case 7:
			return this->currentPS2Controller->GetAnalog(PS2Control::RIGHT_Y);
		case 8:
			return this->currentPS2Controller->GetAnalog(PS2Control::LEFT_X);
		case 9:
			return this->currentPS2Controller->GetAnalog(PS2Control::LEFT_Y);
		// Pressures (only requested in mode 0x79 (Dualshock 2)
		case 10:
			return this->currentPS2Controller->GetButton(PS2Control::RIGHT);
		case 11:
			return this->currentPS2Controller->GetButton(PS2Control::LEFT);
		case 12:
			return this->currentPS2Controller->GetButton(PS2Control::UP);
		case 13:
			return this->currentPS2Controller->GetButton(PS2Control::DOWN);
		case 14:
			return this->currentPS2Controller->GetButton(PS2Control::TRIANGLE);
		case 15:
			return this->currentPS2Controller->GetButton(PS2Control::CIRCLE);
		case 16:
			return this->currentPS2Controller->GetButton(PS2Control::CROSS);
		case 17:
			return this->currentPS2Controller->GetButton(PS2Control::SQUARE);
		case 18:
			return this->currentPS2Controller->GetButton(PS2Control::L1);
		case 19:
			return this->currentPS2Controller->GetButton(PS2Control::R1);
		case 20:
			return this->currentPS2Controller->GetButton(PS2Control::L2);
		case 21:
			return this->currentPS2Controller->GetButton(PS2Control::R2);
		default:
			DevCon.Warning("%s(%02X) overran max expected length (%d > 21)", __FUNCTION__, cmdByte, this->cmdBytesReceived);
			return 0x00;
	}
}

u8 Pad::Config(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	switch (this->cmdBytesReceived)
	{
		case 4:
			if (cmdByte == 0x00)
			{
				if (this->currentPS2Controller->currentPadMode == PadMode::CONFIG)
				{
					this->currentPS2Controller->currentPadMode = this->currentPS2Controller->targetPadMode;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unexpected exit while not in config mode", __FUNCTION__, cmdByte);
				}
			}
			else if (cmdByte == 0x01)
			{
				if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
				{
					this->currentPS2Controller->currentPadMode = PadMode::CONFIG;
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

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->cmdBytesReceived)
	{
		case 4:
			if (cmdByte == 0x01)
			{
				this->currentPS2Controller->analogLight = AnalogLight::ON;
				this->currentPS2Controller->targetPadMode = PadMode::ANALOG;
			}
			else if (cmdByte == 0x00)
			{
				this->currentPS2Controller->analogLight = AnalogLight::OFF;
				this->currentPS2Controller->targetPadMode = PadMode::DIGITAL;
			}
			else
			{
				DevCon.Warning("%s(%02X) Unexpected 4th byte (%d > 1)", __FUNCTION__, cmdByte, cmdByte);
			}
			break;
		case 5:
			this->currentPS2Controller->analogLightLocked = (cmdByte == 0x03);
			break;
		default:
			return 0x00;
	}
}

u8 Pad::StatusInfo(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->cmdBytesReceived)
	{
		case 4:
			return static_cast<u8>(ps2Controllers[currentPort][currentSlot]->physicalType);
		case 5:
			return 0x02; // magic!
		case 6:
			return static_cast<u8>(ps2Controllers[currentPort][currentSlot]->analogLight);
		case 7:
			return 0x02;
		case 8:
			return 0x01;
		case 9:
			return 0x00;
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, cmdBytesReceived);
			return 0xff;
	}
}

u8 Pad::Constant1(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->cmdBytesReceived)
	{
		case 4:
			this->currentPS2Controller->constantStage = cmdByte;
		case 5:
			return 0x00;
		case 6:
			if (this->currentPS2Controller->physicalType == PhysicalType::STANDARD)
			{
				return 0x00;
			}
			else
			{
				return 0x01;
			}
		case 7:
			if (!this->currentPS2Controller->constantStage)
			{
				return 0x02;
			}
			else
			{
				if (this->currentPS2Controller->physicalType == PhysicalType::STANDARD)
				{
					return 0x00;
				}
				else if (this->currentPS2Controller->physicalType == PhysicalType::GUITAR)
				{
					return 0x01;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unrecognized physical type (%02X)", __FUNCTION__, cmdByte, this->currentPS2Controller->physicalType);
					return 0x00;
				}
			}
		case 8:
			if (!this->currentPS2Controller->constantStage)
			{
				return 0x00;
			}
			else
			{
				if (this->currentPS2Controller->physicalType == PhysicalType::STANDARD)
				{
					return 0x00;
				}
				else if (this->currentPS2Controller->physicalType == PhysicalType::GUITAR)
				{
					return 0x01;
				}
				else
				{
					DevCon.Warning("%s(%02X) Unrecognized physical type (%02X)", __FUNCTION__, cmdByte, this->currentPS2Controller->physicalType);
					return 0x00;
				}
			}
		case 9:
			if (!this->currentPS2Controller->constantStage)
			{
				return 0x0A;
			}
			else
			{
				return 0x14;
			}
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->cmdBytesReceived);
			return 0x00;
	}
}

u8 Pad::Constant2(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->cmdBytesReceived)
	{
		case 4:
		case 5:
			return 0x00;
		case 6:
			return 0x02;
		case 7:
			return 0x00;
		case 8:
			if (this->currentPS2Controller->physicalType == PhysicalType::STANDARD)
			{
				return 0x00;
			}
			else if (this->currentPS2Controller->physicalType == PhysicalType::GUITAR)
			{
				return 0x01;
			}
			else
			{
				DevCon.Warning("%s(%02X) Unrecognized physical type (%02X)", __FUNCTION__, cmdByte, this->currentPS2Controller->physicalType);
				return 0x00;
			}
		case 9:
			return 0x00;
		default:
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->cmdBytesReceived);
			return 0x00;
	}
}

u8 Pad::Constant3(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	if (this->currentPS2Controller->currentPadMode != PadMode::CONFIG)
	{
		DevCon.Warning("%s(%02X) called outside of config mode", __FUNCTION__, cmdByte);
		return 0xff;
	}

	switch (this->cmdBytesReceived)
	{
		case 4:
			this->currentPS2Controller->constantStage = cmdByte;
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
			if (!this->currentPS2Controller->constantStage)
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
			DevCon.Warning("%s(%02X) Overran expected length (%d > 9)", __FUNCTION__, cmdByte, this->cmdBytesReceived);
			return 0x00;
	}
}
u8 Pad::VibrationMap(u8 cmdByte)
{
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);

	switch (this->cmdBytesReceived)
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

	switch (this->cmdBytesReceived)
	{
		case 4:
			this->currentPS2Controller->buttonMask = cmdByte;
			break;
		case 5:
			this->currentPS2Controller->buttonMask |= (cmdByte << 8);
			break;
		case 6:
			this->currentPS2Controller->buttonMask |= (cmdByte << 16);
			break;
		case 9:
			switch (this->currentPS2Controller->buttonMask)
			{
				case 0x0003ffff:
					this->currentPS2Controller->analogLight = AnalogLight::ON;
					this->currentPS2Controller->targetPadMode = PadMode::DUALSHOCK2;
					break;
				case 0x3f:
					this->currentPS2Controller->analogLight = AnalogLight::ON;
					this->currentPS2Controller->targetPadMode = PadMode::ANALOG;
					break;
				case 0x03:
					this->currentPS2Controller->analogLight = AnalogLight::OFF;
					this->currentPS2Controller->targetPadMode = PadMode::DIGITAL;
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
	PadState padState;
	padState.version = PAD_STATE_VERSION;
	padState.padCommandType = this->padCommandType;
	padState.cmdBytesReceived = this->cmdBytesReceived;
	padState.currentPort = this->currentPort;
	padState.currentSlot = this->currentSlot;

	for (size_t i = 0; i < MAX_PORTS; i++)
	{
		for (size_t j = 0; j < MAX_SLOTS; j++)
		{
			ControllerState controllerState;
			PS2Controller* activeController = this->ps2Controllers[i][j];
			controllerState.physicalType = activeController->physicalType;
			controllerState.analogLight = activeController->analogLight;
			controllerState.targetPadMode = activeController->targetPadMode;
			controllerState.currentPadMode = activeController->currentPadMode;
			controllerState.buttonStates = activeController->buttonStates;
			controllerState.analogStates = activeController->analogStates;
			controllerState.guitarStates = activeController->guitarStates;
			controllerState.vibrationStates = activeController->vibrationStates;
			controllerState.analogLightLocked = activeController->analogLightLocked;
			controllerState.constantStage = activeController->constantStage;
			controllerState.buttonMask = activeController->buttonMask;
			padState.controllerStates[i][j] = controllerState;
		}
	}

	memcpy(dest, &padState, sizeof(PadState));
}

void Pad::LoadState(pxInputStream& reader)
{
	Console.Indent().WriteLn("Loading PAD");

	if (!reader.IsOk() || !reader.Length())
	{
		Console.Warning("(PAD) Something went wrong when trying to read the save state. Ignoring it and running with whatever the previous state was...");
		return;
	}

	PadState padState;
	reader.Read(&padState, sizeof(PadState));

	if (padState.version != PAD_STATE_VERSION)
	{
		Console.Indent().Warning("PAD version mismatch! Expected 0x%08X, state contains 0x%08X. PAD will not be loaded.", PAD_STATE_VERSION, padState.version);
		return;
	}
	
	this->padCommandType = padState.padCommandType;
	this->cmdBytesReceived = padState.cmdBytesReceived;
	this->currentPort = padState.currentPort;
	this->currentSlot = padState.currentSlot;

	for (size_t i = 0; i < MAX_PORTS; i++)
	{
		for (size_t j = 0; j < MAX_SLOTS; j++)
		{
			ControllerState controllerState = padState.controllerStates[i][j];
			PS2Controller* activeController = this->ps2Controllers[i][j];
			activeController->physicalType = controllerState.physicalType;
			activeController->analogLight = controllerState.analogLight;
			activeController->targetPadMode = controllerState.targetPadMode;
			activeController->currentPadMode = controllerState.currentPadMode;
			activeController->buttonStates = controllerState.buttonStates;
			activeController->analogStates = controllerState.analogStates;
			activeController->guitarStates = controllerState.guitarStates;
			activeController->vibrationStates = controllerState.vibrationStates;
			activeController->analogLightLocked = controllerState.analogLightLocked;
			activeController->constantStage = controllerState.constantStage;
			activeController->buttonMask = controllerState.buttonMask;
		}
	}
}
