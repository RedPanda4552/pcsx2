
#include "PrecompiledHeader.h"

#include "Binding_Xinput.h"

Binding_Xinput::Binding_Xinput(DWORD xinputId, WORD buttonMask, PS2Control ps2Control)
{
	this->xinputId = xinputId;
	this->buttonMask = buttonMask;
	this->analogType = XinputAnalogType::NONE;
	this->triggerType = XinputTriggerType::NONE;
	this->ps2Control = ps2Control;
	this->ps2VibrationMotor = XinputVibrationMotor::NONE;
	this->xinputVibrationMotor = XinputVibrationMotor::NONE;
}

Binding_Xinput::Binding_Xinput(DWORD xinputId, XinputAnalogType analogType, PS2Control ps2Control)
{
	this->xinputId = xinputId;
	this->buttonMask = 0;
	this->analogType = analogType;
	this->triggerType = XinputTriggerType::NONE;
	this->ps2Control = ps2Control;
	this->ps2VibrationMotor = XinputVibrationMotor::NONE;
	this->xinputVibrationMotor = XinputVibrationMotor::NONE;
}

Binding_Xinput::Binding_Xinput(DWORD xinputId, XinputTriggerType triggerType, PS2Control ps2Control)
{
	this->xinputId = xinputId;
	this->buttonMask = 0;
	this->analogType = XinputAnalogType::NONE;
	this->triggerType = triggerType;
	this->ps2Control = ps2Control;
	this->ps2VibrationMotor = XinputVibrationMotor::NONE;
	this->xinputVibrationMotor = XinputVibrationMotor::NONE;
}

Binding_Xinput::Binding_Xinput(DWORD xinputId, XinputVibrationMotor ps2VibrationMotor, XinputVibrationMotor xinputVibrationMotor)
{
	this->xinputId = xinputId;
	this->buttonMask = 0;
	this->analogType = XinputAnalogType::NONE;
	this->triggerType = XinputTriggerType::NONE;
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

XinputTriggerType Binding_Xinput::GetTriggerType() noexcept
{
	return this->triggerType;
}

XinputAnalogType Binding_Xinput::GetAnalogType() noexcept
{
	return this->analogType;
}

PS2Control Binding_Xinput::GetPS2Control() noexcept
{
	return this->ps2Control;
}

XinputVibrationMotor Binding_Xinput::GetPS2VibrationMotor() noexcept
{
	return this->ps2VibrationMotor;
}

XinputVibrationMotor Binding_Xinput::GetXinputVibrationMotor() noexcept 
{
	return this->xinputVibrationMotor;
}
