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
	MYSTERY = 0x40,
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
	NONE = 0x00,
	LEFT_TRIGGER = 0x01,
	RIGHT_TRIGGER = 0x02
};

// Intended for expressing the analog type of a controller connected to
// the host PC. For example, we would use this when setting up an Xinput
// binding relationship between the Xinput's AnalogType::LEFT_X, and the
// PS2's PS2Control::LEFT_X.
enum class AnalogType
{
	NONE = 0x00,
	LEFT_X = 0x01,
	LEFT_Y = 0x02,
	RIGHT_X = 0x10,
	RIGHT_Y = 0x20
};

enum class VibrationMotor
{
	NONE = 0xff,
	SMALL = 0x00,
	LARGE = 0x01
};

enum class PS2Control
{
	NONE = 0xff,
	// Buttons which were exclusively digital. Their numbering does not matter.
	SELECT = 0x00,
	L3 = 0x01,
	R3 = 0x02,
	START = 0x03,
	// The complete axes for each analog. Their numbering lets Pad::Poll retrieve them without explicitly
	// spelling out a return for every single individual analog axis.
	RIGHT_X = 0x06,
	RIGHT_Y = 0x07,
	LEFT_X = 0x08,
	LEFT_Y = 0x09,
	// Buttons which supported pressure. Their numbering lets Pad::Poll retrieve them without explicitly
	// spelling out a return for every single individual button.
	RIGHT = 0x0a,
	LEFT = 0x0b,
	UP = 0x0c,
	DOWN = 0x0d,
	TRIANGLE = 0x0e,
	CIRCLE = 0x0f,
	CROSS = 0x10,
	SQUARE = 0x11,
	L1 = 0x12,
	R1 = 0x13,
	L2 = 0x14,
	R2 = 0x15,
	// These are not truly PS2 controls. These are the analog axes, broken into positive and negative components.
	// Intended to simplify binding (e.g. if using keyboard keys, boolean values,
	// a separate key is required for a positive and negative component for a single axis).
	RIGHT_X_POS = 0x40,
	RIGHT_X_NEG = 0x41,
	RIGHT_Y_POS = 0x42,
	RIGHT_Y_NEG = 0x43,
	LEFT_X_POS = 0x80,
	LEFT_X_NEG = 0x81,
	LEFT_Y_POS = 0x82,
	LEFT_Y_NEG = 0x83
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
	u8 rightXPos = 0x00;
	u8 rightXNeg = 0x00;
	u8 rightY = 0x7f;
	u8 rightYPos = 0x00;
	u8 rightYNeg = 0x00;
	u8 leftX = 0x7f;
	u8 leftXPos = 0x00;
	u8 leftXNeg = 0x00;
	u8 leftY = 0x7f;
	u8 leftYPos = 0x00;
	u8 leftYNeg = 0x00;
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

struct VibrationStates
{
	bool smallMotor = false;
	u8 largeMotor = 0;
};
