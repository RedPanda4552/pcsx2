
#include "PrecompiledHeader.h"

#include "InputMain.h"

InputMain::InputMain()
{
	// Input Interfaces
#ifdef _WIN32
	this->inputInterface_Xinput = new InputInterface_Xinput();
	this->inputInterface_WindowsKeyboard = new InputInterface_WindowsKeyboard();
	// TODO: Nefarius' PS3 driver?
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
}

InputMain::~InputMain()
{
#ifdef _WIN32
	delete inputInterface_Xinput;
	delete inputInterface_WindowsKeyboard;
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
}

void InputMain::PollAllDevices()
{
#ifdef _WIN32
	this->inputInterface_Xinput->Poll();
	this->inputInterface_WindowsKeyboard->Poll();
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
}
