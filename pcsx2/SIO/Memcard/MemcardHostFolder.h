
#pragma once

#include "SIO/Memcard/MemcardHostBase.h"

#include <array>

class MemcardHostFolder : public MemcardHostBase
{
private:
	
public:
	MemcardHostFolder(std::string path);
	~MemcardHostFolder();

	bool IsOpened() override;
	s64 GetSize() override;

	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
