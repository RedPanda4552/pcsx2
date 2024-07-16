
#pragma once

#include "SIO/Memcard/Memcard.h"
#include "SIO/Memcard/MemcardHostBase.h"

class MemcardBase
{
private:
	u32 unifiedSlot;

protected:
	Memcard::HostType hostType;
	std::unique_ptr<MemcardHostBase> memcardHost;
	u32 autoEjectTicks;
	std::chrono::time_point<std::chrono::system_clock> lastWriteTime;

public:
	MemcardBase(u32 unifiedSlot, std::string fullPath);
	virtual ~MemcardBase();

	u32 GetUnifiedSlot();
	MemcardHostBase* GetHost();
	void SendWriteMessageToHost();
	u32 GetAutoEjectTicks();
	void SetAutoEject();

	virtual Memcard::Type GetType() = 0;
	virtual void ExecuteCommand() = 0;
};
