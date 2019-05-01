#pragma once

#include "../CommFunc.h"

class CBtStopContModulatedTx
{
public:
	CBtStopContModulatedTx(void);
public:
	~CBtStopContModulatedTx(void);
public:
	BOOL Run(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
};