#include "../StdAfx.h"
#include "BtStartFrameModulatedTx.h"

CBtStartFrameModulatedTx::CBtStartFrameModulatedTx(void)
{
}

CBtStartFrameModulatedTx::~CBtStartFrameModulatedTx(void)
{

}

BOOL CBtStartFrameModulatedTx::Start(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	TCHAR szCmd[MAX_PATH];
	TCHAR szMessage[DLL_INFO_SIZE];
	TCHAR szBDAddr[32]={0};
	TCHAR szHoppingMode[8]={0};
	TCHAR szLogicalChannel[8]={0};
	TCHAR szPacketType[8]={0};
	TCHAR szFrequency[8]={0};
	TCHAR szModulationType[8]={0};
	TCHAR szPacketLength[8]={0};

	if(strcmp(DutParam.PACKET_TYPE,"LE") == NULL)
	{
		//Frequency
		sprintf_s(szFrequency,sizeof(szFrequency),"%02x",(int)(DutParam.CHANNEL/2));
		//Packet Length
		strcpy_s(szPacketLength,sizeof(szPacketLength),"25");
		//Packet Payload
		if(strcmp(DutParam.PATTERN_TYPE,"ALTERING_1BIT") == 0)
			strcpy_s(szModulationType,sizeof(szModulationType),"08");
		else if(strcmp(DutParam.PATTERN_TYPE,"ALTERING_4BIT") == 0)
			strcpy_s(szModulationType,sizeof(szModulationType),"07");
		else if(strcmp(DutParam.PATTERN_TYPE,"FLOOD_0") == 0)
			strcpy_s(szModulationType,sizeof(szModulationType),"06");
		else if(strcmp(DutParam.PATTERN_TYPE,"FLOOD_1") == 0)
			strcpy_s(szModulationType,sizeof(szModulationType),"04");		
		else//PRBS9
			strcpy_s(szModulationType,sizeof(szModulationType),"00");

		//hcitool cmd 0x3f 0x001e 00 25 00
		sprintf_s(szCmd,MAX_PATH,"hcitool cmd %s %s %s %s %s",
			_T("0x08"),
			_T("0x001e"),
			szFrequency,
			szPacketLength,
			szModulationType
			);
		
		if(g_GlobalInfo.IsConsole)
		{// Jerry Add
			if(!CCommFunc::SerialPortQueryData(szCmd, Test_Info.szKeyWord, Test_Info.TimeOut, lpszRunInfo))
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,lpszRunInfo,Test_Info.TimeOut))
				return FALSE;
			else
				return TRUE;
		}
	}else{
		//Set CFG FREQ
		if(!SetCFG_FREQ(DutParam, lpszRunInfo)) return FALSE;
		//Set CFG PKT
		if(!SetCFG_PKT(DutParam, lpszRunInfo)) return FALSE;
		//Set CFG TX POWER
		if(!SetCFG_TX_POWER(DutParam, lpszRunInfo)) return FALSE;
		//Start Tx Packet Transmit
		if(!StartTxPacketTransmit(DutParam, lpszRunInfo)) return FALSE;
	}// TX_POWER CMD

	return TRUE;
}

BOOL CBtStartFrameModulatedTx::SetCFG_FREQ(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	USHORT TxRxInterval=0;
	TCHAR szCmd[MAX_PATH]={0};
	CString strValue;
	static TCHAR PrevPacketType[10]={0};
	TCHAR CurrPacketType[10]={0};

	if((strcmp(DutParam.PACKET_TYPE,"DH1") == NULL) || (strcmp(DutParam.PACKET_TYPE,"2DH1") == NULL) || (strcmp(DutParam.PACKET_TYPE,"3DH1") == NULL)){
		TxRxInterval = 1250;
		strCopy(CurrPacketType,"1M");
	}else if((strcmp(DutParam.PACKET_TYPE,"DH3") == NULL) || (strcmp(DutParam.PACKET_TYPE,"2DH3") == NULL) || (strcmp(DutParam.PACKET_TYPE,"3DH3") == NULL)){
		TxRxInterval = 3750;
		strCopy(CurrPacketType,"2M");
	}else if((strcmp(DutParam.PACKET_TYPE,"DH5") == NULL) || (strcmp(DutParam.PACKET_TYPE,"2DH5") == NULL) || (strcmp(DutParam.PACKET_TYPE,"3DH5") == NULL)){
		TxRxInterval = 6250;
		strCopy(CurrPacketType,"3M");
	}else{
		sprintf_s(lpszRunInfo, DLL_INFO_SIZE, "Error define to PACKET_TYPE name [%s]",DutParam.PACKET_TYPE);
		return FALSE;
	}
	if(strcmp(PrevPacketType, CurrPacketType) == NULL)
		return TRUE;
	else
		strCopy(PrevPacketType, CurrPacketType);

	strValue.Format("%04x",TxRxInterval);
	strValue = "0x" + strValue.Right(2) + " " + "0x" + strValue.Left(2);

	//hcitool cmd 0x3f 0x00 0xc2 0x02 0x00 0x09 0x00 0x12 0x34 0x04 0x50 0x00 0x00 0x16 0x00 0xE2 0x04 0x53 0x07 0x01 0x00 0x00 0x00 0x00 0x00
	sprintf_s(szCmd, MAX_PATH, "hcitool cmd %s 0x00 0x00 0x16 0x00 %s 0x53 0x07 0x01 0x00 0x00 0x00 0x00 0x00",TX_PREFIX, strValue.GetBuffer());
	
	if(g_GlobalInfo.IsConsole)
	{
		if(!CCommFunc::SerialPortQueryData(szCmd, Test_Info.szKeyWord, Test_Info.TimeOut, lpszRunInfo))
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,lpszRunInfo,Test_Info.TimeOut))
			return FALSE;
		else
			return TRUE;
	}
}

BOOL CBtStartFrameModulatedTx::SetCFG_PKT(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	USHORT PacketCode=0, MaxPacketLength=0;
	TCHAR szCmd[MAX_PATH]={0};
	CString strValue[2];
	static TCHAR PrevPacketType[10]={0};

	if(strcmp(DutParam.PACKET_TYPE,"DH1") == NULL){
		PacketCode = 4;
		MaxPacketLength = 27;
	}else if(strcmp(DutParam.PACKET_TYPE,"DH3") == NULL){
		PacketCode = 11;
		MaxPacketLength = 183;
	}else if(strcmp(DutParam.PACKET_TYPE,"DH5") == NULL){
		PacketCode = 15;
		MaxPacketLength = 339;
	}else if(strcmp(DutParam.PACKET_TYPE,"2DH1") == NULL){
		PacketCode = 20;
		MaxPacketLength = 54;
	}else if(strcmp(DutParam.PACKET_TYPE,"2DH3") == NULL){
		PacketCode = 26;
		MaxPacketLength = 367;
	}else if(strcmp(DutParam.PACKET_TYPE,"2DH5") == NULL){
		PacketCode = 30;
		MaxPacketLength = 679;
	}else if(strcmp(DutParam.PACKET_TYPE,"3DH1") == NULL){
		PacketCode = 24;
		MaxPacketLength = 83;
	}else if(strcmp(DutParam.PACKET_TYPE,"3DH3") == NULL){
		PacketCode = 27;
		MaxPacketLength = 552;
	}else if(strcmp(DutParam.PACKET_TYPE,"3DH5") == NULL){
		PacketCode = 31;
		MaxPacketLength = 1021;
	}else{
		sprintf_s(lpszRunInfo, DLL_INFO_SIZE, "Error define to PACKET_TYPE name [%s]",DutParam.PACKET_TYPE);
		return FALSE;
	}

	if(strcmp(PrevPacketType, DutParam.PACKET_TYPE) == NULL)
		return TRUE;
	else
		strCopy(PrevPacketType, DutParam.PACKET_TYPE);

	strValue[0].Format("%04x",PacketCode);
	strValue[0] = "0x" + strValue[0].Right(2) + " " + "0x" + strValue[0].Left(2);

	strValue[1].Format("%04x",MaxPacketLength);
	strValue[1] = "0x" + strValue[1].Right(2) + " " + "0x" + strValue[1].Left(2);

	//hcitool cmd 0x3f 0x00 0xc2 0x02 0x00 0x09 0x00 0x12 0x34 0x04 0x50 0x00 0x00 0x17 0x00 0x04 0x00 0x1B 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	sprintf_s(szCmd, MAX_PATH, "hcitool cmd %s 0x00 0x00 0x17 0x00 %s %s 0x00 0x00 0x00 0x00 0x00 0x00", TX_PREFIX, strValue[0].GetBuffer(), strValue[1].GetBuffer());
	
	if(g_GlobalInfo.IsConsole)
	{
		if(!CCommFunc::SerialPortQueryData(szCmd, Test_Info.szKeyWord, Test_Info.TimeOut, lpszRunInfo))
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,lpszRunInfo,Test_Info.TimeOut))
			return FALSE;
		else
			return TRUE;
	}
}

BOOL CBtStartFrameModulatedTx::SetCFG_TX_POWER(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	TCHAR szCmd[MAX_PATH]={0};
	CString strValue;
	static short PrevTxPower=DATA_VAL_INIT;

	/*if(DutParam.TX_POWER_TARGET == PrevTxPower)
	return TRUE;
	else*/
	PrevTxPower = (short)DutParam.TX_POWER_TARGET;

	strValue.Format("%04hx", PrevTxPower);
	strValue = "0x" + strValue.Right(2) + " " + "0x" + strValue.Left(2);

	//hcitool cmd 0x3f 0x00 0xc2 0x02 0x00 0x09 0x00 0x12 0x34 0x04 0x50 0x00 0x00 0x34 0x00 0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	sprintf_s(szCmd, MAX_PATH, "hcitool cmd %s 0x00 0x00 0x34 0x00 %s 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00",TX_PREFIX, strValue.GetBuffer());
	
	if(g_GlobalInfo.IsConsole)
	{
		if(!CCommFunc::SerialPortQueryData(szCmd, Test_Info.szKeyWord, Test_Info.TimeOut, lpszRunInfo))
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,lpszRunInfo,Test_Info.TimeOut))
			return FALSE;
		else
			return TRUE;
	}
}

BOOL CBtStartFrameModulatedTx::StartTxPacketTransmit(BT_DUT_PARAM DutParam, LPSTR lpszRunInfo)
{
	USHORT PacketCode=0, MaxPacketLength=0;
	TCHAR szCmd[MAX_PATH]={0};
	CString strValue[2];
	USHORT uFreq;
	if(strcmp(DutParam.PATTERN_TYPE,"PRBS9") == NULL){
		strValue[0] = "0x04 0x00";
	}else if(strcmp(DutParam.PATTERN_TYPE,"ALTERING_1BIT") == NULL){
		strValue[0] = "0x06 0x00";
	}else if(strcmp(DutParam.PATTERN_TYPE,"ALTERING_4BIT") == NULL){
		strValue[0] = "0x07 0x00";
	}

	//0x0962=Freq(2402), 0x0989=Freq(2441)
	uFreq = DutParam.CHANNEL + 0x0962;

	strValue[1].Format("%04x",uFreq);
	strValue[1] = "0x" + strValue[1].Right(2) + " " + "0x" + strValue[1].Left(2);

	//PRBS9-2402
	//hcitool cmd 0x3f 0x00 0xc2 0x02 0x00 0x09 0x00 0x12 0x34 0x04 0x50 0x00 0x00 0x04 0x00 0x62 0x09 0x32 0xFF 0x00 0x00 0x00 0x00 0x00 0x00
	sprintf_s(szCmd, MAX_PATH, "hcitool cmd %s 0x00 0x00 %s %s 0x32 0xFF 0x00 0x00 0x00 0x00 0x00 0x00",TX_PREFIX, strValue[0].GetBuffer(), strValue[1].GetBuffer());
	
	if(g_GlobalInfo.IsConsole)
	{
		if(!CCommFunc::SerialPortQueryData(szCmd, Test_Info.szKeyWord, Test_Info.TimeOut, lpszRunInfo))
			return FALSE;
		else
			return TRUE;
	}
	else
	{
		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,Test_Info.szKeyWord,lpszRunInfo,Test_Info.TimeOut))
			return FALSE;
		else
			return TRUE;
	}
}