
#pragma once

#include "SIO/Memcard/MemcardBase.h"

class MemcardNotConnected : public MemcardBase
{
private:
	

public:
	MemcardNotConnected(u32 unifiedSlot);
	~MemcardNotConnected();

	Memcard::Type GetType() override;
	bool ValidateCapacity() override;
	void ExecuteCommand() override;
};
