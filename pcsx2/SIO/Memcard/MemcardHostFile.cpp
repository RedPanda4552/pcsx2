
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardHostFile.h"
#include <cstring>

#include "Common.h"
#include "common/FileSystem.h"
#include "common/Error.h"
#include "Host.h"
#include "SIO/Memcard/MemcardHostBase.h"
#include "SIO/Memcard/MemcardPS2.h"

bool MemcardHostFile::Seek(u32 addr)
{
	const int seekResult = FileSystem::FSeek64(this->filePtr, addr, SEEK_SET);

	if (seekResult != 0)
	{
		Console.Error("%s Failed to seek to address %08X", __FUNCTION__, addr);
		return false;
	}

	return true;
}

MemcardHostFile::MemcardHostFile(std::string path)
	: MemcardHostBase(path)
{
	this->filePtr = FileSystem::OpenSharedCFile(this->path.c_str(), "r+b", FileSystem::FileShareMode::DenyWrite);
}

MemcardHostFile::~MemcardHostFile()
{
	if (this->filePtr)
	{
		std::fclose(this->filePtr);
		this->filePtr = nullptr;
	}
}

bool MemcardHostFile::IsOpened()
{
	return this->filePtr;
}

s64 MemcardHostFile::GetSize()
{
	if (!this->filePtr)
	{
		return 0;
	}

	return FileSystem::FSize64(this->filePtr);
}

void MemcardHostFile::Write(u32 addr, std::vector<u8>& src)
{
	if (!this->Seek(addr))
	{
		Console.Error("%s Write failed! Could not seek to address (%08X)", __FUNCTION__, addr);
		return;
	}

	const size_t writeResult = std::fwrite(src.data(), src.size(), 1, this->filePtr);

	if (writeResult != 1)
	{
		Console.Error("%s Failed to write to address %08X", __FUNCTION__, addr);
		return;
	}
}

void MemcardHostFile::Read(u32 addr, std::vector<u8>& dest)
{
	if (!this->Seek(addr))
	{
		memset(dest.data(), 0xFF, dest.size());
		Console.Error("%s Read failed! Could not seek to address (%08X)", __FUNCTION__, addr);
		return;
	}

	const size_t readResult = std::fread(dest.data(), dest.size(), 1, this->filePtr);

	if (readResult != 1)
	{
		memset(dest.data(), 0xFF, dest.size());
		Console.Error("%s Failed to read from address %08X", __FUNCTION__, addr);
		return;
	}
}
