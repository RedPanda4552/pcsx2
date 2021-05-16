
#include "PrecompiledHeader.h"

#include "Binding_WindowsKeyboard.h"

Binding_WindowsKeyboard::Binding_WindowsKeyboard(u8 vkeyId, PS2Control ps2Control)
{
	this->vkeyId = vkeyId;
	this->ps2Control = ps2Control;
}

Binding_WindowsKeyboard::~Binding_WindowsKeyboard()
{

}

int Binding_WindowsKeyboard::GetVkeyId()
{
	return this->vkeyId;
}

PS2Control Binding_WindowsKeyboard::GetPS2Control()
{
	return this->ps2Control;
}
