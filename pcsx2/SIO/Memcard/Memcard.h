
#pragma once

class MemcardBase;

namespace Memcard
{
	enum class Type
	{
		NOT_CONNECTED,
		PS1,
		POCKETSTATION,
		PS2
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

	// The storage type to use on the host PC. 
	// - File type uses a single to emulate the entire memcard as a continuous binary blob
	// - Folder type uses a folder on the host filesystem to rebuild the PS2 filesystem on the PC
	enum class StorageType
	{
		NOT_SET = 0,
		FILE = 1,
		FOLDER = 2
	};
	
	static constexpr u32 MAX_SLOTS = 8;
	// First byte of any memory card command, PS1 or PS2.
	static constexpr u8 COMMAND_START = 0x81;
	
	bool Initialize();
	void Shutdown();

	MemcardBase* GetMemcard(const u32 unifiedSlot);
	MemcardBase* GetMemcard(const u32 port, const u32 slot);
} // namespace Memcard
