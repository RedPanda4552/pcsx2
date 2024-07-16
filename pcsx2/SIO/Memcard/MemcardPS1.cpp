
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS1.h"

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
