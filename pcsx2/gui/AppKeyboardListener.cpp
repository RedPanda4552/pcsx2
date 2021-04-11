
#include "PrecompiledHeader.h"

#include "AppKeyboardListener.h" // Read comment at top of this header if you're thinking changing this cpp file is a good idea

AppKeyboardListener::AppKeyboardListener()
{
	queue = new std::queue<KeyEvent*>();
}

AppKeyboardListener::~AppKeyboardListener()
{
	delete queue;
}

void AppKeyboardListener::PollKeyStates()
{
#ifdef _WINDOWS
	bool isShiftDown = false;
	bool isCtrlDown = false;
	bool isAltDown = false;

	// Iterate backwards, so we read alt before F4
	for (s32 i = 255; i >= 0; i--)
	{
		keyState[i] = (GetAsyncKeyState(i) & 0x8000);

		if ((keyState[i] & 0x8000) && keyState[i] != lastState[i])
		{
			if (i == VK_LSHIFT || i == VK_RSHIFT || i == VK_SHIFT)
			{
				isShiftDown = true;
				this->PushKeyEvent(VK_SHIFT, KEYPRESS);
			}
			else if (i == VK_LCONTROL || i == VK_RCONTROL || i == VK_CONTROL)
			{
				isCtrlDown = true;
				this->PushKeyEvent(VK_CONTROL, KEYPRESS);
			}
			else if (i == VK_LMENU || i == VK_RMENU || i == VK_MENU)
			{
				isAltDown = true;
				this->PushKeyEvent(VK_MENU, KEYPRESS);
			}
			else if (i == VK_F4 && isAltDown)
			{
				// don't push anything
			}
			else
			{
				this->PushKeyEvent(i, KEYPRESS);
			}
		}

		lastState[i] = keyState[i];
	}
#endif
#ifdef __unix__
	// TODO
#endif
#ifdef __APPLE__
	// TODO
#endif
}

void AppKeyboardListener::PushKeyEvent(u32 key, u32 event)
{
	KeyEvent* newEvent = new KeyEvent();
	newEvent->key = key;
	newEvent->evt = event;
	queue->push(newEvent);
}

keyEvent* AppKeyboardListener::PopKeyEvent()
{
	if (queue->empty())
	{
		return 0;
	}

	KeyEvent* queuedEvent = queue->front();
	queue->pop();
	static keyEvent ret;
	ret.key = queuedEvent->key;
	ret.evt = queuedEvent->evt;
	delete queuedEvent; 
	return &ret;
}