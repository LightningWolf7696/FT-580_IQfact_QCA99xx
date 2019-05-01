#pragma once

#include "../CommFunc.h"


class CART2_Test
{
public:
	CART2_Test(void);
public:
	~CART2_Test(void);

private:
	
	enum INIT_PROCESS
	{
		PROCESS_CONNECT = 0,
		PROCESS_LOAD,		
		PROCESS_SCRIPT_FILE, 
		PROCESS_BOARD_ID,
		PROCESS_REF_ID,
		PROCESSE_MAC
	};

	enum TEST_TYPE
	{
		NOT_DEFINE_TEST = -1,
		WIFI_TX_POWER_CALIBRATION,
		WIFI_XTAL_CALIBRATION,
		WIFI_POWER_ACCURACY, 
		WIFI_EVM_TEST,
		WIFI_FREQUENCY_ACCURACY,
		WIFI_MASK_TEST, 
		WIFI_RX_SWEEP_TEST,
		TEST_RESULT = 99
	};

	typedef struct _XML_TX_POWER_CALIBRATION
	{
		char szFreq[XML_DATA_SIZE];		
		char szAntenna[XML_DATA_SIZE];
		char szGain[XML_DATA_SIZE];
		char szPower[XML_DATA_SIZE];
		char szPcorr[XML_DATA_SIZE];
		char szVoltage[XML_DATA_SIZE];
		char szTemperature[XML_DATA_SIZE];
	}XML_TX_POWER_CALIBRATION;

	typedef struct _XML_DATA_POWER_ACCURACY
	{
		char szFreq[XML_DATA_SIZE];
		char szDataRate[XML_DATA_SIZE];
		char szAntenna[XML_DATA_SIZE];
		char szExpectPwr[XML_DATA_SIZE];
		char szActualPwr[XML_DATA_SIZE];	
		char szPwrDelta[XML_DATA_SIZE];
	}XML_DATA_POWER_ACCURACY;

	typedef struct _XML_DATA_EVM
	{
		char szFreq[XML_DATA_SIZE];
		char szDataRate[XML_DATA_SIZE];		
		char szAntenna[XML_DATA_SIZE];
		char szExpectPwr[XML_DATA_SIZE];
		char szActualPwr[XML_DATA_SIZE];
		char szActualEVM[XML_DATA_SIZE];
		char szExpectEVM[XML_DATA_SIZE];
		char szPwrDelta[XML_DATA_SIZE];
		BOOL bStreamData;
	}XML_DATA_EVM;

	typedef struct _XML_DATA_FREQUENCY_ACCURACY
	{
		char szFreq[XML_DATA_SIZE];
		char szDataRate[XML_DATA_SIZE];
		char szAntenna[XML_DATA_SIZE];
		char szActualPpm[XML_DATA_SIZE];
		char szPpmLow[XML_DATA_SIZE];
		char szPpmHigh[XML_DATA_SIZE];
	}XML_DATA_FREQUENCY_ACCURACY;

	typedef struct _XML_DATA_SPECTRAL_MASK
	{
		char szFreq[XML_DATA_SIZE];
		char szDataRate[XML_DATA_SIZE];
		char szAntenna[XML_DATA_SIZE];
		char szExpectPwr[XML_DATA_SIZE];
		char szActualMask[XML_DATA_SIZE];
		char szMaskHigh[XML_DATA_SIZE];
	}XML_DATA_SPECTRAL_MASK;

	typedef struct _XML_DATA_RXPER_SWEEP
	{
		char szFreq[XML_DATA_SIZE];
		char szDataRate[XML_DATA_SIZE];
		char szAntenna[XML_DATA_SIZE];
		char szDIP[XML_DATA_SIZE];
		char szFIT[XML_DATA_SIZE];
		char szPerHigh[XML_DATA_SIZE];
	}XML_DATA_RXPER_SWEEP;

	typedef struct _WIFI_TEST_DATA
	{		
		XML_TX_POWER_CALIBRATION	TxCalibration;
		XML_DATA_POWER_ACCURACY		PowerAccuracy;
		XML_DATA_EVM				Evm;
		XML_DATA_FREQUENCY_ACCURACY	FreqAccuracy;
		XML_DATA_SPECTRAL_MASK		Mask;
		XML_DATA_RXPER_SWEEP		RxSweep;
	}WIFI_TEST_DATA, *pWIFI_TEST_DATA;




	WIFI_TEST_DATA	m_WiFiTestData;
	ERRORREPORT		m_ErrorReport;	

	TEST_TYPE		m_eTestType;
	double			m_dbTestTimeCost;
	BOOL			m_StartKeyWord;	
	BOOL			m_AnalyzeFail;
	TCHAR			m_DisplayErrorMsg[ERRORMSG_STRING];

	//Kevin add, for art message time out
	double			m_dbArtMsgTimeOut;
	int				iTestCount;
public:
	CStringList		DontCare_Msg;
public:
	TM_RETURN START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport);
	void CLEAN();
private:
	void CollectTestData(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, LPSTR lpszMessage);	
	//BOOL CombineMessage(LPSTR lpszSrcData, LPSTR lpszAvailData, int nAvailLens, LPSTR szKey);
	void SeparateData(LPSTR lpszMessage);
	
	//void CleanErrorReport();
	BOOL StartProcess(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, LPSTR lpszRunInfo);
	BOOL GetXmlTestInfo(TEST_TYPE eType, LPSTR &szNextTok, LPSTR szKeyName, LPSTR szXmlData);
	BOOL CheckTestEquipment(LPSTR lpszRunInfo);
	void CheckErrorIsDefine();
	void ParserErrorReport(LPSTR szFailTest, LPSTR szErrorMsg, LPSTR szErrorDetail);
	BOOL SetErrorMessage();
	void DisplayTestLog(funcGetMessage cbListMessage, LPSTR szMessage);
	LPSTR DefineUnknowFailedTest();
	BOOL StartAutoProcess(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, LPSTR lpszRunInfo);
	BOOL CreateMacFile();
};
