#include "../../StdAfx.h"
#include "StartFrameModulatedTx.h"

CStartFrameModulatedTx::CStartFrameModulatedTx(void)
{
}

CStartFrameModulatedTx::~CStartFrameModulatedTx(void)
{

}

BOOL CStartFrameModulatedTx::Run(DUT_PARAM DutParam, long sleep, LPSTR lpszRunInfo, BOOL bRunning)
{
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	TCHAR szCmd[100]={0};
	double dbTestTimeCost = GetTickCount();	

	if(!CCommFunc::SetCommandcheapest(DutParam,lpszRunInfo)) return FALSE;
	
	//ChipPowerIndex Setting
	if (bRunning)
	{
		//Convert script parameter to dut parameter.
		if(g_GlobalInfo.SetSorting){
			if(!CCommFunc::ActiveDutParameterTx_Sort(DutParam,lpszRunInfo)) return FALSE;
		}else{
			if(!CCommFunc::ActiveDutParameterTx(DutParam,lpszRunInfo)) return FALSE;
		}
		if(!CCommFunc::UpdateTxFrameCommand(lpszRunInfo)) return FALSE;
		if (g_GlobalInfo.ATE_COMMAND_CONTINUE)		
			if(!CCommFunc::SerialPortQueryData(g_GlobalInfo.ATE_CONTINUE_CMD,g_GlobalInfo.CONSOLE_KEYWORD,g_GlobalInfo.CONSOLE_TIMEOUT,lpszRunInfo)) return FALSE;
		Sleep(g_GlobalInfo.COMMADN_DELAY);
	}else{
		if (!CCommFunc::UpdateATECommandCountinue(lpszRunInfo))	return FALSE;
		UINT ActiveParameter= CCommFunc::ParseAntenna(DutParam.TX_CHAIN,DutParam.BAND,DutParam.RATE);
		if(!CCommFunc::UpdateCalTxPowerIndex(DutParam.TX_POWER_INDEX,ActiveParameter,lpszRunInfo)) return FALSE;
		if (g_GlobalInfo.ATE_COMMAND_CONTINUE)		
			if(!CCommFunc::SerialPortQueryData(g_GlobalInfo.ATE_CONTINUE_CMD,g_GlobalInfo.CONSOLE_KEYWORD,g_GlobalInfo.CONSOLE_TIMEOUT,lpszRunInfo)) return FALSE;
	}

	CCommFunc::OutputCmdLine("                                                                                ");
	strcpy_s(lpszRunInfo,DLL_INFO_SIZE,_T("Success to Packet Tx Start !!"));		

	return TRUE;
}
