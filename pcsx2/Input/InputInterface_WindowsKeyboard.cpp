
#include "PrecompiledHeader.h"

#include "InputInterface_WindowsKeyboard.h"

#include "Windows.h"

InputInterface_WindowsKeyboard::InputInterface_WindowsKeyboard()
{
	
}

InputInterface_WindowsKeyboard::~InputInterface_WindowsKeyboard()
{

}

void InputInterface_WindowsKeyboard::Poll() noexcept
{
	for (size_t i = 0; i < 0xff; i++)
	{
		keyStates[i] = GetKeyState(i);
	}
}
