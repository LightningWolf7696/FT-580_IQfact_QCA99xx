#include "../StdAfx.h"
#include "ART2_Test.h"

CART2_Test::CART2_Test(void)
{
	iTestCount = 0;
	
}

CART2_Test::~CART2_Test(void)
{

}

void CART2_Test::CLEAN()
{
	zeroIt(m_WiFiTestData);
	zeroIt(m_DisplayErrorMsg);	
	m_StartKeyWord = FALSE;
	m_AnalyzeFail = FALSE;
	m_ErrorReport.DisableSfcsLink = FALSE;
	strCopy(m_ErrorReport.ErrorCode,_T("No Error"));
	strCopy(m_ErrorReport.ErrorDetail,_T("No Error"));
	strCopy(m_ErrorReport.ErrorMsg,_T("No Error"));
	strCopy(m_ErrorReport.FailedTest,_T("No Error"));
	m_ErrorReport.TestResult = TRUE;
	m_eTestType = NOT_DEFINE_TEST;	
	DontCare_Msg.RemoveAll();
	return;
}

TM_RETURN CART2_Test::START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport)
{

	TCHAR szSocketMsg[SOCKET_MAX_LENGTH*2];zeroIt(szSocketMsg);
	TCHAR szSocketTemp[SOCKET_MAX_LENGTH];zeroIt(szSocketTemp);
	TCHAR szAvailMsg[SOCKET_MAX_LENGTH];zeroIt(szAvailMsg);
	double dbStartTime = GetTickCount();
	int nTimeOutSec = 30;
	DWORD dwBytesRead;
	BOOL bRtn = FALSE;
	TCHAR szMessage[DLL_INFO_SIZE]={0};	
	//TCHAR szFullCmd[64]={0};	
	TCHAR szLogFile[MAX_PATH]={0};
	sprintf_s(szLogFile,sizeof(szLogFile),"%sOEM\\ART2\\%s\\report\\%s_report.txt",g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER,g_GlobalInfo.szBarcode);
	//	PipeCleanBAT();
	if(CheckFileExist(szLogFile,szMessage))
		DeleteFile(szLogFile);

	HWND hWnd;
	char szAppFilePath[MAX_PATH]={0};		
	sprintf_s(szLogFile,sizeof(szLogFile),"%sOEM\\ART2\\%s\\bin\\cart.exe",g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
	if(!(hWnd = ::FindWindow(NULL,szAppFilePath)))// Fine Handle Window
	{			
		ErrorReport.DisableSfcsLink = TRUE;
		strCopy(ErrorReport.ErrorCode,_T("T089"));
		strCopy(ErrorReport.ErrorDetail,_T("NULL"));
		sprintf_s(ErrorReport.ErrorMsg,sizeof(ErrorReport.ErrorMsg),"Not found OEM application handle.[%s]",szAppFilePath);
		strCopy(ErrorReport.FailedTest,_T("OEM TEST"));
		ErrorReport.TestResult = FALSE;
		return TM_RETURN_ERROR;	
	}
	
	if(strlen(Test_Info.szScript_Name)>0){
		sprintf_s(szAppFilePath,sizeof(szAppFilePath),"%s\n",Test_Info.szScript_Name);// Write test_cal_v3 cmd
		for (int i=0;i<10;i++){
			Sleep(10);
			bRtn=SocketSend(szAppFilePath,szMessage);// Use Socket Send test cmd
			if(bRtn){
				break;
			}
			else{
				ExportTestLog("Socket szMessage ={%s}",szMessage);
			}
		}
	}
	//else if(g_GlobalInfo.ManufactureAutoMode)
	//{
	//	CreateMacFile();
	//	bRtn = StartAutoProcess(cbListMessage, cbXmlMessage, szMessage);
	//}else
	//	bRtn = StartProcess(cbListMessage, cbXmlMessage, szMessage);

	if(!bRtn)
	{
		ErrorReport.DisableSfcsLink = FALSE;
		strCopy(ErrorReport.ErrorCode,_T("T001"));
		sprintf_s(ErrorReport.ErrorDetail,sizeof(ErrorReport.ErrorDetail),"%s",szMessage);
		strCopy(ErrorReport.ErrorMsg,_T("Start ART2 Process failed."));
		strCopy(ErrorReport.FailedTest,_T("OEM TEST"));
		ErrorReport.TestResult = FALSE;
		return TM_RETURN_ERROR;	
	}		


	int i=0;
	TCHAR lpszRunInfo[MAX_PATH]={0};

	m_dbTestTimeCost = GetTickCount();
	do
	{
		m_dbArtMsgTimeOut = GetTickCount();
		do
		{
			zeroIt(szSocketTemp);	
			if(!SocketReceive(szSocketTemp, sizeof(szSocketTemp), dwBytesRead, szMessage))// Use Socket Receive data
			{
				ErrorReport.DisableSfcsLink = FALSE;
				strCopy(ErrorReport.ErrorCode,_T("T089"));
				strCopy(ErrorReport.ErrorDetail,_T("NULL"));
				strCopy(ErrorReport.ErrorMsg,szMessage);
				strCopy(ErrorReport.FailedTest,_T("Read data from socket failed"));
				ErrorReport.TestResult = FALSE;
				return TM_RETURN_ERROR;	
			}	

			strcat_s(szSocketMsg,strlen(szSocketMsg)+strlen(szSocketTemp)+1,szSocketTemp);

			Sleep(10);
			//set time out

			if(((GetTickCount() - m_dbArtMsgTimeOut)/1000) > Test_Info.TimeOut)
			{
				ExportTestLog("m_dbArtMsgTimeOut fun");
				sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Get message is timeout [%d sec]",Test_Info.TimeOut);
				ErrorReport.DisableSfcsLink = TRUE;
				strCopy(ErrorReport.ErrorCode,_T("T089"));
				strCopy(ErrorReport.ErrorDetail,_T("NULL"));
				strCopy(ErrorReport.ErrorMsg,szMessage);
				strCopy(ErrorReport.FailedTest,_T(lpszRunInfo));
				ErrorReport.TestResult = FALSE;
				return TM_RETURN_ERROR;	
			}

		} while (!strstr(szSocketMsg, szSeparateKey));	

		do 
		{
			zeroIt(szAvailMsg);
			if(!CCommFunc::CombineMessage(szSocketMsg, szAvailMsg, sizeof(szAvailMsg), szSeparateKey)) break;//szAvailMsg=analysis data, szSocketMsg=wait analysis data

			CollectTestData(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, szAvailMsg);

			if(m_eTestType == TEST_RESULT)
				goto FinishTest;
		} while (1);

	} while (1);



FinishTest:
	CheckErrorIsDefine();
	memcpy_s(&ErrorReport, sizeof(ErrorReport), &m_ErrorReport, sizeof(ERRORREPORT));

	if(ErrorReport.TestResult)
	{
		TCHAR szDelFilePath[MAX_PATH]={0};
		TCHAR szArguments[MAX_PATH]={0};

		for(int i=0 ; i<2 ; i++)
		{
			if(i == 0){
				sprintf_s(szDelFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\report", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
				sprintf_s(szArguments,sizeof(szArguments),"del *.txt /q /f");
			}else{
				sprintf_s(szDelFilePath,MAX_PATH,"%sOEM\\ART2\\%s\\log", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
				sprintf_s(szArguments,sizeof(szArguments),"del *.log /q /f");
			}
			CCommFunc::DelFolderAllFile(szDelFilePath, szArguments, szMessage);
		}
		return TM_RETURN_PASS;
	}
	else if(ErrorReport.DisableSfcsLink)
		return TM_RETURN_ERROR;
	else
		return TM_RETURN_FAIL;
}
void CART2_Test::CollectTestData(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, LPSTR lpszMessage)
{
	CString strTemp;			

	strTemp.Format(_T("%s"),lpszMessage);	

	if(strstr(lpszMessage,"%"))								
		strTemp.Replace("%","%%%%%%%%");

	DisplayTestLog(cbListMessage, strTemp.GetBuffer());


	if(!(Test_Info.DontCare_Msg.Find(strTemp)!=NULL) )
	{
		SeparateData(lpszMessage);
	}


	
	
	if((!m_StartKeyWord) || (m_eTestType == NOT_DEFINE_TEST))
		return;

//XML	
	if(m_eTestType == WIFI_TX_POWER_CALIBRATION)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","TX_POWER_CALIBRATION");
	else if(m_eTestType == WIFI_XTAL_CALIBRATION)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","XTAL_CALIBRATION");
	else if(m_eTestType == WIFI_POWER_ACCURACY)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","TX_VERIFY_POWER");
	else if(m_eTestType == WIFI_EVM_TEST)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","TX_VERIFY_EVM");
	else if(m_eTestType == WIFI_FREQUENCY_ACCURACY)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","TX_FREQ_ACCURACY");
	else if(m_eTestType == WIFI_MASK_TEST)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","TX_VERIFY_MASK");
	else if(m_eTestType == WIFI_RX_SWEEP_TEST)
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TestCase name=\"%s\">","RX_SWEEP_PER");

	CCommFunc::ExportTestMessage(cbXmlMessage, "<Data>");	

	if(m_eTestType == WIFI_TX_POWER_CALIBRATION){
		CCommFunc::ExportTestMessage(cbXmlMessage, "<FREQ>%s</FREQ>", m_WiFiTestData.TxCalibration.szFreq);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>%s</ANTENNA>", m_WiFiTestData.TxCalibration.szAntenna);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<GAIN>%s</GAIN>", m_WiFiTestData.TxCalibration.szGain);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<POWER>%s</POWER>", m_WiFiTestData.TxCalibration.szPower);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<PCORR>%s</PCORR>", m_WiFiTestData.TxCalibration.szPcorr);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<VOLTAGE>%s</VOLTAGE>", m_WiFiTestData.TxCalibration.szVoltage);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<TEMP>%s</TEMP>", m_WiFiTestData.TxCalibration.szTemperature);
	}else if(m_eTestType == WIFI_POWER_ACCURACY){
		CCommFunc::ExportTestMessage(cbXmlMessage, "<FREQ>%s</FREQ>", m_WiFiTestData.PowerAccuracy.szFreq);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<RATE>%s</RATE>", m_WiFiTestData.PowerAccuracy.szDataRate);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>%s</ANTENNA>", m_WiFiTestData.PowerAccuracy.szAntenna);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_POWER>%s</EXPECT_POWER>", m_WiFiTestData.PowerAccuracy.szExpectPwr);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ACTUAL_POWER>%s</ACTUAL_POWER>", m_WiFiTestData.PowerAccuracy.szActualPwr);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<PWR_DELTA>%s</PWR_DELTA>", m_WiFiTestData.PowerAccuracy.szPwrDelta);
	}else if(m_eTestType == WIFI_EVM_TEST){
		CCommFunc::ExportTestMessage(cbXmlMessage, "<FREQ>%s</FREQ>", m_WiFiTestData.Evm.szFreq);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<RATE>%s</RATE>", m_WiFiTestData.Evm.szDataRate);
		if(m_WiFiTestData.Evm.bStreamData)
			CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>S%s</ANTENNA>", m_WiFiTestData.Evm.szAntenna);
		else
			CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>A%s</ANTENNA>", m_WiFiTestData.Evm.szAntenna);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_POWER>%s</EXPECT_POWER>", m_WiFiTestData.Evm.szExpectPwr);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ACTUAL_POWER>%s</ACTUAL_POWER>", m_WiFiTestData.Evm.szActualPwr);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_EVM>%s</EXPECT_EVM>", m_WiFiTestData.Evm.szExpectEVM);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ACTUAL_EVM>%s</ACTUAL_EVM>", m_WiFiTestData.Evm.szActualEVM);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<PWR_DELTA>%s</PWR_DELTA>", m_WiFiTestData.Evm.szPwrDelta);
	}else if(m_eTestType == WIFI_FREQUENCY_ACCURACY){
			CCommFunc::ExportTestMessage(cbXmlMessage, "<FREQ>%s</FREQ>", m_WiFiTestData.FreqAccuracy.szFreq);
			CCommFunc::ExportTestMessage(cbXmlMessage, "<RATE>%s</RATE>", m_WiFiTestData.FreqAccuracy.szDataRate);
			CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>%s</ANTENNA>", m_WiFiTestData.FreqAccuracy.szAntenna);
			CCommFunc::ExportTestMessage(cbXmlMessage, "<PPM>%s</PPM>", m_WiFiTestData.FreqAccuracy.szActualPpm);
			CCommFunc::ExportTestMessage(cbXmlMessage, "<LOW>%s</LOW>", m_WiFiTestData.FreqAccuracy.szPpmLow);
			CCommFunc::ExportTestMessage(cbXmlMessage, "<HIGH>%s</HIGH>", m_WiFiTestData.FreqAccuracy.szPpmHigh);
	}else if(m_eTestType == WIFI_MASK_TEST){
		CCommFunc::ExportTestMessage(cbXmlMessage, "<FREQ>%s</FREQ>", m_WiFiTestData.Mask.szFreq);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<RATE>%s</RATE>", m_WiFiTestData.Mask.szDataRate);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>%s</ANTENNA>", m_WiFiTestData.Mask.szAntenna);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<EXPECT_POWER>%s</EXPECT_POWER>", m_WiFiTestData.Mask.szExpectPwr);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<VIOLATION_PERCENT>%s</VIOLATION_PERCENT>", m_WiFiTestData.Mask.szActualMask);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<HIGH>%s</HIGH>", m_WiFiTestData.Mask.szMaskHigh);
	}else if(m_eTestType == WIFI_RX_SWEEP_TEST){
		CCommFunc::ExportTestMessage(cbXmlMessage, "<FREQ>%s</FREQ>", m_WiFiTestData.RxSweep.szFreq);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<RATE>%s</RATE>", m_WiFiTestData.RxSweep.szDataRate);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<ANTENNA>%s</ANTENNA>", m_WiFiTestData.RxSweep.szAntenna);		
		CCommFunc::ExportTestMessage(cbXmlMessage, "<VIOLATION_PERCENT>%s</VIOLATION_PERCENT>", m_WiFiTestData.RxSweep.szFIT);
		CCommFunc::ExportTestMessage(cbXmlMessage, "<HIGH>%s</HIGH>", m_WiFiTestData.RxSweep.szPerHigh);
	}
	CCommFunc::ExportTestMessage(cbXmlMessage, "<TEST_TIME>%.3f</TEST_TIME>", (float)((GetTickCount() - m_dbTestTimeCost)/1000));
	CCommFunc::ExportTestMessage(cbXmlMessage, "</Data>");
	CCommFunc::ExportTestMessage(cbXmlMessage, "</TestCase>");
	m_dbTestTimeCost = GetTickCount();

	if(m_eTestType == WIFI_POWER_ACCURACY){
		//Record Spec to SFCS
		CCommFunc::ExportTestMessage(cbSfcsSpecMessage, "WiFiTxPowerVerification-F%sA%sR%s_S_%s", 
			m_WiFiTestData.PowerAccuracy.szFreq, 
			m_WiFiTestData.PowerAccuracy.szAntenna, 
			m_WiFiTestData.PowerAccuracy.szDataRate, 
			m_WiFiTestData.PowerAccuracy.szExpectPwr);
		//Record Result to SFCS
		CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiTxPowerVerification-F%sA%sR%s_%s",
			m_WiFiTestData.PowerAccuracy.szFreq, 
			m_WiFiTestData.PowerAccuracy.szAntenna, 
			m_WiFiTestData.PowerAccuracy.szDataRate,
			m_WiFiTestData.PowerAccuracy.szActualPwr);
	}else if(m_eTestType == WIFI_EVM_TEST){
		//Record Spec to SFCS
		CCommFunc::ExportTestMessage(cbSfcsSpecMessage, "WiFiTxEvmPwr-F%s%s%sR%s_S_%s",
														m_WiFiTestData.Evm.szFreq, 
														(m_WiFiTestData.Evm.bStreamData)? _T("S") : _T("A"),
														m_WiFiTestData.Evm.szAntenna, 
														m_WiFiTestData.Evm.szDataRate,
														m_WiFiTestData.Evm.szExpectPwr);
		CCommFunc::ExportTestMessage(cbSfcsSpecMessage, "WiFiTxEvm-F%s%s%sR%s_S_%s",
														m_WiFiTestData.Evm.szFreq, 
														(m_WiFiTestData.Evm.bStreamData)? _T("S") : _T("A"),
														m_WiFiTestData.Evm.szAntenna, 
														m_WiFiTestData.Evm.szDataRate,
														m_WiFiTestData.Evm.szExpectEVM);	

		//Record Result to SFCS
		CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiTxEvmPwr-F%s%s%sR%s_%s",
														m_WiFiTestData.Evm.szFreq, 
														(m_WiFiTestData.Evm.bStreamData)? _T("S") : _T("A"),
														m_WiFiTestData.Evm.szAntenna, 
														m_WiFiTestData.Evm.szDataRate,
														m_WiFiTestData.Evm.szActualPwr);

		CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiTxEvm-F%s%s%sR%s_%s",
														m_WiFiTestData.Evm.szFreq, 
														(m_WiFiTestData.Evm.bStreamData)? _T("S") : _T("A"),
														m_WiFiTestData.Evm.szAntenna, 
														m_WiFiTestData.Evm.szDataRate,
														m_WiFiTestData.Evm.szActualEVM);	
	}else if(m_eTestType == WIFI_FREQUENCY_ACCURACY){
			if(strcmp(m_WiFiTestData.FreqAccuracy.szAntenna,_T("NONE")) == NULL)
			{
				//Record Result to SFCS
				CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiTxFreqAccuracy-F%s_%s",
					m_WiFiTestData.FreqAccuracy.szFreq,
					m_WiFiTestData.FreqAccuracy.szActualPpm);	
			}else{
				//Record Spec to SFCS
				CCommFunc::ExportTestMessage(cbSfcsSpecMessage, "WiFiTxFrameFreqAccuracy-F%sA%sR%s_M_%s/%s", 
					m_WiFiTestData.FreqAccuracy.szFreq, 
					m_WiFiTestData.FreqAccuracy.szAntenna, 
					m_WiFiTestData.FreqAccuracy.szDataRate, 
					m_WiFiTestData.FreqAccuracy.szPpmHigh,
					m_WiFiTestData.FreqAccuracy.szPpmLow);
				//Record Result to SFCS
				CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiFrameTxFreqAccuracy-F%sA%sR%s_%s",
					m_WiFiTestData.FreqAccuracy.szFreq, 
					m_WiFiTestData.FreqAccuracy.szAntenna, 
					m_WiFiTestData.FreqAccuracy.szDataRate,
					m_WiFiTestData.FreqAccuracy.szActualPpm);	
			}	
	}else if(m_eTestType == WIFI_MASK_TEST){
		//Record Spec to SFCS
		CCommFunc::ExportTestMessage(cbSfcsSpecMessage, "WiFiMaskVerification-F%sA%sR%s_S_%s", 
			m_WiFiTestData.Mask.szFreq, 
			m_WiFiTestData.Mask.szAntenna, 
			m_WiFiTestData.Mask.szDataRate, 
			m_WiFiTestData.Mask.szMaskHigh);
		//Record Result to SFCS
		CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiMaskVerification-F%sA%sR%s_%s",
			m_WiFiTestData.Mask.szFreq, 
			m_WiFiTestData.Mask.szAntenna, 
			m_WiFiTestData.Mask.szDataRate,
			m_WiFiTestData.Mask.szActualMask);
	}else if(m_eTestType == WIFI_RX_SWEEP_TEST){
		//Record Spec to SFCS
		CCommFunc::ExportTestMessage(cbSfcsSpecMessage, "WiFiRxSweepPerVerification-F%sA%sR%s_S_%s", 
			m_WiFiTestData.RxSweep.szFreq, 
			m_WiFiTestData.RxSweep.szAntenna, 
			m_WiFiTestData.RxSweep.szDataRate, 
			m_WiFiTestData.RxSweep.szPerHigh);
		//Record Result to SFCS
		CCommFunc::ExportTestMessage(cbSfcsResultMessage, "WiFiRxSweepPerVerification-F%sA%sR%s_%s",
			m_WiFiTestData.RxSweep.szFreq, 
			m_WiFiTestData.RxSweep.szAntenna, 
			m_WiFiTestData.RxSweep.szDataRate,
			m_WiFiTestData.RxSweep.szFIT);
	}
}
void CART2_Test::DisplayTestLog(funcGetMessage cbListMessage, LPSTR szMessage)
{
	if( (strstr(szMessage,_T("4001 INFO")) != NULL) ||
		(strstr(szMessage,_T("4049 INFO")) != NULL) ||
		(strstr(szMessage,_T("4003 INFO")) != NULL) ||
		(strstr(szMessage,_T("4004 INFO")) != NULL) ||
		(strstr(szMessage,_T("7200 INFO")) != NULL) ||
		(strstr(szMessage,_T("7000 INFO")) != NULL) ||
		(strstr(szMessage,_T("4049 INFO")) != NULL) ||
		(strstr(szMessage,_T("FAIL")) != NULL) ||
		(strstr(szMessage,_T("ERROR")) != NULL) )
		CCommFunc::ExportTestMessage(cbListMessage, szMessage);

	ExportTestLog(szMessage);
	return;
}
void CART2_Test::SeparateData(LPSTR lpszMessage)
{

	CString strKey;	
	char	szSeparate[] = " \t\n\r/" ;
	TCHAR *szToken, *szNextTok;
	int		nAddr,nAddr2;
	TCHAR   szErrKey[XML_DATA_SIZE]={0};
	strKey.Format("%s",lpszMessage);

	if( (strKey.Find(_T("ERROR")) >= 0) && (strKey.Find(_T("2045")) < 0) && (strKey.Find(_T("5013 ERROR No connection to nart")) < 0) ){//ERROR but not (Calibration ERROR	or unload Errror)

		if((strcmp(m_ErrorReport.ErrorCode,_T("T089")) != NULL) && m_ErrorReport.TestResult)
		{
			strCopy(m_ErrorReport.FailedTest, DefineUnknowFailedTest());
			strCopy(m_ErrorReport.ErrorCode,_T("T089"));
			strCopy(m_ErrorReport.ErrorDetail,strKey.GetBuffer());
			strCopy(m_ErrorReport.ErrorMsg,_T("NULL"));		
			m_ErrorReport.TestResult = FALSE;
			m_ErrorReport.DisableSfcsLink = TRUE;
		}

	}
	else if((strKey.Find(_T("ERROR")) >= 0) && (strKey.Find(_T("2047")) < 0)){
		m_eTestType = TEST_RESULT; 
		iTestCount = 0;
	}
	else if(strKey.Find(_T("FAIL")) >= 0){
		if(m_ErrorReport.TestResult)
		{
			strCopy(m_ErrorReport.FailedTest, DefineUnknowFailedTest());
			strCopy(m_ErrorReport.ErrorCode,_T("T089"));
			strCopy(m_ErrorReport.ErrorDetail,strKey.GetBuffer());
			strCopy(m_ErrorReport.ErrorMsg,_T("NULL"));		
			m_ErrorReport.TestResult = FALSE;
			m_ErrorReport.DisableSfcsLink = TRUE;
		}
	}else if(strKey.Find("4001 INFO") >= 0){		
		if(strKey.MakeUpper().Find("CALIBRATION COEFFICIENTS") >= 0)		
			m_eTestType = WIFI_TX_POWER_CALIBRATION;
		else if(strKey.MakeUpper().Find("XTAL CAL RADIO") >= 0 || strKey.MakeUpper().Find("XTAL CAL") >= 0)		
			m_eTestType = WIFI_XTAL_CALIBRATION;
		else if(strKey.MakeUpper().Find("POWER ACCURACY") >= 0)		
			m_eTestType = WIFI_POWER_ACCURACY;
		else if(strKey.MakeUpper().Find("EVM ACCURACY") >= 0)		
			m_eTestType = WIFI_EVM_TEST;
		else if((strKey.MakeUpper().Find("FREQUENCY ACCURACY") >= 0) || (strKey.MakeUpper().Find("FREQUENCY ACURACY") >= 0))
			m_eTestType = WIFI_FREQUENCY_ACCURACY;		
		else if(strKey.MakeUpper().Find("SPECTRAL MASK") >= 0)		
			m_eTestType = WIFI_MASK_TEST;
		else if(strKey.MakeUpper().Find("SENSITIVITY") >= 0 || strKey.MakeUpper().Find("SENS") >= 0)		
			m_eTestType = WIFI_RX_SWEEP_TEST;
		else
			m_eTestType = NOT_DEFINE_TEST;
	}else if( (((nAddr = strKey.Find(_T("7200 INFO $Test.Error = "))) >= 0) || ((nAddr2 = strKey.Find(_T("7200 INFO Test.Error = "))) >= 0)) && (m_ErrorReport.TestResult)){
		CString strErrorCode;
		if(nAddr > 0)
			strErrorCode = strKey.Mid(nAddr+strlen(_T("7200 INFO $Test.Error = "))).Trim();
		else
			strErrorCode = strKey.Mid(nAddr+strlen(_T("7200 INFO Test.Error = "))).Trim();
		if(strcmp(strErrorCode.GetBuffer(),"0") != NULL)
		{	
			if(m_AnalyzeFail)
			{//Analyze fail
				if(::MessageBox(NULL,_T("Anaylze fail, do you want to send to SFCS ?"), _T("ERROR"), MB_YESNO | MB_SETFOREGROUND) == IDYES)
				{
					m_ErrorReport.TestResult = FALSE;
					m_ErrorReport.DisableSfcsLink = FALSE;
					sprintf_s(m_ErrorReport.ErrorCode,sizeof(m_ErrorReport.ErrorCode), "T%03s",strErrorCode.GetBuffer());				
				}else{
					m_ErrorReport.TestResult = FALSE;
					m_ErrorReport.DisableSfcsLink = TRUE;
					strCopy(m_ErrorReport.ErrorCode,_T("T089"));
					strCopy(m_ErrorReport.ErrorDetail,_T("NULL"));
					strCopy(m_ErrorReport.ErrorMsg,_T("Anaylze fail"));
					strCopy(m_ErrorReport.FailedTest,_T("OEM TEST"));
				}
			}else{
				m_ErrorReport.TestResult = FALSE;
				m_ErrorReport.DisableSfcsLink = FALSE;
				sprintf_s(m_ErrorReport.ErrorCode,sizeof(m_ErrorReport.ErrorCode), "T%03s",strErrorCode.GetBuffer());
			}
		}
	}else if(strKey.Find("Done Test") >= 0){
		/*++iTestCount;
		if(iTestCount == g_GlobalInfo.NUMBER_OF_WIFI_INTERFACE || m_ErrorReport.TestResult == FALSE)
		{
		m_eTestType = TEST_RESULT;
		iTestCount = 0;
		}*/
		m_eTestType = TEST_RESULT;
	}



	if(m_eTestType == WIFI_TX_POWER_CALIBRATION)
	{
		if(strKey.Find("4003 INFO") >= 0)	
		{////frequency   txchain    txgain     power     pcorr   voltage      temp	
			m_StartKeyWord = TRUE;
			szToken = strtok_s(lpszMessage, szSeparate, &szNextTok);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szFreq);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szAntenna);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szGain);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szPower);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szPcorr);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szVoltage);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.TxCalibration.szTemperature);
		}
	}else if(m_eTestType == WIFI_POWER_ACCURACY){
		if(strKey.Find("4003 INFO") >= 0)	
		{
			m_StartKeyWord = TRUE;			
			szToken = strtok_s(lpszMessage, szSeparate, &szNextTok);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.PowerAccuracy.szFreq);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.PowerAccuracy.szDataRate);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.PowerAccuracy.szAntenna);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.PowerAccuracy.szExpectPwr);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.PowerAccuracy.szActualPwr);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.PowerAccuracy.szPwrDelta);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, szErrKey);
			if(strcmp(szErrKey,"fail") == NULL) SetErrorMessage();
		}
	}else if(m_eTestType == WIFI_EVM_TEST){
		if(strKey.Find("4049 INFO") >= 0)
			m_WiFiTestData.Evm.bStreamData = (strKey.Find("stream") >= 0)? TRUE : FALSE;
		else if(strKey.Find("4003 INFO") >= 0){
			m_StartKeyWord = TRUE;
			szToken = strtok_s(lpszMessage, szSeparate, &szNextTok);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szFreq);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szDataRate);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szAntenna);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szExpectPwr);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szActualPwr);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szActualEVM);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szExpectEVM);

			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, szErrKey);
			if(strcmp(szErrKey,"fail") == NULL) SetErrorMessage();			
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Evm.szPwrDelta);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, szErrKey);
			if(strcmp(szErrKey,"fail") == NULL) SetErrorMessage();
			if((strlen(m_WiFiTestData.Evm.szActualPwr) > 20) || (strlen(m_WiFiTestData.Evm.szActualEVM) > 20))
				m_AnalyzeFail = TRUE;
		}
	}else if(m_eTestType == WIFI_FREQUENCY_ACCURACY){
		if(strKey.Find("4003 INFO") >= 0)	
		{
			m_StartKeyWord = TRUE;
			szToken = strtok_s(lpszMessage, szSeparate, &szNextTok);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.FreqAccuracy.szFreq);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.FreqAccuracy.szDataRate);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.FreqAccuracy.szAntenna);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.FreqAccuracy.szActualPpm);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.FreqAccuracy.szPpmLow);
			if(!GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.FreqAccuracy.szPpmHigh))
			{// Single Tone
				strCopy(szErrKey,m_WiFiTestData.FreqAccuracy.szPpmLow);
				strCopy(m_WiFiTestData.FreqAccuracy.szDataRate,_T("NONE"));
				strCopy(m_WiFiTestData.FreqAccuracy.szAntenna,_T("NONE"));
				strCopy(m_WiFiTestData.FreqAccuracy.szPpmLow,_T("NONE"));
				strCopy(m_WiFiTestData.FreqAccuracy.szPpmHigh,_T("NONE"));
				if(strcmp(szErrKey,"fail") == NULL) SetErrorMessage();

			}else{//Frame				
				GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, szErrKey);
				if(strcmp(szErrKey,"fail") == NULL) SetErrorMessage();
			}
		}
	}else if(m_eTestType == WIFI_MASK_TEST){
		if(strKey.Find("4003 INFO") >= 0)	
		{			
			m_StartKeyWord = TRUE;
			szToken = strtok_s(lpszMessage, szSeparate, &szNextTok);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Mask.szFreq);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Mask.szDataRate);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Mask.szAntenna);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Mask.szExpectPwr);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Mask.szActualMask);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.Mask.szMaskHigh);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, szErrKey);
			if(strcmp(szErrKey,".") != NULL) SetErrorMessage();
		}
	}else if(m_eTestType == WIFI_RX_SWEEP_TEST){
		if(strKey.Find("4003 INFO") >= 0)	
		{			
			m_StartKeyWord = TRUE;
			szToken = strtok_s(lpszMessage, szSeparate, &szNextTok);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.RxSweep.szFreq);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.RxSweep.szDataRate);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.RxSweep.szAntenna);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.RxSweep.szDIP);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.RxSweep.szFIT);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, m_WiFiTestData.RxSweep.szPerHigh);
			szToken = strtok_s(NULL,szSeparate, &szNextTok);
			GetXmlTestInfo(m_eTestType, szNextTok, szSeparate, szErrKey);
			if(strcmp(szErrKey,"fail") == NULL) SetErrorMessage();

		}
	}	

	if(strstr(lpszMessage,"4004 INFO ----------"))
	{
		m_eTestType = NOT_DEFINE_TEST;
		m_StartKeyWord = FALSE;
	}

	return;
}
BOOL CART2_Test::GetXmlTestInfo(TEST_TYPE eType, LPSTR &szNextTok, LPSTR szKeyName, LPSTR szXmlData)
{	
	TCHAR *szToken;
	CString strKey;

	szToken = strtok_s(NULL, szKeyName, &szNextTok);	
	strKey.Format(_T("%s"), szToken);
	strKey = strKey.Trim();
	sprintf_s(szXmlData, XML_DATA_SIZE, "%s", strKey);

	return (szToken == NULL)? FALSE : TRUE;
}

void CART2_Test::CheckErrorIsDefine()
{
	if(!m_ErrorReport.TestResult)
	{
		if(strcmp(m_ErrorReport.ErrorCode,_T("T001")) == NULL)
			ParserErrorReport("INITIALIZE","NO CARD FOUND","NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T004")) == NULL)
			ParserErrorReport("5G TX CALIBRATION FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T005")) == NULL)
			ParserErrorReport("2G TX CALIBRATION FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T007")) == NULL)
			ParserErrorReport("5G TX PPM FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T008")) == NULL)
			ParserErrorReport("2G TX PPM FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T011")) == NULL)
			ParserErrorReport("5G TX EVM FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T012")) == NULL)
			ParserErrorReport("5G RX SENSITIVITY FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T013")) == NULL)
			ParserErrorReport("2G TX EVM FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T014")) == NULL)
			ParserErrorReport("2G RX SENSITIVITY FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T020")) == NULL)
			ParserErrorReport("5G TX SPECTRAL MASK FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T021")) == NULL)
			ParserErrorReport("2G TX SPECTRAL MASK FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T028")) == NULL)
			ParserErrorReport("5G TX POWER FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T034")) == NULL)
			ParserErrorReport("2G TX POWER FAIL",m_DisplayErrorMsg,"NULL");
		else if(strcmp(m_ErrorReport.ErrorCode,_T("T1109")) == NULL)
			ParserErrorReport("XTAL CALIBRATION FAIL",m_DisplayErrorMsg,"NULL");
		else{
			m_ErrorReport.DisableSfcsLink = TRUE;
		}
	}

	return;
}
LPSTR CART2_Test::DefineUnknowFailedTest()
{
	if(m_eTestType == WIFI_POWER_ACCURACY)
		return _T("WIFI_POWER_ACCURACY");	
	else if(m_eTestType == WIFI_EVM_TEST)
		return _T("WIFI_EVM_TEST");
	else if(m_eTestType == WIFI_FREQUENCY_ACCURACY)
		return _T("WIFI_FREQUENCY_ACCURACY");
	else if(m_eTestType == WIFI_MASK_TEST)
		return _T("WIFI_MASK_TEST");
	else if(m_eTestType == WIFI_RX_SWEEP_TEST)
		return _T("WIFI_RX_SWEEP_TEST");
	else
		return _T("ERROR NOT DEFINE");
}
void CART2_Test::ParserErrorReport(LPSTR szFailTest, LPSTR szErrorMsg, LPSTR szErrorDetail)
{
	strCopy(m_ErrorReport.FailedTest, szFailTest);
	strCopy(m_ErrorReport.ErrorDetail,szErrorDetail);
	strCopy(m_ErrorReport.ErrorMsg,szErrorMsg);		
	return;
}

BOOL CART2_Test::SetErrorMessage()
{

	if(strlen(m_DisplayErrorMsg) == 0)
	{
		switch(m_eTestType)
		{		
		case WIFI_POWER_ACCURACY:
			sprintf_s(m_DisplayErrorMsg,"Tx Power not within limits, [FREQ:%s;RATE:%s;ANT:%s;ACTUAL:%s;EXPECT:%s]",
				m_WiFiTestData.PowerAccuracy.szFreq,
				m_WiFiTestData.PowerAccuracy.szDataRate,
				m_WiFiTestData.PowerAccuracy.szAntenna,
				m_WiFiTestData.PowerAccuracy.szActualPwr,
				m_WiFiTestData.PowerAccuracy.szExpectPwr);
			break;
		case WIFI_EVM_TEST:
			if(m_WiFiTestData.Evm.bStreamData)
			{
				sprintf_s(m_DisplayErrorMsg,"Tx Evm not within limits, [FREQ:%s;RATE:%s;STREAM:%s;EXPECT_PWR:%s;ACTUAL_PWR:%s;EXPECT_EVM:%s;ACTUAL_EVM:%s]",
					m_WiFiTestData.Evm.szFreq,
					m_WiFiTestData.Evm.szDataRate,
					m_WiFiTestData.Evm.szAntenna,
					m_WiFiTestData.Evm.szExpectPwr,
					m_WiFiTestData.Evm.szActualPwr,
					m_WiFiTestData.Evm.szExpectEVM,
					m_WiFiTestData.Evm.szActualEVM);
			}else{
				sprintf_s(m_DisplayErrorMsg,"Tx Evm not within limits, [FREQ:%s;RATE:%s;ANT:%s;EXPECT_PWR:%s;ACTUAL_PWR:%s;EXPECT_EVM:%s;ACTUAL_EVM:%s]",
					m_WiFiTestData.Evm.szFreq,
					m_WiFiTestData.Evm.szDataRate,
					m_WiFiTestData.Evm.szAntenna,
					m_WiFiTestData.Evm.szExpectPwr,
					m_WiFiTestData.Evm.szActualPwr,
					m_WiFiTestData.Evm.szExpectEVM,
					m_WiFiTestData.Evm.szActualEVM);

			}

			break;
		case WIFI_FREQUENCY_ACCURACY:
			sprintf_s(m_DisplayErrorMsg,"Tx Freq accuracy not within limits, [FREQ:%s;RATE:%s;ANT:%s;ACTUAL:%s;HIGH:%s;LOW:%s]",
				m_WiFiTestData.FreqAccuracy.szFreq,
				m_WiFiTestData.FreqAccuracy.szDataRate,
				m_WiFiTestData.FreqAccuracy.szAntenna,
				m_WiFiTestData.FreqAccuracy.szActualPpm,
				m_WiFiTestData.FreqAccuracy.szPpmHigh,
				m_WiFiTestData.FreqAccuracy.szPpmLow);
			break;
		case WIFI_MASK_TEST:	
			sprintf_s(m_DisplayErrorMsg,"Tx spectral mask not within limits, [FREQ:%s;RATE:%s;ANT:%s;PWR:%s;ACTUAL:%s;HIGH:%s]",
				m_WiFiTestData.Mask.szFreq,
				m_WiFiTestData.Mask.szDataRate,
				m_WiFiTestData.Mask.szAntenna,
				m_WiFiTestData.Mask.szExpectPwr,
				m_WiFiTestData.Mask.szActualMask,
				m_WiFiTestData.Mask.szMaskHigh);
			break;
		case WIFI_RX_SWEEP_TEST:
			sprintf_s(m_DisplayErrorMsg,"Rx PER not within limits, [FREQ:%s;RATE:%s;ANT:%s;DIP:%s;FIT:%s;HIGH:%s]",
				m_WiFiTestData.RxSweep.szFreq,
				m_WiFiTestData.RxSweep.szDataRate,
				m_WiFiTestData.RxSweep.szAntenna,
				m_WiFiTestData.RxSweep.szDIP,
				m_WiFiTestData.RxSweep.szFIT,
				m_WiFiTestData.RxSweep.szPerHigh);
			break;

		default:
			return FALSE;
		}//switch(m_eTestType)
	}//if(strlen(m_DisplayErrorMsg) == 0)

	return TRUE;
}