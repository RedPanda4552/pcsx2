
#include "PrecompiledHeader.h"

#include "PS2Controller.h"

#include <Xinput.h>

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
	Binding_Xinput* l2 = new Binding_Xinput(0, TriggerType::LEFT_TRIGGER, PS2Control::L2);
	Binding_Xinput* r1 = new Binding_Xinput(0, XINPUT_GAMEPAD_RIGHT_SHOULDER, PS2Control::R1);
	Binding_Xinput* r2 = new Binding_Xinput(0, TriggerType::RIGHT_TRIGGER, PS2Control::R2);
	Binding_Xinput* leftX = new Binding_Xinput(0, AnalogType::LEFT_X, PS2Control::LEFT_X);
	Binding_Xinput* leftY = new Binding_Xinput(0, AnalogType::LEFT_Y, PS2Control::LEFT_Y);
	Binding_Xinput* rightX = new Binding_Xinput(0, AnalogType::RIGHT_X, PS2Control::RIGHT_X);
	Binding_Xinput* rightY = new Binding_Xinput(0, AnalogType::RIGHT_Y, PS2Control::RIGHT_Y);

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
	this->xinputBindings.push_back(leftX);
	this->xinputBindings.push_back(leftY);
	this->xinputBindings.push_back(rightX);
	this->xinputBindings.push_back(rightY);
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
	}
}

void PS2Controller::SetAnalog(PS2Control ps2Control, u8 newValue)
{
	switch (ps2Control)
	{
		case PS2Control::LEFT_X:
			this->analogStates.leftX = newValue;
			break;
		case PS2Control::LEFT_Y:
			this->analogStates.leftY = newValue;
			break;
		case PS2Control::RIGHT_X:
			this->analogStates.rightX = newValue;
			break;
		case PS2Control::RIGHT_Y:
			this->analogStates.rightY = newValue;
			break;
		default:
			DevCon.Warning("%s(%02X) called with a non-analog PS2Control (%d)", __FUNCTION__, ps2Control);
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
			DevCon.Warning("%s(%02X) called with a non-pressure PS2Control (%d)", __FUNCTION__, ps2Control);
			return 0x7f;
	}
}

u8 PS2Controller::GetAnalog(PS2Control ps2Control)
{
	switch (ps2Control)
	{
		case PS2Control::LEFT_X:
			return this->analogStates.leftX;
		case PS2Control::LEFT_Y:
			return this->analogStates.leftY;
		case PS2Control::RIGHT_X:
			return this->analogStates.rightX;
		case PS2Control::RIGHT_Y:
			return this->analogStates.rightY;
		default:
			DevCon.Warning("%s(%02X) called with a non-analog PS2Control (%d)", __FUNCTION__, ps2Control);
			return 0x7f;
	}
}
