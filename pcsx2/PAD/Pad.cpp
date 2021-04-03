
#include "PrecompiledHeader.h"

#include "Pad.h"

Pad::Pad()
{
#ifdef _WINDOWS
	this->inputInterface_Xinput = new InputInterface_Xinput();
	this->inputInterface_WindowsKeyboard = new InputInterface_WindowsKeyboard();

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			ps2Controllers[i][j] = new PS2Controller();
		}
	}
#endif
}

Pad::~Pad()
{
#ifdef _WINDOWS
	delete this->inputInterface_Xinput;
	delete this->inputInterface_WindowsKeyboard;
#endif
}

// FIXME: This should really be a PS2Controller member.
void Pad::UpdateBoundInputs(PS2Controller* ps2Controller)
{
#ifdef _WINDOWS
	const size_t xinputSize = ps2Controller->xinputBindings.size();

	if (xinputSize > 0)
	{
		this->inputInterface_Xinput->Poll();
	}

	for (size_t i = 0; i < xinputSize; i++)
	{
		Binding_Xinput* binding = ps2Controller->xinputBindings.at(i);
		
		if (binding->GetButtonMask() != 0)
		{
			bool buttonValue = this->inputInterface_Xinput->GetButtonValue(binding->GetXinputId(), binding->GetButtonMask());
			ps2Controller->SetButton(binding->GetPS2Control(), buttonValue ? 0xff : 0x00);
		}
		else if (static_cast<u8>(binding->GetTriggerType()) != 0)
		{
			BYTE triggerValue = this->inputInterface_Xinput->GetTriggerValue(binding->GetXinputId(), binding->GetTriggerType());
			ps2Controller->SetButton(binding->GetPS2Control(), triggerValue);
		}
		else if (static_cast<u8>(binding->GetAnalogType()) != 0)
		{
			SHORT analogValue = this->inputInterface_Xinput->GetAnalogValue(binding->GetXinputId(), binding->GetAnalogType());
			u8 analogValueNormalized = 0x7f + (analogValue / 0xff); 
			ps2Controller->SetAnalog(binding->GetPS2Control(), analogValueNormalized);
		}
	}

	const size_t winKeySize = ps2Controller->windowsKeyboardBindings.size();

	if (winKeySize > 0)
	{
		this->inputInterface_WindowsKeyboard->Poll();
	}

	for (size_t i = 0; i < winKeySize; i++)
	{
		// TODO: Set PS2 values
	}
#endif
}

u8 Pad::PadCommandInit(u8 port, u8 slot)
{
	DevCon.WriteLn("%s(%d, %d)", __FUNCTION__, port, slot);
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
//	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);
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
			case PadCommandType::BUTTON_QUERY:
				return ButtonQuery(cmdByte);
			case PadCommandType::POLL:
				return Poll(cmdByte);
			case PadCommandType::CONFIG:
				return Config(cmdByte);
			case PadCommandType::MODE_SWITCH:
				return ModeSwitch(cmdByte);
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
				return AnalogEdit(cmdByte);
			default:
				DevCon.Warning("(PAD) Second byte in PAD command (%02X) did not match any known PAD modes!", padCommandType);
				return 0xff;
		}
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
	DevCon.WriteLn("%s(%02X)", __FUNCTION__, cmdByte);
	// Digital byte 1
	if (this->cmdBytesReceived == 4)
	{
		if (!skipVibration)
		{
			// TODO: Implement PS2Controller::SetVibration()
		}
		
		return this->currentPS2Controller->GetFirstDigitalByte();
	}
	// Digital byte 2
	else if (this->cmdBytesReceived == 5)
	{
		if (!skipVibration)
		{
			// TODO: Implement PS2Controller::SetVibration()
		}
		
		return this->currentPS2Controller->GetSecondDigitalByte();
	}
	// Analog bytes (6-9)
	else if (this->cmdBytesReceived <= 9)
	{
		if (this->currentPS2Controller->targetPadMode == PadMode::DIGITAL)
		{
			DevCon.Warning("%s(%02X) Unexpected analog request in digital mode", __FUNCTION__, cmdByte);
			return 0x7f;
		}
		else
		{
			return 0x7f; // TODO: Implement PS2Controller::GetAnalog(PS2Control)
		}
	}
	// Pressure bytes (10-21)
	else if (this->cmdBytesReceived <= 21)
	{
		if (this->currentPS2Controller->targetPadMode == PadMode::DIGITAL)
		{
			DevCon.Warning("%s(%02X) Unexpected pressure request in digital mode", __FUNCTION__, cmdByte);
			return 0x00;
		}
		else if (this->currentPS2Controller->currentPadMode == PadMode::CONFIG)
		{
			DevCon.Warning("%s(%02X) Unexpected pressure request in config mode", __FUNCTION__, cmdByte);
			return 0x00;
		}
		else
		{
			return 0x00; // TODO: Implement PS2Controller::GetPressure(PS2Control)
		}
	}
	else
	{
		DevCon.Warning("%s(%02X) Overran expected length (%d > 21)", __FUNCTION__, cmdByte, cmdBytesReceived);
		return 0xff;
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
				this->currentPS2Controller->targetPadMode = PadMode::ANALOG;
			}
			else if (cmdByte == 0x00)
			{
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

// Expected DS2 Sequence:    00 00 02 00 0A / 00 00 00 00 14
// Expected Guitar Sequence: 00 01 02 00 0A / 00 01 01 01 14
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

// Expected DS2 Sequence:    00 02 00 00 00
// Expected Guitar Sequence: 00 02 00 01 00
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
	DevCon.Warning("Unimplemented: %s(%02X)", __FUNCTION__, cmdByte);
	return 0xff;
}
u8 Pad::VibrationMap(u8 cmdByte)
{
	DevCon.Warning("Unimplemented: %s(%02X)", __FUNCTION__, cmdByte);
	return 0xff;
}

u8 Pad::AnalogEdit(u8 cmdByte)
{
	DevCon.Warning("Unimplemented: %s(%02X)", __FUNCTION__, cmdByte);
	return 0xff;
}
