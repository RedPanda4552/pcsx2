
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardNotConnected.h"

#include "SIO/Memcard/MemcardBase.h"
#include "SIO/Sio2.h"
#include "SIO/Sio0.h"

#include "common/Console.h"
#include <vector>

MemcardNotConnected::MemcardNotConnected(u32 unifiedSlot, std::string path)
	: MemcardBase(unifiedSlot, path)
{

}
MemcardNotConnected::~MemcardNotConnected() = default;

Memcard::Type MemcardNotConnected::GetType()
{
	return Memcard::Type::NOT_CONNECTED;
}

CreateResult MemcardNotConnected::Create()
{
	// No card to create, just say we are OK.
	return CreateResult::OK;
}

bool MemcardNotConnected::ValidateCapacity()
{
	return true;
}

bool MemcardNotConnected::IsFormatted()
{
	return false;
}

void MemcardNotConnected::ExecuteCommand()
{
	// Do nothing, drain FIFO IN, fill FIFO OUT with nothing.
	while (!g_Sio2FifoIn.empty())
	{
		g_Sio2FifoIn.pop_front();
		g_Sio2FifoOut.push_back(0xFF);
	}
}

bool MemcardNotConnected::IsOpened()
{
	// There is no file/folder attached to this memory card. Just say true.
	return true;
}

s64 MemcardNotConnected::GetSize()
{
	// No card, no size.
	return 0;
}

void MemcardNotConnected::Write(u32 addr, std::vector<u8>& src)
{
	// Do nothing
	DevCon.Warning("MemcardNotConnected: Something is attempting to write to me, but I am a disconnected memcard!");
}

void MemcardNotConnected::Read(u32 addr, std::vector<u8>& dest)
{
	// Fill buffer with 0xFF since that's technically "nothing"
	memset(dest.data(), 0xFF, dest.size());
	DevCon.Warning("MemcardNotConnected: Something is attempting to read from me, but I am a disconnected memcard!");
}
