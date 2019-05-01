#pragma once

#include "../CommFunc.h"

class CStartContModulatedTx
{
public:
	CStartContModulatedTx(void);
public:
	~CStartContModulatedTx(void);

public:
	BOOL Run(DUT_PARAM DutParam, long sleep, LPSTR lpszRunInfo);

};
