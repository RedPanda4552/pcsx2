
#include "PrecompiledHeader.h"

#include "SIO/Memcard/Memcard.h"

#include "SIO/Sio.h"
#include "SIO/Memcard/MemcardBase.h"
#include "SIO/Memcard/MemcardPS2.h"
#include "SIO/Memcard/MemcardPS1.h"
#include "SIO/Memcard/MemcardNotConnected.h"

#include "Common.h"
#include "common/Path.h"
#include "common/FileSystem.h"
#include "Host.h"

namespace Memcard
{
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
		}
		else
		{
			const std::string fullPath = Path::Combine(EmuFolders::MemoryCards, fileName);

			if (fullPath.ends_with(".ps2"))
			{
				s_memcards.at(i) = std::make_unique<MemcardPS2>(i, fullPath);
			}
			else if (fullPath.ends_with(".mcd"))
			{
				s_memcards.at(i) = std::make_unique<MemcardPS1>(i, fullPath);
			}
			else
			{
				s_memcards.at(i) = std::make_unique<MemcardNotConnected>(i);

				Host::ReportFormattedErrorAsync("Memory Card", 
					"Access denied to memory card: \n\n%s\n\n"
					"PCSX2 cannot access your memory card. Either you do not have permission to open it, or another program is currently accessign it.%s",
					fullPath.c_str(),
#ifdef WIN32
					"\n\nIf your memory card is in a write-protected folder such as \"Program Files\" or \"Program Files (x86)\", move it to another folder, such as \"Documents\" or \"Desktop\"."
#else
					""
#endif
				);
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
	return s_memcards[unifiedSlot].get();
}

MemcardBase* Memcard::GetMemcard(const u32 port, const u32 slot)
{
	const u32 unifiedSlot = sioConvertPortAndSlotToPad(port, slot);
	return s_memcards[unifiedSlot].get();
}
