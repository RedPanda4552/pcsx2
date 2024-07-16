// SPDX-FileCopyrightText: 2002-2024 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "PrecompiledHeader.h"

#include "SIO/Sio2.h"

#include "SIO/Sio.h"
#include "SIO/SioTypes.h"
#include "SIO/Pad/Pad.h"
#include "SIO/Pad/PadBase.h"
#include "SIO/Memcard/MemcardBase.h"
#include "SIO/Multitap/MultitapProtocol.h"

#include "Common.h"
#include "Host.h"
#include "IopDma.h"
#include "Recording/InputRecording.h"
#include "StateWrapper.h"

#define SIO2LOG_ENABLE 0
#define Sio2Log if (SIO2LOG_ENABLE) DevCon

std::deque<u8> g_Sio2FifoIn;
std::deque<u8> g_Sio2FifoOut;

Sio2 g_Sio2;

Sio2::Sio2() = default;
Sio2::~Sio2() = default;

bool Sio2::Initialize()
{
	this->SoftReset();

	for (size_t i = 0; i < send3.size(); i++)
	{
		send3[i] = 0;
	}

	for (size_t i = 0; i < send1.size(); i++)
	{
		send1[i] = 0;
		send2[i] = 0;
	}

	dataIn = 0;
	dataOut = 0;
	SetCtrl(Sio2Ctrl::SIO2MAN_RESET);
	SetRecv1(Recv1::DISCONNECTED);
	recv2 = Recv2::DEFAULT;
	recv3 = Recv3::DEFAULT;
	unknown1 = 0;
	unknown2 = 0;
	iStat = 0;

	port = 0;

	while (!g_Sio2FifoOut.empty())
	{
		g_Sio2FifoOut.pop_front();
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mcds[i][j].term = 0x55;
			mcds[i][j].port = i;
			mcds[i][j].slot = j;
			mcds[i][j].FLAG = 0x08;
			mcds[i][j].autoEjectTicks = 0;
		}
	}

	mcd = &mcds[0][0];
	return true;
}

bool Sio2::Shutdown()
{
	return true;
}

void Sio2::SoftReset()
{
	send3Read = false;
	send3Position = 0;
	commandLength = 0;
	processedLength = 0;
	// Clear dmaBlockSize, in case the next SIO2 command is not sent over DMA11.
	dmaBlockSize = 0;
	send3Complete = false;

	// Anything in g_Sio2FifoIn which was not necessary to consume should be cleared out prior to the next SIO2 cycle.
	while (!g_Sio2FifoIn.empty())
	{
		g_Sio2FifoIn.pop_front();
	}

	if (!g_Sio2FifoOut.empty())
	{
		Console.Warning("%s FIFO OUT not empty on reset! Still has %d elements, force draining it now...", __FUNCTION__, g_Sio2FifoOut.size());

		while (!g_Sio2FifoOut.empty())
		{
			g_Sio2FifoOut.pop_front();
		}
	}

	// RECV1 should always be reassembled based on the devices being probed by the packet.
	recv1 = 0;
}

void Sio2::Interrupt()
{
	iopIntcIrq(17);
}

void Sio2::SetCtrl(u32 value)
{
	Sio2Log.WriteLn("%s(%08X)", __FUNCTION__, value);
	this->ctrl = value;

	if (this->ctrl & Sio2Ctrl::START_TRANSFER)
	{
		Interrupt();
	}
}

void Sio2::SetSend3(size_t position, u32 value)
{
	Sio2Log.WriteLn(
		"%s(%08X, %08X) port = %d pos = %d / %d)", 
		__FUNCTION__, 
		position, 
		value, 
		value & 0x01,
		(value >> 8) & Send3::COMMAND_LENGTH_MASK, 
		(value >> 18) & Send3::COMMAND_LENGTH_MASK
	);
	this->send3[position] = value;

	if (position == 0)
	{
		SoftReset();
	}
}

void Sio2::SetRecv1(u32 value)
{
	Sio2Log.WriteLn("%s(%08X)", __FUNCTION__, value);
	this->recv1 = value;
}

void Sio2::Pad()
{
	MultitapProtocol& mtap = g_MultitapArr.at(port);
	PadBase* pad = Pad::GetPad(port, mtap.GetPadSlot());

	// Update the third nibble with which ports have been accessed
	if (this->recv1 & Recv1::ONE_PORT_OPEN)
	{
		this->recv1 &= ~(Recv1::ONE_PORT_OPEN);
		this->recv1 |= Recv1::TWO_PORTS_OPEN;
	}
	else
	{
		this->recv1 |= Recv1::ONE_PORT_OPEN;
	}

	// This bit is always set, whether the pad is present or missing
	this->recv1 |= Recv1::NO_DEVICES_MISSING;

	// If the currently accessed pad is missing, also tick those bits
	if (pad->GetType() == Pad::ControllerType::NotConnected || pad->ejectTicks)
	{
		if (!port)
		{
			this->recv1 |= Recv1::PORT_1_MISSING;
		}
		else
		{
			this->recv1 |= Recv1::PORT_2_MISSING;
		}
	}

	pad->SoftReset();

	// Then for every byte in g_Sio2FifoIn, pass to PAD and see what it kicks back to us.
	while (!g_Sio2FifoIn.empty())
	{
		// If the pad is "ejected", respond with nothing
		if (pad->ejectTicks)
		{
			g_Sio2FifoIn.pop_front();
			g_Sio2FifoOut.push_back(0xFF);
		}
		// Else, actually forward to the pad.
		else
		{
			const u8 commandByte = g_Sio2FifoIn.front();
			g_Sio2FifoIn.pop_front();
			const u8 responseByte = pad->SendCommandByte(commandByte);
			g_Sio2FifoOut.push_back(responseByte);
		}
	}

	// If the pad is "ejected", then decrement one tick.
	// This needs to happen AFTER anything else which might
	// consider if the pad is "ejected"!
	if (pad->ejectTicks)
	{
		pad->ejectTicks -= 1;
	}
}

void Sio2::Multitap()
{
	const bool multitapEnabled = EmuConfig.Pad.IsMultitapPortEnabled(this->port);
	
	// Update the third nibble with which ports have been accessed
	if (this->recv1 & Recv1::ONE_PORT_OPEN)
	{
		this->recv1 &= ~(Recv1::ONE_PORT_OPEN);
		this->recv1 |= Recv1::TWO_PORTS_OPEN;
	}
	else
	{
		this->recv1 |= Recv1::ONE_PORT_OPEN;
	}

	// This bit is always set, whether the pad is present or missing
	this->recv1 |= Recv1::NO_DEVICES_MISSING;

	// If the currently accessed multitap is missing, also tick those bits.
	// MTAPMAN is special though.
	// 
	// For PADMAN and pads, the bits represented by PORT_1_MISSING and PORT_2_MISSING
	// are always faithful - suppose your game only opened port 2 for some reason,
	// then a disconnect value would look like 0x0002D100.
	//
	// MTAPMAN however does not check the bit set by 0x00020000. It only checks the bit
	// set by 0x00010000. So even if port 2 is being addressed, RECV1 should be 0x0001D100
	// (or 0x0001D200 if there are both ports being accessed in that packet).
	if (!multitapEnabled)
	{
		this->recv1 |= Recv1::PORT_1_MISSING;
	}

	g_MultitapArr.at(this->port).SendToMultitap();
}

void Sio2::Infrared()
{
	SetRecv1(Recv1::DISCONNECTED);

	g_Sio2FifoIn.pop_front();
	const u8 responseByte = 0xff;

	while (g_Sio2FifoOut.size() < commandLength)
	{
		g_Sio2FifoOut.push_back(responseByte);
	}
}

void Sio2::Memcard()
{
	MultitapProtocol& mtap = g_MultitapArr.at(port);
	MemcardBase* memcard = Memcard::GetMemcard(port, mtap.GetMemcardSlot());

	// Update the third nibble with which ports have been accessed
	if (this->recv1 & Recv1::ONE_PORT_OPEN)
	{
		this->recv1 &= ~(Recv1::ONE_PORT_OPEN);
		this->recv1 |= Recv1::TWO_PORTS_OPEN;
	}
	else
	{
		this->recv1 |= Recv1::ONE_PORT_OPEN;
	}

	// This bit is always set, whether the memcard is present or missing
	this->recv1 |= Recv1::NO_DEVICES_MISSING;

	// If the currently accessed memcard is missing, also tick those bits
	if (memcard->GetType() == Memcard::Type::NOT_CONNECTED || memcard->GetAutoEjectTicks())
	{
		if (!port)
		{
			this->recv1 |= Recv1::PORT_1_MISSING;
		}
		else
		{
			this->recv1 |= Recv1::PORT_2_MISSING;
		}
	}

	// Check if auto ejection is active. If so, zero out the fifo to simulate dead air over the wire.
	if (memcard->GetAutoEjectTicks())
	{
		while (!g_Sio2FifoIn.empty())
		{
			g_Sio2FifoIn.pop_front();
			g_Sio2FifoOut.push_back(0xFF);
		}

		return;
	}

	memcard->ExecuteCommand();
}

void Sio2::InvalidDevice()
{
	this->recv1 = Recv1::BOTH_PORTS_MISSING;

	while (!g_Sio2FifoIn.empty())
	{
		g_Sio2FifoIn.pop_front();
		g_Sio2FifoOut.push_back(0xFF);
	}
}

void Sio2::Write(u8 data)
{
	Sio2Log.WriteLn("%s(%02X)", __FUNCTION__, data);

	if (!send3Read)
	{
		// No more SEND3 positions to access, but the game is still sending us SIO2 writes. Lets ignore them.
		if (send3Position > send3.size())
		{
			Console.Warning("%s(%02X) Received data after exhausting all SEND3 values!", __FUNCTION__, data);
			return;
		}

		const u32 currentSend3 = send3[send3Position];
		port = currentSend3 & Send3::PORT;
		commandLength = (currentSend3 >> 8) & Send3::COMMAND_LENGTH_MASK;
		send3Read = true;

		// The freshly read SEND3 position had a length of 0, so we are done handling SIO2 commands until
		// the next SEND3 writes.
		if (commandLength == 0)
		{
			send3Complete = true;
		}

		// If the prior command did not need to fully pop g_Sio2FifoIn, do so now,
		// so that the next command isn't trying to read the last command's leftovers.
		while (!g_Sio2FifoIn.empty())
		{
			g_Sio2FifoIn.pop_front();
		}
	}

	if (send3Complete)
	{
		return;
	}

	g_Sio2FifoIn.push_back(data);

	// We have received as many command bytes as we expect, and...
	//
	// ... These were from direct writes into IOP memory (DMA block size is zero when direct writes occur)
	// ... These were from SIO2 DMA (DMA block size is non-zero when SIO2 DMA occurs)
	if ((g_Sio2FifoIn.size() == g_Sio2.commandLength && g_Sio2.dmaBlockSize == 0) || g_Sio2FifoIn.size() == g_Sio2.dmaBlockSize)
	{
		// Go ahead and prep so the next write triggers a load of the new SEND3 value.
		g_Sio2.send3Read = false;
		g_Sio2.send3Position++;

		// Check the command type to determine which device to route the command to.
		const Sio::CommandType commandType = static_cast<Sio::CommandType>(g_Sio2FifoIn.front());

		switch (commandType)
		{
			case Sio::CommandType::PAD:
				this->Pad();
				break;
			case Sio:: CommandType::MULTITAP:
				this->Multitap();
				break;
			case Sio::CommandType::INFRARED:
				this->Infrared();
				break;
			case Sio::CommandType::MEMCARD:
				this->Memcard();
				break;
			default:
				Console.Error("%s Unhandled command type %02X", __FUNCTION__, static_cast<u8>(commandType));
				this->InvalidDevice();
				break;
		}

		// If command was sent over SIO2 DMA, align g_Sio2FifoOut to the block size
		if (g_Sio2.dmaBlockSize > 0)
		{
			const size_t dmaDiff = g_Sio2FifoOut.size() % g_Sio2.dmaBlockSize;

			if (dmaDiff > 0)
			{
				const size_t padding = g_Sio2.dmaBlockSize - dmaDiff;

				for (size_t i = 0; i < padding; i++)
				{
					g_Sio2FifoOut.push_back(0xFF);
				}
			}
		}
	}
}

u8 Sio2::Read()
{
	u8 ret = 0xff;

	if (this->dmaBlockSize > 0)
	{
		if (this->readCounter % this->dmaBlockSize == 0)
		{
			Sio2Log.WriteLn("== New DMA block ==");
		}

		this->readCounter++;
	}

	if (!g_Sio2FifoOut.empty())
	{
		ret = g_Sio2FifoOut.front();
		g_Sio2FifoOut.pop_front();
		Sio2Log.WriteLn("%s() (%02X)", __FUNCTION__, ret);
	}
	else
	{
		//DevCon.Warning("%s() g_Sio2FifoOut underflow! Returning 0xff.", __FUNCTION__);
		Sio2Log.WriteLn("%s() (%02X) (fifo empty)", __FUNCTION__, ret);
	}

	return ret;
}

bool Sio2::DoState(StateWrapper& sw)
{
	if (!sw.DoMarker("Sio2"))
		return false;

	sw.Do(&send3);
	sw.Do(&send1);
	sw.Do(&send2);
	sw.Do(&dataIn);
	sw.Do(&dataOut);
	sw.Do(&ctrl);
	sw.Do(&recv1);
	sw.Do(&recv2);
	sw.Do(&recv3);
	sw.Do(&unknown1);
	sw.Do(&unknown2);
	sw.Do(&iStat);
	sw.Do(&port);
	sw.Do(&send3Read);
	sw.Do(&send3Position);
	sw.Do(&commandLength);
	sw.Do(&processedLength);
	sw.Do(&dmaBlockSize);
	sw.Do(&send3Complete);

	sw.Do(&g_Sio2FifoIn);
	sw.Do(&g_Sio2FifoOut);

	// CRCs for memory cards.
	// If the memory card hasn't changed when loading state, we can safely skip ejecting it.
	u64 mcdCrcs[SIO::PORTS][SIO::SLOTS];
	if (sw.IsWriting())
	{
		for (u32 port = 0; port < SIO::PORTS; port++)
		{
			for (u32 slot = 0; slot < SIO::SLOTS; slot++)
				mcdCrcs[port][slot] = mcds[port][slot].GetChecksum();
		}
	}
	sw.DoBytes(mcdCrcs, sizeof(mcdCrcs));

	if (sw.IsReading())
	{
		bool ejected = false;
		for (u32 port = 0; port < SIO::PORTS && !ejected; port++)
		{
			for (u32 slot = 0; slot < SIO::SLOTS; slot++)
			{
				if (mcdCrcs[port][slot] != mcds[port][slot].GetChecksum())
				{
					AutoEject::SetAll();
					ejected = true;
					break;
				}
			}
		}
	}

	sw.Do(&sioLastFrameMcdBusy);
	return sw.IsGood();
}
