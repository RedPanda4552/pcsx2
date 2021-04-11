
#include "PrecompiledHeader.h"

#include "Binding_Xinput.h"

Binding_Xinput::Binding_Xinput(DWORD xinputId, WORD buttonMask, PS2Control ps2Control)
{
	this->xinputId = xinputId;
	this->buttonMask = buttonMask;
	this->analogType = AnalogType::NONE;
	this->triggerType = TriggerType::NONE;
	this->ps2Control = ps2Control;
	this->ps2VibrationMotor = VibrationMotor::NONE;
	this->xinputVibrationMotor = VibrationMotor::NONE;
}

Binding_Xinput::Binding_Xinput(DWORD xinputId, AnalogType analogType, PS2Control ps2Control)
{
	this->xinputId = xinputId;
	this->buttonMask = 0;
	this->analogType = analogType;
	this->triggerType = TriggerType::NONE;
	this->ps2Control = ps2Control;
	this->ps2VibrationMotor = VibrationMotor::NONE;
	this->xinputVibrationMotor = VibrationMotor::NONE;
}

Binding_Xinput::Binding_Xinput(DWORD xinputId, TriggerType triggerType, PS2Control ps2Control)
{
	this->xinputId = xinputId;
	this->buttonMask = 0;
	this->analogType = AnalogType::NONE;
	this->triggerType = triggerType;
	this->ps2Control = ps2Control;
	this->ps2VibrationMotor = VibrationMotor::NONE;
	this->xinputVibrationMotor = VibrationMotor::NONE;
}

Binding_Xinput::Binding_Xinput(DWORD xinputId, VibrationMotor ps2VibrationMotor, VibrationMotor xinputVibrationMotor)
{
	this->xinputId = xinputId;
	this->buttonMask = 0;
	this->analogType = AnalogType::NONE;
	this->triggerType = TriggerType::NONE;
	this->ps2Control = PS2Control::NONE;
	this->ps2VibrationMotor = ps2VibrationMotor;
	this->xinputVibrationMotor = xinputVibrationMotor;
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

VibrationMotor Binding_Xinput::GetPS2VibrationMotor() noexcept
{
	return this->ps2VibrationMotor;
}

VibrationMotor Binding_Xinput::GetXinputVibrationMotor() noexcept 
{
	return this->xinputVibrationMotor;
}
