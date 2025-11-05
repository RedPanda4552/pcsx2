
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS1.h"
#include "SIO/Memcard/MemcardHostBase.h"

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
	if (this->GetMemcardHost() != nullptr)
	{
		const u32 fileSize = this->GetMemcardHost()->GetSize();
		return fileSize == MemcardPS1::CARD_SIZE_BYTES;
	}

	return false;
}

void MemcardPS1::ExecuteCommand()
{

}
