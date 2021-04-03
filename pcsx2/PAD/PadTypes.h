#pragma once

enum class AnalogLight
{
	OFF = 0x00,
	ON = 0x01
};

enum class PhysicalType
{
	STANDARD = 0x03,
	GUITAR = 0x01
};

enum class PadMode
{
	DIGITAL = 0x41,
	ANALOG = 0x73,
	DUALSHOCK2 = 0x79,
	CONFIG = 0xf3
};

enum class PadCommandType
{
	NOT_SET = 0x00,
	BUTTON_QUERY = 0x41,
	POLL = 0x42,
	CONFIG = 0x43,
	MODE_SWITCH = 0x44,
	STATUS_INFO = 0x45,
	CONST_1 = 0x46,
	CONST_2 = 0x47,
	CONST_3 = 0x4c,
	VIBRATION_MAP = 0x4d,
	ANALOG_EDIT = 0x4f
};

enum class TriggerType
{
	LEFT_TRIGGER = 0x01,
	RIGHT_TRIGGER = 0x02
};

enum class AnalogType
{
	LEFT_X = 0x01,
	LEFT_Y = 0x02,
	RIGHT_X = 0x10,
	RIGHT_Y = 0x20
};

enum class PS2Control
{
	SELECT,
	L3,
	R3,
	START,
	UP,
	RIGHT,
	DOWN,
	LEFT,
	L2,
	R2,
	L1,
	R1,
	TRIANGLE,
	CIRCLE,
	CROSS,
	SQUARE,
	RIGHT_X,
	RIGHT_Y,
	LEFT_X,
	LEFT_Y
};

struct ButtonStates
{
	bool select = false;
	bool L3 = false;
	bool R3 = false;
	bool start = false;
	u8 up = 0;
	u8 right = 0;
	u8 down = 0;
	u8 left = 0;
	u8 L2 = 0;
	u8 R2 = 0;
	u8 L1 = 0;
	u8 R1 = 0;
	u8 triangle = 0;
	u8 circle = 0;
	u8 cross = 0;
	u8 square = 0;
};

struct AnalogStates
{
	u8 rightX = 0x7f;
	u8 rightY = 0x7f;
	u8 leftX = 0x7f;
	u8 leftY = 0x7f;
};

// Guitars are a bit unusual, u8 values are used for the non-string buttons,
// but wammy seems to be the only one which uses an actual range of 0x7f unpressed to 0x00 full pressed?
// The others we're just guessing are the same 0x7f unpressed, 0x00 pressed.
struct GuitarStates
{
	bool green = false;
	bool red = false;
	bool yellow = false;
	bool blue = false;
	bool orange = false;
	u8 up = 0x7f;
	u8 down = 0x7f;
	u8 select = 0x7f;
	u8 start = 0x7f;
	u8 wammy = 0x7f;
};
