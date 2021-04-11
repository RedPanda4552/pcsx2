// God please just make it go away, please, god.
// wx is supposed to (at least for Windows, probably other OSes too) be able to register hotkeys with the OS.
// At a minimum, Windows allows you to register them, it then provides a handle and you can listen to that for
// hotkey presses. Easy enough. It seems like PCSX2 never used this.
//
// Instead, PCSX2 used to ask PAD, even when it was still a plugin, to notify it of any key press events. Wat.
// 
// This is our sort of reimplementation of what PAD would have done, as a holdover while we wait for a wx replacement.
// Rather than waste time with trying to get wx hotkey registration working, we'll do what core used to ask PAD to do.
// Just, in the core instead. That means writing a class for each "OS configuration" (Windows, SDL, etc) and doing
// keyboard polls on each.
// 
// Please, if you are thinking about modifying this. Consider waiting until wx is gone, and REPLACE this with a proper
// system which will register a handle with the OS to receive key press messages.
#pragma once

#include "PS2Edefs.h"

#include <queue>

class KeyEvent
{
public:
	u32 key;
	u32 evt;
};

class AppKeyboardListener
{
private:
#ifdef _WINDOWS
	SHORT keyState[256], lastState[256];
#endif
#ifdef __linux__

#endif
#ifdef __APPLE__

#endif
	std::queue<KeyEvent*>* queue;

public:
	AppKeyboardListener();
	~AppKeyboardListener();
	void PollKeyStates();
	void PushKeyEvent(u32 key, u32 event);
	keyEvent* PopKeyEvent();
};
