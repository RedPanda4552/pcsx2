
#pragma once

#include "SIO/Memcard/MemcardBase.h"

class MemcardNotConnected : public MemcardBase
{
private:
	
protected:
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
	MemcardNotConnected(u32 unifiedSlot);
	~MemcardNotConnected();

	Memcard::Type GetType() override;
	void ExecuteCommand() override;
};
