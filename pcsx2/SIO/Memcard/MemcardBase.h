
#pragma once

#include "SIO/Memcard/Memcard.h"

enum class CreateResult
{
	OK,
	FAILED_TO_OPEN,
	FAILED_TO_WRITE_BLOCK
};

class MemcardBase
{
private:
	u32 unifiedSlot;

protected:
	size_t absoluteSize;
	std::string path;
	std::chrono::time_point<std::chrono::system_clock> lastWriteTime;
	std::chrono::time_point<std::chrono::system_clock> lastReadTime;
	// Decremented once per frame if nonzero, indicates how many more frames must pass before
	// memcards are considered "no longer being written to". Used as a way to detect if it is
	// unsafe to shutdown the VM due to memcard access.
	std::atomic_uint32_t currentBusyTicks = 0;
	uint32_t lastBusyFrame = 0;

public:
	MemcardBase(u32 unifiedSlot, std::string path);
	virtual ~MemcardBase();

	std::string GetPath();
	u32 GetUnifiedSlot();
	
	void SendWriteMessageToHost();
	void SendReadMessageToHost();

	void TickBusy();
	void SetBusy();
	bool IsBusy();
	bool IsPastSaveStateThreshold();

	virtual Memcard::Type GetType() = 0;
	// Look up the host associated with this emulated memcard, and check that its size
	// matches a known good size for this type of emulated memcard.
	virtual bool ValidateCapacity() = 0;
	virtual bool IsFormatted() = 0;
	virtual void ExecuteCommand() = 0;
	
	// The max theoretical capacity of the PS2 filesystem is 2 GB; this must be at least a u32 to hold that size,
	// but we are also using negatives to signal when a host type has no physical size associated with it. This
	// is typed as s64 to give us the depth needed for that.
	virtual s64 GetSize() = 0;
	virtual CreateResult Create() = 0;
	virtual bool IsOpened() = 0;
	virtual void Write(u32 addr, std::vector<u8>& src) = 0;
	virtual void Read(u32 addr, std::vector<u8>& dest) = 0;
};
