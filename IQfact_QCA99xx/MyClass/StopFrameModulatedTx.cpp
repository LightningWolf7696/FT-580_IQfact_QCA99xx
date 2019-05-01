#include "../../StdAfx.h"
#include "StopFrameModulatedTx.h"

CStopFrameModulatedTx::CStopFrameModulatedTx(void)
{
}

CStopFrameModulatedTx::~CStopFrameModulatedTx(void)
{

}

BOOL CStopFrameModulatedTx::Run(DUT_PARAM DutParam, LPSTR lpszRunInfo, BOOL bRunning)
{
	double dbTestTimeCost = GetTickCount();
	TCHAR szCmd[DLL_INFO_SIZE]={0};
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	 memset(g_GlobalInfo.ATE_CONTINUE_CMD, 0, sizeof(g_GlobalInfo.ATE_CONTINUE_CMD));
	if(bRunning){
		if(strcmp(g_Dut_Info.CHIP_NAME,_T("MT7615N"))==NULL){//Jerry Add
			if(strlen(g_ATECommand.Setstop)>0)
				sprintf_s(szCmd,sizeof(szCmd),"iwpriv %s %s",g_ATECommand.SetProcess,g_ATECommand.Setstop);
			else{
				sprintf_s(szCmd,sizeof(szCmd),"iwpriv %s %s;iwpriv %s %s",g_ATECommand.SetProcess,g_ATECommand.TxStop,g_ATECommand.SetProcess,g_ATECommand.SetTxFrame);
			}
			//strcat(g_GlobalInfo.ATE_CONTINUE_CMD,szCmd);
			if(!CCommFunc::SerialPortQueryData(szCmd,g_GlobalInfo.CONSOLE_KEYWORD,g_GlobalInfo.CONSOLE_TIMEOUT,lpszRunInfo)) return FALSE;
		}
		else{
			if(!CCommFunc::UpdateStartCommand(lpszRunInfo)) return FALSE;
			if (g_GlobalInfo.ATE_COMMAND_CONTINUE){		
				if(!CCommFunc::SerialPortQueryData(g_GlobalInfo.ATE_CONTINUE_CMD,g_GlobalInfo.CONSOLE_KEYWORD,g_GlobalInfo.CONSOLE_TIMEOUT,lpszRunInfo)) return FALSE;
			}
		}

	}
	strcpy_s(lpszRunInfo,DLL_INFO_SIZE,_T("Success to Frames Tx Stop !!"));
	return TRUE;
}
