#include "../StdAfx.h"
#include "BtVerifyTestData.h"

CBtVerifyTestData::CBtVerifyTestData(void)
{
}

CBtVerifyTestData::~CBtVerifyTestData(void)
{

}
void CBtVerifyTestData::Clear(){
	res=TM_RETURN_PASS;
	zeroIt(EXPECT_VALUE);zeroIt(DUT_VALUE);zeroIt(szMessage);
	memset(&Flag,0x00,sizeof(FLAG));

}
TM_RETURN CBtVerifyTestData::Run(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	double		dbTestTimeCost=GetTickCount();
	szWorkName="";

	for (work_flow =0;work_flow<=CHECK_BT_POWER_LEVEL;work_flow++){

		if(szWorkName.GetLength()>0) break;
		switch(work_flow){
		case CHECK_BT_MAC:
			if(!Flag.CHECK_BT_MAC) continue;
			szWorkName="CHECK_BT_MAC";
			ExportTestLog("==========%s==========",szWorkName);
			strCopy(ErrorReport.ErrorCode,_T("T084"));
			Str_Tmp=CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Test_Info.MAC_RULE);
			strCopy(EXPECT_VALUE,Str_Tmp.GetBuffer());
			res=Check_Bt_Mac(EXPECT_VALUE,DUT_VALUE);
			break;
		case CHECK_BT_POWER_LEVEL:
			if(!Flag.CHECK_BT_POWER_LEVEL) continue;
			szWorkName="CHECK_BT_POWER_LEVEL";
			ExportTestLog("==========%s==========",szWorkName);
			strCopy(ErrorReport.ErrorCode,_T("T022"));
			strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
			res=Check_Bt_Power_level(EXPECT_VALUE,DUT_VALUE);
			break;

		}//switch
	}//for

	if(szWorkName.GetLength()>0){
		sprintf_s(szMessage,DLL_INFO_SIZE,"Compare %s Result %s EXPECT={%s} DUT={%s}",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",EXPECT_VALUE,DUT_VALUE);
		CCommFunc::ExportTestMessage(cbListMessage,szMessage);
		ExportTestLog(szMessage);
		if (res==TM_RETURN_FAIL){
			ErrorReport.DisableSfcsLink = FALSE;
			ErrorReport.TestResult = FALSE;
			strCopy(ErrorReport.ErrorDetail,_T("NULL"));
			strCopy(ErrorReport.ErrorMsg,szMessage);
			strCopy(ErrorReport.FailedTest,_T("WIFI_EEPROM_VERIFICATION"));

			//res = TM_RETURN_FAIL;
		}
		else if(res==TM_RETURN_ERROR){
			ErrorReport.DisableSfcsLink = TRUE;
			ErrorReport.TestResult = FALSE;
			strCopy(ErrorReport.ErrorDetail,_T("NULL"));
			strCopy(ErrorReport.ErrorMsg,szMessage);
			strCopy(ErrorReport.FailedTest,_T("WIFI_EEPROM_VERIFICATION"));
			strCopy(ErrorReport.ErrorCode,"T089");
		}
		//xml
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","WIFI_EEPROM_VERIFICATION");
		CCommFunc::ExportTestMessage(cbXmlMessage, "<Data>");
		CCommFunc::ExportTestMessage(cbXmlMessage, "<%s>%s</%s>",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",szWorkName);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_VALUE>%s</EXPECT_VALUE>",EXPECT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<DUT_VALUE>%s</DUT_VALUE>",DUT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage,"<TEST_TIME>%.3f</TEST_TIME>", (float)((GetTickCount() - dbTestTimeCost)/1000));
		CCommFunc::ExportTestMessage(cbXmlMessage, "</Data>");
		CCommFunc::ExportTestMessage(cbXmlMessage, "</TestCase>");
		//sfcs
		work_flow=work_flow-1;
		if( (nRetryCnt >=0 && res==TM_RETURN_PASS)||(nRetryCnt==0 && res==TM_RETURN_FAIL) ){

			szWorkName.Replace("_","-");
			sprintf_s(szMessage,DLL_INFO_SIZE,"*%s-%s_%s",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",DUT_VALUE);
			CCommFunc::ExportTestMessage(cbSfcsResultMessage,szMessage);
			ExportTestLog("SFCS szMessage : %s",szMessage);

		}
		ExportTestLog("==========%s==========\r\n",szWorkName);
	}
	return res;	
}

TM_RETURN CBtVerifyTestData::Check_Bt_Mac(LPSTR EXPECT,LPSTR DUT){
	TM_RETURN res=TM_RETURN_FAIL;
	CString strMAC="";
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	/*if(!SerialPortCleanData(szMessage))
	goto ERRORED;*/

	/*if(!CCommFunc::SerialPortQueryData(szCmd, g_GlobalInfo.CONSOLE_KEYWORD, g_GlobalInfo.CONSOLE_TIMEOUT, szMessage))
	goto ERRORED;*/
	/*if(!CCommFunc::ConSoleQueryData(szCmd, g_GlobalInfo.CONSOLE_KEYWORD, g_GlobalInfo.CONSOLE_TIMEOUT, szMessage))
	goto FAILED;*/

	//if(g_GlobalInfo.IsConsole){
	//	if(!CCommFunc::ConSoleWithLog(Test_Info.szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut))
	//		res=TM_RETURN_FAIL;
	//}

	if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,"hciconfig",Test_Info.szKeyWord,szMessage,Test_Info.TimeOut))
		res=TM_RETURN_FAIL;


	strMAC = CCommFunc::TraceStringMid(szMessage,_T("BD Address:"),_T("ACL MTU"));
	strMAC = strMAC.Trim();
	strMAC.Replace(":","");
	sprintf_s(DUT,DLL_INFO_SIZE,"%s",strMAC);
	
	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}

	return res;
}
TM_RETURN CBtVerifyTestData::Check_Bt_Power_level(LPSTR EXPECT,LPSTR DUT){
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	CString str="";

	if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,"bccmd -t HCI psget -s 0 0x22c8 ",Test_Info.szKeyWord,szMessage,Test_Info.TimeOut))
		res=TM_RETURN_FAIL;

	str=CCommFunc::TraceStringMid(szMessage,"(",")");
	sprintf_s(DUT,DLL_INFO_SIZE,"%s",str.GetBuffer());

	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}

	return res;
}