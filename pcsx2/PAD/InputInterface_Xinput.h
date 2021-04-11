
#pragma once

#include "InputInterface.h"

#include "PadTypes.h"
#include "PS2Controller.h"

#include <Xinput.h>

#define XINPUT_ID_CHECK(xinputId)                                              \
	if (xinputId >= XUSER_MAX_COUNT)                                           \
	{                                                                          \
		DevCon.Warning(                                                        \
			"(PAD) %s invoked with out of bounds deviceIdentifier (%d >= %d)", \
			__FUNCTION__,                                                      \
			xinputId,                                                          \
			XUSER_MAX_COUNT);                                                  \
		return 0;                                                              \
	}

class InputInterface_Xinput
{
private:
	XINPUT_STATE state[XUSER_MAX_COUNT];
	XINPUT_VIBRATION vibration[XUSER_MAX_COUNT];
	WORD lowFreqSpeed[XUSER_MAX_COUNT];
	WORD highFreqSpeed[XUSER_MAX_COUNT];

public:
	InputInterface_Xinput() noexcept;
	~InputInterface_Xinput() noexcept;
	bool GetXinputState() noexcept;
	bool StageVibration(const DWORD xinputId, const VibrationMotor xinputVibrationMotor, const WORD strength);
	bool SendVibration(const DWORD xinputId);
	bool GetButtonValue(const DWORD xinputId, const WORD buttonMask);
	BYTE GetTriggerValue(const DWORD xinputId, const TriggerType triggerType);
	SHORT GetAnalogValue(const DWORD xinputId, const AnalogType analogType);
	void Poll(PS2Controller* ps2Controller);
};
