#pragma once

enum class XinputControlType
{
	STANDARD_BUTTON = 0x00,
	TRIGGER = 0x01,
	ANALOG_STICK = 0x02
};

enum class XinputTriggerType
{
	NONE = 0xff,
	LEFT_TRIGGER = 0x00,
	RIGHT_TRIGGER = 0x01
};

enum class XinputAnalogType
{
	NONE = 0xff,
	LEFT_X = 0x00,
	LEFT_Y = 0x01,
	RIGHT_X = 0x80,
	RIGHT_Y = 0x81
};

enum class XinputVibrationMotor
{
	NONE = 0xff,
	SMALL = 0x00,
	LARGE = 0x01
};
