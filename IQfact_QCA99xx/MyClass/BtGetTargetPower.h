#pragma once

#include "../CommFunc.h"

class CBtGetTargetPower
{
public:
	CBtGetTargetPower(void);
public:
	~CBtGetTargetPower(void);

public:
	BOOL Start(BT_DUT_PARAM &DutParam, POWER_TABLE PwrTable, LPSTR lpszRunInfo);

};
