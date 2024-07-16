
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

bool MemcardPS1::IsFormatted()
{
	std::vector<u8> buf(FRAME_SIZE, 0xFF);
	this->Read(0, buf);
	
	bool hasPrefix = strncmp(reinterpret_cast<const char*>(buf.data()), PS1_FORMAT_STRING, PS1_FORMAT_STRING_LENGTH) == 0;
	u8 checksum = 0;

	// XOR all except the final byte of the frame
	for (u8 i = 0; i < FRAME_SIZE - 1; i++)
	{
		checksum ^= buf.at(i);
	}

	bool checksumMatches = checksum == buf.at(FRAME_SIZE - 1);
	return hasPrefix && checksumMatches;
}

void MemcardPS1::ExecuteCommand()
{

}
