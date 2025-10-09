
#pragma once

// Length of the string found at the front of a memory card superblock, indicating it has been formatted.
// Excludes the version number.
constexpr u8 FORMAT_STRING_LENGTH = 28;
constexpr const char* FORMAT_STRING = "Sony PS2 Memory Card Format ";

class MemcardHostBase
{
private:

protected:
	std::string path = "";
	std::FILE* filePtr;

public:
	MemcardHostBase(std::string path);
	virtual ~MemcardHostBase();
	
	std::string GetPath();
	bool IsFormatted();
	
	virtual bool IsOpened() = 0;
	// The max theoretical capacity of the PS2 filesystem is 2 GB; this must be at least a u32 to hold that size,
	// but we are also using negatives to signal when a host type has no physical size associated with it. This
	// is typed as s64 to give us the depth needed for that.
	virtual s64 GetSize() = 0;

	virtual void Write(u32 addr, std::vector<u8>& src) = 0;
	virtual void Read(u32 addr, std::vector<u8>& dest) = 0;
};
