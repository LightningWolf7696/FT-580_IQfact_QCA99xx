#include "../StdAfx.h"
#include "WriteTestData.h"

CWriteTestData::CWriteTestData(void)
{
}

CWriteTestData::~CWriteTestData(void)
{

}
void CWriteTestData::Clean(){
	memset(&Flag,0x00,sizeof(FLAG));
	zeroIt(EXPECT_VALUE);zeroIt(DUT_VALUE);zeroIt(INI_STR);zeroIt(szMessage);zeroIt(Test_Info.Country_Code);
	zeroIt(Test_Info.MAC_RULE);zeroIt(Test_Info.szCmd);zeroIt(g_GlobalInfo.IsColon);
}
TM_RETURN CWriteTestData::START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt){
	
	zeroIt(DUT_VALUE);
	szWorkName="";
	double		dbTestTimeCost=GetTickCount();
	for (work_flow=0;work_flow<=PIPE_WRITE_DUT_ENV;work_flow++){
		if(szWorkName.GetLength()>0) break;

		switch(work_flow){
			case PIPE_WRITE_IDL_MAC:// Write Mac Tool (emmc & flash) for WRTQ-334 Ignition
				if(!Flag.PIPE_WRITE_IDL_MAC) continue;
				szWorkName="PIPE_WRITE_IDL_MAC";
				ExportTestLog("==========%s==========",szWorkName);
				strCopy(EXPECT_VALUE,"PASS");
				strCopy(ErrorReport.ErrorCode,_T("T1108"));
				res=Pipe_Write_Idl_Mac(Config_Name,Test_Info.Country_Code,EXPECT_VALUE,DUT_VALUE);
				break;
			case PIPE_WRITE_LICENSE_KEY://Jerry Add Write License Key for WSQ50
				if(!Flag.PIPE_WRITE_LICENSE_KEY) continue;
				szWorkName="PIPE_WRITE_LICENSE_KEY";
				ExportTestLog("==========%s==========",szWorkName);
				strCopy(EXPECT_VALUE,"PASS");
				strCopy(ErrorReport.ErrorCode,_T("T546"));
				res=Pipe_Write_License_Key(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
				break;
			case PIPE_WRITE_DUT_ENV://Jerry Add Write DUT Env.
				if(!Flag.PIPE_WRITE_DUT_ENV) continue;
				szWorkName="PIPE_WRITE_DUT_"+CString(Test_Info.szItem);
				ExportTestLog("==========%s==========",szWorkName);
				strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
				if(strlen(Test_Info.SN)>0)//SN
					strCopy(ErrorReport.ErrorCode,_T("T546"));
				else if(g_GlobalInfo.IsColon)//Colon Mac
					strCopy(ErrorReport.ErrorCode,_T("T1108"));
				else if(strlen(Test_Info.Country_Code)>0)//CountryCode
					strCopy(ErrorReport.ErrorCode,_T("T1294"));
				else if(strstr(Test_Info.szItem,_T("CLOSEWIFI")))
					strCopy(ErrorReport.ErrorCode,_T("T546"));
				else
					strCopy(ErrorReport.ErrorCode,Test_Info.ErrorCode);
				res=Pipe_Write_Dut_Env(Test_Info.szCmd,Test_Info.szItem,Test_Info.MAC_RULE,EXPECT_VALUE,DUT_VALUE);
				break;
			//case PIPE_WRITE_DUT_SN:// Jerry Add Write SN
			//	if(!Flag.PIPE_WRITE_DUT_SN) continue;
			//	szWorkName="PIPE_WRITE_DUT_SN";
			//	ExportTestLog("==========%s==========",szWorkName);
			//	strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
			//	strCopy(ErrorReport.ErrorCode,_T("T546"));
			//	res=Pipe_Write_SN(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			//	break;
			//case PIPE_WRITE_DUT_MAC://Jerry Add Write Colon Mac
			//	if(!Flag.PIPE_WRITE_DUT_MAC) continue;
			//	szWorkName="PIPE_WRITE_DUT_MAC_"+CString(Test_Info.szItem);
			//	ExportTestLog("==========%s==========",szWorkName);
			//	strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
			//	strCopy(ErrorReport.ErrorCode,_T("T546"));
			//	res=Pipe_Write_Dut_Mac(g_GlobalInfo.Pipe_OEM_ID,Test_Info.szCmd,Test_Info.MAC_RULE,EXPECT_VALUE,DUT_VALUE);
			//	break;
			//case PIPE_WRITE_COUNTRY_CODE://Jerry Add Write Country Code
			//	if(!Flag.PIPE_WRITE_COUNTRY_CODE) continue;
			//	szWorkName="PIPE_WRITE_COUNTRY_CODE";
			//	ExportTestLog("==========%s==========",szWorkName);
			//	strCopy(EXPECT_VALUE,g_GlobalInfo.INI_STR);
			//	strCopy(ErrorReport.ErrorCode,_T("T1294"));
			//	res=Pipe_Write_Country_Code(Test_Info.szCmd,EXPECT_VALUE,DUT_VALUE);
			//	break;
		}//switch
	}//for
	if(szWorkName.GetLength()>0){

		sprintf_s(szMessage,DLL_INFO_SIZE,"Compare %s Result %s EXPECT={%s} DUT={%s}",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",EXPECT_VALUE,DUT_VALUE);
		CCommFunc::ExportTestMessage(cbListMessage,szMessage);
		ExportTestLog(szMessage);
		if(res==TM_RETURN_FAIL){
			strCopy(ErrorReport.ErrorMsg,szMessage);
			strCopy(ErrorReport.ErrorDetail,_T("NULL"));
			strCopy(ErrorReport.FailedTest,_T("WIFI_PROGRAM"));
			ErrorReport.TestResult = FALSE;
			ErrorReport.DisableSfcsLink = FALSE;
		}

		else if(res==TM_RETURN_ERROR){
			ErrorReport.DisableSfcsLink = TRUE;
			ErrorReport.TestResult = FALSE;
			strCopy(ErrorReport.ErrorDetail,_T("NULL"));
			strCopy(ErrorReport.ErrorMsg,szMessage);
			strCopy(ErrorReport.FailedTest,_T("WIFI_PROGRAM"));
			strCopy(ErrorReport.ErrorCode,"T089");
		}
		//xml
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","WIFI_PROGRAM");
		CCommFunc::ExportTestMessage(cbXmlMessage, "<Data>");
		CCommFunc::ExportTestMessage(cbXmlMessage, "<%s>%s</%s>",szWorkName,(res==TM_RETURN_PASS)?"PASS":"FAIL",szWorkName);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_VALUE>%s</EXPECT_VALUE>",EXPECT_VALUE);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<DUT_VALUE>%s</DUT_VALUE>",DUT_VALUE);
		//CCommFunc::ExportTestMessage(cbXmlMessage, "<Write_Value>%s</Write_Value>",Write_Value);
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
TM_RETURN CWriteTestData::Pipe_Write_Idl_Mac(LPSTR Config,LPSTR Country,LPSTR EXPECT,LPSTR DUT){// for Ignition
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szCmd[MAX_PATH];zeroIt(szCmd);
	TCHAR szKeyWord[MAX_PATH];zeroIt(szKeyWord);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);

	//CString str=g_GlobalInfo.szBarcode;
	CString str=CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Test_Info.MAC_RULE);

	str.MakeLower();
	if(strstr(Config,"sap_prod.ini")!=NULL){
		sprintf_s(szCmd,sizeof(szCmd),"idl_prod.exe --macstart %s --inifile %s --ctrycode %s",str.GetBuffer(),Config,Country);
	}//Flash write mac and country code tool
	else{
		sprintf_s(szCmd,sizeof(szCmd),"portal2_emmc.exe --macstart %s --inifile %s ",str.GetBuffer(),Config);
	}//Emmc write mac tool
	
	//sprintf_s(szCmd,sizeof(szCmd),"idl_prod.exe --macstart %s --inifile rw_mac.ini --ctrycode %s",str.GetBuffer(),Country);

	if(!PipeLineCleanData(g_GlobalInfo.Pipe_SYS_ID,szMessage)) 
		return TM_RETURN_ERROR;
	if(!CCommFunc::PipeCalibration(g_GlobalInfo.Pipe_SYS_ID,"OEM\\IDL_TOOL",szKeyWord)) 
		return TM_RETURN_ERROR;

	CCommFunc::mydelay(200);
    
	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_SYS_ID,szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut)){
		str=szMessage;
		res=TM_RETURN_PASS;// for portal2_emmc.exe
	}


	if(str.Find("Write Successfully")!=SEARCH_NOT_FOUND){
		res=TM_RETURN_PASS;
	}
	sprintf_s(DUT,DLL_INFO_SIZE,"%s",(res==TM_RETURN_PASS)?"PASS":"FAIL");


	return res;
}

TM_RETURN CWriteTestData::Pipe_Write_License_Key(LPSTR szTcl,LPSTR EXPECT,LPSTR DUT)
{// for Zyxel Write License Key
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szCmd[MAX_PATH];zeroIt(szCmd);
	TCHAR szKeyWord[MAX_PATH];zeroIt(szKeyWord);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	TCHAR szTempMessage[DLL_INFO_SIZE];zeroIt(szTempMessage);
	TCHAR szOldPath[MAX_PATH];zeroIt(szOldPath);
	TCHAR szNewPath[MAX_PATH];zeroIt(szNewPath);
	CString szInput=szTcl;

	//strCopy(Net_Info.szSN,"S100Z47000006");//Test SN
	CString str=CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Test_Info.MAC_RULE);
	str.MakeLower();
	szInput.Replace("MAC",str);
	szInput.Replace("SN",Net_Info.szSN);
	sprintf_s(szCmd,"%s",szInput);
	if(strstr(szCmd,_T("crtmfg")) != NULL){
		if(!PipeLineCleanData(g_GlobalInfo.Pipe_SYS_ID,szMessage)) 
			return TM_RETURN_ERROR;
		if(!CCommFunc::PipeCalibration(g_GlobalInfo.Pipe_SYS_ID,"OEM\\tftp",szKeyWord)) 
			return TM_RETURN_ERROR;
		CCommFunc::mydelay(200);

		if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_SYS_ID,szCmd,szKeyWord,szMessage,Test_Info.TimeOut)){
			CString tmpstr=szMessage;
			res=TM_RETURN_PASS;
		}
	}else{
		if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szInput.GetBuffer(),Test_Info.szKeyWord,szMessage)){
			if(strstr(szInput,_T("crt_tool")) != NULL){// Move bin File to Tempbin Folder
				sprintf_s(szOldPath,DLL_INFO_SIZE,"%sOEM\\tftp\\%s-%s.bin",g_GlobalInfo.szLocalPath,str,Net_Info.szSN);
				sprintf_s(szNewPath,DLL_INFO_SIZE,"%sOEM\\tftp\\Tempbin\\%s-%s.bin",g_GlobalInfo.szLocalPath,str,Net_Info.szSN);
				DeleteFile(szNewPath);
				MoveFile(szOldPath,szNewPath);
				if(CheckFileExist(szNewPath,szTempMessage))
					ExportTestLog("[Success Move %s-%s.bin to Tempbin Folder.]",str,Net_Info.szSN);
				else
					ExportTestLog("[Fail Move %s-%s.bin to Tempbin Folder.]",str,Net_Info.szSN);
			}
			str=szMessage;
			res=TM_RETURN_PASS;
		}
	}
	sprintf_s(DUT,DLL_INFO_SIZE,"%s",(res==TM_RETURN_PASS)?"PASS":"FAIL");
	return res;
}

TM_RETURN CWriteTestData::Pipe_Write_Dut_Env(LPSTR szCmd,LPSTR szItem,int Mac_Count ,LPSTR EXPECT,LPSTR DUT)
{// Jerry Add
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];
	CString szInput=szCmd;

	CCommFunc::PipeLineMsgClear(g_GlobalInfo.Pipe_OEM_ID);

	if(strlen(Test_Info.SN)>0){//Write SN cmd
		szInput.Replace("X",Test_Info.SN);
	}
	else if(g_GlobalInfo.IsColon){//Write Colon Mac cmd
		szInput.Replace("X",CCommFunc::InsetIt(CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Mac_Count).GetBuffer(),":",2));
	}
	else if(strlen(Test_Info.Country_Code)>0){//Write CountryCode cmd
		szInput.Replace("X",Test_Info.Country_Code);
	}

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szInput.GetBuffer(),Test_Info.szKeyWord,szMessage,5)){
		res=TM_RETURN_PASS;
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",EXPECT);
	}

	return res;
}

TM_RETURN CWriteTestData::Pipe_Write_SN(LPSTR szTcl,LPSTR EXPECT,LPSTR DUT)
{//Jerry Add
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];
	CString szInput=szTcl;

	CCommFunc::PipeLineMsgClear(g_GlobalInfo.Pipe_OEM_ID);

	szInput.Replace("X",Test_Info.SN);

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szInput.GetBuffer(),Test_Info.szKeyWord,szMessage)){
		res=TM_RETURN_PASS;
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",EXPECT);
	}


	return res;
}

TM_RETURN CWriteTestData::Pipe_Write_Dut_Mac(int PipeId,LPSTR szTcl,int Mac_Count,LPSTR EXPECT,LPSTR DUT)
{//Jerry Add
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];
	CString szInput=szTcl;

	CCommFunc::PipeLineMsgClear(g_GlobalInfo.Pipe_OEM_ID);
	szInput.Replace("X",CCommFunc::InsetIt(CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,Mac_Count).GetBuffer(),":",2));
	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szInput.GetBuffer(),Test_Info.szKeyWord,szMessage,5)){
		res=TM_RETURN_PASS;
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",EXPECT);
	}


	return res;
}

TM_RETURN CWriteTestData::Pipe_Write_Country_Code(LPSTR szTcl,LPSTR EXPECT,LPSTR DUT)
{//Jerry Add
	TM_RETURN res=TM_RETURN_FAIL;
	TCHAR szMessage[DLL_INFO_SIZE];
	CString szInput=szTcl;

	CCommFunc::PipeLineMsgClear(g_GlobalInfo.Pipe_OEM_ID);

	if(CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szInput.GetBuffer(),Test_Info.szKeyWord,szMessage,5)){
		res=TM_RETURN_PASS;
		sprintf_s(DUT,DLL_INFO_SIZE,"%s",EXPECT);
	}


	return res;
}