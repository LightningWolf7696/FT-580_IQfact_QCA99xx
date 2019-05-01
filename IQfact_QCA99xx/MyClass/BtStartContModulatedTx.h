#pragma once

#include "../CommFunc.h"

class CBtStartContModulatedTx
{
public:
	CBtStartContModulatedTx(void);
public:
	~CBtStartContModulatedTx(void);
public:
	BOOL Run(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
};