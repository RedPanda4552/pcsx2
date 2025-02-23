
#include "PrecompiledHeader.h"

#include "SIO/Memcard/Memcard.h"

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

namespace Memcard
{
	static std::array<std::unique_ptr<MemcardHostBase>, Memcard::MAX_SLOTS> s_memcardHosts;
	static std::array<std::unique_ptr<MemcardBase>, Memcard::MAX_SLOTS> s_memcards;
} // namespace Memcard

bool Memcard::Initialize()
{
	for (auto i = 0; i < Memcard::MAX_SLOTS; i++)
	{
		const std::string fileName = EmuConfig.Mcd[i].Filename;

		if (fileName.empty())
		{
			s_memcards.at(i) = std::make_unique<MemcardNotConnected>(i);
			s_memcardHosts.at(i) = nullptr;
		}
		else
		{
			const std::string fullPath = Path::Combine(EmuFolders::MemoryCards, fileName);

			// First, determine if the host is a file or folder, and set up a host object for it.
			if (FileSystem::FileExists(fullPath.c_str()))
			{
				s_memcardHosts.at(i) = std::make_unique<MemcardHostFile>(fullPath);
			}
			else if (FileSystem::DirectoryExists(fullPath.c_str()))
			{
				s_memcardHosts.at(i) = std::make_unique<MemcardHostFolder>(fullPath);
			}
			else
			{
				s_memcardHosts.at(i) = nullptr;
				s_memcards.at(i) = std::make_unique<MemcardNotConnected>(i);

				Host::ReportFormattedErrorAsync("Memory Card", 
					"Memory card not found: \n\n%s\n\n"
					"PCSX2 will treat this memory card as ejected for this session.",
					fullPath.c_str()
				);
				
				continue;
			}

			// If the host failed to open, then ditch the card
			if (!s_memcardHosts.at(i)->IsOpened())
			{
				s_memcardHosts.at(i) = nullptr;
				s_memcards.at(i) = std::make_unique<MemcardNotConnected>(i);

				Host::ReportFormattedErrorAsync("Memory Card", 
					"Access denied to memory card: \n\n%s\n\n"
					"PCSX2 will treat this memory card as ejected for this session. Another instance of PCSX2 may be using this memory card. Close any other instances of PCSX2, or restart your computer.%s",
					fullPath.c_str(),
#ifdef WIN32
					"\n\nIf your memory card is in a write-protected folder such as \"Program Files\" or \"Program Files (x86)\", move it to another folder, such as \"Documents\" or \"Desktop\"."
#else
					""
#endif
				);

				continue;
			}

			// Now that we know the host is fine, determine what type of emulated memory card we are creating.
			if (fullPath.ends_with(".ps2"))
			{
				s_memcards.at(i) = std::make_unique<MemcardPS2>(i);
			}
			else if (fullPath.ends_with(".mcd"))
			{
				s_memcards.at(i) = std::make_unique<MemcardPS1>(i);
			}
			else
			{
				s_memcardHosts.at(i) = nullptr;
				s_memcards.at(i) = std::make_unique<MemcardNotConnected>(i);
				
				Host::ReportFormattedErrorAsync("Memory Card", 
					"Unrecognized file extension: \n\n%s\n\n"
					"Please check your memory card settings and insert a valid memory card.",
					fullPath.c_str()
				);

				continue;
			}

			// Validate that the capacity is okay for the type of emulated card
			MemcardBase* memcard = s_memcards.at(i).get();

			if (!memcard->ValidateCapacity())
			{
				s_memcardHosts.at(i) = nullptr;
				s_memcards.at(i) = std::make_unique<MemcardNotConnected>(i);
				
				Host::ReportFormattedErrorAsync("Memory Card", 
					"Malformed memory card: \n\n%s\n\n"
					"File size does not match a known memory card size. Please check your memory card settings and insert a valid memory card.",
					fullPath.c_str()
				);

				continue;
			}
		}
	}
	
	return true;
}

void Memcard::Shutdown()
{
	for (std::unique_ptr<MemcardBase>& memcardPtr : s_memcards)
	{
		memcardPtr.reset();
	}
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
