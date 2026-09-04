
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS2Folder.h"
#include <memory>

#include "Common.h"
#include "Config.h"
#include "Host.h"
#include "common/FileSystem.h"
#include "common/Path.h"
#include "common/StringUtil.h"

DirectoryEntry::DirectoryEntry(std::string path)
{
    this->path = path;

    if (FileSystem::FileExists(path.c_str()))
    {
        this->type = DirectoryEntryType::FILE;
        this->filePtr = FileSystem::OpenSharedCFile(path.c_str(), "r+b", FileSystem::FileShareMode::DenyWrite);
    }
    else if (FileSystem::DirectoryExists(path.c_str()))
    {
        this->type = DirectoryEntryType::DIRECTORY;
        std::vector<FILESYSTEM_FIND_DATA> files;
        FileSystem::FindFiles(path.c_str(), "*", FILESYSTEM_FIND_FILES | FILESYSTEM_FIND_FOLDERS | FILESYSTEM_FIND_HIDDEN_FILES, &files);

        for (FILESYSTEM_FIND_DATA& fd : files)
        {
            // Any items prefixed with _pcsx2_ are metadata which should not go in to the directory tree.
            if (fd.FileName.starts_with("_pcsx2_"))
            {
                continue;
            }

            children.push_back(std::make_unique<DirectoryEntry>(Path::Combine(path, fd.FileName)));
        }
    }
    else
    {
        this->type = DirectoryEntryType::NOT_SET;

        Console.Warning("Folder memory card ignoring path '");
        Host::ReportErrorAsync("Folder Memory Card Error", fmt::format(
            "Unexpected item found at path '{}', does not match a file or directory. Please report this to the PCSX2 team. PCSX2 will attempt to continue, but your memory card may not function properly.", 
            this->path
        ));
    }
}

DirectoryEntry::~DirectoryEntry()
{
    if (this->filePtr)
    {
        std::fclose(this->filePtr);
    }
}

DirectoryEntryType DirectoryEntry::GetType()
{
    return this->type;
}

bool DirectoryEntry::HasChildren()
{
    return this->children.size() > 0;
}

void DirectoryEntry::AddChild(std::unique_ptr<DirectoryEntry>)
{

}

void MemcardPS2Folder::RebuildIndex()
{
    for (std::string filter : Memcard::filters)
    {
        this->AddFolderFromRoot(filter);
    }
}

// Reads a folder from the memcard's root, and adds it and all contents inside to the memcard.
// Utilizes the supplied filter string to determine which folder on the host to add. In cases
// of games with multiple save files, where each is its own folder with a different suffix,
// all folders starting with the filter will be added.
void MemcardPS2Folder::AddFolderFromRoot(std::string_view filter)
{
    if (!rootDir)
    {
        Console.Warning(fmt::format("Failed to add folder {} to memcard {}, rootDir not yet initialized", filter, this->path));
        return;
    }
    
    std::vector<FILESYSTEM_FIND_DATA> files;
    FileSystem::FindFiles(this->path.c_str(), fmt::format("{}*", filter).c_str(), FILESYSTEM_FIND_FILES | FILESYSTEM_FIND_FOLDERS | FILESYSTEM_FIND_HIDDEN_FILES, &files);

    for (FILESYSTEM_FIND_DATA& fd : files)
    {
        std::unique_ptr<DirectoryEntry> child = std::make_unique<DirectoryEntry>(Path::Combine(path, fd.FileName));
        this->rootDir->AddChild(std::move(child));
    }
}

void MemcardPS2Folder::ResolveAddressToFile(u32 addr)
{

}

MemcardPS2Folder::MemcardPS2Folder(u32 unifiedSlot, std::string path)
    : MemcardPS2(unifiedSlot, path)
{
    
}

MemcardPS2Folder::~MemcardPS2Folder() = default;

s64 MemcardPS2Folder::GetSize()
{
    return -1;
}

CreateResult MemcardPS2Folder::Create()
{

}

bool MemcardPS2Folder::IsOpened()
{
    return this->rootDir != nullptr;
}

void MemcardPS2Folder::Write(u32 addr, std::vector<u8>& src)
{

}

void MemcardPS2Folder::Read(u32 addr, std::vector<u8>& dest)
{
    
}
