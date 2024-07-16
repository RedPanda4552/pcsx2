
#pragma once

#include "SIO/Memcard/MemcardBase.h"

class MemcardNotConnected : public MemcardBase
{
private:
	

public:
	MemcardNotConnected(u32 unifiedSlot, std::string path);
	~MemcardNotConnected();

	Memcard::Type GetType() override;
	CreateResult Create() override;
	bool ValidateCapacity() override;
	bool IsFormatted() override;
	void ExecuteCommand() override;

	bool IsOpened() override;
	s64 GetSize() override;
	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
