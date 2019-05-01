#include "../StdAfx.h"
#include "BtGetTargetPower.h"

CBtGetTargetPower::CBtGetTargetPower(void)
{
}

CBtGetTargetPower::~CBtGetTargetPower(void)
{

}

BOOL CBtGetTargetPower::Start(BT_DUT_PARAM &DutParam, POWER_TABLE PwrTable, LPSTR lpszRunInfo)
{
	BOOL bRtn = TRUE;

	if((_stricmp(DutParam.PACKET_TYPE,"DH1") == 0) || (_stricmp(DutParam.PACKET_TYPE,"DH3") == 0) || (_stricmp(DutParam.PACKET_TYPE,"DH5") == 0))
		DutParam.TX_POWER_TARGET = PwrTable.BT_1M;
	else if((_stricmp(DutParam.PACKET_TYPE,"2DH1") == 0) || (_stricmp(DutParam.PACKET_TYPE,"2DH3") == 0) || (_stricmp(DutParam.PACKET_TYPE,"2DH5") == 0))
		DutParam.TX_POWER_TARGET = PwrTable.BT_2M;
	else if((_stricmp(DutParam.PACKET_TYPE,"3DH1") == 0) || (_stricmp(DutParam.PACKET_TYPE,"3DH3") == 0) || (_stricmp(DutParam.PACKET_TYPE,"3DH5") == 0))
		DutParam.TX_POWER_TARGET = PwrTable.BT_3M;
	else if(_stricmp(DutParam.PACKET_TYPE,"LE") == 0)
		DutParam.TX_POWER_TARGET = PwrTable.BT_LE;
	else
		bRtn = FALSE;
	return bRtn;
}
