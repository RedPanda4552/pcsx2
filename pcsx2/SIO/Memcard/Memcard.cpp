
#include "PrecompiledHeader.h"

#include "SIO/Memcard/Memcard.h"

#include "SIO/Sio.h"
#include "SIO/Memcard/MemcardBase.h"
#include "SIO/Memcard/MemcardNotConnected.h"
#include "SIO/Memcard/MemcardPS1.h"
#include "SIO/Memcard/MemcardPS1File.h"
#include "SIO/Memcard/MemcardPS2.h"
#include "SIO/Memcard/MemcardPS2File.h"
#include "SIO/Memcard/MemcardPS2Folder.h"

#include "Config.h"
#include "Common.h"
#include "common/Path.h"
#include "common/FileSystem.h"
#include "Host.h"
#include "IconsPromptFont.h"

#include <memory>

// Private items we don't want exposed directly in the Memcard.h header
namespace Memcard
{
	static std::array<std::unique_ptr<MemcardBase>, Memcard::MAX_SLOTS> s_memcards;
} // namespace Memcard

bool Memcard::Initialize()
{
	for (u32 i = 0; i < Memcard::MAX_SLOTS; i++)
	{
		Memcard::InsertMemcard(i);
	}
	
	return true;
}

void Memcard::Shutdown()
{
	for (u32 i = 0; i < Memcard::MAX_SLOTS; i++)
	{
		Memcard::RemoveMemcard(i);
	}
}

bool Memcard::CreateMemcard(const u32 unifiedSlot)
{
	const std::string fileName = EmuConfig.Mcd[unifiedSlot].Filename;
	const std::string fullPath = Path::Combine(EmuFolders::MemoryCards, fileName);
	const MemoryCardType type = EmuConfig.Mcd[unifiedSlot].Type;

	if (fileName.empty())
	{
		Host::ReportFormattedErrorAsync("Cannot Create Memory Card", 
			"Cannot create Memory Card for empty slot (%d)",
			unifiedSlot
		);

		return false;
	}

	if (FileSystem::FileExists(fileName.c_str()) || FileSystem::DirectoryExists(fileName.c_str()))
	{
		Host::ReportFormattedErrorAsync("Cannot Create Memory Card", 
			"Memory Card already exists: \n\n%s",
			fullPath.c_str()
		);

		return false;
	}

	MemcardBase* memcardPtr = s_memcards.at(unifiedSlot).get();
	CreateResult res = memcardPtr->Create();

	switch (res)
	{
		case CreateResult::OK:
			Console.WriteLn("Created Memory Card: %s (mapped to unified slot %d)", fullPath.c_str(), unifiedSlot);
			return true;
		case CreateResult::FAILED_TO_OPEN:
			Host::ReportFormattedErrorAsync("Cannot Create Memory Card", 
				"An error occurred while trying to get a file descriptor for path:\n\n%s",
				fullPath.c_str()
			);
			return false;
		default:
			Host::ReportFormattedErrorAsync("Cannot Create Memory Card", 
				"An unhandled error was caught, please report this to the PCSX2 team.\n\n%s",
				fullPath.c_str()
			);
			return false;
	}
}

bool Memcard::CreateMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = SIO::ConvertToUnifiedSlot(port, slot);
	return Memcard::CreateMemcard(unifiedSlot);
}

// Attempts to insert a memory card into the specified slot.
// Opens the config and determines what type of memcard to insert depending
// on what the current filename for the given slot is.
bool Memcard::InsertMemcard(const u32 unifiedSlot)
{
	const std::string fileName = EmuConfig.Mcd[unifiedSlot].Filename;
	const std::string fullPath = Path::Combine(EmuFolders::MemoryCards, fileName);

	// If no file name present for this slot, then set as no memcard connected.
	if (fileName.empty())
	{
		s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot, fullPath);
		return true;
	}

	for (std::string ps2Extension : PS2_MEMCARD_FILE_EXTENSIONS)
	{
		if (fileName.ends_with(ps2Extension))
		{
			if (FileSystem::FileExists(fullPath.c_str()))
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardPS2File>(unifiedSlot, fullPath);
			}
			else if (FileSystem::DirectoryExists(fullPath.c_str()))
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardPS2Folder>(unifiedSlot, fullPath);
			}
			// If the file/folder name specified could not be resolved to an existing file/folder,
			// then wipe back to not connected and warn the user.
			else
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot, fullPath);

				Host::ReportFormattedErrorAsync("Cannot Insert Memory Card", 
					"Configured Memory Card not found: \n\n%s\n\n"
					"PCSX2 will treat this Memory Card as ejected.",
					fullPath.c_str()
				);
			}

			return true;
		}
	}

	for (std::string ps1Extension : PS1_MEMCARD_FILE_EXTENSIONS)
	{
		if (fileName.ends_with(ps1Extension))
		{
			if (FileSystem::FileExists(fullPath.c_str()))
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardPS1File>(unifiedSlot, fullPath);
				return true;
			}
			else if (FileSystem::DirectoryExists(fullPath.c_str()))
			{
				Host::ReportFormattedErrorAsync("Cannot Insert Memory Card", 
					"Unsupported memory card type, folders are not supported for PS1\n\n%s\n\n"
					"PCSX2 will treat this Memory Card as ejected.",
					fullPath.c_str()
				);

				return false;
			}
			// If the file/folder name specified could not be resolved to an existing file/folder,
			// then wipe back to not connected and warn the user.
			else
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot, fullPath);

				Host::ReportFormattedErrorAsync("Cannot Insert Memory Card", 
					"Configured Memory Card not found: \n\n%s\n\n"
					"PCSX2 will treat this Memory Card as ejected.",
					fullPath.c_str()
				);
			}

			return true;
		}
	}
	
	// If file extension is not valid, control will fall through to here.
	s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot, fullPath);

	Host::ReportFormattedErrorAsync("Cannot Insert Memory Card", 
		"Configured Memory Card is not a recognized file extension: \n\n%s\n\n"
		"PCSX2 will treat this Memory Card as ejected.",
		fullPath.c_str()
	);
}

bool Memcard::InsertMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = sioConvertPortAndSlotToPad(port, slot);
	return Memcard::InsertMemcard(unifiedSlot);
}

void Memcard::RemoveMemcard(const u32 unifiedSlot)
{
	s_memcards.at(unifiedSlot).reset();
}

void Memcard::RemoveMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = sioConvertPortAndSlotToPad(port, slot);
	return Memcard::RemoveMemcard(unifiedSlot);
}

MemcardBase* Memcard::GetMemcard(const u32 unifiedSlot)
{
	return s_memcards.at(unifiedSlot).get();
}

MemcardBase* Memcard::GetMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = sioConvertPortAndSlotToPad(port, slot);
	return s_memcards.at(unifiedSlot).get();
}

// Resolve what type of emulated memcard a host file/folder is representing.
Memcard::Type Memcard::GetMemcardType(std::string fullPath)
{
	// First compare with known PS2 memcard file extensions
	for (std::string ps2Extension : Memcard::PS2_MEMCARD_FILE_EXTENSIONS)
	{
		if (fullPath.ends_with(ps2Extension))
		{
			return Memcard::Type::PS2;
		}
	}

	// If not PS2, then check PS1 types
	for (std::string ps1Extension : Memcard::PS1_MEMCARD_FILE_EXTENSIONS)
	{
		if (fullPath.ends_with(ps1Extension))
		{
			return Memcard::Type::PS1;
		}
	}

	return Memcard::Type::NOT_CONNECTED;
}

// Get a vector of memory cards currently in the configured memory card directory that are available for PCSX2.
// Includes or excludes currently slotted and in use memory cards depending on includeInUse. Primarily used by
// the frotend to list out available memory cards in the UI.
std::vector<Memcard::AvailableMemcardSummary> Memcard::GetAvailableMemcards(bool includeInUse)
{
	std::vector<FILESYSTEM_FIND_DATA> files;
	FileSystem::FindFiles(EmuFolders::MemoryCards.c_str(), "*", FILESYSTEM_FIND_FILES | FILESYSTEM_FIND_FOLDERS | FILESYSTEM_FIND_HIDDEN_FILES, &files);

	std::vector<Memcard::AvailableMemcardSummary> summaries;
	summaries.reserve(files.size());

	for (FILESYSTEM_FIND_DATA& fd : files)
	{
		std::string baseName(Path::GetFileName(fd.FileName));

		// If we only want unused cards, skip over them here.
		if (!includeInUse)
		{
			bool in_use = false;

			for (size_t i = 0; i < std::size(EmuConfig.Mcd); i++)
			{
				if (EmuConfig.Mcd[i].Filename == baseName)
				{
					in_use = true;
					break;
				}
			}

			if (in_use)
			{
				continue;
			}
		}

		std::unique_ptr<MemcardBase> memcard = nullptr;
		
		// First test if this is a PS2 memory card
		for (std::string ps2Extension : Memcard::PS2_MEMCARD_FILE_EXTENSIONS)
		{
			if (fd.FileName.ends_with(ps2Extension))
			{
				// If folder
				if (fd.Attributes & FILESYSTEM_FILE_ATTRIBUTE_DIRECTORY)
				{
					memcard = std::make_unique<MemcardPS2Folder>(0, fd.FileName);
				}
				// If file
				else
				{
					memcard = std::make_unique<MemcardPS2File>(0, fd.FileName);
				}

				break;
			}
		}

		// If no PS2 match, then check PS1 next.
		if (!memcard)
		{
			for (std::string ps1Extension : Memcard::PS1_MEMCARD_FILE_EXTENSIONS)
			{
				if (fd.FileName.ends_with(ps1Extension))
				{
					// If folder
					if (fd.Attributes & FILESYSTEM_FILE_ATTRIBUTE_DIRECTORY)
					{
						// Do nothing, we don't support PS1 folder memcards...
					}
					// If file
					else
					{
						memcard = std::make_unique<MemcardPS1File>(0, fd.FileName);
					}

					break;
				}
			}
		}

		// Only add to the summaries list if we had a good match.
		if (memcard)
		{
			summaries.push_back({
				std::move(baseName), 
				std::move(fd.FileName), 
				fd.ModificationTime,
				Memcard::Type::PS2,
				Memcard::HostType::FOLDER, 
				memcard->GetSize(), 
				memcard->IsFormatted()
			});
			continue;
		}
	}

	return summaries;
}

bool Memcard::IsBusy()
{
	for (u32 i = 0; i < Memcard::MAX_SLOTS; i++)
	{
		MemcardBase* memcard = s_memcards.at(i).get();

		if (memcard->IsBusy())
		{
			return true;
		}
	}

	return false;
}

void Memcard::Freeze()
{
	for (u32 i = 0; i < Memcard::MAX_SLOTS; i++)
	{
		MemcardBase* memcard = s_memcards.at(i).get();

		if (memcard->IsPastSaveStateThreshold())
		{
			Host::AddIconOSDMessage("MemcardSaveStateThreshold", ICON_PF_MEMORY_CARD,
			TRANSLATE_SV("MemoryCard", "The virtual console hasn't saved to your memory card in a long time.\nSavestates should not be used in place of in-game saves."), Host::OSD_INFO_DURATION);
		}
	}
}