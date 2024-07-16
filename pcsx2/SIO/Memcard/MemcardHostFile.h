
#pragma once

#include "MemcardHostBase.h"

class MemcardHostFile : public MemcardHostBase
{
private:
	bool Seek(u32 addr);

public:
	MemcardHostFile(std::string path);
	~MemcardHostFile();

	bool IsOpened() override;
	s64 GetSize() override;

	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
