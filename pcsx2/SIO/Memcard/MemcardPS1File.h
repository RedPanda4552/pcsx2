
#pragma once

#include "MemcardPS1.h"

#include "common/FileSystem.h"

class MemcardPS1File : public MemcardPS1
{
private:
    FileSystem::ManagedCFilePtr filePtr;
    
    bool Seek(u32 addr);

public:
    MemcardPS1File(u32 unifiedSlot, std::string path);
    ~MemcardPS1File();

    s64 GetSize() override;
    CreateResult Create() override;
    bool IsOpened() override;
	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
