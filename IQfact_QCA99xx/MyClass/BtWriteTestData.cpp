#include "../StdAfx.h"
#include "BtWriteTestData.h"

CBtWriteTestData::CBtWriteTestData(void)
{
}

CBtWriteTestData::~CBtWriteTestData(void)
{

}
void CBtWriteTestData::Clear(){
	res=TM_RETURN_PASS;
	zeroIt(EXPECT_VALUE);zeroIt(DUT_VALUE);
	memset(&Flag,0x00,sizeof(FLAG));
}
TM_RETURN CBtWriteTestData::Run(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{   //Jerry Add
	double dbTestTimeCost=GetTickCount();
	szWorkName="";

	for(work_flow =0;work_flow<=WRITE_BT_POWER_LEVEL;work_flow++){
		
		if(strlen(szWorkName)>0) break;

		switch(work_flow){
		case WRITE_BT_MAC:
			if(!Flag.WRITE_BT_MAC) continue;
			szWorkName="WRITE_BT_MAC";
			ExportTestLog("==========%s==========",szWorkName);
			strCopy(ErrorReport.ErrorCode,_T("T772"));
			strCopy(EXPECT_VALUE,"PASS");
			res=Write_Bt_Mac(EXPECT_VALUE,DUT_VALUE);
			break;
		case WRITE_BT_POWER_LEVEL:
			if(!Flag.WRITE_BT_POWER_LEVEL) continue;
			szWorkName="WRITE_BT_POWER_LEVEL";
			ExportTestLog("==========%s==========",szWorkName);
			strCopy(ErrorReport.ErrorCode,_T("T022"));
			strCopy(EXPECT_VALUE,"PASS");
			res=Write_Bt_Power_Level(g_GlobalInfo.INI_STR,EXPECT_VALUE,DUT_VALUE);
			break;
		}//switch
	}//for

	if(strlen(szWorkName)>0){
		sprintf_s(szMessage,DLL_INFO_SIZE,"Compare %s Result %s EXPECT={%s} DUT={%s}",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",EXPECT_VALUE,DUT_VALUE);
		CCommFunc::ExportTestMessage(cbListMessage,szMessage);
		ExportTestLog(szMessage);
		if(res==TM_RETURN_FAIL){
			ErrorReport.DisableSfcsLink = FALSE;
			ErrorReport.TestResult = FALSE;
			strCopy(ErrorReport.ErrorDetail,_T("NULL"));
			strCopy(ErrorReport.ErrorMsg,szMessage);
			strCopy(ErrorReport.FailedTest,_T("WIFI_PROGRAM"));

			//res = TM_RETURN_FAIL
		}
		else if(res==TM_RETURN_ERROR){
			ErrorReport.DisableSfcsLink = TRUE;
			ErrorReport.TestResult = FALSE;
			strCopy(ErrorReport.ErrorDetail,_T("NULL"));
			strCopy(ErrorReport.ErrorMsg,szMessage);
			strCopy(ErrorReport.FailedTest,_T("WIFI_PROGRAM"));
			strCopy(ErrorReport.ErrorCode,"T089");
		}
		//XML
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","WIFI_PROGRAM");
		CCommFunc::ExportTestMessage(cbXmlMessage, "<Data>");
		CCommFunc::ExportTestMessage(cbXmlMessage, "<%s>%s</%s>",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",szWorkName);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_VALUE>%s</EXPECT_VALUE>",EXPECT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<DUT_VALUE>%s</DUT_VALUE>",DUT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage,"<TEST_TIME>%.3f</TEST_TIME>", (float)((GetTickCount() - dbTestTimeCost)/1000));
		CCommFunc::ExportTestMessage(cbXmlMessage, "</Data>");
		CCommFunc::ExportTestMessage(cbXmlMessage, "</TestCase>");
		//SFCS
		work_flow=work_flow-1;
		if( (nRetryCnt >= 0 && res==TM_RETURN_PASS) || (nRetryCnt == 0 && res==TM_RETURN_FAIL)){
			szWorkName.Replace("_","-");
			sprintf_s(szMessage,DLL_INFO_SIZE,"*%s-%s_%s",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",DUT_VALUE);
			CCommFunc::ExportTestMessage(cbSfcsResultMessage,szMessage);
			ExportTestLog("SFCS szMessage : %s",szMessage);
		}
		ExportTestLog("==========%s==========\r\n",szWorkName);
	}
	return res;

}

TM_RETURN CBtWriteTestData::Write_Bt_Mac(LPSTR EXPECT,LPSTR DUT){
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	TCHAR szCmd[MAX_PATH]={0};
	CString strMAC;
	TCHAR szExpectMAC[32]={0};

	if(!MacAddrToCalc(g_GlobalInfo.szBarcode, szExpectMAC, Test_Info.MAC_RULE, szMessage, DLL_INFO_SIZE)) return TM_RETURN_ERROR;

	strMAC.Format("%s",szExpectMAC);

	//===Write MAC:1C904B123456====
	//bccmd psset -r -s 0x0000 0x0001 0x12 0x00 0x56 0x34 0x4B 0x00 0x90 0x1C
	strMAC = "bccmd psset -r -s 0x0000 0x0001 0x" + strMAC.Mid(6,2) + " 0x00 0x" + strMAC.Mid(10,2) + " 0x" + strMAC.Mid(8,2) + " 0x" + strMAC.Mid(4,2) + " 0x00 0x" + strMAC.Mid(2,2) + " 0x" + strMAC.Mid(0,2); 

	sprintf_s(szCmd,sizeof(szCmd),"%s",strMAC.GetBuffer());


	/*if(!SerialPortCleanData(szMessage))
		goto ERRORED;
*/
	/*if(!CCommFunc::SerialPortQueryData(szCmd, g_GlobalInfo.CONSOLE_KEYWORD, g_GlobalInfo.CONSOLE_TIMEOUT, szMessage))
	goto FAILED;*/

	/*if(!CCommFunc::ConSoleQueryData(szCmd, g_GlobalInfo.CONSOLE_KEYWORD, g_GlobalInfo.CONSOLE_TIMEOUT, szMessage))
	goto FAILED;
	*/
	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut)){
		sprintf_s(DUT,DLL_INFO_SIZE,"%s","PASS");
	}
	else{
		sprintf_s(DUT,DLL_INFO_SIZE,"%s","FAIL");
	}
	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}

	return res;

}
TM_RETURN CBtWriteTestData::Write_Bt_Power_Level(LPSTR PowerLevel,LPSTR EXPECT,LPSTR DUT){
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szCmd[MAX_PATH];zeroIt(szCmd);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szCmd);

	sprintf_s(szCmd,sizeof(szCmd),"bccmd -t hci psset 0x22c8 0x000%s ",PowerLevel);//bccmd -t hci psset 0x22c8 0x0004 

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut)){
		sprintf_s(DUT,DLL_INFO_SIZE,"%s","PASS");
	}
	else{
		sprintf_s(DUT,DLL_INFO_SIZE,"%s","FAIL");
	}
	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}

	return res;
}