#pragma once

#include "../CommFunc.h"

class CBtStartFrameModulatedTx
{
public:
	CBtStartFrameModulatedTx(void);
public:
	~CBtStartFrameModulatedTx(void);

public:
	BOOL Start(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);

private:
	BOOL SetCFG_PKT(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
	BOOL SetCFG_FREQ(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
	BOOL SetCFG_TX_POWER(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);
	BOOL StartTxPacketTransmit(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo);

};
