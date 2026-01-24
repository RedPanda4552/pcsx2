
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardBase.h"
#include <atomic>

#include "common/FileSystem.h"
#include "common/Path.h"
#include "Common.h"
#include "Counters.h"
#include "Host.h"
#include "fmt/core.h"
#include "IconsFontAwesome6.h"

// Number of frames a memory card should be considered busy for after doing a write operation.
constexpr u32 MEMCARD_BUSY_TICKS = 300;
// Number of frames after which saving a savestate should warn about not using savestates for permanent storage.
constexpr u32 SAVESTATE_DEPENDENCY_TICKS = 60 * 60 * 60 * 2;

MemcardBase::MemcardBase(u32 unifiedSlot, std::string path)
{
	this->unifiedSlot = unifiedSlot;
	this->path = path;
	this->lastWriteTime = std::chrono::system_clock::now();
}

MemcardBase::~MemcardBase() = default;

std::string MemcardBase::GetPath()
{
	return this->path;
}

u32 MemcardBase::GetUnifiedSlot()
{
	return this->unifiedSlot;
}

// Sends OSD message to host, and a console line to indicate that a memory card write is in progress.
void MemcardBase::SendWriteMessageToHost()
{
	const std::chrono::duration<float> elapsed = std::chrono::system_clock::now() - this->lastWriteTime;

	if (elapsed > std::chrono::seconds(5))
	{
		Console.WriteLn(fmt::format(
			"Writing to Memory Card '{}'...",
			Path::GetFileName(this->GetPath())
		));
		Host::AddIconOSDMessage(
			fmt::format(
				"MemoryCardWrite{}",
				this->GetUnifiedSlot()),
			ICON_FA_SD_CARD,
			fmt::format(
				TRANSLATE_FS("MemoryCard", "Writing to Memory Card '{}'..."),
				Path::GetFileName(this->GetPath())),
			Host::OSD_INFO_DURATION);
		this->lastWriteTime = std::chrono::system_clock::now();
	}
}

// Sends OSD message to host, and a console line to indicate that a memory card read is in progress.
void MemcardBase::SendReadMessageToHost()
{
	const std::chrono::duration<float> elapsed = std::chrono::system_clock::now() - this->lastReadTime;

	if (elapsed > std::chrono::seconds(5))
	{
		Console.WriteLn(fmt::format(
			"Reading from Memory Card '{}'...",
			Path::GetFileName(this->GetPath())
		));
		Host::AddIconOSDMessage(
			fmt::format(
				"MemoryCardRead{}",
				this->GetUnifiedSlot()),
			ICON_FA_SD_CARD,
			fmt::format(
				TRANSLATE_FS("MemoryCard", "Reading from Memory Card '{}'..."),
				Path::GetFileName(this->GetPath())),
			Host::OSD_INFO_DURATION);
		this->lastReadTime = std::chrono::system_clock::now();
	}
}

void MemcardBase::TickBusy()
{
	if (this->currentBusyTicks.load(std::memory_order_relaxed) == 0)
	{
		return;
	}

	currentBusyTicks.fetch_sub(1, std::memory_order_release);
}

void MemcardBase::SetBusy()
{
	this->currentBusyTicks.store(MEMCARD_BUSY_TICKS, std::memory_order_release);
	this->lastBusyFrame = g_FrameCount;
}

bool MemcardBase::IsBusy()
{
	return (this->currentBusyTicks.load(std::memory_order_acquire) > 0);
}

bool MemcardBase::IsPastSaveStateThreshold()
{
	return (g_FrameCount - this->lastBusyFrame > SAVESTATE_DEPENDENCY_TICKS);
}
