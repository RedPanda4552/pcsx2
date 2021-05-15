
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

PS2Controller::~PS2Controller()
{

}

void PS2Controller::Debug_SetBindings()
{
	Binding_Xinput* up = new Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_UP, PS2Control::UP);
	Binding_Xinput* right = new Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_RIGHT, PS2Control::RIGHT);
	Binding_Xinput* down = new Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_DOWN, PS2Control::DOWN);
	Binding_Xinput* left = new Binding_Xinput(0, XINPUT_GAMEPAD_DPAD_LEFT, PS2Control::LEFT);
	Binding_Xinput* triangle = new Binding_Xinput(0, XINPUT_GAMEPAD_Y, PS2Control::TRIANGLE);
	Binding_Xinput* circle = new Binding_Xinput(0, XINPUT_GAMEPAD_B, PS2Control::CIRCLE);
	Binding_Xinput* cross = new Binding_Xinput(0, XINPUT_GAMEPAD_A, PS2Control::CROSS);
	Binding_Xinput* square = new Binding_Xinput(0, XINPUT_GAMEPAD_X, PS2Control::SQUARE);
	Binding_Xinput* start = new Binding_Xinput(0, XINPUT_GAMEPAD_START, PS2Control::START);
	Binding_Xinput* select = new Binding_Xinput(0, XINPUT_GAMEPAD_BACK, PS2Control::SELECT);
	Binding_Xinput* l1 = new Binding_Xinput(0, XINPUT_GAMEPAD_LEFT_SHOULDER, PS2Control::L1);
	Binding_Xinput* l2 = new Binding_Xinput(0, XinputTriggerType::LEFT_TRIGGER, PS2Control::L2, 0.2f);
	Binding_Xinput* r1 = new Binding_Xinput(0, XINPUT_GAMEPAD_RIGHT_SHOULDER, PS2Control::R1);
	Binding_Xinput* r2 = new Binding_Xinput(0, XinputTriggerType::RIGHT_TRIGGER, PS2Control::R2, 0.2f);
	Binding_Xinput* leftXPos = new Binding_Xinput(0, XinputAnalogType::LEFT_X_POS, PS2Control::LEFT_X_POS, 0.2f);
	Binding_Xinput* leftXNeg = new Binding_Xinput(0, XinputAnalogType::LEFT_X_NEG, PS2Control::LEFT_X_NEG, 0.2f);
	Binding_Xinput* leftYPos = new Binding_Xinput(0, XinputAnalogType::LEFT_Y_POS, PS2Control::LEFT_Y_NEG, 0.2f);
	Binding_Xinput* leftYNeg = new Binding_Xinput(0, XinputAnalogType::LEFT_Y_NEG, PS2Control::LEFT_Y_POS, 0.2f);
	Binding_Xinput* rightXPos = new Binding_Xinput(0, XinputAnalogType::RIGHT_X_POS, PS2Control::RIGHT_X_POS, 0.2f);
	Binding_Xinput* rightXNeg = new Binding_Xinput(0, XinputAnalogType::RIGHT_X_NEG, PS2Control::RIGHT_X_NEG, 0.2f);
	Binding_Xinput* rightYPos = new Binding_Xinput(0, XinputAnalogType::RIGHT_Y_POS, PS2Control::RIGHT_Y_NEG, 0.2f);
	Binding_Xinput* rightYNeg = new Binding_Xinput(0, XinputAnalogType::RIGHT_Y_NEG, PS2Control::RIGHT_Y_POS, 0.2f);
	Binding_Xinput* smallVib = new Binding_Xinput(0, XinputVibrationMotor::SMALL, XinputVibrationMotor::SMALL);
	Binding_Xinput* largeVib = new Binding_Xinput(0, XinputVibrationMotor::LARGE, XinputVibrationMotor::LARGE);

	this->xinputBindings.push_back(up);
	this->xinputBindings.push_back(right);
	this->xinputBindings.push_back(down);
	this->xinputBindings.push_back(left);
	this->xinputBindings.push_back(triangle);
	this->xinputBindings.push_back(circle);
	this->xinputBindings.push_back(cross);
	this->xinputBindings.push_back(square);
	this->xinputBindings.push_back(start);
	this->xinputBindings.push_back(select);
	this->xinputBindings.push_back(l1);
	this->xinputBindings.push_back(l2);
	this->xinputBindings.push_back(r1);
	this->xinputBindings.push_back(r2);
	this->xinputBindings.push_back(leftXPos);
	this->xinputBindings.push_back(leftXNeg);
	this->xinputBindings.push_back(leftYPos);
	this->xinputBindings.push_back(leftYNeg);
	this->xinputBindings.push_back(rightXPos);
	this->xinputBindings.push_back(rightXNeg);
	this->xinputBindings.push_back(rightYPos);
	this->xinputBindings.push_back(rightYNeg);
	this->xinputBindings.push_back(smallVib);
	this->xinputBindings.push_back(largeVib);
}

void PS2Controller::SetButton(PS2Control ps2Control, u8 newValue)
{
	switch (ps2Control)
	{
		case PS2Control::SELECT:
			this->buttonStates.select = newValue;
			break;
		case PS2Control::L3:
			this->buttonStates.L3 = newValue;
			break;
		case PS2Control::R3:
			this->buttonStates.R3 = newValue;
			break;
		case PS2Control::START:
			this->buttonStates.start = newValue;
			break;
		case PS2Control::UP:
			this->buttonStates.up = newValue;
			break;
		case PS2Control::RIGHT:
			this->buttonStates.right = newValue;
			break;
		case PS2Control::DOWN:
			this->buttonStates.down = newValue;
			break;
		case PS2Control::LEFT:
			this->buttonStates.left = newValue;
			break;
		case PS2Control::L2:
			this->buttonStates.L2 = newValue;
			break;
		case PS2Control::R2:
			this->buttonStates.R2 = newValue;
			break;
		case PS2Control::L1:
			this->buttonStates.L1 = newValue;
			break;
		case PS2Control::R1:
			this->buttonStates.R1 = newValue;
			break;
		case PS2Control::TRIANGLE:
			this->buttonStates.triangle = newValue;
			break;
		case PS2Control::CIRCLE:
			this->buttonStates.circle = newValue;
			break;
		case PS2Control::CROSS:
			this->buttonStates.cross = newValue;
			break;
		case PS2Control::SQUARE:
			this->buttonStates.square = newValue;
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
			this->analogStates.leftXPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::LEFT_X_NEG:
			this->analogStates.leftXNeg = 0x7f - (factor * 0x7f);
			break;
		case PS2Control::LEFT_Y_POS:
			this->analogStates.leftYPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::LEFT_Y_NEG:
			this->analogStates.leftYNeg = 0x7f - (factor * 0x7f);
			break;
		case PS2Control::RIGHT_X_POS:
			this->analogStates.rightXPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::RIGHT_X_NEG:
			this->analogStates.rightXNeg = 0x7f - (factor * 0x7f);
			break;
		case PS2Control::RIGHT_Y_POS:
			this->analogStates.rightYPos = 0x7f + (factor * 0x80);
			break;
		case PS2Control::RIGHT_Y_NEG:
			this->analogStates.rightYNeg = 0x7f - (factor * 0x7f);
			break;
		default:
			DevCon.Warning("%s(%02X, %02X) called with a non-analog PS2Control", __FUNCTION__, ps2Control, newValue);
			return;
	}
}

u8 PS2Controller::GetFirstDigitalByte()
{
	u8 ret = 0xff;
	ret &= ~(this->buttonStates.select);
	ret &= ~(this->buttonStates.L3 << 1);
	ret &= ~(this->buttonStates.R3 << 2);
	ret &= ~(this->buttonStates.start << 3);
	ret &= ~(static_cast<bool>(this->buttonStates.up) << 4);
	ret &= ~(static_cast<bool>(this->buttonStates.right) << 5);
	ret &= ~(static_cast<bool>(this->buttonStates.down) << 6);
	ret &= ~(static_cast<bool>(this->buttonStates.left) << 7);
	return ret;
}

u8 PS2Controller::GetSecondDigitalByte()
{
	u8 ret = 0xff;
	ret &= ~(static_cast<bool>(this->buttonStates.L2));
	ret &= ~(static_cast<bool>(this->buttonStates.R2) << 1);
	ret &= ~(static_cast<bool>(this->buttonStates.L1) << 2);
	ret &= ~(static_cast<bool>(this->buttonStates.R1) << 3);
	ret &= ~(static_cast<bool>(this->buttonStates.triangle) << 4);
	ret &= ~(static_cast<bool>(this->buttonStates.circle) << 5);
	ret &= ~(static_cast<bool>(this->buttonStates.cross) << 6);
	ret &= ~(static_cast<bool>(this->buttonStates.square) << 7);
	return ret;
}

u8 PS2Controller::GetButton(PS2Control ps2Control)
{
	switch (ps2Control)
	{
		case PS2Control::RIGHT:
			return this->buttonStates.right;
		case PS2Control::LEFT:
			return this->buttonStates.left;
		case PS2Control::UP:
			return this->buttonStates.up;
		case PS2Control::DOWN:
			return this->buttonStates.down;
		case PS2Control::TRIANGLE:
			return this->buttonStates.triangle;
		case PS2Control::CIRCLE:
			return this->buttonStates.circle;
		case PS2Control::CROSS:
			return this->buttonStates.cross;
		case PS2Control::SQUARE:
			return this->buttonStates.square;
		case PS2Control::L1:
			return this->buttonStates.L1;
		case PS2Control::R1:
			return this->buttonStates.R1;
		case PS2Control::L2:
			return this->buttonStates.L2;
		case PS2Control::R2:
			return this->buttonStates.R2;
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
			this->analogStates.leftX = this->analogStates.leftXPos + this->analogStates.leftXNeg - 0x7f;
			return this->analogStates.leftX;
		case PS2Control::LEFT_Y:
			this->analogStates.leftY = this->analogStates.leftYPos + this->analogStates.leftYNeg - 0x7f;
			return this->analogStates.leftY;
		case PS2Control::RIGHT_X:
			this->analogStates.rightX = this->analogStates.rightXPos + this->analogStates.rightXNeg - 0x7f;
			return this->analogStates.rightX;
		case PS2Control::RIGHT_Y:
			this->analogStates.rightY = this->analogStates.rightYPos + this->analogStates.rightYNeg - 0x7f;
			return this->analogStates.rightY;
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
			this->vibrationStates.smallMotor = strength;
			break;
		case VibrationMotor::LARGE:
			this->vibrationStates.largeMotor = strength;
			break;
		default:
			break;
	}
}
