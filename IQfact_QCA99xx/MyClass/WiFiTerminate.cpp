#include "../StdAfx.h"
#include "WiFiTerminate.h"

CWiFiTerminate::CWiFiTerminate(void)
{
}

CWiFiTerminate::~CWiFiTerminate(void)
{

}

BOOL CWiFiTerminate::START(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	TCHAR szAppFilePath[MAX_PATH];zeroIt(szAppFilePath);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	HWND hWnd;

	for(int i=0 ; i<2 ; i++)
	{
		if(i == 0)
			sprintf_s(szAppFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\cart.exe", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
		else
			sprintf_s(szAppFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\nart.exe", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
		//Check application file exist.
		if(!CheckFileExist(szAppFilePath,szMessage))
		{
			sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Failed to open %s file (%s)!!",szAppFilePath, szMessage);
			return FALSE;
		}

		//Clean Oem Application
		do
		{
			if((hWnd = ::FindWindow(NULL,szAppFilePath)))
				::SendMessage(hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);					
		}while(hWnd);// Close cart/nart.exe
	}
	return TRUE;
}

