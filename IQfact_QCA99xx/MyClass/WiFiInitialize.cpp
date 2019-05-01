#include "../StdAfx.h"
#include "WiFiInitialize.h"

CWiFiInitialize::CWiFiInitialize(void)
{
}

CWiFiInitialize::~CWiFiInitialize(void)
{

}
void CWiFiInitialize::Clean(){

	zeroIt(Flag);
	res=TRUE;

}

BOOL CWiFiInitialize::START(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	szWorkName="";
	for(int work_flow=0;work_flow<=(int)ART2_OEM_TOOL;work_flow++){

		if (szWorkName.GetLength()>0) break;

		switch(work_flow){
		case START_DUT_SERVICE:
			if (!Flag.START_DUT_SERVICE) continue;
			if(strstr(Test_Info.szCmd,"wifi down") || strstr(Test_Info.szCmd,"qcmbr")){
				szWorkName="START_DUT_SERVICE";
			}else
				szWorkName="PIPE_INPUT_CMD";
			ExportTestLog("==========%s==========",szWorkName);
			res=Start_Dut_Service(Test_Info.szCmd,Test_Info.szKeyWord,Test_Info.TimeOut);//cmd: wifi down; /etc/init.d/qcmbr start;/etc/init.d/art start
			break;
		case ART2_OEM_TOOL:
			if(!Flag.ART2_OEM_TOOL) continue;
			szWorkName="ART2_OEM_TOOL";
			ExportTestLog("==========%s==========",szWorkName);
			res=Art2_Oem_Tool(Flag.ONLY_CART,lpszRunInfo);//Open cart/nart.exe
		}//switch
	}

	if (szWorkName.GetLength()>0){
		ExportTestLog("==========%s==========\r\n",szWorkName);
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s_%s",szWorkName.GetBuffer(),(res)?"PASS":"FAIL");
	}
	else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"WIFI_LOAD_DUT_REQUIRED_PASS");
	}

	return res;
}
BOOL CWiFiInitialize::Start_Dut_Service(LPSTR szCmd,LPSTR szKeyWord,UINT TimeOut){
	BOOL res=FALSE;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);

	res=CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,szKeyWord,szMessage,TimeOut);

	return res;
}
BOOL CWiFiInitialize::Art2_Oem_Tool(BOOL Only_Cart,LPSTR lpszRunInfo){
	//BOOL res=FALSE;
	char szMessage[DLL_INFO_SIZE]={0};
	char szAppFilePath[MAX_PATH]={0};
	HWND hWnd = NULL;
	char szRunMode[16]={0};
	char szArguments[100]={0};
	char szLogFile[MAX_PATH]={0};

	if(!SocketClose(szMessage))//Init Socket Close
	{
		sprintf_s(lpszRunInfo, DLL_INFO_SIZE, "Socket close failed,[MSG :%s]",szMessage);
		return FALSE;
	}
	for(int i=0 ; i<2 ; i++)
	{
		if(Only_Cart){
			i=1;
		}
		if(i == 0)
			sprintf_s(szAppFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\nart.exe", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
		else
			sprintf_s(szAppFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\cart.exe", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);

		if(!CheckFileExist(szAppFilePath,szMessage))
		{
			sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Failed to open %s file (%s)!!",szAppFilePath, szMessage);
			return FALSE;
		}
		do
		{
			if((hWnd = ::FindWindow(NULL,szAppFilePath)))
				::SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);					
		}while(hWnd);
	}//for loop

	if(!SocketInitial_TCP(TRUE,"127.0.0.1",2398,szMessage))// Init use Socket TCP IP and Port
	{
		strcpy_s(lpszRunInfo, DLL_INFO_SIZE, "Open socket failed,[IP:127.0.0.1,Port:2398]");
		return FALSE;
	}
	for(int i=0 ; i<2 ; i++)
	{

		if(Only_Cart){
			i=1;
		}
		if(i == 0)
			sprintf_s(szAppFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\nart.exe", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
		else
			sprintf_s(szAppFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\cart.exe", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);

		//Open Customer Application file		
		(!i)? strCopy(szArguments,""): strCopy(szArguments,"-gui localhost:2398");	

		//sprintf_s(szLogFile,sizeof(szLogFile),"%sOEM\\ART2\\%s\\report\\%s_report.txt",g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER,g_GlobalInfo.szWiFiMac);

		if(!PipeOpenBAT(szAppFilePath,szArguments,szLogFile,lpszRunInfo))
		{
			sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't create pipe process;[Name:%s;Argument:%s]",szAppFilePath,szArguments);
			return FALSE;
		}
		DWORD stime=GetTickCount();
		while(1)
		{
			//if( (hWnd = ::FindWindow(NULL,szAppFilePath)) )
			hWnd = ::FindWindow(NULL,szAppFilePath);
			ExportTestLog("%s hwnd = %p",szAppFilePath,hWnd);
			if(hWnd){
				break;
			}
			else{
				if( ((GetTickCount() - stime)/1000) > 10)
				{
					AfxMessageBox("Please close other running cart/nart of application.",MB_SETFOREGROUND);
					if(!PipeOpenBAT(szAppFilePath,szArguments,szLogFile,lpszRunInfo))
					{
						sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't create pipe process;[Name:%s;Argument:%s]",szAppFilePath,szArguments);
						return FALSE;
					}
					stime=GetTickCount();
				}
				Sleep(500);
			}
		}


		Sleep(1000);
	}//for loop
	strcpy_s(lpszRunInfo,DLL_INFO_SIZE,"DUT was successfully initialized.");

	return TRUE;
}