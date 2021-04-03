
#pragma once

#include "InputInterface.h"

class InputInterface_WindowsKeyboard : InputInterface
{
private:

public:
	InputInterface_WindowsKeyboard();
	~InputInterface_WindowsKeyboard();
	void Poll() noexcept;
};
