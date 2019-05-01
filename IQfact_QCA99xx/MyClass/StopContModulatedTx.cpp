#include "../StdAfx.h"
#include "StopContModulatedTx.h"

CStopContModulatedTx::CStopContModulatedTx(void)
{
}

CStopContModulatedTx::~CStopContModulatedTx(void)
{

}

BOOL CStopContModulatedTx::Run(DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	if(strstr(Test_Info.ChipMode,_T("BT")))
		return TRUE;
}
