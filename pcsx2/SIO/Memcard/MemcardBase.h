
#pragma once

#include "SIO/Memcard/Memcard.h"

class MemcardBase
{
private:
	u32 unifiedSlot;
	std::string fullPath;

protected:
	Memcard::StorageType storageType;
	std::FILE* filePtr;
	u32 autoEjectTicks;
	std::chrono::time_point<std::chrono::system_clock> lastWriteTime;

	bool IsOpened();

	virtual bool Seek(u32 addr) = 0;
	virtual bool SeekFile(u32 addr) = 0;
	virtual bool SeekFolder(u32 addr) = 0;

	virtual void Write(u32 addr, std::vector<u8>& src) = 0;
	virtual void WriteFile(u32 addr, std::vector<u8>& src) = 0;
	virtual void WriteFolder(u32 addr, std::vector<u8>& src) = 0;
	
	virtual void Read(u32 addr, std::vector<u8>& dest) = 0;
	virtual void ReadFile(u32 addr, std::vector<u8>& dest) = 0;
	virtual void ReadFolder(u32 addr, std::vector<u8>& dest) = 0;

public:
	MemcardBase(u32 unifiedSlot, std::string fullPath);
	virtual ~MemcardBase();

	u32 GetUnifiedSlot();
	void SendWriteMessageToHost();
	u32 GetAutoEjectTicks();
	void SetAutoEject();

	virtual Memcard::Type GetType() = 0;
	virtual void ExecuteCommand() = 0;
};
