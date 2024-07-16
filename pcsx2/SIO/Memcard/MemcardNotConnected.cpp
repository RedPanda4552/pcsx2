
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardNotConnected.h"

#include "SIO/Sio2.h"
#include "SIO/Sio0.h"

MemcardNotConnected::MemcardNotConnected(u32 unifiedSlot)
	: MemcardBase(unifiedSlot, "")
{

}
MemcardNotConnected::~MemcardNotConnected() = default;

Memcard::Type MemcardNotConnected::GetType()
{
	return Memcard::Type::NOT_CONNECTED;
}

// No-op. Let SIO2 and SIO0 sit with no response.
void MemcardNotConnected::ExecuteCommand()
{
	while (!g_Sio2FifoIn.empty())
	{
		g_Sio2FifoIn.pop_front();
		g_Sio2FifoOut.push_back(0xFF);
	}
}
