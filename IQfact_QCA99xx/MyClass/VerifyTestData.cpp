#include "../StdAfx.h"
#include "VerifyTestData.h"


//BOOL CVerifyTestData::BreakThread;

CVerifyTestData::CVerifyTestData(void)
{

}

CVerifyTestData::~CVerifyTestData(void)
{
}
BOOL CVerifyTestData::Clean(){
	work_flow=0;
	zeroIt(g_GlobalInfo.INI_STR);zeroIt(Test_Info.Str_End);zeroIt(Test_Info.Str_Start);zeroIt(g_GlobalInfo.IsColon);
	zeroIt(Test_Info.szItem);zeroIt(Test_Info.szCmd);zeroIt(Test_Info.szKeyWord);zeroIt(Flag);zeroIt(Test_Info.Country_Code);
	return TRUE;
}

TM_RETURN CVerifyTestData::START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	res=TM_RETURN_PASS;
	double dbTestTimeCost = GetTickCount();
	szCheckName="";
	for(work_flow;work_flow<=((int)PIPE_CHECK_DUT_COUNTRY);work_flow++){

		if(szCheckName.GetLength()>0) break;

		switch(work_flow){
		case PIPE_CHECK_DUT_MAC:// Check DUT Mac
			if(!Flag.PIPE_CHECK_DUT_MAC) continue;
			szCheckName="PIPE_CHECK_DUT_MAC_"+CString(Test_Info.szItem);
			ExportTestLog("==========%s==========",szCheckName);
			strCopy(ErrorReport.ErrorCode,_T("T084"));
			if(g_GlobalInfo.IsColon){
				Str_Tmp=CCommFunc::InsetIt(CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Test_Info.MAC_RULE).GetBuffer(),":",2);
			}else{
				Str_Tmp=CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Test_Info.MAC_RULE);
			}
			strCopy(EXPECT_VALUE,Str_Tmp);
			res=Pipe_Check_Dut_Mac(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			break;
		case PIPE_CHECK_DUT_ENV://Jerry Add Check Dut Env.
			if(!Flag.PIPE_CHECK_DUT_ENV) continue;
			szCheckName="PIPE_CHECK_DUT_"+CString(Test_Info.szItem);
			ExportTestLog("==========%s==========",szCheckName);
			//strCopy(ErrorReport.ErrorCode,_T("T1065"));
			//strCopy(Test_Info.SN,"S100Z47000002");//Test SN
			if(strlen(Test_Info.SN)>0){
				strCopy(ErrorReport.ErrorCode,_T("T1108"));
				strCopy(EXPECT_VALUE,Test_Info.SN);
				zeroIt(Test_Info.SN);
			}
			else if(strlen(Test_Info.Country_Code)>0){
				strCopy(ErrorReport.ErrorCode,_T("T1295"));
				strCopy(EXPECT_VALUE,Test_Info.Country_Code);
			}
			else if(strstr(Test_Info.szItem,_T("CLOSEWIFI"))){
				strCopy(ErrorReport.ErrorCode,_T("T1108"));
				strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
			}
			else if(strstr(Test_Info.szItem,_T("FW"))){
				strCopy(ErrorReport.ErrorCode,_T("T729"));
				strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
			}
			else
				strCopy(ErrorReport.ErrorCode,Test_Info.ErrorCode);
			res=Pipe_Check_Dut_Env(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			break;
		//case PIPE_CHECK_DUT_SN://Jerry Add Check SN
		//	if(!Flag.PIPE_CHECK_DUT_SN) continue;
		//	szCheckName="PIPE_CHECK_DUT_SN";
		//	ExportTestLog("==========%s==========",szCheckName);
		//	strCopy(ErrorReport.ErrorCode,_T("T1108"));
		//	strCopy(EXPECT_VALUE,Test_Info.SN);
		//	res=Pipe_Check_SN(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
		//	break;
		//case PIPE_CHECK_COUNTRY_CODE://Jerry Add Check Country Code
		//	if(!Flag.PIPE_CHECK_COUNTRY_CODE) continue;
		//	szCheckName="PIPE_CHECK_COUNTRY_CODE";
		//	ExportTestLog("==========%s==========",szCheckName);
		//	strCopy(ErrorReport.ErrorCode,_T("T1295"));
		//	strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
		//	res=Pipe_Check_Country_Code(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
		//	break;
		/*case PIPE_CHECK_DUT_XTAL:
			if(!Flag.PIPE_CHECK_DUT_XTAL) continue;
			szCheckName="PIPE_CHECK_DUT_XTAL";
			ExportTestLog("==========%s==========",szCheckName);
			strCopy(ErrorReport.ErrorCode,_T("T1109"));
			strCopy(EXPECT_VALUE,"PASS");
			res=Pipe_Check_Xtal(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			break;
		case PIPE_CHECK_DUT_INTERFACE:
			if(!Flag.PIPE_CHECK_DUT_INTERFACE) continue;
			szCheckName="PIPE_CHECK_DUT_INTERFACE";
			ExportTestLog("==========%s==========",szCheckName);
			strCopy(ErrorReport.ErrorCode,_T("T126"));
			strCopy(EXPECT_VALUE,"PASS");
			res=Pipe_Check_InterFace(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			break;
		case PIPE_CHECK_DUT_COUNTRY:
			if(!PIPE_CHECK_DUT_COUNTRY) continue;
			szCheckName=Test_Info.szTestName;
			ExportTestLog("==========%s==========",szCheckName);
			strCopy(ErrorReport.ErrorCode,_T("T1295"));
			strCopy(EXPECT_VALUE,Test_Info.Country_Code);
			res=Pipe_Check_Dut_Country(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			break;*/
		}//switch
	}//for loop

	if(szCheckName.GetLength()>0){


		sprintf_s(szMessage,DLL_INFO_SIZE,"COMPARE %s_VALUE %s EXPECT={%s} DUT={%s}",szCheckName.GetBuffer(),(res==TM_RETURN_PASS)?"PASS":(res==TM_RETURN_FAIL)?"FAIL":"ERROR",EXPECT_VALUE,DUT_VALUE);
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
		CCommFunc::ExportTestMessage(cbXmlMessage, "<%s>%s</%s>",szCheckName,(res==TM_RETURN_FAIL)?"FAIL":"PASS",szCheckName);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECTVALUE>%s</EXPECTVALUE>",EXPECT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<DUT_VALUE>%s</DUT_VALUE>",DUT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage,"<TEST_TIME>%.3f</TEST_TIME>", (float)((GetTickCount() - dbTestTimeCost)/1000));
		CCommFunc::ExportTestMessage(cbXmlMessage, "</Data>");
		CCommFunc::ExportTestMessage(cbXmlMessage, "</TestCase>");

		work_flow=work_flow-1;

		//sfcs
		if( (nRetryCnt >=0 && res==TM_RETURN_PASS)||(nRetryCnt==0 && res==TM_RETURN_FAIL) ){
			szCheckName.Replace("_","-");
			sprintf_s(szMessage,DLL_INFO_SIZE,"*%s_%s",szCheckName.GetBuffer(),DUT_VALUE);
			ExportTestLog("SFCS szMessage : %s",szMessage);
			CCommFunc::ExportTestMessage(cbSfcsResultMessage,szMessage);
		}
		ExportTestLog("==========%s==========\r\n",szCheckName);

	}//szworkname>1
	return res;
}
TM_RETURN CVerifyTestData::Pipe_Check_Dut_Mac(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT){
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	CString str="";
	
	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage)){
		str=szMessage;
		str=str.GetBuffer()+strlen(szCmd);
		str=CCommFunc::TraceStringMid(str.GetBuffer(),Test_Info.Str_Start,Test_Info.Str_End);
		if(!g_GlobalInfo.IsColon){
			str.Replace(":","");
		}
		str.MakeUpper();
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",str.GetBuffer());
	}

	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}


	return res;
}

TM_RETURN CVerifyTestData::Pipe_Check_Dut_Env(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT)
{//Jerry Add
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	CString str;

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5)){
		str=szMessage;
		str=str.GetBuffer()+strlen(szCmd);
		str=CCommFunc::TraceStringMid(str.GetBuffer(),Test_Info.Str_Start,Test_Info.Str_End);
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",str.GetBuffer());
	}

	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}
	return res;
}

TM_RETURN CVerifyTestData::Pipe_Check_SN(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT){//Jerry Add

	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	CString str;

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5)){
		str=szMessage;
		str=str.GetBuffer()+strlen(szCmd);
		str=CCommFunc::TraceStringMid(str.GetBuffer(),Test_Info.Str_Start,Test_Info.Str_End);
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",str.GetBuffer());
	}

	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}
	return res;
}

TM_RETURN CVerifyTestData::Pipe_Check_Country_Code(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT){

	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	CString str;

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5)){
		str=szMessage;
		str=str.GetBuffer()+strlen(szCmd);
		str=CCommFunc::TraceStringMid(str.GetBuffer(),Test_Info.Str_Start,Test_Info.Str_End);
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",str.GetBuffer());
	}

	if(CCommFunc::IsSame(EXPECT,DUT)){
		res=TM_RETURN_PASS;
	}
	return res;
}

//TM_RETURN CVerifyTestData::Pipe_Check_Xtal(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT){
//	TM_RETURN res=TM_RETURN_FAIL;
//	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
//	CString str="";
//
//	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5)){
//		str=szMessage;
//	}
//	if(str.Find(Test_Info.szKeyWord) != SEARCH_NOT_FOUND){
//		res=TM_RETURN_FAIL;
//	}
//	else{
//		res=TM_RETURN_PASS;
//	}
//	sprintf_s(DUT,DLL_INFO_SIZE,"%s",(res==TM_RETURN_PASS)?"PASS":"FAIL");	
//
//	return res;
//}
//TM_RETURN CVerifyTestData::Pipe_Check_InterFace(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT)
//{
//	TM_RETURN res=TM_RETURN_FAIL;
//	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
//	CString str="";
//	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5)){
//		str=szMessage;
//	}
//
//	if(str.Find(Test_Info.szKeyWord) != SEARCH_NOT_FOUND){//Found
//		res=TM_RETURN_PASS;
//	}
//	else{
//		res=TM_RETURN_FAIL;
//	}
//	sprintf_s(DUT,DLL_INFO_SIZE,"%s",(res==TM_RETURN_PASS)?"PASS":"FAIL");	
//	return res;
//}
//TM_RETURN CVerifyTestData::Pipe_Check_Dut_Country(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT){
//	TM_RETURN res=TM_RETURN_FAIL;
//	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
//	CString str="";
//
//	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5)){
//		str=szMessage;
//	}
//
//	if(str.Find(Test_Info.szKeyWord) != SEARCH_NOT_FOUND){//Found
//		res=TM_RETURN_PASS;
//	}
//	else{
//		res=TM_RETURN_FAIL;
//	}
//	sprintf_s(DUT,DLL_INFO_SIZE,"%s",(res==TM_RETURN_PASS)?EXPECT:"FAIL");	
//	return res;
//}