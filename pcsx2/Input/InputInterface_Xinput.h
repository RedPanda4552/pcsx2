
#pragma once

#include "InputInterface.h"

#include "Types_Xinput.h"

#include <Xinput.h>

#define IsXinputIdInBounds(xinputId)                                           \
	if (xinputId >= XUSER_MAX_COUNT)                                           \
	{                                                                          \
		DevCon.Warning(                                                        \
			"(PAD) %s invoked with out of bounds deviceIdentifier (%d >= %d)", \
			__FUNCTION__,                                                      \
			xinputId,                                                          \
			XUSER_MAX_COUNT);                                                  \
		return false;                                                          \
	}

class InputInterface_Xinput : public InputInterface
{
private:
	XINPUT_STATE state[XUSER_MAX_COUNT];
	XINPUT_VIBRATION vibration[XUSER_MAX_COUNT];
	WORD lowFreqSpeed[XUSER_MAX_COUNT];
	WORD highFreqSpeed[XUSER_MAX_COUNT];
public:
	InputInterface_Xinput() noexcept;
	bool StageVibration(const DWORD xinputId, const XinputVibrationMotor xinputVibrationMotor, const WORD strength);
	bool SendVibration(const DWORD xinputId);
	bool GetButtonValue(const DWORD xinputId, const WORD buttonMask);
	BYTE GetTriggerValue(const DWORD xinputId, const XinputTriggerType triggerType);
	SHORT GetAnalogValue(const DWORD xinputId, const XinputAnalogType analogType);
	void Poll();
};
