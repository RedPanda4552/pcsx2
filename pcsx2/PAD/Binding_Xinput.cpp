
#include "PrecompiledHeader.h"

#include "Binding_Xinput.h"

Binding_Xinput::Binding_Xinput()
{

}

Binding_Xinput::~Binding_Xinput()
{

}

DWORD Binding_Xinput::GetXinputId() noexcept
{
	return this->xinputId;
}

WORD Binding_Xinput::GetButtonMask() noexcept
{
	return this->buttonMask;
}

TriggerType Binding_Xinput::GetTriggerType() noexcept
{
	return this->triggerType;
}

AnalogType Binding_Xinput::GetAnalogType() noexcept
{
	return this->analogType;
}

PS2Control Binding_Xinput::GetPS2Control() noexcept
{
	return this->ps2Control;
}
