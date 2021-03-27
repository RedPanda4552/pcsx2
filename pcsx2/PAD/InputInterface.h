
#pragma once

class InputInterface
{
private:

public:
	virtual ~InputInterface() = 0;
	virtual void Poll() = 0;
};
