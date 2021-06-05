
#pragma once

#include "InputInterface.h"

#include <array>

static constexpr u8 VKEY_COUNT = 0xff;

class InputInterface_WindowsKeyboard : public InputInterface
{
private:
	std::array<SHORT, VKEY_COUNT> keyStates;
public:
	InputInterface_WindowsKeyboard();
	std::array<SHORT, VKEY_COUNT> GetKeyStates();
	void Poll() noexcept;
};
