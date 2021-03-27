
#include "PrecompiledHeader.h"

#include "PS2Controller.h"

PS2Controller::PS2Controller()
{

}

PS2Controller::~PS2Controller()
{

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

}

u8 PS2Controller::GetFirstDigitalByte()
{
	u8 ret = 0;
	ret &= this->buttonStates.select;
	ret &= this->buttonStates.L3 << 1;
	ret &= this->buttonStates.R3 << 2;
	ret &= this->buttonStates.start << 3;
	ret &= static_cast<bool>(this->buttonStates.up) << 4;
	ret &= static_cast<bool>(this->buttonStates.right) << 5;
	ret &= static_cast<bool>(this->buttonStates.down) << 6;
	ret &= static_cast<bool>(this->buttonStates.left) << 7;
	return ret;
}

u8 PS2Controller::GetSecondDigitalByte()
{
	u8 ret = 0;
	ret &= static_cast<bool>(this->buttonStates.L2);
	ret &= static_cast<bool>(this->buttonStates.R2) << 1;
	ret &= static_cast<bool>(this->buttonStates.L1) << 2;
	ret &= static_cast<bool>(this->buttonStates.R1) << 3;
	ret &= static_cast<bool>(this->buttonStates.triangle) << 4;
	ret &= static_cast<bool>(this->buttonStates.circle) << 5;
	ret &= static_cast<bool>(this->buttonStates.cross) << 6;
	ret &= static_cast<bool>(this->buttonStates.square) << 7;
	return ret;
}
