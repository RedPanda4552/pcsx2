
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardHostFolder.h"
#include <cstdio>

#include "common/Path.h"
#include "common/FileSystem.h"

FileEntry::FileEntry(std::string fileName)
{
	this->m_fileName = fileName;
}

MemcardHostFolder::MemcardHostFolder(std::string path)
	: MemcardHostBase(path)
{
	// Open the superblock file on the inherited filePtr so we can keep it open for the lifecycle of the card,
	// and check it later without constantly opening and closing handles.
	const std::string superblockPath(Path::Combine(path, SUPERBLOCK_FILENAME));
	this->filePtr = FileSystem::OpenSharedCFile(superblockPath.c_str(), "r+b", FileSystem::FileShareMode::DenyWrite);
}

MemcardHostFolder::~MemcardHostFolder()
{
	if (this->filePtr)
	{
		std::fclose(this->filePtr);
		this->filePtr = nullptr;
	}
}

bool MemcardHostFolder::IsOpened()
{
	return this->filePtr;
}

s64 MemcardHostFolder::GetSize()
{
	return FOLDER_MEMCARD_REPORTED_SIZE;
}

void MemcardHostFolder::Write(u32 addr, std::vector<u8>& src)
{

}

void MemcardHostFolder::Read(u32 addr, std::vector<u8>& dest)
{

}
