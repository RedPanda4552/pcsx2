
#pragma once

#include "InputInterface_Xinput.h"
#include "InputInterface_WindowsKeyboard.h"

#include <vector>
#include <queue>

class InputMain
{
public:
#ifdef _WIN32
	InputInterface_Xinput* inputInterface_Xinput;
	InputInterface_WindowsKeyboard* inputInterface_WindowsKeyboard;
#endif
#ifdef __linux__
	// TODO: evdev
	// TODO: Does a POSIX keyboard lib exist?
#endif
#ifdef __APPLE__
	// TODO: Does a POSIX keyboard lib exist?
#endif
	// TODO: HID PS4
	// TODO: HID Switch Pro

	InputMain();
	~InputMain();
	void PollAllDevices();
};
