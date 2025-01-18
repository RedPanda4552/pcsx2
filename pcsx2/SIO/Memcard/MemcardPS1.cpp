
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS1.h"

bool MemcardPS1::Seek(u32 addr)
{
	return false;
}

bool MemcardPS1::SeekFile(u32 addr)
{
	return false;
}

bool MemcardPS1::SeekFolder(u32 addr)
{
	return false;
}

void MemcardPS1::Write(u32 addr, std::vector<u8>& src)
{
	
}

void MemcardPS1::WriteFile(u32 addr, std::vector<u8>& src)
{
	
}

void MemcardPS1::WriteFolder(u32 addr, std::vector<u8>& src)
{
	
}

void MemcardPS1::Read(u32 addr, std::vector<u8>& dest)
{
	memset(dest.data(), dest.size(), 0xFF);
}

void MemcardPS1::ReadFile(u32 addr, std::vector<u8>& dest)
{
	
}

void MemcardPS1::ReadFolder(u32 addr, std::vector<u8>& dest)
{

}

MemcardPS1::MemcardPS1(u32 unifiedSlot, std::string fullPath)
	: MemcardBase(unifiedSlot, fullPath)
{

}
MemcardPS1::~MemcardPS1() = default;

Memcard::Type MemcardPS1::GetType()
{
	return Memcard::Type::PS1;
}

void MemcardPS1::ExecuteCommand()
{

}
