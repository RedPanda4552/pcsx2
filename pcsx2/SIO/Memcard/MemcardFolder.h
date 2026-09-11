
#pragma once

#include "SIO/Memcard/MemcardBase.h"

class MemcardFolder : public MemcardBase
{
private:

public:
	MemcardFolder();
	~MemcardFolder();

	void Read(u32 addr, u32 len, std::vector<u8>& dest) override;
	void Write(u32 addr, u32 len, std::vector<u8>& src) override;
};
