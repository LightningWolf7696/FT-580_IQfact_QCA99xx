#include "../StdAfx.h"
#include "BtInitialize.h"

CBtInitialize::CBtInitialize(void)
{
}

CBtInitialize::~CBtInitialize(void)
{

}
void CBtInitialize::Clean(){
	zeroIt(szMessage);
	memset(&Flag,0x00,sizeof(FLAG));
}
BOOL CBtInitialize::START(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	szWorkName="";
	for(int work_flow=0;work_flow<=(int)CONSOLE_INPUT_TEST_CMD;work_flow++){

		if (szWorkName.GetLength()>0) break;
		switch(work_flow){//input BT initial cmd

		case PIPE_INPUT_TEST_CMD:
			if(!Flag.PIPE_INPUT_TEST_CMD)continue;
			szWorkName="PIPE_INPUT_TEST_CMD";
			ExportTestLog("==========%s==========",szWorkName);
			res=CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,Test_Info.szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut);// Input Pipe BT init cmd
			break;
		case CONSOLE_KEYWORD:
			if(!Flag.CONSOLE_KEYWORD)continue;
			szWorkName="CONSOLE_KEYWORD";
			ExportTestLog("==========%s==========",szWorkName);
			res=CCommFunc::SerialPortGetKeyWord(Test_Info.szKeyWord,Test_Info.TimeOut,szMessage);
			break;
		case CONSOLE_INPUT_TEST_CMD://Jerry Add
			if(!Flag.CONSOLE_INPUT_TEST_CMD)continue;
			szWorkName="CONSOLE_INPUT_TEST_CMD";
			ExportTestLog("==========%s==========",szWorkName);
			res=CCommFunc::ConSoleWithLog(Test_Info.szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut);// Input Console init cmd
			break;
		}//switch
	}//for loop
	if(szWorkName.GetLength()>0){
		ExportTestLog("==========%s==========\r\n",szWorkName);
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s_%s",szWorkName.GetBuffer(),(res)?"PASS":"FAIL");
	}else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s","BT_INITIALIZE_FTAPI_PASS");
	}

	return res;
}