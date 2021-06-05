
#include "PrecompiledHeader.h"

#include "PS2Controller.h"

#include <Xinput.h>
#include "Input/Types_Xinput.h"

// Determine if an input PS2Control is a button.
// Used later when evaluating bindings to determine if normalization is required or not.
bool PS2Controller::IsPS2ControlButton(PS2Control ps2Control)
{
	u8 value = static_cast<u8>(ps2Control);
	return value != static_cast<u8>(PS2Control::NONE) && value < static_cast<u8>(PS2Control::RIGHT_X_POS);
}

// Determine if an input PS2Control is an analog stick.
// Used later when evaluating bindings to determine if normalization is required or not.
bool PS2Controller::IsPS2ControlAnalog(PS2Control ps2Control)
{
	u8 value = static_cast<u8>(ps2Control);
	return value != static_cast<u8>(PS2Control::NONE) && value >= static_cast<u8>(PS2Control::RIGHT_X_POS);
}

PS2Controller::PS2Controller()
{
	
}

void PS2Controller::Debug_SetBindings()
{
#ifdef WIN32
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_UP, PS2Control::UP));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_RIGHT, PS2Control::RIGHT));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_DOWN, PS2Control::DOWN));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_LEFT, PS2Control::LEFT));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_Y, PS2Control::TRIANGLE));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_B, PS2Control::CIRCLE));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_A, PS2Control::CROSS));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_X, PS2Control::SQUARE));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_START, PS2Control::START));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_BACK, PS2Control::SELECT));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_LEFT_SHOULDER, PS2Control::L1));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputTriggerType::LEFT_TRIGGER, PS2Control::L2, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XINPUT_GAMEPAD_RIGHT_SHOULDER, PS2Control::R1));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputTriggerType::RIGHT_TRIGGER, PS2Control::R2, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::LEFT_X_POS, PS2Control::LEFT_X_POS, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::LEFT_X_NEG, PS2Control::LEFT_X_NEG, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::LEFT_Y_POS, PS2Control::LEFT_Y_NEG, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::LEFT_Y_NEG, PS2Control::LEFT_Y_POS, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::RIGHT_X_POS, PS2Control::RIGHT_X_POS, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::RIGHT_X_NEG, PS2Control::RIGHT_X_NEG, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::RIGHT_Y_POS, PS2Control::RIGHT_Y_NEG, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputAnalogType::RIGHT_Y_NEG, PS2Control::RIGHT_Y_POS, 0.2f));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputVibrationMotor::SMALL, XinputVibrationMotor::SMALL));
	this->xinputBindings.push_back(Binding_Xinput(0, XinputVibrationMotor::LARGE, XinputVibrationMotor::LARGE));
#endif
}

void PS2Controller::SetButton(PS2Control ps2Control, u8 newValue)
{
	switch (ps2Control)
	{
		case PS2Control::SELECT:
			this->controllerState.buttonStates.select = newValue;
			break;
		case PS2Control::L3:
			this->controllerState.buttonStates.L3 = newValue;
			break;
		case PS2Control::R3:
			this->controllerState.buttonStates.R3 = newValue;
			break;
		case PS2Control::START:
			this->controllerState.buttonStates.start = newValue;
			break;
		case PS2Control::UP:
			this->controllerState.buttonStates.up = newValue;
			break;
		case PS2Control::RIGHT:
			this->controllerState.buttonStates.right = newValue;
			break;
		case PS2Control::DOWN:
			this->controllerState.buttonStates.down = newValue;
			break;
		case PS2Control::LEFT:
			this->controllerState.buttonStates.left = newValue;
			break;
		case PS2Control::L2:
			this->controllerState.buttonStates.L2 = newValue;
			break;
		case PS2Control::R2:
			this->controllerState.buttonStates.R2 = newValue;
			break;
		case PS2Control::L1:
			this->controllerState.buttonStates.L1 = newValue;
			break;
		case PS2Control::R1:
			this->controllerState.buttonStates.R1 = newValue;
			break;
		case PS2Control::TRIANGLE:
			this->controllerState.buttonStates.triangle = newValue;
			break;
		case PS2Control::CIRCLE:
			this->controllerState.buttonStates.circle = newValue;
			break;
		case PS2Control::CROSS:
			this->controllerState.buttonStates.cross = newValue;
			break;
		case PS2Control::SQUARE:
			this->controllerState.buttonStates.square = newValue;
			break;
		default:
			DevCon.Warning("%s(%02X, %02X) called with a non-button PS2Control", __FUNCTION__, ps2Control, newValue);
			return;
	}
}

void PS2Controller::SetAnalog(PS2Control ps2Control, u8 newValue)
{
	float factor = (float)newValue / 0xff;

	switch (ps2Control)
	{
		// TODO: Bind buttons to the components on a single axis, see what behavior is (override, cancel?)
		case PS2Control::LEFT_X_POS:
			this->controllerState.analogStates.leftXPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::LEFT_X_NEG:
			this->controllerState.analogStates.leftXNeg = 0x7f - (factor * 0x7f);
			break;
		case PS2Control::LEFT_Y_POS:
			this->controllerState.analogStates.leftYPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::LEFT_Y_NEG:
			this->controllerState.analogStates.leftYNeg = 0x7f - (factor * 0x7f);
			break;
		case PS2Control::RIGHT_X_POS:
			this->controllerState.analogStates.rightXPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::RIGHT_X_NEG:
			this->controllerState.analogStates.rightXNeg = 0x7f - (factor * 0x7f);
			break;
		case PS2Control::RIGHT_Y_POS:
			this->controllerState.analogStates.rightYPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::RIGHT_Y_NEG:
			this->controllerState.analogStates.rightYNeg = 0x7f - (factor * 0x7f);
			break;
		default:
			DevCon.Warning("%s(%02X, %02X) called with a non-analog PS2Control", __FUNCTION__, ps2Control, newValue);
			return;
	}
}

u8 PS2Controller::GetFirstDigitalByte()
{
	u8 ret = 0xff;
	ret &= ~(this->controllerState.buttonStates.select);
	ret &= ~(this->controllerState.buttonStates.L3 << 1);
	ret &= ~(this->controllerState.buttonStates.R3 << 2);
	ret &= ~(this->controllerState.buttonStates.start << 3);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.up) << 4);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.right) << 5);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.down) << 6);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.left) << 7);
	return ret;
}

u8 PS2Controller::GetSecondDigitalByte()
{
	u8 ret = 0xff;
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.L2));
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.R2) << 1);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.L1) << 2);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.R1) << 3);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.triangle) << 4);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.circle) << 5);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.cross) << 6);
	ret &= ~(static_cast<bool>(this->controllerState.buttonStates.square) << 7);
	return ret;
}

u8 PS2Controller::GetButton(PS2Control ps2Control)
{
	switch (ps2Control)
	{
		case PS2Control::RIGHT:
			return this->controllerState.buttonStates.right;
		case PS2Control::LEFT:
			return this->controllerState.buttonStates.left;
		case PS2Control::UP:
			return this->controllerState.buttonStates.up;
		case PS2Control::DOWN:
			return this->controllerState.buttonStates.down;
		case PS2Control::TRIANGLE:
			return this->controllerState.buttonStates.triangle;
		case PS2Control::CIRCLE:
			return this->controllerState.buttonStates.circle;
		case PS2Control::CROSS:
			return this->controllerState.buttonStates.cross;
		case PS2Control::SQUARE:
			return this->controllerState.buttonStates.square;
		case PS2Control::L1:
			return this->controllerState.buttonStates.L1;
		case PS2Control::R1:
			return this->controllerState.buttonStates.R1;
		case PS2Control::L2:
			return this->controllerState.buttonStates.L2;
		case PS2Control::R2:
			return this->controllerState.buttonStates.R2;
		default:
			DevCon.Warning("%s(%02X) called with a non-pressure PS2Control", __FUNCTION__, ps2Control);
			return 0x7f;
	}
}

u8 PS2Controller::GetAnalog(PS2Control ps2Control)
{
	switch (ps2Control)
	{
		case PS2Control::LEFT_X:
			this->controllerState.analogStates.leftX = this->controllerState.analogStates.leftXPos + this->controllerState.analogStates.leftXNeg - 0x7f;
			return this->controllerState.analogStates.leftX;
		case PS2Control::LEFT_Y:
			this->controllerState.analogStates.leftY = this->controllerState.analogStates.leftYPos + this->controllerState.analogStates.leftYNeg - 0x7f;
			return this->controllerState.analogStates.leftY;
		case PS2Control::RIGHT_X:
			this->controllerState.analogStates.rightX = this->controllerState.analogStates.rightXPos + this->controllerState.analogStates.rightXNeg - 0x7f;
			return this->controllerState.analogStates.rightX;
		case PS2Control::RIGHT_Y:
			this->controllerState.analogStates.rightY = this->controllerState.analogStates.rightYPos + this->controllerState.analogStates.rightYNeg - 0x7f;
			return this->controllerState.analogStates.rightY;
		default:
			DevCon.Warning("%s(%02X) called with a non-analog PS2Control (%d)", __FUNCTION__, ps2Control);
			return 0x7f;
	}
}

void PS2Controller::SetVibration(VibrationMotor ps2VibrationMotor, u8 strength)
{
	switch (ps2VibrationMotor)
	{
		case VibrationMotor::SMALL:
			this->controllerState.vibrationStates.smallMotor = strength;
			break;
		case VibrationMotor::LARGE:
			this->controllerState.vibrationStates.largeMotor = strength;
			break;
		default:
			break;
	}
}
