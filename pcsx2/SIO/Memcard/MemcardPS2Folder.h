
#pragma once 

#include "SIO/Memcard/MemcardPS2.h"

constexpr const char* SUPERBLOCK_FILENAME = "_pcsx2_superblock";

class MemcardPS2Folder : public MemcardPS2
{
private:
    std::FILE* rootDirPtr;
    
public:
    MemcardPS2Folder(u32 unifiedSlot, std::string path);
    ~MemcardPS2Folder();

    s64 GetSize() override;
    CreateResult Create() override;
    bool IsOpened() override;
	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
