
#pragma once

#include "SIO/Memcard/MemcardBase.h"

class MemcardPS1 : public MemcardBase
{
private:
	u32 currentCommandByte = 0;
	bool isMemcardCommand = false;
	Memcard::Command currentCommand = Memcard::Command::NOT_SET;

protected:
	// =========================================================================
	// Host storage implementations
	// =========================================================================

	bool Seek(u32 addr) override;
	bool SeekFile(u32 addr) override;
	bool SeekFolder(u32 addr) override;

	void Write(u32 addr, std::vector<u8>& src) override;
	void WriteFile(u32 addr, std::vector<u8>& src) override;
	void WriteFolder(u32 addr, std::vector<u8>& src) override;
	
	void Read(u32 addr, std::vector<u8>& dest) override;
	void ReadFile(u32 addr, std::vector<u8>& dest) override;
	void ReadFolder(u32 addr, std::vector<u8>& dest) override;

public:
	MemcardPS1(u32 unifiedSlot, std::string fullPath);
	~MemcardPS1();

	Memcard::Type GetType() override;
	void ExecuteCommand() override;
};
