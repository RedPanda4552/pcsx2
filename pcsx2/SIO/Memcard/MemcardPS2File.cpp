
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS2File.h"

#include "SIO/Memcard/MemcardBase.h"
#include "SIO/Memcard/MemcardPS2.h"

#include "common/Console.h"
#include "common/FileSystem.h"
#include "Host.h"

bool MemcardPS2File::Seek(u32 addr)
{
	const int seekResult = FileSystem::FSeek64(this->filePtr, addr, SEEK_SET);

	if (seekResult != 0)
	{
		Console.Error("Memcard: Failed to seek to address %08X", addr);
		return false;
	}

	return true;
}

MemcardPS2File::MemcardPS2File(u32 unifiedSlot, std::string path)
    : MemcardPS2(unifiedSlot, path)
{
    this->filePtr = FileSystem::OpenSharedCFile(path.c_str(), "r+b", FileSystem::FileShareMode::DenyWrite);
	this->absoluteSize = FileSystem::FSize64(this->filePtr);
	this->clusterCount = static_cast<ClusterCount>(this->absoluteSize / (PAGE_LENGTH + ECC_LENGTH) / PAGES_PER_CLUSTER);
	this->eraseBlockCount = static_cast<EraseBlockCount>(this->absoluteSize / ERASE_BLOCK_LENGTH);
}

MemcardPS2File::~MemcardPS2File() = default;

s64 MemcardPS2File::GetSize()
{
	return this->absoluteSize;
}

CreateResult MemcardPS2File::Create()
{
    if (!this->filePtr)
    {
        return CreateResult::FAILED_TO_OPEN;
    }

	this->Seek(0);
    u8 buf[ERASE_BLOCK_LENGTH];
    std::memset(buf, 0xFF, sizeof(buf));

    for (u32 i = 0; i < static_cast<u32>(eraseBlockCount); i++)
    {
        if (std::fwrite(buf, sizeof(buf), 1, this->filePtr) != 1)
        {
            return CreateResult::FAILED_TO_WRITE_BLOCK;
        }
    }

    return CreateResult::OK;
}

bool MemcardPS2File::IsOpened()
{
	return this->filePtr;
}

void MemcardPS2File::Write(u32 addr, std::vector<u8>& src)
{
    if (!this->Seek(addr))
	{
		Console.Error("Memcard: Write failed! Could not seek to address (%08X)", addr);
		return;
	}

	const size_t writeResult = std::fwrite(src.data(), src.size(), 1, this->filePtr);

	if (writeResult != 1)
	{
		Console.Error("Memcard: Failed to write to address %08X", addr);
		return;
	}
}

void MemcardPS2File::Read(u32 addr, std::vector<u8>& dest)
{
    if (!this->Seek(addr))
	{
		memset(dest.data(), 0xFF, dest.size());
		Console.Error("Memcard: Read failed! Could not seek to address (%08X)", addr);
		return;
	}

	const size_t readResult = std::fread(dest.data(), dest.size(), 1, this->filePtr);

	if (readResult != 1)
	{
		memset(dest.data(), 0xFF, dest.size());
		Console.Error("Memcard: Failed to read from address %08X", addr);
		return;
	}
}
