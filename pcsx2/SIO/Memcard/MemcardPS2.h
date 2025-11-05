
#pragma once

#include "SIO/Memcard/MemcardBase.h"

constexpr const char* FORMAT_STRING = "Sony PS2 Memory Card Format ";
constexpr u8 FORMAT_STRING_LENGTH = 28;

// Though spec allows for 512 or 1024, OEM cards only use 512.
constexpr u16 PAGE_LENGTH = 512;
// 16 or 32, if page length is 512 or 1024 respectively. OEM cards only use 16.
constexpr u16 ECC_LENGTH = 16;
// Can be 2 or 1 if page length is 512, only 1 if page length is 1024. OEM cards only use 2.
constexpr u8 PAGES_PER_CLUSTER = 2;
// Can be 16 or less. OEM cards only use 16.
constexpr u8 PAGES_PER_ERASE_BLOCK = 16;
// Convenience constant for the length of an erase block in bytes
constexpr u32 ERASE_BLOCK_LENGTH = PAGES_PER_ERASE_BLOCK * (PAGE_LENGTH + ECC_LENGTH);

// Describes card size in units of erase blocks
enum class EraseBlockCount : u32
{
	NOT_SET = 0x00000000,
	x8MB = 0x00000400,
	x16MB = 0x00000800,
	x32MB = 0x00001000,
	x64MB = 0x00002000,
	x128MB = 0x00004000,
	x256MB = 0x00008000,
	x512MB = 0x00010000,
	x1024MB = 0x00020000,
	x2048MB = 0x00040000
};

// Describes card size in units of clusters
enum class ClusterCount : u32
{
	NOT_SET = 0x00000000,
	x8MB = 0x00002000,
	x16MB = 0x00004000,
	x32MB = 0x00008000,
	x64MB = 0x00010000,
	x128MB = 0x00020000,
	x256MB = 0x00040000,
	x512MB = 0x00080000,
	x1024MB = 0x00100000,
	x2048MB = 0x00200000
};

enum class Terminator : u8
{
	NOT_READY = 0x66,
	READY = 0x55
};

// Copy of the memory card superblock
/*
struct Superblock
{
	char formatStr[28]; // Magic string indicating card is formatted for PS2
	char version[12]; // Version number, unclear if this comes from the card or IOP modules
	u16 pageLength; // Bytes in a page, not including ECC. Default 512, can be 1024.
	u16 pagesPerCluster; // Pages in a cluster. Default 2 is only allowed for pageLength = 512. Can also be 1.
	u16 pagesPerEraseBlock; // Pages in an erase block. Default 16, can be less, cannot be more.
	u16 unk_2E; // Unknown
	u32 clusterCount; // Total count of clusters on the card. Default 8192.
	u32 firstDataCluster; // Relative to start of the card. Default 41.
	u32 firstBackupCluster; // Relative to firstDataCluster. Default 8135.
	u32 rootDirectoryCluster; // Relative to firstDataCluster. Always 0.
	u32 backupDataBlock; // Stores a copy of an erase block while it is being rewritten
	u32 backupPositionBlock; // Stores the index of the erase block that was copied to backupDataBlock
	u32 indirectFatClusterList[32]; // List of clusters composing the indirect FAT. Zeros are treated as nulls. Default is [8, 0, ..., 0].
	u32 badEraseBlockList[32]; // List of erase blocks with bad flash modules. Default is [0, ..., 0]
	u8 cardType; // Always 2, for PS2. So... why does this exist?
	u8 cardFlags; // Assortment of bit flags for some card properties. Default 0x52.
};
*/

class MemcardPS2 : public MemcardBase
{
private:
	u8 terminator = static_cast<u8>(Terminator::READY);
	u32 currentPage = 0;
	u32 currentAddr = 0;
	u32 lastClearedEraseBlockAddr = 0;

	void Probe();
	void WriteDeleteEnd();
	void SetPage();
	void GetSpecs();
	void SetTerminator();
	void GetTerminator();
	void WriteData();
	void ReadData();
	void ReadWriteEnd();
	void EraseBlock();
	void UnknownBoot();
	void AuthXor();
	void AuthF3();
	void AuthF7();

protected:
	EraseBlockCount eraseBlockCount = EraseBlockCount::NOT_SET;
	ClusterCount clusterCount = ClusterCount::NOT_SET;

public:
	MemcardPS2(u32 unifiedSlot, std::string path);
	~MemcardPS2();

	Memcard::Type GetType() override;
	bool ValidateCapacity() override;
	bool IsFormatted() override;
	void ExecuteCommand() override;
};
