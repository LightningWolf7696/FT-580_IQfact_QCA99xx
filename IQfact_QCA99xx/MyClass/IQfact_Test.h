#pragma once

#include "../CommFunc.h"


class CIQfact_Test
{
public:
	CIQfact_Test(void);
public:
	~CIQfact_Test(void);

private:
	//#define XML_DATA_SIZE 16
	enum TEST_TYPE
	{
		WIFI_OTHER_TEST = -2,
		NOT_DEFINE_TEST = -1,		
		WIFI_EVM_TEST = 0,
		WIFI_MASK_TEST = 1,
		WIFI_RXPER_TEST = 2,
		WIFI_TX_CAL =3,
		WIFI_RX_SWEEP_TEST = 4,
	
		TEST_RESULT = 99
	};

	typedef enum _ANTENNA_PATH{
		ANTENNA_A 	= 0x01,
		ANTENNA_B 		,
		ANTENNA_AB		,
		ANTENNA_C		,
		ANTENNA_AC		,
		ANTENNA_BC		,
		ANTENNA_ABC		,
		ANTENNA_D		,
		ANTENNA_AD		,
		ANTENNA_BD		,
		ANTENNA_ABD		,
		ANTENNA_CD		,
		ANTENNA_ACD		,
		ANTENNA_BCD		,
		ANTENNA_ABCD	
	} ANTENNA_PATH;

	typedef struct _XML_ANTENNA
	{
		char A[XML_DATA_SIZE];
		char B[XML_DATA_SIZE];
		char C[XML_DATA_SIZE];
		char D[XML_DATA_SIZE];
	}XML_ANTENNA;

	typedef struct _XML_DATA
	{
		char szFreq[XML_DATA_SIZE];
		char szDataRate[XML_DATA_SIZE];
		char szBandWidth[XML_DATA_SIZE];
		char szExpectPwr[XML_DATA_SIZE];
		char szActualPwr[XML_DATA_SIZE];
		char szPPM[XML_DATA_SIZE];
		char szEvm[XML_DATA_SIZE];
		char szMaskViolationPercent[XML_DATA_SIZE];
		char szRxPER[XML_DATA_SIZE];
		char szStartRxPER[XML_DATA_SIZE];
		char szStopRxPER[XML_DATA_SIZE];
		XML_ANTENNA	Antenna;
	}XML_DATA;

	typedef struct _TEST_INFO{
		TCHAR	Test_Item[32];
		TCHAR   Dut_Value[32];
		TCHAR   Lowerlimit[32];
		TCHAR   Upperlmit[32];
		TCHAR	Uint[32];
		TCHAR   Result[32];
		TCHAR   Test_Time[32];
		//cal
		TCHAR	Triggerlevel[6];
		TCHAR	Tx_Power_Goal[32];
		TCHAR	Measured_Avg_Power[32];
		TCHAR	Gain[6];
		TCHAR	Freq[6];
		TCHAR	txChainMask[3];
	}RF_INFO, *PRF_INFO;
	

	CString			Xml_Data_All;
	CString			Sfcs_Result_Data;
	CString			Sfcs_Spec_Data;

	RF_INFO			Rf_Info;
	XML_DATA		m_Xml_Data;
	ERRORREPORT		m_ErrorReport;	


	TEST_TYPE		m_eTestType;
	double			m_dbTestTimeCost;
	BOOL			m_FinishedKeyWord;
	TCHAR			m_szCurrTestInfo[64];
	BOOL			Init;
	//CStdioFile file;
public:
	TM_RETURN START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt,BOOL bRetry);
	void CLEAN();
public://iqfact function
	void AnalysisTestDatav2(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, LPSTR lpszMessage);
	//BOOL CombineMessage(LPSTR lpszSrcData, LPSTR lpszAvailData, int nAvailLens, LPSTR szKey);
	CString GetAntennaQuantity(XML_ANTENNA Antenna);
	CString AddXmlHeader(LPSTR Header,LPSTR Data);
	CString AddXmlTestCase(LPSTR TestCaseName,LPSTR Data);
	CString AntennaToXml(XML_ANTENNA* Antenna,LPSTR Action);
	void AddSfcsHeaderSpec(XML_DATA Xml,CString &Sfcs_Spec,CString &Sfcs_Result);
	void AddSfcsHeaderResult(XML_DATA Xml,CString &Sfcs_Spec,CString &Sfcs_Result);
	void ParserfuncGetMessage(funcGetMessage cbfun,LPSTR Data,LPSTR Fixed);
public://art function





};
