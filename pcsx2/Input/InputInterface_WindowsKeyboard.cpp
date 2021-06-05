
#include "PrecompiledHeader.h"

#include "InputInterface_WindowsKeyboard.h"

#include "Windows.h"

InputInterface_WindowsKeyboard::InputInterface_WindowsKeyboard()
{
	
}

std::array<SHORT, VKEY_COUNT> InputInterface_WindowsKeyboard::GetKeyStates()
{
	return this->keyStates;
}

void InputInterface_WindowsKeyboard::Poll() noexcept
{
	for (size_t i = 0; i < 0xff; i++)
	{
		keyStates.at(i) = GetKeyState(i);
	}
}
