
#include "PrecompiledHeader.h"

#include "InputInterface_Xinput.h"

InputInterface_Xinput::InputInterface_Xinput() noexcept
{
	
}

InputInterface_Xinput::~InputInterface_Xinput() noexcept
{

}

void InputInterface_Xinput::Poll() noexcept
{
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		ZeroMemory(&state[i], sizeof(XINPUT_STATE));
		DWORD res = XInputGetState(i, &state[i]);

		if (res == ERROR_SUCCESS)
		{
			// connected
		}
		else
		{
			// disconnected
		}
	}
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
