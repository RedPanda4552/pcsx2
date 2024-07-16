
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
