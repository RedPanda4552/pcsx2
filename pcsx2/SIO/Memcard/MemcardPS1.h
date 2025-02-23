
#pragma once

#include "SIO/Memcard/MemcardBase.h"

class MemcardPS1 : public MemcardBase
{
private:
	u32 currentCommandByte = 0;
	bool isMemcardCommand = false;
	Memcard::Command currentCommand = Memcard::Command::NOT_SET;

public:
	MemcardPS1(u32 unifiedSlot);
	~MemcardPS1();

	Memcard::Type GetType() override;
	void ExecuteCommand() override;
};
