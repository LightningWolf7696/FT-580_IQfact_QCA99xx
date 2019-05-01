#pragma once

#include "../CommFunc.h"

class CBtStopFrameModulatedTx
{
public:
	CBtStopFrameModulatedTx(void);
public:
	~CBtStopFrameModulatedTx(void);

public:
	BOOL Start(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);

};
