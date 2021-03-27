
#include "PrecompiledHeader.h"

#include "Pad.h"

Pad::Pad()
{
#ifdef _WINDOWS
	this->inputInterface_Xinput = new InputInterface_Xinput();
	this->inputInterface_WindowsKeyboard = new InputInterface_WindowsKeyboard();
#endif
}

Pad::~Pad()
{
#ifdef _WINDOWS
	delete this->inputInterface_Xinput;
	delete this->inputInterface_WindowsKeyboard;
#endif
}

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
		Binding_Xinput binding = ps2Controller->xinputBindings.at(i);
		
		if (binding.GetButtonMask() != 0)
		{
			bool buttonValue = this->inputInterface_Xinput->GetButtonValue(binding.GetXinputId(), binding.GetButtonMask());
			ps2Controller->SetButton(binding.GetPS2Control(), buttonValue);
		}
		else if (static_cast<u8>(binding.GetTriggerType()) != 0)
		{
			BYTE triggerValue = this->inputInterface_Xinput->GetTriggerValue(binding.GetXinputId(), binding.GetTriggerType());
			ps2Controller->SetButton(binding.GetPS2Control(), triggerValue);
		}
		else if (static_cast<u8>(binding.GetAnalogType()) != 0)
		{
			SHORT analogValue = this->inputInterface_Xinput->GetAnalogValue(binding.GetXinputId(), binding.GetAnalogType());
			u8 analogValueNormalized = 0x7f + (analogValue / 0xff); 
			ps2Controller->SetAnalog(binding.GetPS2Control(), analogValueNormalized);
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

u8 Pad::PadCommandInit(u8 port, u8 slot) noexcept
{
	cmdBytesReceived = 1;
	padCommandType = PadCommandType::NOT_SET;
	currentPort = port;
	currentSlot = slot;
	return 0xff;
}

u8 Pad::PadCommandExec(u8 cmdByte)
{
	cmdBytesReceived++;

	if (cmdBytesReceived == 2)
	{
		padCommandType = static_cast<PadCommandType>(cmdByte);
		PS2Controller* ps2Controller = ps2Controllers[currentPort][currentSlot];
		return static_cast<u8>(ps2Controller->currentPadMode);
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

u8 Pad::ButtonQuery(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::Poll(u8 cmdByte) noexcept
{
	// Digital byte 1
	if (this->cmdBytesReceived == 4)
	{
		return ps2Controllers[currentPort][currentSlot]->GetFirstDigitalByte();
	}
	// Digital byte 2
	else if (this->cmdBytesReceived == 5)
	{
		return ps2Controllers[currentPort][currentSlot]->GetSecondDigitalByte();
	}
	// Analog bytes (6-9)
	else if (this->cmdBytesReceived <= 9)
	{
		return 0x7f;
	}
	// Pressure bytes (10-21)
	else if (this->cmdBytesReceived <= 21)
	{
		return 0x00;
	}
	else
	{
		return 0xff;
	}
}

u8 Pad::Config(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::ModeSwitch(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::StatusInfo(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::Constant1(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::Constant2(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::Constant3(u8 cmdByte) noexcept
{
	return 0xff;
}
u8 Pad::VibrationMap(u8 cmdByte) noexcept
{
	return 0xff;
}

u8 Pad::AnalogEdit(u8 cmdByte) noexcept
{
	return 0xff;
}
