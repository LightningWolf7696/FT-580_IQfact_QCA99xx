#include "../StdAfx.h"
#include "BtStopFrameModulatedTx.h"

CBtStopFrameModulatedTx::CBtStopFrameModulatedTx(void)
{
}

CBtStopFrameModulatedTx::~CBtStopFrameModulatedTx(void)
{

}

BOOL CBtStopFrameModulatedTx::Start(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	TCHAR szCmd[MAX_PATH];


	if(strcmp(DutParam.PACKET_TYPE,"LE") == NULL)
		strCopy(szCmd,_T("hcitool cmd 0x08 0x001f"));
	else
		sprintf_s(szCmd, MAX_PATH, "hcitool cmd %s%s",TX_PREFIX, _T(" 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00"));
	
	if(g_GlobalInfo.IsConsole)
	{//Jerry Add
		if(!CCommFunc::SerialPortQueryData(szCmd, Test_Info.szKeyWord, Test_Info.TimeOut, lpszRunInfo))
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,lpszRunInfo,Test_Info.TimeOut))
			return FALSE;
		else
			return TRUE;
	}

}
