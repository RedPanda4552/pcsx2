
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS2Folder.h"

MemcardPS2Folder::MemcardPS2Folder(u32 unifiedSlot, std::string path)
    : MemcardPS2(unifiedSlot, path)
{

}

MemcardPS2Folder::~MemcardPS2Folder() = default;

s64 MemcardPS2Folder::GetSize()
{
    return -1;
}

CreateResult MemcardPS2Folder::Create()
{

}

bool MemcardPS2Folder::IsOpened()
{
    return this->rootDirPtr;
}

void MemcardPS2Folder::Write(u32 addr, std::vector<u8>& src)
{

}

void MemcardPS2Folder::Read(u32 addr, std::vector<u8>& dest)
{
    
}
