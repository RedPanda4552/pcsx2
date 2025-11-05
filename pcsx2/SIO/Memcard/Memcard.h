
#pragma once

#include <vector>

class MemcardBase;

namespace Memcard
{
	constexpr std::array<std::string, 5> MEMCARD_FILE_EXTENSIONS = { ".ps2", ".bin", ".mc2", ".mcd", ".mcr" };
	
	// Known virtual memory card file extensions:
	// .ps2 - PS2 - PCSX2 memory card (raw format)
	// .bin - PS2 - OpenPS2Loader VMC file (like .ps2, but with no ECC)
	// .mc2 - PS2 - Memcard PRO2 file (identical to .bin)
	constexpr std::array<std::string, 3> PS2_MEMCARD_FILE_EXTENSIONS = { ".ps2", ".bin", ".mc2" };

	// .mcd - PS1 - Modern file extension (raw format)
	// .mcr - PS1 - Legacy file extension (raw format)
	constexpr std::array<std::string, 2> PS1_MEMCARD_FILE_EXTENSIONS = { ".mcd", ".mcr" };

	enum class Type
	{
		NOT_CONNECTED,
		PS1,
		PS2
	};

	enum class HostType
	{
		NOT_SET = 0,
		FILE = 1,
		FOLDER = 2
	};

	// Struct to package up a summary of memcard information useful to the frontend for display purposes.
	struct AvailableMemcardSummary
	{
		std::string name;
		std::string path;
		std::time_t modified_time;
		Memcard::Type type;
		Memcard::HostType hostType;
		s64 size;
		bool formatted;
	};

	enum class Command : u8
	{
		NOT_SET = 0x00,
		PROBE = 0x11,
		WRITE_DELETE_END = 0x12,
		SET_ERASE_PAGE = 0x21,
		SET_WRITE_PAGE = 0x22,
		SET_READ_PAGE = 0x23,
		GET_SPECS = 0x26,
		SET_TERMINATOR = 0x27,
		GET_TERMINATOR = 0x28,
		WRITE_DATA = 0x42,
		READ_DATA = 0x43,
		PS1_READ = 0x52,
		PS1_STATE = 0x53,
		PS1_WRITE = 0x57,
		PS1_POCKETSTATION = 0x58,
		READ_WRITE_END = 0x81,
		ERASE_BLOCK = 0x82,
		UNKNOWN_BOOT = 0xbf,
		AUTH_XOR = 0xf0,
		AUTH_F3 = 0xf3,
		AUTH_F7 = 0xf7
	};

	
	
	static constexpr u32 MAX_SLOTS = 8;
	// First byte of any memory card command, PS1 or PS2.
	static constexpr u8 COMMAND_START = 0x81;
	
	bool Initialize();
	void Shutdown();

	bool CreateMemcard(const u32 unifiedSlot);
	bool CreateMemcard(const u32 port, const u32 slot);
	bool InsertMemcard(const u32 unifiedSlot);
	bool InsertMemcard(const u32 port, const u32 slot);
	void RemoveMemcard(const u32 unifiedSlot);
	void RemoveMemcard(const u32 port, const u32 slot);
	MemcardBase* GetMemcard(const u32 unifiedSlot);
	MemcardBase* GetMemcard(const u32 port, const u32 slot);

	Memcard::Type GetMemcardType(std::string fullPath);
	std::vector<Memcard::AvailableMemcardSummary> GetAvailableMemcards(bool includeInUse);
} // namespace Memcard
