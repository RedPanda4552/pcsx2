
#pragma once

#include "SIO/Memcard/Memcard.h"

class MemcardBase
{
private:
	u32 unifiedSlot;

protected:
	u32 autoEjectTicks;
	std::chrono::time_point<std::chrono::system_clock> lastWriteTime;

public:
	MemcardBase(u32 unifiedSlot);
	virtual ~MemcardBase();

	u32 GetUnifiedSlot();
	void SendWriteMessageToHost();
	u32 GetAutoEjectTicks();
	void SetAutoEject();
	MemcardHostBase* GetMemcardHost();

	virtual Memcard::Type GetType() = 0;
	virtual bool ValidateCapacity() = 0;
	virtual void ExecuteCommand() = 0;
};
