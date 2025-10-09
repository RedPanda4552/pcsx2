
#include "PrecompiledHeader.h"

#include "SIO/Memcard/Memcard.h"
#include <memory>

#include "SIO/Memcard/MemoryCardFolder.h"
#include "SIO/Sio.h"
#include "SIO/Memcard/MemcardPS2.h"
#include "SIO/Memcard/MemcardPS1.h"
#include "SIO/Memcard/MemcardNotConnected.h"
#include "SIO/Memcard/MemcardHostBase.h"
#include "SIO/Memcard/MemcardHostFile.h"
#include "SIO/Memcard/MemcardHostFolder.h"

#include "Common.h"
#include "common/Path.h"
#include "common/FileSystem.h"
#include "Host.h"

// Private items we don't want exposed directly in the Memcard.h header
namespace Memcard
{
	static std::array<std::unique_ptr<MemcardHostBase>, Memcard::MAX_SLOTS> s_memcardHosts;
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

void Memcard::CreateMemcard(const u32 unifiedSlot)
{
	const std::string fileName = EmuConfig.Mcd[unifiedSlot].Filename;
	const std::string fullPath = Path::Combine(EmuFolders::MemoryCards, fileName);

	if (FileSystem::FileExists(fileName.c_str()) || FileSystem::DirectoryExists(fileName.c_str()))
	{
		Host::ReportFormattedErrorAsync("Cannot Create Memory Card", 
			"Memory Card already exists: \n\n%s\n\n",
			fullPath.c_str()
		);
	}
}

void Memcard::CreateMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = SIO::ConvertToUnifiedSlot(port, slot);
	return Memcard::InsertMemcard(unifiedSlot);
}

// Attempts to insert a memory card into the specified slot.
// Opens the config and determines what type of memcard to insert depending
// on what the current filename for the given slot is.
void Memcard::InsertMemcard(const u32 unifiedSlot)
{
	const std::string fileName = EmuConfig.Mcd[unifiedSlot].Filename;

	// If no file name present for this slot, then set as no memcard connected.
	if (fileName.empty())
	{
		s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot);
		s_memcardHosts.at(unifiedSlot) = nullptr;
	}
	// Else, we have a file/folder name present...
	else
	{
		const std::string fullPath = Path::Combine(EmuFolders::MemoryCards, fileName);

		// First, determine if the host is a file or folder, and set up a host object for it.
		if (FileSystem::FileExists(fullPath.c_str()))
		{
			s_memcardHosts.at(unifiedSlot) = std::make_unique<MemcardHostFile>(fullPath);
		}
		else if (FileSystem::DirectoryExists(fullPath.c_str()))
		{
			s_memcardHosts.at(unifiedSlot) = std::make_unique<MemcardHostFolder>(fullPath);
		}
		// If the file/folder name specified could not be resolved to an actual existing file/folder,
		// then wipe back to not connected and warn the user.
		else
		{
			s_memcardHosts.at(unifiedSlot) = nullptr;
			s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot);

			Host::ReportFormattedErrorAsync("Memory Card", 
				"Memory Card not found: \n\n%s\n\n"
				"PCSX2 will treat this memory card as ejected.",
				fullPath.c_str()
			);
			
			return;
		}

		// If the host failed to open, then ditch the card
		if (!s_memcardHosts.at(unifiedSlot)->IsOpened())
		{
			s_memcardHosts.at(unifiedSlot) = nullptr;
			s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot);

			Host::ReportFormattedErrorAsync("Memory Card", 
				"Could not open memory card: \n\n%s\n\n"
				"PCSX2 will treat this memory card as ejected. Another instance of PCSX2 may be using this memory card, or you may not have permission to write to this file/folder.\n"
				"You may be able to fix this by closing any other instances of PCSX2, or restarting your computer.",
				fullPath.c_str()
			);

			return;
		}

		// Now that we know the host is fine, determine what type of emulated memory card we are creating.
		const Memcard::Type memcardType = Memcard::GetMemcardType(fullPath);

		switch (memcardType)
		{
			case Memcard::Type::PS2:
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardPS2>(unifiedSlot);
				break;
			}
			case Memcard::Type::PS1:
			{
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardPS1>(unifiedSlot);
				break;
			}
			default:
			{
				s_memcardHosts.at(unifiedSlot) = nullptr;
				s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot);
				
				Host::ReportFormattedErrorAsync("Memory Card", 
					"Unrecognized file extension: \n\n%s\n\n"
					"Please check your memory card settings and insert a valid memory card.",
					fullPath.c_str()
				);

				return;
			}
		}

		// Validate that the capacity is okay for the type of emulated card
		MemcardBase* memcard = s_memcards.at(unifiedSlot).get();

		if (!memcard->ValidateCapacity())
		{
			s_memcardHosts.at(unifiedSlot) = nullptr;
			s_memcards.at(unifiedSlot) = std::make_unique<MemcardNotConnected>(unifiedSlot);
			
			Host::ReportFormattedErrorAsync("Memory Card", 
				"Malformed memory card: \n\n%s\n\n"
				"File size does not match a known memory card size. Please check your memory card settings and insert a valid memory card.",
				fullPath.c_str()
			);

			return;
		}
	}
}

void Memcard::InsertMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = sioConvertPortAndSlotToPad(port, slot);
	return Memcard::InsertMemcard(unifiedSlot);
}

void Memcard::RemoveMemcard(const u32 unifiedSlot)
{
	s_memcards.at(unifiedSlot).reset();
	s_memcardHosts.at(unifiedSlot).reset();
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

MemcardHostBase* Memcard::GetMemcardHost(const u32 unifiedSlot)
{
	return s_memcardHosts.at(unifiedSlot).get();
}

MemcardHostBase* Memcard::GetMemcardHost(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = sioConvertPortAndSlotToPad(port, slot);
	return s_memcardHosts.at(unifiedSlot).get();
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

		std::unique_ptr<MemcardHostBase> memcardHost = nullptr;

		// First test if this is a PS2 memory card
		for (std::string ps2Extension : Memcard::PS2_MEMCARD_FILE_EXTENSIONS)
		{
			if (fd.FileName.ends_with(ps2Extension))
			{
				// Now determine if it is a file or folder
				if (fd.Attributes & FILESYSTEM_FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string superblockPath = Path::Combine(fd.FileName, SUPERBLOCK_FILENAME);
					

					if (FileSystem::FileExists(superblockPath.c_str()))
					{
						memcardHost = std::make_unique<MemcardHostFolder>(fd.FileName);
						summaries.push_back(
							{
								std::move(baseName), 
								std::move(fd.FileName), 
								fd.ModificationTime,
								Memcard::Type::PS2,
								Memcard::HostType::FOLDER, 
								memcardHost->GetSize(), 
								memcardHost->IsFormatted()
							}
						);
					}
				}
				else
				{

				}
			}
		}

		// Skip any items which don't match a known memory card file extension
		for (std::string extension : Memcard::MEMCARD_FILE_EXTENSIONS)
		{
			if (fd.FileName.ends_with(extension))
			{
				AvailableMemcardSummary summary;

				// Folder memcards: Check for a superblock file
				if (fd.Attributes & FILESYSTEM_FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string folderMemcardPath = Path::Combine(EmuFolders::MemoryCards.c_str(), fd.FileName);
					std::string superblockPath = Path::Combine(folderMemcardPath, SUPERBLOCK_FILENAME);

					if (FileSystem::FileExists(superblockPath.c_str()))
					{
						summary.name = fd.FileName;
						summary.path = folderMemcardPath;
						summary.modified_time = fd.ModificationTime;
						summary.type = Memcard::Type::PS2;
						summary.hostType = Memcard::HostType::FOLDER;
						summary.size = hostFolder->GetSize();

					}

					FolderMemoryCard sourceFolderMemoryCard;
					Pcsx2Config::McdOptions config;
					config.Enabled = true;
					config.Type = MemoryCardType::Folder;
					sourceFolderMemoryCard.Open(fd.FileName, config, (8 * 1024 * 1024) / FolderMemoryCard::ClusterSize, false, "");

					
					sourceFolderMemoryCard.Close(false);
				}
				else
				{
					if (fd.Size < MCD_SIZE)
						continue;

					const bool formatted = FileMcd_IsMemoryCardFormatted(fd.FileName);
					summaries.push_back({std::move(baseName), std::move(fd.FileName), fd.ModificationTime,
						MemoryCardType::File, GetMemoryCardFileTypeFromSize(fd.Size),
						static_cast<u32>(fd.Size), formatted});
				}
				break;
			}
		}
	}

	return summaries;
}
