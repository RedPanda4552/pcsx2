
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardHostBase.h"

#include "common/FileSystem.h"

MemcardHostBase::MemcardHostBase(std::string path)
{
	this->path = path;
}

MemcardHostBase::~MemcardHostBase() = default;

std::string MemcardHostBase::GetPath()
{
	return this->path;
}

bool MemcardHostBase::IsFormatted()
{
	std::vector<u8> buf;
	buf.resize(FORMAT_STRING_LENGTH);
	this->Read(0, buf);
	return strncmp(reinterpret_cast<const char*>(buf.data()), FORMAT_STRING, FORMAT_STRING_LENGTH) == 0;
}
