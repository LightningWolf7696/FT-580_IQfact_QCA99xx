#include "../StdAfx.h"
#include "PreLoadImage.h"

#define _USE_CONSOLE_TO_TEST
BOOL CPreLoadImage::BreakThread;

CPreLoadImage::CPreLoadImage(void)
{
}

CPreLoadImage::~CPreLoadImage(void)
{

}

TM_RETURN CPreLoadImage::START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	TM_RETURN res=TM_RETURN_FAIL;
	//TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);

	//HWND tftpd32_handle=NULL,clear_handle=NULL,listbox_handle=NULL;
	//int count=0,tmp=0;
	//CString str="";
	//CString szWorkName="CHECK_DUT_LOAD_FW";

	//if(!CCommFunc::OpenExecSoft("tftp","tftpd32")){
	//	return TM_RETURN_ERROR;
	//}
	//

	//tftpd32_handle=FindWindow(NULL,"Tftpd32 by Ph. Jounin");
	//clear_handle=FindWindowEx(tftpd32_handle,NULL,"Button","C&lear");
	//listbox_handle=FindWindowEx(tftpd32_handle,NULL,"ListBox",NULL);

	//do {
	//	SendMessage(clear_handle,BM_CLICK,0,NULL);
	//} while (SendMessage(listbox_handle,LB_GETCOUNT,NULL,NULL));

	//if(IsPsuCtrl){
	//	if(!CallBackFn.cbPSCtrl(TRUE,szMessage)){
	//		ExportTestLog("PSU : szMessage = {%s}",szMessage);
	//		return TM_RETURN_ERROR;
	//	}
	//}
	//

	//double StartTime =GetTickCount();
	//while(((GetTickCount()-StartTime)/1000)<g_GlobalInfo.INIT_TIME_OUT_SEC){
	//	count=SendMessage(listbox_handle,LB_GETCOUNT,NULL,NULL);
	//	for (int i=0;i<count;i++){
	//		zeroIt(szMessage);
	//		SendMessage(listbox_handle,LB_GETTEXT,i,(LPARAM)szMessage);
	//		str=str+szMessage;
	//	}
	//	if(strstr(str,"resent")!=NULL){
	//		res=TM_RETURN_PASS;
	//		break;
	//	}
	//	else{
	//		str="";
	//	}
	//}//while
	//
	//ExportTestLog("tftpd32 szMessage = {%s}",str);

	//if(res!=TM_RETURN_PASS){
	//	if(IsPsuCtrl){
	//		if(!CallBackFn.cbPSCtrl(FALSE,szMessage)){
	//			ExportTestLog("PSU : szMessage = {%s}",szMessage);
	//			res=TM_RETURN_ERROR;
	//		}

	//	}
	//	

	//	CCommFunc::CloseExecSoft("tftpd32.exe");
	//	CCommFunc::RefreshTaskbarIcon();
	//}

	//sprintf_s(szMessage,"%s_%s",szWorkName,(res==TM_RETURN_PASS)?"PASS":(res==TM_RETURN_FAIL)?"FAIL":"ERROR");
	//CCommFunc::ExportTestMessage(cbListMessage,szMessage);

	//if(res==TM_RETURN_FAIL){
	//	ErrorReport.DisableSfcsLink=FALSE;
	//	ErrorReport.TestResult=FALSE;
	//	strCopy(ErrorReport.ErrorCode,"T002");
	//	strCopy(ErrorReport.ErrorDetail,"NULL");
	//	strCopy(ErrorReport.ErrorMsg,szMessage);
	//}
	//else if(res==TM_RETURN_ERROR){
	//	ErrorReport.DisableSfcsLink=TRUE;
	//	ErrorReport.TestResult=FALSE;
	//	strCopy(ErrorReport.ErrorCode,"T089");
	//	strCopy(ErrorReport.ErrorDetail,"NULL");
	//	strCopy(ErrorReport.ErrorMsg,szMessage);
	//}

	return res;
}
