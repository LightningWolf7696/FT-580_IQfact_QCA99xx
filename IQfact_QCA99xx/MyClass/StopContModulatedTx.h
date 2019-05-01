#pragma once

#include "../CommFunc.h"

class CStopContModulatedTx
{
public:
	CStopContModulatedTx(void);
public:
	~CStopContModulatedTx(void);

public:
	BOOL Run(DUT_PARAM DutParam, LPSTR lpszRunInfo);

};
