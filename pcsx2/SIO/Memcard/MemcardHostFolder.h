
#pragma once

#include "SIO/Memcard/MemcardHostBase.h"

#include <array>

const char* SUPERBLOCK_FILENAME = "_pcsx2_superblock";
constexpr s64 FOLDER_MEMCARD_REPORTED_SIZE = -1; // Using -1 as a way to signal "no fixed size"

// A representation of a file within a save game. This is an individual file
// as would be seen if browsing the memory card's filesystem directly.
class FileEntry
{
private:
	// Name of the file, must be null terminated.
	std::string m_fileName;

public:
	FileEntry(std::string fileName);
	~FileEntry();

};

// Represents a single entry of a folder memory card. This is the top level
// directory, named after the game's serial, and all of the file contents within.
// PCSX2's meta files are not present here.
class SaveGameEntry
{
private:
	// Vector of all files. Files are ordered by creation date, matching the
	// behavior of the PS2 memory card filesystem.
	std::vector<FileEntry> m_fileEntries;
	
public:
	SaveGameEntry();
	~SaveGameEntry();
};



class MemcardHostFolder : public MemcardHostBase
{
private:
	
	
public:
	MemcardHostFolder(std::string path);
	~MemcardHostFolder();

	bool IsOpened() override;
	s64 GetSize() override;

	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
