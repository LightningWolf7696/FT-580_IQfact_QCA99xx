#pragma once

#include "../../CommFunc.h"

class CStopFrameModulatedTx
{
public:
	CStopFrameModulatedTx(void);
public:
	~CStopFrameModulatedTx(void);

public:
	BOOL Run(DUT_PARAM DutParam, LPSTR lpszRunInfo, BOOL bRunning=TRUE);

};
