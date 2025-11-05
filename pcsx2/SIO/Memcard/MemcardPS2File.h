
#pragma once 

#include "SIO/Memcard/MemcardPS2.h"

#include "common/FileSystem.h"

class MemcardPS2File : public MemcardPS2
{
private:
    FileSystem::ManagedCFilePtr filePtr;

    bool Seek(u32 addr);
    
public:
    MemcardPS2File(u32 unifiedSlot, std::string path);
    ~MemcardPS2File();

    s64 GetSize() override;
    CreateResult Create() override;
    bool IsOpened() override;
	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
