#include "../StdAfx.h"
#include "StartContModulatedTx.h"

CStartContModulatedTx::CStartContModulatedTx(void)
{
}

CStartContModulatedTx::~CStartContModulatedTx(void)
{

}

BOOL CStartContModulatedTx::Run(DUT_PARAM DutParam, long sleep, LPSTR lpszRunInfo)
{
	if(strstr(Test_Info.ChipMode,_T("BT")))
		return TRUE;
}
