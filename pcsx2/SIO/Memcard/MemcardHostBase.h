
#pragma once

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
	
	virtual bool IsOpened() = 0;
	virtual s64 GetSize() = 0;

	virtual void Write(u32 addr, std::vector<u8>& src) = 0;
	virtual void Read(u32 addr, std::vector<u8>& dest) = 0;
};
