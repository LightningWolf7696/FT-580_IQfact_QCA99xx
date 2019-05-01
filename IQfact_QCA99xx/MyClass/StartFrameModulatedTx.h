#pragma once

#include "../CommFunc.h"

class CStartFrameModulatedTx
{
public:
	CStartFrameModulatedTx(void);
public:
	~CStartFrameModulatedTx(void);

public:
	BOOL Run(DUT_PARAM DutParam, long sleep, LPSTR lpszRunInfo, BOOL bRunning=TRUE);
private: 
	BOOL StartFrameTxPowerIndexSetting(LPSTR lpszRunInfo);
};
