
#include "PrecompiledHeader.h"

#include "InputInterface_Xinput.h"

InputInterface_Xinput::InputInterface_Xinput() noexcept
{
	ZeroMemory(state, sizeof(XINPUT_STATE) * 4);
	ZeroMemory(vibration, sizeof(XINPUT_VIBRATION) * 4);
	ZeroMemory(lowFreqSpeed, sizeof(WORD) * XUSER_MAX_COUNT);
	ZeroMemory(highFreqSpeed, sizeof(WORD) * XUSER_MAX_COUNT);
}

InputInterface_Xinput::~InputInterface_Xinput() noexcept
{

}

bool InputInterface_Xinput::GetXinputState() noexcept
{
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		ZeroMemory(&state[i], sizeof(XINPUT_STATE));
		DWORD res = XInputGetState(i, &state[i]);

		if (res == ERROR_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool InputInterface_Xinput::StageVibration(const DWORD xinputId, const VibrationMotor xinputVibrationMotor, const WORD strength)
{
	XINPUT_ID_CHECK(xinputId);

	if (xinputVibrationMotor == VibrationMotor::LARGE)
	{
		lowFreqSpeed[xinputId] = strength;
	}
	else if (xinputVibrationMotor == VibrationMotor::SMALL)
	{
		highFreqSpeed[xinputId] = strength;
	}

	return true;
}

bool InputInterface_Xinput::SendVibration(const DWORD xinputId)
{
	XINPUT_ID_CHECK(xinputId);

	ZeroMemory(&vibration[xinputId], sizeof(XINPUT_VIBRATION));
	vibration->wLeftMotorSpeed = lowFreqSpeed[xinputId];
	vibration->wRightMotorSpeed = highFreqSpeed[xinputId];
	DWORD res = XInputSetState(xinputId, &vibration[xinputId]);

	if (res == ERROR_SUCCESS)
	{
		// Vibration updated
	}
	else
	{
		// Error
	}

	return true;
}

bool InputInterface_Xinput::GetButtonValue(const DWORD xinputId, const WORD buttonMask)
{
	XINPUT_ID_CHECK(xinputId);
	return (state[xinputId].Gamepad.wButtons & buttonMask) > 0;
}

BYTE InputInterface_Xinput::GetTriggerValue(const DWORD xinputId, const TriggerType triggerType)
{
	XINPUT_ID_CHECK(xinputId);
	
	switch (triggerType)
	{
		case TriggerType::LEFT_TRIGGER:
			return state[xinputId].Gamepad.bLeftTrigger;
		case TriggerType::RIGHT_TRIGGER:
			return state[xinputId].Gamepad.bRightTrigger;
		default:
			return 0;
	}
}

SHORT InputInterface_Xinput::GetAnalogValue(const DWORD xinputId, const AnalogType analogType)
{
	XINPUT_ID_CHECK(xinputId);

	switch (analogType)
	{
		case AnalogType::LEFT_X:
			return state[xinputId].Gamepad.sThumbLX;
		case AnalogType::LEFT_Y:
			return state[xinputId].Gamepad.sThumbLY;
		case AnalogType::RIGHT_X:
			return state[xinputId].Gamepad.sThumbRX;
		case AnalogType::RIGHT_Y:
			return state[xinputId].Gamepad.sThumbRY;
		default:
			return 0;
	}
}

void InputInterface_Xinput::Poll(PS2Controller* ps2Controller)
{
	const size_t xinputSize = ps2Controller->xinputBindings.size();

	if (xinputSize > 0)
	{
		this->GetXinputState();
	}

	for (size_t i = 0; i < xinputSize; i++)
	{
		Binding_Xinput* binding = ps2Controller->xinputBindings.at(i);

		if (binding->GetButtonMask() != 0)
		{
			bool buttonValue = this->GetButtonValue(binding->GetXinputId(), binding->GetButtonMask());
			ps2Controller->SetButton(binding->GetPS2Control(), buttonValue ? 0xff : 0x00);
		}
		else if (static_cast<u8>(binding->GetTriggerType()) != 0)
		{
			BYTE triggerValue = this->GetTriggerValue(binding->GetXinputId(), binding->GetTriggerType());
			ps2Controller->SetButton(binding->GetPS2Control(), triggerValue);
		}
		else if (static_cast<u8>(binding->GetAnalogType()) != 0)
		{
			SHORT analogValue = this->GetAnalogValue(binding->GetXinputId(), binding->GetAnalogType());
			s32 larger = analogValue + 0x8000;
			float f = (float)larger / 0xffff;
			u8 analogValueNormalized = f * 0xff;

			if (binding->GetPS2Control() == PS2Control::LEFT_Y || binding->GetPS2Control() == PS2Control::RIGHT_Y)
			{
				analogValueNormalized = 0xff - analogValueNormalized;
			}

			ps2Controller->SetAnalog(binding->GetPS2Control(), analogValueNormalized);
		}
		else if (static_cast<u8>(binding->GetPS2VibrationMotor()) != 0xff)
		{
			if (binding->GetPS2VibrationMotor() == VibrationMotor::SMALL)
			{
				float f = (float)ps2Controller->vibrationStates.smallMotor / 0xff;
				WORD vibrationNormalized = f * 0xffff;
				this->StageVibration(binding->GetXinputId(), binding->GetXinputVibrationMotor(), vibrationNormalized);
			}
			else if (binding->GetPS2VibrationMotor() == VibrationMotor::LARGE)
			{
				float f = (float)ps2Controller->vibrationStates.largeMotor / 0xff;
				WORD vibrationNormalized = f * 0xffff;
				this->StageVibration(binding->GetXinputId(), binding->GetXinputVibrationMotor(), vibrationNormalized);
			}
		}
	}

	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		this->SendVibration(i);
	}
}
