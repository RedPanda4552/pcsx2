
#include "PrecompiledHeader.h"

#include "SIO/Memcard/MemcardPS2.h"
#include "SIO/Memcard/MemcardHostBase.h"

#include "SIO/Sio.h"
#include "SIO/Sio2.h"

#include "Common.h"

void MemcardPS2::Probe()
{
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::WriteDeleteEnd()
{
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::SetPage()
{
	const u8 pageLSB = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);
	const u8 page2nd = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);
	const u8 page3rd = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);
	const u8 pageMSB = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);
	const u8 expectedChecksum = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);

	const u8 computedChecksum = pageLSB ^ page2nd ^ page3rd ^ pageMSB;
	
	if (computedChecksum != expectedChecksum)
	{
		Console.Warning("%s Warning! Memcard sector checksum mismatch! (Expected %02X != Actual %02X) Please report to the PCSX2 team!", __FUNCTION__, expectedChecksum, computedChecksum);
	}

	this->currentPage = pageLSB | (page2nd << 8) | (page3rd << 16) | (pageMSB << 24);
	this->currentAddr = (MemcardPS2::PAGE_LENGTH + MemcardPS2::ECC_LENGTH) * this->currentPage;
	
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::GetSpecs()
{
	const u32 sizeInPages = static_cast<u32>(this->clusterCount) * MemcardPS2::PAGES_PER_CLUSTER;
	u8 checksum = 0x00;
	g_Sio2FifoOut.push_back(0x2B);
	
	constexpr u8 pageSizeLSB = (MemcardPS2::PAGE_LENGTH & 0xFF);
	checksum ^= pageSizeLSB;
	g_Sio2FifoOut.push_back(pageSizeLSB);

	constexpr u8 pageSizeMSB = ((MemcardPS2::PAGE_LENGTH >> 8) & 0xFF);
	checksum ^= pageSizeMSB;
	g_Sio2FifoOut.push_back(pageSizeMSB);

	constexpr u8 eraseBlockSizeLSB = (MemcardPS2::PAGES_PER_ERASE_BLOCK & 0xFF);
	checksum ^= eraseBlockSizeLSB;
	g_Sio2FifoOut.push_back(eraseBlockSizeLSB);

	constexpr u8 eraseBlockSizeMSB = ((MemcardPS2::PAGES_PER_ERASE_BLOCK >> 8) & 0xFF);
	checksum ^= eraseBlockSizeMSB;
	g_Sio2FifoOut.push_back(eraseBlockSizeMSB);

	const u8 pageCountLSB = (sizeInPages & 0xFF);
	checksum ^= pageCountLSB;
	g_Sio2FifoOut.push_back(pageCountLSB);

	const u8 pageCount2nd = ((sizeInPages >> 8) & 0xFF);
	checksum ^= pageCount2nd;
	g_Sio2FifoOut.push_back(pageCount2nd);

	const u8 pageCount3rd = ((sizeInPages >> 16) & 0xFF);
	checksum ^= pageCount3rd;
	g_Sio2FifoOut.push_back(pageCount3rd);

	const u8 pageCountMSB = ((sizeInPages >> 24) & 0xFF);
	checksum ^= pageCountMSB;
	g_Sio2FifoOut.push_back(pageCountMSB);
	
	g_Sio2FifoOut.push_back(checksum);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::SetTerminator()
{
	this->terminator = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::GetTerminator()
{
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::WriteData()
{
	MemcardBusy::SetBusy();
	this->SendWriteMessageToHost();

	g_Sio2FifoOut.push_back(0x00);
	g_Sio2FifoOut.push_back(0x2B);
	const u8 writeLength = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();

	if (this->currentAddr + writeLength >= this->lastClearedEraseBlockAddr + MemcardPS2::ERASE_BLOCK_LENGTH);
	{
		Console.Warning(
			"%s Write outside of cleared erase block! Write addr = 0x%08X, len = %d; erase block addr = 0x%08X, len = %d", 
			__FUNCTION__, 
			this->currentAddr, 
			writeLength, 
			this->lastClearedEraseBlockAddr, 
			MemcardPS2::ERASE_BLOCK_LENGTH
		);
	}

	u8 checksum = 0x00;
	std::vector<u8> buf;

	for (size_t writeCounter = 0; writeCounter < writeLength; writeCounter++)
	{
		const u8 writeByte = g_Sio2FifoIn.front();
		g_Sio2FifoIn.pop_front();
		checksum ^= writeByte;
		buf.push_back(writeByte);
		g_Sio2FifoOut.push_back(0x00);
	}

	this->GetMemcardHost()->Write(this->currentAddr, buf);
	g_Sio2FifoOut.push_back(checksum);
	g_Sio2FifoOut.push_back(this->terminator);
	this->currentAddr += writeLength;
}

void MemcardPS2::ReadData()
{
	const u8 readLength = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();
	g_Sio2FifoOut.push_back(0x00);
	g_Sio2FifoOut.push_back(0x2B);
	std::vector<u8> buf;
	buf.resize(readLength);
	this->GetMemcardHost()->Read(this->currentAddr, buf);
	u8 checksum = 0x00;

	for (const u8 readByte : buf)
	{
		checksum ^= readByte;
		g_Sio2FifoOut.push_back(readByte);
	}

	g_Sio2FifoOut.push_back(checksum);
	g_Sio2FifoOut.push_back(this->terminator);
	this->currentAddr += readLength;
}

void MemcardPS2::ReadWriteEnd()
{
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::EraseBlock()
{
	MemcardBusy::SetBusy();
	this->SendWriteMessageToHost();

	if (this->currentAddr % MemcardPS2::ERASE_BLOCK_LENGTH != 0)
	{
		Console.Warning("%s Misaligned address is not the front of an erase block! (%08X)", __FUNCTION__, this->currentAddr);
	}

	std::vector<u8> clearData;
	clearData.resize(MemcardPS2::ERASE_BLOCK_LENGTH);
	memset(clearData.data(), 0xFF, clearData.size());
	this->GetMemcardHost()->Write(this->currentAddr, clearData);
	this->lastClearedEraseBlockAddr = this->currentAddr;

	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

void MemcardPS2::UnknownBoot()
{
	g_Sio2FifoOut.push_back(0x00);
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

// A particularly convoluted command which appears to be a part of the auth process.
// These commands really just seem to follow a nonsensical control flow where what
// we call "modeByte" below changes depending on command length and that in turn
// changes whatever magical sequence of bytes the PS2 wants to get back. None of
// this stuff really makes sense right now, and some digging in to IOP modules will
// be necessary to shed further light on this. In the meantime, all of the modules
// seem to be happy with this, so for now it shall remain.
void MemcardPS2::AuthXor()
{
	const u8 modeByte = g_Sio2FifoIn.front();
	g_Sio2FifoIn.pop_front();

	switch (modeByte)
	{
		// When encountered, the command length in RECV3 is guaranteed to be 14,
		// and the PS2 is expecting us to XOR the data it is about to send.
		case 0x01: case 0x02: case 0x04:
		case 0x0F: case 0x11: case 0x13:
		{
			// Long + XOR
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x2B);
			u8 xorResult = 0x00;

			for (size_t xorCounter = 0; xorCounter < 8; xorCounter++)
			{
				const u8 toXOR = g_Sio2FifoIn.front();
				g_Sio2FifoIn.pop_front();
				xorResult ^= toXOR;
				g_Sio2FifoOut.push_back(0x00);
			}

			g_Sio2FifoOut.push_back(xorResult);
			g_Sio2FifoOut.push_back(this->terminator);
			break;
		}
		// When encountered, the command length in RECV3 is guaranteed to be 5,
		// and there is no attempt to XOR anything.
		case 0x00: case 0x03: case 0x05:
		case 0x08: case 0x09: case 0x0A:
		case 0x0C: case 0x0D: case 0x0E:
		case 0x10: case 0x12: case 0x14:
		{
			// Short + No XOR
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x2B);
			g_Sio2FifoOut.push_back(this->terminator);
			break;
		}
		// When encountered, the command length in RECV3 is guaranteed to be 14,
		// and the PS2 is about to send us data, BUT the PS2 does NOT want us
		// to send the XOR, it wants us to send the 0x2B and terminator as the
		// last two bytes.
		case 0x06: case 0x07: case 0x0B:
		{
			// Long + No XOR
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x00);
			g_Sio2FifoOut.push_back(0x2B);
			g_Sio2FifoOut.push_back(this->terminator);
			break;
		}
		default:
			Console.Warning("%s(queue) Unexpected modeByte (%02X), please report to the PCSX2 team", __FUNCTION__, modeByte);
			break;
	}
}

// Calling this function seems to switch the terminator to what we are calling its "READY" value.
// Like AuthXor, this one is kinda nonsensical and no one really understands "why", just that "it works".
void MemcardPS2::AuthF3()
{
	this->terminator = static_cast<u8>(Terminator::READY);
	g_Sio2FifoOut.push_back(0x00);
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

// ???
void MemcardPS2::AuthF7()
{
	g_Sio2FifoOut.push_back(0x00);
	g_Sio2FifoOut.push_back(0x2B);
	g_Sio2FifoOut.push_back(this->terminator);
}

MemcardPS2::MemcardPS2(u32 unifiedSlot)
	: MemcardBase(unifiedSlot)
{
	this->clusterCount = static_cast<ClusterCount>(this->GetMemcardHost()->GetSize() / (MemcardPS2::PAGE_LENGTH + ECC_LENGTH) / MemcardPS2::PAGES_PER_CLUSTER);
}

MemcardPS2::~MemcardPS2() = default;

Memcard::Type MemcardPS2::GetType()
{
	return Memcard::Type::PS2;
}

bool MemcardPS2::ValidateCapacity()
{
	const u32 standardFileSize = (static_cast<u32>(ClusterCount::x8MB) * MemcardPS2::PAGES_PER_CLUSTER) * (MemcardPS2::PAGE_LENGTH + ECC_LENGTH);
	const u32 maxFileSize = (static_cast<u32>(ClusterCount::x2048MB) * MemcardPS2::PAGES_PER_CLUSTER) * (MemcardPS2::PAGE_LENGTH + ECC_LENGTH);
	
	if (this->GetMemcardHost() != nullptr)
	{
		const u32 fileSize = this->GetMemcardHost()->GetSize();

		if (fileSize > 0)
		{
			u32 compareSize = standardFileSize;
			bool sizeMatches = false;

			do
			{
				if (fileSize == compareSize)
				{
					sizeMatches = true;
					break;
				}

				compareSize *= 2;
			} while (compareSize <= maxFileSize);

			return sizeMatches;
		}
	}

	return false;
}

void MemcardPS2::ExecuteCommand()
{
	if (g_Sio2FifoIn.empty())
	{
		DevCon.Warning("%s called with empty g_Sio2FifoIn!");
		return;
	}

	const u8 firstCmdByte = g_Sio2FifoIn.front();

	// If the memory card was passed a command which is not a memory card command,
	// then drain the transfer buffer and respond with dead air.
	// SIO2 code should already be dealing with this, but we'll leave this here for safety.
	if (firstCmdByte != Memcard::COMMAND_START)
	{
		while (!g_Sio2FifoIn.empty())
		{
			g_Sio2FifoIn.pop_front();
			g_Sio2FifoOut.push_back(0xFF);
		}
		
		return;
	}
	// Else, lets pop the front byte off and respond with 0, then start processing the command contents.
	else
	{
		g_Sio2FifoIn.pop_front();
		g_Sio2FifoOut.push_back(0x00);
		const Memcard::Command currentCommand = static_cast<Memcard::Command>(g_Sio2FifoIn.front());
		g_Sio2FifoIn.pop_front();

		switch (currentCommand)
		{
			case Memcard::Command::PROBE:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->Probe();
				break;
			}
			case Memcard::Command::WRITE_DELETE_END:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->WriteDeleteEnd();
				break;
			}
			case Memcard::Command::SET_ERASE_PAGE:
			case Memcard::Command::SET_WRITE_PAGE:
			case Memcard::Command::SET_READ_PAGE:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->SetPage();
				break;
			}
			case Memcard::Command::GET_SPECS:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->GetSpecs();
				break;
			}
			case Memcard::Command::SET_TERMINATOR:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->SetTerminator();
				break;
			}
			case Memcard::Command::GET_TERMINATOR:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->GetTerminator();
				break;
			}
			case Memcard::Command::WRITE_DATA:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->WriteData();
				break;
			}
			case Memcard::Command::READ_DATA:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->ReadData();
				break;
			}
				g_Sio2FifoOut.push_back(0x00);
			case Memcard::Command::READ_WRITE_END:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->ReadWriteEnd();
				break;
			}
			case Memcard::Command::ERASE_BLOCK:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->EraseBlock();
				break;
			}
			case Memcard::Command::UNKNOWN_BOOT:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->UnknownBoot();
				break;
			}
			case Memcard::Command::AUTH_XOR:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->AuthXor();
				break;
			}
			case Memcard::Command::AUTH_F3:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->AuthF3();
				break;
			}
			case Memcard::Command::AUTH_F7:
			{
				g_Sio2FifoOut.push_back(0x00);
				this->AuthF7();
				break;
			}
			default:
			{
				Console.Warning("%s Unrecognized command (%02X)", __FUNCTION__, currentCommand);

				while (!g_Sio2FifoIn.empty())
				{
					g_Sio2FifoIn.pop_front();
					g_Sio2FifoOut.push_back(0xFF);
				}
				break;
			}
		}
	}
}
