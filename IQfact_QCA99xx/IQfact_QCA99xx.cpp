// IQfact_QCA99xx.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include "IQfact_QCA99xx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果這個 DLL 是動態地對 MFC DLL 連結，
//		那麼從這個 DLL 匯出的任何會呼叫
//		MFC 內部的函式，都必須在函式一開頭加上 AFX_MANAGE_STATE
//		巨集。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此處為正常函式主體
//		}
//
//		這個巨集一定要出現在每一個
//		函式中，才能夠呼叫 MFC 的內部。這意味著
//		它必須是函式內的第一個陳述式
//		，甚至必須在任何物件變數宣告前面
//		，因為它們的建構函式可能會產生對 MFC
//		DLL 內部的呼叫。
//
//		請參閱 MFC 技術提示 33 和 58 中的
//		詳細資料。
//

// CIQfact_QCA99xxApp

BEGIN_MESSAGE_MAP(CIQfact_QCA99xxApp, CWinApp)
END_MESSAGE_MAP()

GLOBAL_INFO					g_GlobalInfo;
CABLELOSS					g_CableLoss;
CALLBACKFN					CallBackFn;
NET_INFO					Net_Info;
TEST_INFO					Test_Info;
IQFACT_SCRIPT				Iqfact_Script;

// CIQfact_QCA99xxApp 建構

CIQfact_QCA99xxApp::CIQfact_QCA99xxApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}


// 僅有的一個 CIQfact_QCA99xxApp 物件

CIQfact_QCA99xxApp theApp;


// CIQfact_QCA99xxApp 初始設定

BOOL CIQfact_QCA99xxApp::InitInstance()
{
	CWinApp::InitInstance();
	//Open the Pipe ID
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);

	if(!PipeLineOpen(&g_GlobalInfo.Pipe_SYS_ID,"cmd.exe",szMessage)){
		::MessageBox(NULL, szMessage, _T("PipeLineOpen Failure"), MB_OK|MB_SETFOREGROUND);
		exit(0);//arp -d & write Mac or License Key tool
	}

	if(!PipeLineOpen(&g_GlobalInfo.Pipe_OEM_ID,"cmd.exe",szMessage)){
		::MessageBox(NULL, szMessage, _T("PipeLineOpen Failure"), MB_OK|MB_SETFOREGROUND);
		exit(0);//plink & /etc/init.d/.. & write dut env & verify dut env
	}
	if(!PipeLineOpen(&g_GlobalInfo.Pipe_PROG_ID,"cmd.exe",szMessage)){
		::MessageBox(NULL, szMessage, _T("PipeLineOpen Failure"), MB_OK|MB_SETFOREGROUND);
		exit(0);// IQrun_Console -run
	}

	GetLocalDirectory(g_GlobalInfo.szLocalPath,szMessage);
	return TRUE;
}



extern "C" BOOL PASCAL EXPORT CleanToDutParameter(LPSTR ActionName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(strcmp(ActionName,_T("WIFI_LOAD_DUT_REQUIRED")) == NULL)
		theApp.m_LoadDutRequired.Clean();
	else if(strcmp(ActionName,_T("WIFI_INITIALIZE_FTAPI")) == NULL)
		theApp.m_WiFiInitialize.Clean();
	else if(strcmp(ActionName,_T("WIFI_GET_OEM_MESSAGE")) == NULL){
		theApp.m_ART2_Test.CLEAN();
		theApp.m_IQfact_Test.CLEAN();
	}
	else if(strcmp(ActionName,_T("WIFI_EEPROM_VERIFICATION")) == NULL)
		theApp.m_VerifyTestData.Clean();
	else if(strcmp(ActionName,_T("WIFI_PROGRAM")) == NULL)
		theApp.m_WriteTestData.Clean();

	/*else if(strcmp(ActionName,_T("WIFI_TERMINATE_FTAPI")) == NULL)
		theApp.m_WiFiTerminate.Clean();*/
	else if(strcmp(ActionName,_T("BT_INITIALIZE_FTAPI")) == NULL)
		theApp.m_BtInitialize.Clean();
	else if(strcmp(ActionName,_T("BT_PROGRAM")) == NULL)
		theApp.m_BtWriteTestData.Clear();
	else if(strcmp(ActionName,_T("BT_EEPROM_VERIFICATION")) == NULL)
		theApp.m_BtVerifyTestData.Clear();



	return TRUE;
}

extern "C" BOOL PASCAL EXPORT SetToDutParameter(LPSTR ActionName, LPSTR ParameterName,LPSTR ParameterValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	//g_GlobalInfo
	if(strcmp(ParameterName, _T("IS_ART")) == NULL)
		g_GlobalInfo.IsArt2 = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName,_T("IS_CONSOLE")) == NULL)//Jerry Add
		g_GlobalInfo.IsConsole =(strcmp(ParameterValue,_T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName,_T("IS_COLON")) == NULL)//Jerry Add
		g_GlobalInfo.IsColon =(strcmp(ParameterValue,_T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("MAC_RULE")) == NULL){
		Test_Info.MAC_RULE = atoi(ParameterValue);	
	}
	//WIFI_LOAD_DUT_REQUIRED
	else if(strcmp(ParameterName, _T("ENV_PRE_SET")) == NULL)
		theApp.m_LoadDutRequired.Flag.ENV_PRE_SET = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("GEN_TESTSCRIPT")) == NULL)
		theApp.m_LoadDutRequired.Flag.GEN_TESTSCRIPT = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("COLSE_OEM_SOFT")) == NULL)
		theApp.m_LoadDutRequired.Flag.COLSE_OEM_SOFT = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("TFTPD32_INIT")) == NULL)
		theApp.m_LoadDutRequired.Flag.TFTPD32_INIT = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PLINK_INIT")) == NULL)
		theApp.m_LoadDutRequired.Flag.PLINK_INIT = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("REG_DELETE")) == NULL)
		theApp.m_LoadDutRequired.Flag.REG_DELETE = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_ARP")) == NULL)
		theApp.m_LoadDutRequired.Flag.PIPE_ARP = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PASER_CABLELOSS")) == NULL)
		theApp.m_LoadDutRequired.Flag.PASER_CABLELOSS = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName,_T("SEND_MSG")) == NULL)
		theApp.m_LoadDutRequired.Flag.SEND_MSG = (strcmp(ParameterValue,_T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("CONSOLE_MSG")) == NULL)
		strCopy(Test_Info.Console_Msg,ParameterValue);

	else if(strcmp(ParameterName, _T("SCRIPT_MAC_RULES")) == NULL)
		Iqfact_Script.Mac_Rules.Add(ParameterValue);
	else if(strcmp(ParameterName, _T("SAMPLE_SCRIPT_NAME")) == NULL)
		strCopy(Iqfact_Script.Sample_Script,ParameterValue);
	else if(strcmp(ParameterName, _T("TEST_SCRIPT_NAME")) == NULL)
		strCopy(Test_Info.szScript_Name,ParameterValue);

	//WIFI_INITIALIZE_FTAPI
	else if(strcmp(ParameterName, _T("START_DUT_SERVICE")) == NULL ||strcmp(ParameterName, _T("PIPE_INPUT_CMD")) == NULL)
		theApp.m_WiFiInitialize.Flag.START_DUT_SERVICE = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("ART2_OEM_TOOL")) == NULL)
		theApp.m_WiFiInitialize.Flag.ART2_OEM_TOOL = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("ONLY_CART")) == NULL)
		theApp.m_WiFiInitialize.Flag.ONLY_CART = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	

	//BT_INITIALIZE_FTAPI
	else if(strcmp(ParameterName, _T("PIPE_INPUT_TEST_CMD")) == NULL)
		theApp.m_BtInitialize.Flag.PIPE_INPUT_TEST_CMD = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName,_T("CONSOLE_INPUT_TEST_CMD")) == NULL)//Jerry Add
		theApp.m_BtInitialize.Flag.CONSOLE_INPUT_TEST_CMD =(strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName,_T("CONSOLE_KEYWORD")) == NULL)
		theApp.m_BtInitialize.Flag.CONSOLE_KEYWORD =(strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("WRITE_BT_MAC")) == NULL)
		theApp.m_BtWriteTestData.Flag.WRITE_BT_MAC = (strcmp(ParameterValue,"TRUE") == NULL)? TRUE: FALSE;	
	else if(strcmp(ParameterName, _T("WRITE_BT_POWER_LEVEL")) == NULL)
		theApp.m_BtWriteTestData.Flag.WRITE_BT_POWER_LEVEL = (strcmp(ParameterValue,"TRUE") == NULL)? TRUE: FALSE;	
	else if(strcmp(ParameterName, _T("CHECK_BT_MAC")) == NULL)
		theApp.m_BtVerifyTestData.Flag.CHECK_BT_MAC = (strcmp(ParameterValue,"TRUE") == NULL)? TRUE: FALSE;	
	else if(strcmp(ParameterName, _T("CHECK_BT_POWER_LEVEL")) == NULL)
		theApp.m_BtVerifyTestData.Flag.CHECK_BT_POWER_LEVEL = (strcmp(ParameterValue,"TRUE") == NULL)? TRUE: FALSE;	
	else if(strcmp(ParameterName, _T("CHIP_MODE")) == NULL)
		strCopy(Test_Info.ChipMode,ParameterValue);

	else if(_strnicmp(ParameterName, _T("BT_1M_PWR"),9) == NULL){
		theApp.m_PwrTable.BT_1M = atof(ParameterValue);
	}else if(_strnicmp(ParameterName, _T("BT_2M_PWR"),9) == NULL){
		theApp.m_PwrTable.BT_2M = atof(ParameterValue);
	}else if(_strnicmp(ParameterName, _T("BT_3M_PWR"),9) == NULL){
		theApp.m_PwrTable.BT_3M = atof(ParameterValue);
	}else if(_strnicmp(ParameterName, _T("BT_LE_PWR"),9) == NULL){
		theApp.m_PwrTable.BT_LE = atof(ParameterValue);
	}

	//WIFI_PROGRAM
	else if(strcmp(ParameterName, _T("PIPE_WRITE_IDL_MAC")) == NULL)
		theApp.m_WriteTestData.Flag.PIPE_WRITE_IDL_MAC = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_WRITE_LICENSE_KEY")) == NULL)//Jerry Add
		theApp.m_WriteTestData.Flag.PIPE_WRITE_LICENSE_KEY = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_WRITE_DUT_SN")) == NULL)//Jerry Add
		theApp.m_WriteTestData.Flag.PIPE_WRITE_DUT_SN = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_WRITE_DUT_MAC")) == NULL)//Jerry Add
		theApp.m_WriteTestData.Flag.PIPE_WRITE_DUT_MAC = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_WRITE_COUNTRY_CODE")) == NULL)//Jerry Add
		theApp.m_WriteTestData.Flag.PIPE_WRITE_COUNTRY_CODE = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_WRITE_DUT_ENV")) == NULL)//Jerry Add
		theApp.m_WriteTestData.Flag.PIPE_WRITE_DUT_ENV = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;

	//WIFI_EEPROM_VERIFICATION
	else if(strcmp(ParameterName, _T("PIPE_CHECK_DUT_MAC")) == NULL)
		theApp.m_VerifyTestData.Flag.PIPE_CHECK_DUT_MAC = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_CHECK_DUT_SN")) == NULL)//Jerry Add
		theApp.m_VerifyTestData.Flag.PIPE_CHECK_DUT_SN = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_CHECK_COUNTRY_CODE")) == NULL)//Jerry Add
		theApp.m_VerifyTestData.Flag.PIPE_CHECK_COUNTRY_CODE = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName,_T("PIPE_CHECK_DUT_ENV")) == NULL)//Jerry Add
		theApp.m_VerifyTestData.Flag.PIPE_CHECK_DUT_ENV = (strcmp(ParameterValue,_T("TRUE")) == NULL)? TRUE:FALSE;
	/*else if(strcmp(ParameterName, _T("PIPE_CHECK_DUT_MAC")) == NULL)
		theApp.m_VerifyTestData.Flag.PIPE_CHECK_DUT_MAC = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;
	else if(strcmp(ParameterName, _T("PIPE_CHECK_DUT_MAC")) == NULL)
		theApp.m_VerifyTestData.Flag.PIPE_CHECK_DUT_MAC = (strcmp(ParameterValue, _T("TRUE")) == NULL)? TRUE:FALSE;*/

	else if(strcmp(ParameterName, _T("DONTCARE_MSG")) == NULL)
		theApp.m_ART2_Test.DontCare_Msg.AddHead(ParameterValue);
	else if(strcmp(ParameterName, _T("IQFACT_CABLELOSS")) == NULL)
		strCopy(g_CableLoss.Iqfact_CableLoss,ParameterValue);
	else if(strcmp(ParameterName, _T("ITEM")) == NULL)
		strCopy(Test_Info.szItem,ParameterValue);
	else if(strcmp(ParameterName, _T("STR_START")) == NULL)
		strCopy(Test_Info.Str_Start,ParameterValue);
	else if(strcmp(ParameterName, _T("STR_END")) == NULL)
		strCopy(Test_Info.Str_End,ParameterValue);
	else if(strcmp(ParameterName, _T("SCRIPT_NAME")) == NULL)
		strCopy(Test_Info.szScript_Name,ParameterValue);
	else if(strcmp(ParameterName, _T("CMD")) == NULL)
		strCopy(Test_Info.szCmd,ParameterValue);
	else if(strcmp(ParameterName,_T("ERROR_CODE")) == NULL)
		strCopy(Test_Info.ErrorCode,ParameterValue);
	else if(strcmp(ParameterName, _T("KEYWORD")) == NULL)
		strCopy(Test_Info.szKeyWord,ParameterValue);
	else if(strcmp(ParameterName, _T("USERNAME")) == NULL)
		strCopy(Net_Info.UserName,ParameterValue);
	else if(strcmp(ParameterName, _T("PASSWORD")) == NULL)
		strCopy(Net_Info.PassWord,ParameterValue);
	else if(strcmp(ParameterName, _T("TIMEOUT")) == NULL)
		Test_Info.TimeOut=atoi(ParameterValue);
	else if(strcmp(ParameterName, _T("CUSTOMER")) == NULL)
		strCopy(g_GlobalInfo.CUSTOMER,ParameterValue);
	else if(strcmp(ParameterName, _T("CONFIG_NAME")) == NULL)
		strCopy(theApp.m_WriteTestData.Config_Name,ParameterValue);
	else if(strcmp(ParameterName, _T("COUNTRY_CODE")) == NULL)
		strCopy(Test_Info.Country_Code,ParameterValue);
	else
		strCopy(g_GlobalInfo.INI_STR,ParameterValue);

	return TRUE;
}

extern "C" BOOL PASCAL EXPORT PreShootToDutParameter(LPSTR ActionName, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}



extern "C" BOOL PASCAL EXPORT WiFiInitialize(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_WiFiInitialize.START(bRetry,lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT WiFiTerminate(BOOL bRetry, LPSTR lpszRunInfo)
{
	return theApp.m_WiFiTerminate.START(bRetry,lpszRunInfo);
}

extern "C" BOOL PASCAL EXPORT LoadDutRequired(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_LoadDutRequired.START(bRetry,lpszRunInfo);
}
extern "C" TM_RETURN PASCAL EXPORT WriteResultToDut(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_WriteTestData.START(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport,nRetryCnt);
}
extern "C" TM_RETURN PASCAL EXPORT ReturnOemAppMessage(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt,BOOL bRetry)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (g_GlobalInfo.IsArt2)
		return theApp.m_ART2_Test.START(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport);
	else
		return theApp.m_IQfact_Test.START(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport,nRetryCnt,bRetry);
}
extern "C" TM_RETURN PASCAL EXPORT EepromVerification(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	

	return theApp.m_VerifyTestData.START(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport,nRetryCnt);
}
extern "C" TM_RETURN PASCAL EXPORT ReturnPreloadMessage(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return theApp.m_PreLoadImage.START(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport);
}
extern "C" BOOL PASCAL EXPORT BtInitialize(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_BtInitialize.START(bRetry, lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT BtGetTargetPower(BT_DUT_PARAM &DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());//1st GetTrgetPower	
	return theApp.m_BtGetTargetPower.Start(DutParam, theApp.m_PwrTable, lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT BtStartFrameModulatedTx(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 2nd StartFrameModulatedTx	
	return theApp.m_BtStartFrameModulatedTx.Start(DutParam, lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT BtStopFrameModulatedTx(BT_DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 3rd StopFrameModulatedTX	
	return theApp.m_BtStopFrameModulatedTx.Start(DutParam, lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT BtStartContModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 2rd StopFrameModulatedTX	for BT
	return theApp.m_BtStartContModulatedTx.Run(DutParam, lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT BtStopContModulatedTx(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo){
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// 3rd StopFrameModulatedTX	for BT
	return theApp.m_BtStopContModulatedTx.Run(DutParam, lpszRunInfo);
}
extern "C" TM_RETURN PASCAL EXPORT BtEepromVerification(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	return theApp.m_BtVerifyTestData.Run(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport,nRetryCnt);
}

extern "C" TM_RETURN PASCAL EXPORT BtWriteResultToDut(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	return theApp.m_BtWriteTestData.Run(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, ErrorReport,nRetryCnt);
}
extern "C" BOOL PASCAL EXPORT LOAD_GUI_INFORMATION(LPSTR lpszOP, LPSTR lpszBarcode, LPSTR lpszPN, SFCS_MODE eSfcsMode, BOOL bStopOnFailed, HWND hParentWnd, ...)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());//Load GUI Info.
	strCopy(g_GlobalInfo.szOP,lpszOP);
	strCopy(g_GlobalInfo.szBarcode,lpszBarcode);
	strCopy(g_GlobalInfo.szPN,lpszPN);
	g_GlobalInfo.Main_Handle=hParentWnd;

	return TRUE;
}
extern "C" BOOL PASCAL EXPORT RestartDutTest()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TCHAR szDelFilePath[MAX_PATH]={0};
	TCHAR szArguments[MAX_PATH]={0};
	TCHAR szMessage[DLL_INFO_SIZE]={0};
	TCHAR szLocalPath[MAX_PATH]={0};
	TCHAR szFilePath[MAX_PATH]={0};

	GetLocalDirectory(szLocalPath,szMessage);	
	//Clear .txt files.
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,UART_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,UART_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);

	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,PIPE_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,PIPE_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);

	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,TELNET_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,TELNET_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);

	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,SOCKET_COMMAND);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);
	sprintf_s(szFilePath,sizeof(szFilePath),"%s%s",szLocalPath,SOCKET_MESSAGE);
	if(CheckFileExist(szFilePath,szMessage))
		DeleteFile(szFilePath);


	return TRUE;
}
extern "C" BOOL PASCAL EXPORT APPLICATION_CLOSE_TO_DUT()
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}
extern "C" BOOL PASCAL EXPORT ParserWiFiCableLossToDut(double *dbCableLoss, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());// Load CableLoss.ini to Customer(IQfact .csv ART2 .art)	

	zeroIt(g_CableLoss.TxPath);
	zeroIt(g_CableLoss.RxPath);
	zeroIt(g_CableLoss.GuPath);

	memcpy(g_CableLoss.TxPath,&dbCableLoss[0],sizeof(g_CableLoss.TxPath));
	memcpy(g_CableLoss.RxPath,&dbCableLoss[WIFI_CABLELOSS_ANTENNA*WIFI_CABLELOSS_MAX_ARY*WIFI_CABLELOSS_RESULT],sizeof(g_CableLoss.RxPath));
	memcpy(g_CableLoss.GuPath,&dbCableLoss[WIFI_CABLELOSS_ANTENNA*WIFI_CABLELOSS_MAX_ARY*WIFI_CABLELOSS_RESULT*2],sizeof(g_CableLoss.GuPath));
	if (g_GlobalInfo.IsArt2)// Analysis ART2(.art) or IQfact CableLoss(.csv)
		return theApp.m_ParserCableLoss.ParserART2CableLoss(lpszRunInfo);
	else
		return theApp.m_ParserCableLoss.ParserIQfactCableLoss(lpszRunInfo);
}

extern "C" BOOL PASCAL EXPORT StartContModulatedTx(DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.m_StartContModulatedTx.Run(DutParam, theApp.m_Sleep, lpszRunInfo);
}
extern "C" BOOL PASCAL EXPORT StopContModulatedTx(DUT_PARAM DutParam,LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	return theApp.m_StopContModulatedTx.Run(DutParam, lpszRunInfo);
}

extern "C" BOOL PASCAL EXPORT SfcsResponds(int MESSAGE_TYPE, LPSTR PREPOSING_CODE, LPSTR RESPONS_MSG, LPSTR lpszRunInfo)
{//Jerry Add Query SFCS Message
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ExportTestLog("PREPOSING_CODE = %s",PREPOSING_CODE);
	ExportTestLog("RESPONS_MSG = %s",RESPONS_MSG);
	ExportTestLog("szMessage = %s",lpszRunInfo);

	//if(strstr(PREPOSING_CODE,"FCSN") || strstr(PREPOSING_CODE,"CSN") || strstr(PREPOSING_CODE,"FCUSTSN")){
	if(strstr(PREPOSING_CODE,"FCSN") || strstr(PREPOSING_CODE,"CSN") ){
		zeroIt(Test_Info.SN);zeroIt(Net_Info.szSN);
		strCopy(Test_Info.SN,RESPONS_MSG);strCopy(Net_Info.szSN,RESPONS_MSG);
	}

	return TRUE;
}

extern "C" BOOL PASCAL EXPORT PowerSupplyCtrl(funcSetPowerSupply cbPSCtrl, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CallBackFn.cbPSCtrl = cbPSCtrl;
	return TRUE;
}

extern "C" BOOL PASCAL EXPORT GetRdModeLicense()
{//Jerry Add
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TRUE;
}

