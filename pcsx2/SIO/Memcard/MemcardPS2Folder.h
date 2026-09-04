
#pragma once 

#include <memory>
#include "SIO/Memcard/Memcard.h"
#include "SIO/Memcard/MemcardPS2.h"

constexpr const char* SUPERBLOCK_FILENAME = "_pcsx2_superblock";

// Specifies what type of item a directory entry is pointing to.
enum class DirectoryEntryType : s32
{
    NOT_SET = -1,
    DIRECTORY = 0,
    FILE = 1
};

class DirectoryEntry
{
private:
    std::string path;
    DirectoryEntryType type;
    std::vector<std::unique_ptr<DirectoryEntry>> children;
    std::FILE* filePtr;
    
public:
    DirectoryEntry(std::string path);
    ~DirectoryEntry();

    DirectoryEntryType GetType();
    bool HasChildren();
    void AddChild(std::unique_ptr<DirectoryEntry>);
};

class MemcardPS2Folder : public MemcardPS2
{
private:
    std::vector<std::string_view> filterList;
    std::unique_ptr<DirectoryEntry> rootDir;

    void RebuildIndex();
    void AddFolderFromRoot(std::string_view filter);
    void ResolveAddressToFile(u32 addr);
    
public:
    MemcardPS2Folder(u32 unifiedSlot, std::string path);
    ~MemcardPS2Folder();

    s64 GetSize() override;
    CreateResult Create() override;
    bool IsOpened() override;
	void Write(u32 addr, std::vector<u8>& src) override;
	void Read(u32 addr, std::vector<u8>& dest) override;
};
