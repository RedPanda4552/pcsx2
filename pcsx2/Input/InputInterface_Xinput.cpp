
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

bool InputInterface_Xinput::StageVibration(const DWORD xinputId, const XinputVibrationMotor xinputVibrationMotor, const WORD strength)
{
	IsXinputIdInBounds(xinputId);

	if (xinputVibrationMotor == XinputVibrationMotor::LARGE)
	{
		lowFreqSpeed[xinputId] = strength;
	}
	else if (xinputVibrationMotor == XinputVibrationMotor::SMALL)
	{
		highFreqSpeed[xinputId] = strength;
	}

	return true;
}

bool InputInterface_Xinput::SendVibration(const DWORD xinputId)
{
	IsXinputIdInBounds(xinputId);

	ZeroMemory(&vibration[xinputId], sizeof(XINPUT_VIBRATION));
	vibration->wLeftMotorSpeed = lowFreqSpeed[xinputId];
	vibration->wRightMotorSpeed = highFreqSpeed[xinputId];
	XInputSetState(xinputId, &vibration[xinputId]);
	return true;
}

bool InputInterface_Xinput::GetButtonValue(const DWORD xinputId, const WORD buttonMask)
{
	IsXinputIdInBounds(xinputId);
	return (state[xinputId].Gamepad.wButtons & buttonMask) > 0;
}

BYTE InputInterface_Xinput::GetTriggerValue(const DWORD xinputId, const XinputTriggerType xinputTriggerType)
{
	IsXinputIdInBounds(xinputId);
	
	switch (xinputTriggerType)
	{
		case XinputTriggerType::LEFT_TRIGGER:
			return state[xinputId].Gamepad.bLeftTrigger;
		case XinputTriggerType::RIGHT_TRIGGER:
			return state[xinputId].Gamepad.bRightTrigger;
		default:
			return 0;
	}
}

SHORT InputInterface_Xinput::GetAnalogValue(const DWORD xinputId, const XinputAnalogType xinputAnalogType)
{
	IsXinputIdInBounds(xinputId);
	SHORT value = 0;

	switch (xinputAnalogType)
	{
		case XinputAnalogType::LEFT_X_POS:
			value = state[xinputId].Gamepad.sThumbLX;
			return value > 0 ? value : 0;
		case XinputAnalogType::LEFT_X_NEG:
			value = state[xinputId].Gamepad.sThumbLX;
			return value < 0 ? value : 0;
		case XinputAnalogType::LEFT_Y_POS:
			value = state[xinputId].Gamepad.sThumbLY;
			return value > 0 ? value : 0;
		case XinputAnalogType::LEFT_Y_NEG:
			value = state[xinputId].Gamepad.sThumbLY;
			return value < 0 ? value : 0;
		case XinputAnalogType::RIGHT_X_POS:
			value = state[xinputId].Gamepad.sThumbRX;
			return value > 0 ? value : 0;
		case XinputAnalogType::RIGHT_X_NEG:
			value = state[xinputId].Gamepad.sThumbRX;
			return value < 0 ? value : 0;
		case XinputAnalogType::RIGHT_Y_POS:
			value = state[xinputId].Gamepad.sThumbRY;
			return value > 0 ? value : 0;
		case XinputAnalogType::RIGHT_Y_NEG:
			value = state[xinputId].Gamepad.sThumbRY;
			return value < 0 ? value : 0;
		default:
			return 0;
	}
}

void InputInterface_Xinput::Poll()
{
	for (size_t i = 0; i < XUSER_MAX_COUNT; i++)
	{
		ZeroMemory(&state[i], sizeof(XINPUT_STATE));
		XInputGetState(i, &state[i]);
	}
}
