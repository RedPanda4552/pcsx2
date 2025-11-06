
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS1.h"

MemcardPS1::MemcardPS1(u32 unifiedSlot, std::string path)
	: MemcardBase(unifiedSlot, path)
{

}

MemcardPS1::~MemcardPS1() = default;

Memcard::Type MemcardPS1::GetType()
{
	return Memcard::Type::PS1;
}

bool MemcardPS1::ValidateCapacity()
{
	const u32 fileSize = this->GetSize();
	return fileSize == MemcardPS1::CARD_SIZE_BYTES;
}

void MemcardPS1::ExecuteCommand()
{

}
