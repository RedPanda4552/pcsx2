
#pragma once

#include "SIO/Memcard/MemcardBase.h"

constexpr const char* PS1_FORMAT_STRING = "MC";
constexpr u8 PS1_FORMAT_STRING_LENGTH = 2;

// Frame size in bytes
constexpr u32 FRAME_SIZE = 128;
// Block size in frames
constexpr u32 FRAMES_PER_BLOCK = 64;
// Memory card size in blocks
constexpr u32 BLOCK_COUNT = 16;
// Convenience constant for the size of an entire card
constexpr u32 PS1_CARD_SIZE = BLOCK_COUNT * FRAMES_PER_BLOCK * FRAME_SIZE;

class MemcardPS1 : public MemcardBase
{
private:
	static constexpr u32 CARD_SIZE_BYTES = 128 * 1024;

	u32 currentCommandByte = 0;
	bool isMemcardCommand = false;
	Memcard::Command currentCommand = Memcard::Command::NOT_SET;

public:
	MemcardPS1(u32 unifiedSlot, std::string path);
	~MemcardPS1();

	Memcard::Type GetType() override;
	bool ValidateCapacity() override;
	bool IsFormatted() override;
	void ExecuteCommand() override;
};
