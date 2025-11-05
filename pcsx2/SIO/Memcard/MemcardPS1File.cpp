
#include "PrecompiledHeader.h"

#include "MemcardPS1File.h"
#include "SIO/Memcard/MemcardPS1.h"

#include "common/Console.h"

bool MemcardPS1File::Seek(u32 addr)
{
	const int seekResult = FileSystem::FSeek64(this->filePtr.get(), addr, SEEK_SET);

	if (seekResult != 0)
	{
		Console.Error("Memcard: Failed to seek to address %08X", addr);
		return false;
	}

	return true;
}

MemcardPS1File::MemcardPS1File(u32 unifiedSlot, std::string path)
    : MemcardPS1(unifiedSlot, path)
{
    this->filePtr = FileSystem::OpenManagedCFile(path.c_str(), "wb");
}

MemcardPS1File::~MemcardPS1File() = default;

s64 MemcardPS1File::GetSize()
{
	return PS1_CARD_SIZE;
}

CreateResult MemcardPS1File::Create()
{
    if (!this->filePtr)
    {
        return CreateResult::FAILED_TO_OPEN;
    }

    this->Seek(0);
    u8 buf[FRAME_SIZE * FRAMES_PER_BLOCK];
    std::memset(buf, 0xFF, sizeof(buf));

    for (u32 i = 0; i < BLOCK_COUNT; i++)
    {
        if (std::fwrite(buf, sizeof(buf), 1, this->filePtr.get()) != 1)
        {
            return CreateResult::FAILED_TO_WRITE_BLOCK;
        }
    }

    return CreateResult::OK;
}

bool MemcardPS1File::IsOpened()
{
	return this->filePtr.get();
}

void MemcardPS1File::Write(u32 addr, std::vector<u8>& src)
{
    if (!this->Seek(addr))
	{
		Console.Error("Memcard: Write failed! Could not seek to address (%08X)", addr);
		return;
	}

	const size_t writeResult = std::fwrite(src.data(), src.size(), 1, this->filePtr.get());

	if (writeResult != 1)
	{
		Console.Error("Memcard: Failed to write to address %08X", addr);
		return;
	}
}

void MemcardPS1File::Read(u32 addr, std::vector<u8>& dest)
{
    if (!this->Seek(addr))
	{
		memset(dest.data(), 0xFF, dest.size());
		Console.Error("Memcard: Read failed! Could not seek to address (%08X)", addr);
		return;
	}

	const size_t readResult = std::fread(dest.data(), dest.size(), 1, this->filePtr.get());

	if (readResult != 1)
	{
		memset(dest.data(), 0xFF, dest.size());
		Console.Error("Memcard: Failed to read from address %08X", addr);
		return;
	}
}
