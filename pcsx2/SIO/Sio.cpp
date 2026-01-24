// SPDX-FileCopyrightText: 2002-2025 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "SIO/Sio.h"

#include "SIO/SioTypes.h"
#include "SIO/Memcard/MemoryCardProtocol.h"
#include "Counters.h"

#include "Host.h"
#include "IconsPromptFont.h"

#include <atomic>

_mcd mcds[2][4];
_mcd *mcd;

void sioNextFrame() {
	for ( uint port = 0; port < 2; ++port ) {
		for ( uint slot = 0; slot < 4; ++slot ) {
			mcds[port][slot].NextFrame();
		}
	}
}

void sioSetGameSerial( const std::string& serial ) {
	for ( uint port = 0; port < 2; ++port ) {
		for ( uint slot = 0; slot < 4; ++slot ) {
			if ( mcds[port][slot].ReIndex( serial ) ) {
				AutoEject::Set( port, slot );
			}
		}
	}
}

std::tuple<u32, u32> sioConvertPadToPortAndSlot(u32 index)
{
	if (index > 4) // [5,6,7]
		return std::make_tuple(1, index - 4); // 2B,2C,2D
	else if (index > 1) // [2,3,4]
		return std::make_tuple(0, index - 1); // 1B,1C,1D
	else // [0,1]
		return std::make_tuple(index, 0); // 1A,2A
}

u32 sioConvertPortAndSlotToPad(u32 port, u32 slot)
{
	if (slot == 0)
		return port;
	else if (port == 0) // slot=[0,1]
		return slot + 1; // 2,3,4
	else
		return slot + 4; // 5,6,7
}

bool sioPadIsMultitapSlot(u32 index)
{
	return (index >= 2);
}

bool sioPortAndSlotIsMultitap(u32 port, u32 slot)
{
	return (slot != 0);
}

void AutoEject::CountDownTicks()
{
	bool reinserted = false;
	for (size_t port = 0; port < SIO::PORTS; port++)
	{
		for (size_t slot = 0; slot < SIO::SLOTS; slot++)
		{
			if (mcds[port][slot].autoEjectTicks > 0)
			{
				if (--mcds[port][slot].autoEjectTicks == 0)
					reinserted |= EmuConfig.Mcd[sioConvertPortAndSlotToPad(port, slot)].Enabled;
			}
		}
	}

	if (reinserted)
	{
		Host::AddIconOSDMessage("AutoEjectAllSet", ICON_PF_MEMORY_CARD,
			TRANSLATE_SV("MemoryCard", "Memory Cards reinserted."), Host::OSD_INFO_DURATION);
	}
}

u32 SIO::ConvertToUnifiedSlot(const u32 port, const u32 slot)
{
	if (slot == 0)
	{
		return port; // 0, 1
	}
	else if (port == 0)
	{
		return slot + 1; // 2,3,4
	}
	else
	{
		return slot + 4; // 5,6,7
	}
}

std::tuple<u32, u32> SIO::ConvertToPortAndSlot(const u32 unifiedSlot)
{
	if (unifiedSlot > 4) // [5,6,7]
	{
		return std::make_tuple(1, unifiedSlot - 4); // 2B,2C,2D
	} 
	else if (unifiedSlot > 1) // [2,3,4]
	{
		return std::make_tuple(0, unifiedSlot - 1); // 1B,1C,1D
	}
	else // [0,1]
	{
		return std::make_tuple(unifiedSlot, 0); // 1A,2A
	}
}
