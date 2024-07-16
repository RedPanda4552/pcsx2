
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardBase.h"

#include "SIO/Memcard/MemcardHostFile.h"
#include "SIO/Memcard/MemcardHostFolder.h"

#include "common/FileSystem.h"
#include "common/Path.h"
#include "Host.h"
#include "fmt/core.h"
#include "IconsFontAwesome5.h"

MemcardBase::MemcardBase(u32 unifiedSlot, std::string fullPath)
{
	this->unifiedSlot = unifiedSlot;
	this->autoEjectTicks = 0;
	this->lastWriteTime = std::chrono::system_clock::now();
	
	if (fullPath == "")
	{
		return;
	}
	else if (FileSystem::FileExists(fullPath.c_str()))
	{
		this->hostType = Memcard::HostType::FILE;
		this->memcardHost = std::make_unique<MemcardHostFile>(fullPath);
	}
	else if (FileSystem::DirectoryExists(fullPath.c_str()))
	{
		this->hostType = Memcard::HostType::FOLDER;
		this->memcardHost = std::make_unique<MemcardHostFolder>(fullPath);
	}
}

MemcardBase::~MemcardBase() = default;

u32 MemcardBase::GetUnifiedSlot()
{
	return this->unifiedSlot;
}

MemcardHostBase* MemcardBase::GetHost()
{
	return this->memcardHost.get();
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
				Path::GetFileName(this->memcardHost->GetPath())),
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
