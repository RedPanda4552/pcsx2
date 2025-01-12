
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardBase.h"

#include "common/FileSystem.h"
#include "common/Path.h"
#include "Host.h"
#include "fmt/core.h"
#include "IconsFontAwesome5.h"

bool MemcardBase::IsOpened()
{
	switch (this->storageType)
	{
		case Memcard::StorageType::FILE:
		{
			return this->filePtr;
		}
		case Memcard::StorageType::FOLDER:
		{
			return true;
		}
		default:
		{
			return false;
		}
	}
}

MemcardBase::MemcardBase(u32 unifiedSlot, std::string fullPath)
{
	this->unifiedSlot = unifiedSlot;
	this->fullPath = fullPath;
	this->autoEjectTicks = 0;
	this->lastWriteTime = std::chrono::system_clock::now();
	
	if (fullPath.empty())
	{
		this->storageType = Memcard::StorageType::NOT_SET;
	}
	else if (FileSystem::FileExists(fullPath.c_str()))
	{
		this->storageType = Memcard::StorageType::FILE;
	}
	else if (FileSystem::DirectoryExists(fullPath.c_str()))
	{
		this->storageType = Memcard::StorageType::FOLDER;
	}
}

MemcardBase::~MemcardBase() = default;

u32 MemcardBase::GetUnifiedSlot()
{
	return this->unifiedSlot;
}

void MemcardBase::SendWriteMessageToHost()
{
	const std::chrono::duration<float> elapsed = std::chrono::system_clock::now() - this->lastWriteTime;

	if (elapsed > std::chrono::seconds(5))
	{
		Host::AddIconOSDMessage(
			fmt::format(
				"MemoryCardSave{}",
				this->GetUnifiedSlot()),
			ICON_FA_SD_CARD,
			fmt::format(
				TRANSLATE_FS("MemoryCard", "Memory Card '{}' written to storage."),
				Path::GetFileName(this->fullPath)),
			Host::OSD_INFO_DURATION);
		this->lastWriteTime = std::chrono::system_clock::now();
	}
}

u32 MemcardBase::GetAutoEjectTicks()
{
	return this->autoEjectTicks;
}

void MemcardBase::SetAutoEject()
{
	this->autoEjectTicks = 100;
}
