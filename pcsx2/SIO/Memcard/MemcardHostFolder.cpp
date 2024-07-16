
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardHostFolder.h"

MemcardHostFolder::MemcardHostFolder(std::string path)
	: MemcardHostBase(path)
{

}

MemcardHostFolder::~MemcardHostFolder() = default;

bool MemcardHostFolder::IsOpened()
{
	return true;
}

s64 MemcardHostFolder::GetSize()
{
	return -1;
}

void MemcardHostFolder::Write(u32 addr, std::vector<u8>& src)
{

}

void MemcardHostFolder::Read(u32 addr, std::vector<u8>& dest)
{

}
