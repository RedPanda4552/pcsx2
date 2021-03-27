
#pragma once

#include "InputInterface.h"

#include "PadTypes.h"

#include <Xinput.h>

#define XINPUT_ID_CHECK(xinputId)                                                                                                                   \
	if (xinputId >= XUSER_MAX_COUNT)                                                                                                                \
	{                                                                                                                                               \
		DevCon.Warning(                                                                                                                             \
			"(PAD) InputInterface_Xinput::GetInputValue(deviceIdentifier, inputIdentifier) invoked with out of bounds deviceIdentifier (%d >= %d)", \
			xinputId,                                                                                                                               \
			XUSER_MAX_COUNT);                                                                                                                       \
		return 0;                                                                                                                                   \
	}

class InputInterface_Xinput : InputInterface
{
private:
	XINPUT_STATE state[XUSER_MAX_COUNT];

public:
	InputInterface_Xinput() noexcept;
	~InputInterface_Xinput() noexcept;
	void Poll() noexcept override;
	bool GetButtonValue(DWORD xinputId, WORD buttonMask);
	BYTE GetTriggerValue(DWORD xinputId, TriggerType triggerType);
	SHORT GetAnalogValue(DWORD xinputId, AnalogType analogType);
};
