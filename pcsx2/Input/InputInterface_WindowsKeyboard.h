
#pragma once

#include "InputInterface.h"

class InputInterface_WindowsKeyboard : public InputInterface
{
public:
	SHORT keyStates[256];

	InputInterface_WindowsKeyboard();
	~InputInterface_WindowsKeyboard();
	void Poll() noexcept;
};
