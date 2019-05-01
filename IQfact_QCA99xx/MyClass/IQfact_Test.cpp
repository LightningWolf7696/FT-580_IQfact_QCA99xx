#include "../StdAfx.h"
#include "IQfact_Test.h"
#include <io.h>

#define szSeparateKey  "\n"
#define LINE_MAX_STRING 260
CIQfact_Test::CIQfact_Test(void)
{
}

CIQfact_Test::~CIQfact_Test(void)
{

}

void CIQfact_Test::CLEAN()
{
	zeroIt(Rf_Info);zeroIt(m_Xml_Data);zeroIt(Test_Info.szScript_Name);
	Init=FALSE;m_FinishedKeyWord=FALSE;
	Xml_Data_All="";Sfcs_Result_Data="";Sfcs_Spec_Data="";

	m_ErrorReport.DisableSfcsLink = FALSE;
	strCopy(m_ErrorReport.ErrorCode,_T(""));
	strCopy(m_ErrorReport.ErrorDetail,_T("No Error"));
	strCopy(m_ErrorReport.ErrorMsg,_T("No Error"));
	strCopy(m_ErrorReport.FailedTest,_T("No Error"));
	m_ErrorReport.TestResult = TRUE;
	return;
}



TM_RETURN CIQfact_Test::START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt,BOOL bRetry)
{
	TM_RETURN res;
	m_eTestType =NOT_DEFINE_TEST;
	TCHAR szTemp[MAX_PATH];zeroIt(szTemp);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	TCHAR szPipeBuf[CONSOLE_MAX_LENGTH];zeroIt(szPipeBuf);
	TCHAR szStrloss[CONSOLE_MAX_LENGTH];zeroIt(szStrloss);
	CString szConSoleMsg,szLogBuf;
	int n =0;
	sprintf_s(szTemp,sizeof(szTemp),"OEM\\%s",g_GlobalInfo.CUSTOMER);


	if(bRetry && strstr(m_ErrorReport.ErrorCode,"T001")!=NULL ) // try to init
	{
		Xml_Data_All="";Sfcs_Result_Data="";Sfcs_Spec_Data="";
		if(!ProcessKill("IQrun_Console.exe",szMessage)){
			res=TM_RETURN_ERROR;
			goto FinishTest;
		}

		CCommFunc::OutputMsgToBoth(cbListMessage,"Try to ReStart Service");
		for (int i=0;i<5;i++){
			if(!PipeLineCleanData(g_GlobalInfo.Pipe_OEM_ID,szMessage)){
				res=TM_RETURN_ERROR;
				goto FinishTest;
			}
			Sleep(10);
			CCommFunc::PipeLineMsgClear(g_GlobalInfo.Pipe_OEM_ID);
		}//for loop

		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,"/etc/init.d/qcmbr restart","#",szMessage,30))
		{
			res=TM_RETURN_FAIL;
			strCopy(m_ErrorReport.ErrorCode,_T("T001"));
			goto FinishTest;
		}
	}
	else if(strlen(m_ErrorReport.ErrorCode)>0)
	{
		ExportTestLog("Error Code = %s,Do not Retry",m_ErrorReport.ErrorCode);
		goto FinishTest;
	}



	if (!CCommFunc::PipeCalibration(g_GlobalInfo.Pipe_PROG_ID,szTemp,szMessage)){// path calibration
		res= TM_RETURN_ERROR;
		goto FinishTest;
	}
	else{//start to run test
		zeroIt(szTemp);
		zeroIt(m_ErrorReport);// clear Error Code
		sprintf_s(szPipeBuf,sizeof(szPipeBuf),"IQrun_Console.exe -run %s\n",Test_Info.szScript_Name);//run IQfact cmd 
		PipeLineWriteData(g_GlobalInfo.Pipe_PROG_ID,szPipeBuf,szMessage);
		do {
			zeroIt(szPipeBuf);
			if (!PipeLineReadData(g_GlobalInfo.Pipe_PROG_ID,szPipeBuf,sizeof(szPipeBuf),szMessage)){
				res=TM_RETURN_ERROR;
				goto FinishTest;
			}
			szConSoleMsg=szPipeBuf;
			if(strlen(szStrloss)>0){
				szConSoleMsg=szStrloss+szConSoleMsg;
			}
			while(1){
				if(!CCommFunc::CombineMessage(szConSoleMsg.GetBuffer(), szTemp, sizeof(szTemp), szSeparateKey)){//szTemp=analysis data, szConSoleMsg=wait analysis data
					//if szConSoleMsg.GetBuffer() not end of "\n"
					zeroIt(szStrloss);
					strCopy(szStrloss,szConSoleMsg.GetBuffer());
					break;
				}else{
					szLogBuf=szTemp;
					szLogBuf.Replace("%","Percent");
					szLogBuf.Trim();
					ExportTestLog(szLogBuf);
					//need to data proc
					AnalysisTestDatav2(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, szLogBuf.GetBuffer());

				}
			}//while(1)

		} while (strstr(szTemp,"Please")==NULL);//loop to find please remove dut ...
	}
	

	//CStdioFile file;//Test Analysis data
	//file.Open("C:\\backup.LOG",CFile::modeRead);
	//while(file.ReadString(szConSoleMsg)){
	//AnalysisTestDatav2(cbListMessage, cbXmlMessage, cbSfcsSpecMessage, cbSfcsResultMessage, szConSoleMsg.GetBuffer());
	//}
	//file.Close();



FinishTest:
	if(strstr(m_ErrorReport.ErrorCode,"T089")!=NULL || res==TM_RETURN_ERROR){
		res=TM_RETURN_ERROR;
		strCopy(m_ErrorReport.ErrorCode,_T("T089"));
		m_ErrorReport.TestResult=FALSE;
		m_ErrorReport.DisableSfcsLink = TRUE;
	}else if(strlen(m_ErrorReport.ErrorCode)>0){
		res=TM_RETURN_FAIL;
		m_ErrorReport.TestResult=FALSE;
		m_ErrorReport.DisableSfcsLink = FALSE;
	}else{
		m_ErrorReport.TestResult=TRUE;
		m_ErrorReport.DisableSfcsLink = FALSE;
		res=TM_RETURN_PASS;
	}

	if( (nRetryCnt >=0 && res==TM_RETURN_PASS)||(nRetryCnt==0 && res==TM_RETURN_FAIL) )
	{
		ParserfuncGetMessage(cbXmlMessage,Xml_Data_All.GetBuffer(),"\n");//XML format
		ParserfuncGetMessage(cbSfcsSpecMessage,Sfcs_Spec_Data.GetBuffer(),"@");//SFCS format
		ParserfuncGetMessage(cbSfcsResultMessage,Sfcs_Result_Data.GetBuffer(),"@");//SFCS format
	}
	ErrorReport=m_ErrorReport;
	return res;
}
void CIQfact_Test::AnalysisTestDatav2(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, LPSTR lpszMessage)
{
	CString str=lpszMessage;
	CString Title="*************************************** ";
	CString tmp;
	TCHAR szSktmp[100000];//Jerry Add
	TCHAR szRktmp[100000];//Jerry Add
	if(strstr(str.GetBuffer(),"F A I L")!=NULL){
		m_FinishedKeyWord=TRUE;
	}
	else if(m_FinishedKeyWord){
		if(strstr(str.GetBuffer(),"[Failed]")!=NULL){//	Failed Process
			m_FinishedKeyWord=FALSE;
			if(strstr(str.GetBuffer(),"INSERT_DUT")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "INSERT_DUT");
				strCopy(m_ErrorReport.ErrorCode,_T("T001"));
			}
			else if (strstr(str.GetBuffer(),"REMOVE_DUT")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "REMOVE_DUT");
				strCopy(m_ErrorReport.ErrorCode,_T("T001"));
			}
			else if(strstr(str.GetBuffer(),"INITIALIZE_DUT")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "INITIALIZE_DUT");
				strCopy(m_ErrorReport.ErrorCode,_T("T001"));
			}
			else if(strstr(str.GetBuffer(),"XTAL_CALIBRATION")!=NULL){
				//str="T189@"+CCommFunc::TraceStringMid(str.GetBuffer(),".",":");
				//str.Replace(" ","-");
				//str.Replace("_","-");
				//strCopy(m_ErrorReport.FailedTest, "XTAL_CALIBRATION");
				//strCopy(m_ErrorReport.ErrorCode,str.GetBuffer());
				strCopy(m_ErrorReport.FailedTest, "XTAL_CALIBRATION");
				strCopy(m_ErrorReport.ErrorCode,_T("T1109"));

			}
			else if(strstr(str.GetBuffer(),"TX_VERIFY_SPECTRUM")!=NULL){//Jerry Add
				strCopy(m_ErrorReport.FailedTest, "TX_VERIFY_SPECTRUM");
				strCopy(m_ErrorReport.ErrorCode,_T("T220"));
			}
			else if(strstr(str.GetBuffer(),"TX_CALIBRATION")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "TX_CALIBRATION");
				strCopy(m_ErrorReport.ErrorCode,_T("T189"));
			}
            else if(strstr(str.GetBuffer(),"RX_CALIBRATION")!=NULL){
                strCopy(m_ErrorReport.FailedTest, "RX_CALIBRATION");
                strCopy(m_ErrorReport.ErrorCode,_T("T189"));
            }
			else if (strstr(str.GetBuffer(),"RX_VERIFY_PER")!=NULL){
				//str="T222@"+CCommFunc::TraceStringMid(str.GetBuffer(),".",":");
				//str.Replace(" ","-");
				//str.Replace("_","-");
				//strCopy(m_ErrorReport.FailedTest, "RX_VERIFY_PER");
				//strCopy(m_ErrorReport.ErrorCode,str.GetBuffer());
				strCopy(m_ErrorReport.FailedTest, "RX_VERIFY_PER");
				strCopy(m_ErrorReport.ErrorCode,_T("T222"));
			}
			else if (strstr(str.GetBuffer(),"TX_VERIFY_EVM")!=NULL){
				//str="T214@"+CCommFunc::TraceStringMid(str.GetBuffer(),".",":");
				//str.Replace(" ","-");
				//str.Replace("_","-");
				//strCopy(m_ErrorReport.FailedTest, "TX_VERIFY_EVM");
				//strCopy(m_ErrorReport.ErrorCode,str.GetBuffer());
				//strCopy(m_ErrorReport.FailedTest, "TX_VERIFY_EVM");
				//strCopy(m_ErrorReport.ErrorCode,_T("T214"));

			}
			else if (strstr(str.GetBuffer(),"TX_VERIFY_MASK")!=NULL){
				//str="T021@"+CCommFunc::TraceStringMid(str.GetBuffer(),".",":");
				//str.Replace(" ","-");
				//str.Replace("_","-");
				//strCopy(m_ErrorReport.FailedTest, "TX_VERIFY_MASK");
				//strCopy(m_ErrorReport.ErrorCode,str.GetBuffer());
				strCopy(m_ErrorReport.FailedTest, "TX_VERIFY_MASK");
				strCopy(m_ErrorReport.ErrorCode,_T("T021"));

			}
			else if (strstr(str.GetBuffer(),"CONNECT_IQ_TESTER")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "CONNECT_IQ_TESTER");
				strCopy(m_ErrorReport.ErrorCode,_T("T089"));
			}
			else if (strstr(str.GetBuffer(),"DISCONNECT_IQ_TESTER")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "CONNECT_IQ_TESTER");
				strCopy(m_ErrorReport.ErrorCode,_T("T089"));
			}
			else if (strstr(str.GetBuffer(),"WRITE_MAC_ADDRESS")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "WRITE_MAC_ADDRESS");
				strCopy(m_ErrorReport.ErrorCode,_T("T772"));
			}
			else if (strstr(str.GetBuffer(),"FINALIZE_EEPROM")!=NULL){
				strCopy(m_ErrorReport.FailedTest, "FINALIZE_EEPROM");
				strCopy(m_ErrorReport.ErrorCode,_T("T156"));
			}

			strCopy(m_ErrorReport.ErrorMsg,CCommFunc::TraceStringMid(str.GetBuffer(),"",":"));
		}//find fail keyword
	}
	else if(strstr(str.GetBuffer(),"__________")!=NULL){//Find Title
		if(strstr(str.GetBuffer(),"RX_VERIFY_PER")!=NULL){
			m_eTestType=WIFI_RXPER_TEST;
			Xml_Data_All=Xml_Data_All+"<TestCase name=\"RX_VERIFY_PER\">\n";
			Xml_Data_All=Xml_Data_All+"<Data>\n";
		}
		else if(strstr(str.GetBuffer(),"TX_VERIFY_MASK")!=NULL){
			m_eTestType=WIFI_MASK_TEST;
			Xml_Data_All=Xml_Data_All+"<TestCase name=\"TX_VERIFY_MASK\">\n";
			Xml_Data_All=Xml_Data_All+"<Data>\n";
		}
		else if(strstr(str.GetBuffer(),"TX_VERIFY_EVM")!=NULL){
			m_eTestType=WIFI_EVM_TEST;
			Xml_Data_All=Xml_Data_All+"<TestCase name=\"TX_VERIFY_EVM\">\n";
			Xml_Data_All=Xml_Data_All+"<Data>\n";
		}
		else if(strstr(str.GetBuffer(),"TX_CALIBRATION")!=NULL){
			m_eTestType=WIFI_TX_CAL;
			Xml_Data_All=Xml_Data_All+"<TestCase name=\"TX_CALIBRATION\">\n";
			Xml_Data_All=Xml_Data_All+"<Data>\n";
		}
		else{
			m_eTestType=WIFI_OTHER_TEST;
		}
		str=str.Mid(0,str.Find("__"));
		CCommFunc::ExportTestMessage(cbListMessage,Title+str+Title);
	}
	else if(strstr(str.GetBuffer(),"SKIP_ON_FAIL")!=NULL || strstr(str.GetBuffer(),"ALWAYS_SKIP")!=NULL){
		//need to clear
		CCommFunc::ExportTestMessage(cbListMessage,str.GetBuffer());
		CCommFunc::ExportTestMessage(cbListMessage," ");

		if (m_eTestType>=WIFI_EVM_TEST && m_eTestType<=WIFI_TX_CAL)
		{
			Xml_Data_All=Xml_Data_All+"</Data>\n";
			Xml_Data_All=Xml_Data_All+"</TestCase>\n";
		}

		memset(&Rf_Info,0x00,sizeof(Rf_Info));
		memset(&m_Xml_Data,0x00,sizeof(XML_DATA));
		m_eTestType=NOT_DEFINE_TEST;
		Init=FALSE;
	}
	else if(m_eTestType==WIFI_TX_CAL){//Analysis TX_CAL
		if (!Init){
			CCommFunc::ExportTestMessage(cbListMessage,CCommFunc::StrAddSpeace("Trigger level",30)+
				CCommFunc::StrAddSpeace("Tx_Power_Goal",20)+CCommFunc::StrAddSpeace("Measured_Avg_Power",20)+
				CCommFunc::StrAddSpeace("Gain",20)+CCommFunc::StrAddSpeace("Freq",20)+
				CCommFunc::StrAddSpeace("txChainMask",20));
			Init=TRUE;
		}
		else if(strstr(str,"Trigger level")!=NULL){

			sprintf_s(Rf_Info.Triggerlevel,sizeof(Rf_Info.Triggerlevel),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"Trigger level:",","));
			sprintf_s(Rf_Info.Tx_Power_Goal,sizeof(Rf_Info.Tx_Power_Goal),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"Tx_Power_Goal:",","));
			sprintf_s(Rf_Info.Measured_Avg_Power,sizeof(Rf_Info.Measured_Avg_Power),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"Measured_Avg_Power:",","));
			sprintf_s(Rf_Info.Gain,sizeof(Rf_Info.Gain),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"Gain:",","));
			sprintf_s(Rf_Info.Freq,sizeof(Rf_Info.Freq),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"Freq:",","));
			sprintf_s(Rf_Info.txChainMask,sizeof(Rf_Info.txChainMask),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"txChainMask:",""));

			tmp=tmp+CCommFunc::StrAddSpeace(Rf_Info.Triggerlevel,30)+
				CCommFunc::StrAddSpeace(Rf_Info.Tx_Power_Goal,20)+
				CCommFunc::StrAddSpeace(Rf_Info.Measured_Avg_Power,20)+
				CCommFunc::StrAddSpeace(Rf_Info.Gain,20)+
				CCommFunc::StrAddSpeace(Rf_Info.Freq,20)+
				CCommFunc::StrAddSpeace(Rf_Info.txChainMask,20);
			CCommFunc::ExportTestMessage(cbListMessage,tmp);	
			//xml
			sprintf_s(szSktmp,sizeof(szSktmp),"WiFiTxPwrCal-F%s-A%s-G%s-P_S_%s/",
				Rf_Info.Freq,
				Rf_Info.txChainMask,
				Rf_Info.Gain,
				Rf_Info.Tx_Power_Goal);//Jerry Add
			ParserfuncGetMessage(cbSfcsSpecMessage,szSktmp,"@");//Jerry Add
			sprintf_s(szRktmp,sizeof(szRktmp),"WiFiTxPwrCal-F%s-A%s-G%s-P_%s",
				Rf_Info.Freq,
				Rf_Info.txChainMask,
				Rf_Info.Gain,
				Rf_Info.Measured_Avg_Power);//Jerry Add
			ParserfuncGetMessage(cbSfcsResultMessage,szRktmp,"@");
			Xml_Data_All=Xml_Data_All+AddXmlHeader("Triggerlevel",Rf_Info.Triggerlevel)+AddXmlHeader("Tx_Power_Goal",Rf_Info.Tx_Power_Goal)+AddXmlHeader("Measured_Avg_Power",Rf_Info.Measured_Avg_Power)
				+AddXmlHeader("Gain",Rf_Info.Gain)+AddXmlHeader("Freq",Rf_Info.Freq)+AddXmlHeader("txChainMask",Rf_Info.txChainMask);



		}
		else if(strstr(str,"ERROR_MESSAGE")!=NULL){// clear 
			CCommFunc::ExportTestMessage(cbListMessage, str+" "+Rf_Info.Test_Time);	
			CCommFunc::ExportTestMessage(cbListMessage," ");
			memset(&Rf_Info,0x00,sizeof(Rf_Info));
			m_eTestType=NOT_DEFINE_TEST;
			Init=FALSE;
			Xml_Data_All=Xml_Data_All+"</Data>\n";
			Xml_Data_All=Xml_Data_All+"</TestCase>\n";
		}
	}
	else if (m_eTestType>=WIFI_EVM_TEST && m_eTestType<=WIFI_RXPER_TEST){// Analysis TX_EVM and RX_PER
		if(strstr(str,"(")!=NULL && strstr(str,")")!=NULL && strstr(str,",")!=NULL){
			sprintf_s(Rf_Info.Lowerlimit,sizeof(Rf_Info.Lowerlimit),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"(",",").GetBuffer());
			sprintf_s(Rf_Info.Upperlmit,sizeof(Rf_Info.Upperlmit),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),",",")").GetBuffer());

			if(strlen(Rf_Info.Upperlmit)>0 || strlen(Rf_Info.Lowerlimit)>0){//RD give spec or IEEE Standard
				if (!Init){
					Init=TRUE;
					CCommFunc::ExportTestMessage(cbListMessage,CCommFunc::StrAddSpeace("TestItem",30)+
						CCommFunc::StrAddSpeace("Dut_Values",20)+CCommFunc::StrAddSpeace("Lowerlimit",20)+
						CCommFunc::StrAddSpeace("Upperlimit",20)+CCommFunc::StrAddSpeace("Uint",20)+
						CCommFunc::StrAddSpeace("Result",20));
				}// if init
				sprintf_s(Rf_Info.Test_Item,sizeof(Rf_Info.Test_Item),"%s",CCommFunc::TraceStringMid(str.GetBuffer(),"",":").GetBuffer());
				tmp=CCommFunc::TraceStringMid(str.GetBuffer(),":","(");
				sprintf_s(Rf_Info.Dut_Value,sizeof(Rf_Info.Dut_Value),"%s",CCommFunc::TraceStringMid(tmp.GetBuffer(),""," ").GetBuffer());
				tmp=tmp.GetBuffer()+strlen(Rf_Info.Dut_Value);
				tmp.Replace(" ","");
				sprintf_s(Rf_Info.Uint,sizeof(Rf_Info.Uint),"%s",tmp.GetBuffer());
				sprintf_s(Rf_Info.Result,sizeof(Rf_Info.Result),"%s", (strstr(str,"Failed")!=NULL )?"Fail":"Pass");
				tmp="";

				tmp=tmp+CCommFunc::StrAddSpeace(Rf_Info.Test_Item,30)+
					CCommFunc::StrAddSpeace(Rf_Info.Dut_Value,20)+
					CCommFunc::StrAddSpeace((strlen(Rf_Info.Lowerlimit)>0)?Rf_Info.Lowerlimit:"-",20)+
					CCommFunc::StrAddSpeace((strlen(Rf_Info.Upperlmit)>0)?Rf_Info.Upperlmit:"-",20)+
					CCommFunc::StrAddSpeace((strstr(Rf_Info.Uint,"%")!=NULL)?"Percent":Rf_Info.Uint,20)+
					CCommFunc::StrAddSpeace(Rf_Info.Result,20);
				CCommFunc::ExportTestMessage(cbListMessage,tmp);	

				if(strstr(Rf_Info.Result,"Fail")){// Jerry Add
					if(strstr(Rf_Info.Test_Item,"POWER_AVG_DBM")){
						strCopy(m_ErrorReport.ErrorCode,_T("T830"));
						strCopy(m_ErrorReport.FailedTest,"POWER_AVG_DBM");
					}else if(strstr(Rf_Info.Test_Item,"FREQ_ERROR_AVG")){
						strCopy(m_ErrorReport.ErrorCode,_T("T799"));
						strCopy(m_ErrorReport.FailedTest,"FREQ_ERROR_AVG");
					}else if(strstr(Rf_Info.Test_Item,"EVM_AVG_DB")){
						strCopy(m_ErrorReport.ErrorCode,_T("T214"));
						strCopy(m_ErrorReport.FailedTest,"EVM_AVG_DB");
					}
				}

				//xml
				Xml_Data_All=Xml_Data_All+AddXmlHeader(Rf_Info.Test_Item,Rf_Info.Dut_Value);
				//sfcs spec Jerry Add
				AddSfcsHeaderSpec(m_Xml_Data,Sfcs_Spec_Data,Sfcs_Result_Data);//TX_EVM & RX_PER (SFCS format)

				if(strstr(str.GetBuffer(),"VIOLATION_PERCENT")!=NULL){
					strCopy(m_Xml_Data.szMaskViolationPercent,Rf_Info.Dut_Value);
				}
				else if(strstr(str.GetBuffer(),"PER")!=NULL){
					strCopy(m_Xml_Data.szRxPER,Rf_Info.Dut_Value);
				}
				else if(strstr(str.GetBuffer(),"EVM_AVG_DB")!=NULL){
					strCopy(m_Xml_Data.szEvm,Rf_Info.Dut_Value);
				}
				else if(strstr(str.GetBuffer(),"FREQ_ERROR_AVG")!=NULL){
					strCopy(m_Xml_Data.szPPM,Rf_Info.Dut_Value);
				}
				else if(strstr(str.GetBuffer(),"POWER_AVG_DBM")!=NULL){
					strCopy(m_Xml_Data.szActualPwr,Rf_Info.Dut_Value);
				}
			}
		}//else if(strstr(str,"(")!=NULL && strstr(str,")")!=NULL)
		else if(strstr(str.GetBuffer(),"BSS_FREQ_MHZ_PRIMARY")!=NULL){
			tmp=CCommFunc::TraceStringMid(str.GetBuffer(),":","");
			sprintf_s(m_Xml_Data.szFreq,sizeof(m_Xml_Data.szFreq),"%s",CCommFunc::TraceStringMid(tmp.GetBuffer()," "," ").GetBuffer());
			Xml_Data_All=Xml_Data_All+AddXmlHeader("FREQ_MHZ",tmp.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"TX_POWER_DBM")!=NULL){
			tmp=CCommFunc::TraceStringMid(str.GetBuffer(),":","");
			sprintf_s(m_Xml_Data.szExpectPwr,sizeof(m_Xml_Data.szExpectPwr),"%s",CCommFunc::TraceStringMid(tmp.GetBuffer()," "," ").GetBuffer());
			Xml_Data_All=Xml_Data_All+AddXmlHeader("TX_POWER_DBM",tmp.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"BSS_BANDWIDTH")!=NULL){
			tmp=CCommFunc::TraceStringMid(str.GetBuffer(),":","");
			sprintf_s(m_Xml_Data.szBandWidth,sizeof(m_Xml_Data.szBandWidth),"%s",CCommFunc::TraceStringMid(tmp.GetBuffer(),"-"," ").GetBuffer());
			Xml_Data_All=Xml_Data_All+AddXmlHeader("BSS_BANDWIDTH",tmp.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"DATA_RATE")!=NULL){
			tmp=CCommFunc::TraceStringMid(str.GetBuffer(),":","");
			sprintf_s(m_Xml_Data.szDataRate,sizeof(m_Xml_Data.szDataRate),"%s",CCommFunc::TraceStringMid(tmp.GetBuffer()," ","").GetBuffer());
			if(strstr(m_Xml_Data.szDataRate," ") != NULL){//Jerry Add
				tmp=CCommFunc::TraceStringMid(tmp.GetBuffer()," "," ");
				sprintf_s(m_Xml_Data.szDataRate,sizeof(m_Xml_Data.szDataRate),"%s",tmp.GetBuffer());
			}
			Xml_Data_All=Xml_Data_All+AddXmlHeader("DATA_RATE",tmp.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"RX_POWER_DBM")!=NULL){
			tmp=CCommFunc::TraceStringMid(str.GetBuffer(),":","");
			Xml_Data_All=Xml_Data_All+AddXmlHeader("RX_POWER_DBM",tmp.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"TX1")!=NULL || strstr(str.GetBuffer(),"TX2")!=NULL || strstr(str.GetBuffer(),"TX3")!=NULL || strstr(str.GetBuffer(),"TX4")!=NULL ){
			Xml_Data_All=Xml_Data_All+AntennaToXml(&m_Xml_Data.Antenna,str.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"RX1")!=NULL || strstr(str.GetBuffer(),"RX2")!=NULL || strstr(str.GetBuffer(),"RX3")!=NULL || strstr(str.GetBuffer(),"RX4")!=NULL ){
			Xml_Data_All=Xml_Data_All+AntennaToXml(&m_Xml_Data.Antenna,str.GetBuffer());
		}
		else if(strstr(str.GetBuffer(),"Test Time")!=NULL){
			sprintf_s(Rf_Info.Test_Time,sizeof(Rf_Info.Test_Time),"%s",str);
			tmp=CCommFunc::TraceStringMid(str.GetBuffer(),"=","s");
			Xml_Data_All=Xml_Data_All+AddXmlHeader("Test_Time",tmp.GetBuffer());
		}
		else if(strstr(str,"ERROR_MESSAGE")!=NULL){// clear
			CCommFunc::ExportTestMessage(cbListMessage, str+" "+Rf_Info.Test_Time);	
			CCommFunc::ExportTestMessage(cbListMessage," ");
			//xml
			Xml_Data_All=Xml_Data_All+"</Data>\n";
			Xml_Data_All=Xml_Data_All+"</TestCase>\n";
			//sfcs result
			AddSfcsHeaderResult(m_Xml_Data,Sfcs_Spec_Data,Sfcs_Result_Data);//TX_EVM & RX_PER (SFCS format)

			memset(&Rf_Info,0x00,sizeof(Rf_Info));
			memset(&m_Xml_Data,0x00,sizeof(XML_DATA));
			m_eTestType=NOT_DEFINE_TEST;
			Init=FALSE;
		}

	}
	return;

}
void CIQfact_Test::ParserfuncGetMessage(funcGetMessage cbfun,LPSTR Data,LPSTR Fixed){
	int n=0;//Upload funcGetMessage 
	CString str=Data;
	CString tmp="";
	if(strlen(Data)>0)
	{
		while(n < str.GetLength())
		{
			tmp=str.Tokenize(Fixed,n);
			//ExportTestLog(tmp);
			CCommFunc::ExportTestMessage(cbfun,tmp.GetBuffer());
		}
	}

	return;
}
void CIQfact_Test::AddSfcsHeaderSpec(XML_DATA Xml,CString &Sfcs_Spec,CString &Sfcs_Result){
	//Jerry Add
	TCHAR szTemp[DLL_INFO_SIZE];zeroIt(szTemp);
	//Each Item spec can save it.
	if(strstr(Rf_Info.Test_Item,"POWER") != NULL){
		sprintf_s(szTemp,sizeof(szTemp),"WiFiTxPwr-F%s-A%s-R%s-BW%s_M_%s/%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Rf_Info.Upperlmit,
			Rf_Info.Lowerlimit);	
		Sfcs_Spec=Sfcs_Spec+szTemp+"@";
	}else if(strstr(Rf_Info.Test_Item,"EVM") != NULL){//WiFiTxEVM-F%sA%sR%sBW%sP%s_S_%s/
		sprintf_s(szTemp,sizeof(szTemp),"WiFiTxEVM-F%s-A%s-R%s-BW%s_S_%s/",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Rf_Info.Upperlmit);	
		Sfcs_Spec=Sfcs_Spec+szTemp+"@";
	}else if(strstr(Rf_Info.Test_Item,"FREQ") != NULL){
		sprintf_s(szTemp,sizeof(szTemp),"WiFiFreqErr-F%s-A%s-R%s-BW%s_M_%s/%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate,
			Xml.szBandWidth,
			Rf_Info.Upperlmit,
			Rf_Info.Lowerlimit);	
		Sfcs_Spec=Sfcs_Spec+szTemp+"@";
	}else if(strstr(Rf_Info.Test_Item,"PER") != NULL && strlen(Rf_Info.Test_Item)<4){
		sprintf_s(szTemp,sizeof(szTemp),"WiFiRxPer-F%s-A%s-R%s-BW%s_M_%s/%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Rf_Info.Upperlmit,
			Rf_Info.Lowerlimit);	
		Sfcs_Spec=Sfcs_Spec+szTemp+"@";
	}else if(strstr(Rf_Info.Test_Item,"VIOLATION") != NULL){
		sprintf_s(szTemp,sizeof(szTemp),"WiFiMask-F%s-A%s-R%s-BW%s_M_%s/%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Rf_Info.Upperlmit,
			Rf_Info.Lowerlimit);	
		Sfcs_Spec=Sfcs_Spec+szTemp+"@";
	}

	return ;
}
void CIQfact_Test::AddSfcsHeaderResult(XML_DATA Xml,CString &Sfcs_Spec,CString &Sfcs_Result){
	//Jerry Add
	TCHAR szTemp[DLL_INFO_SIZE];zeroIt(szTemp);
	//Each Item result can save it.
	if(strlen(Xml.szActualPwr)>0|| strlen(Xml.szEvm)>0|| strlen(Xml.szPPM)>0)
	{
		if(strlen(Xml.szEvm)>0){
			sprintf_s(szTemp,sizeof(szTemp),"WiFiTxEVM-F%s-A%s-R%s-BW%s_%s",
				Xml.szFreq, 
				GetAntennaQuantity(Xml.Antenna),
				Xml.szDataRate, 
				Xml.szBandWidth,
				Xml.szEvm);	
			Sfcs_Result=Sfcs_Result+szTemp+"@";
		}
		if (strlen(Xml.szPPM)>0){
			sprintf_s(szTemp,sizeof(szTemp),"WiFiFreqErr-F%s-A%s-R%s-BW%s_%s",
				Xml.szFreq, 
				GetAntennaQuantity(Xml.Antenna),
				Xml.szDataRate, 
				Xml.szBandWidth,
				Xml.szPPM);	
			Sfcs_Result=Sfcs_Result+szTemp+"@";
		}
		if(strlen(Xml.szActualPwr)>0){
		sprintf_s(szTemp,sizeof(szTemp),"WiFiTxPwr-F%s-A%s-R%s-BW%s_%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Xml.szActualPwr);
		Sfcs_Result=Sfcs_Result+szTemp+"@";
		}
	}
	//else if(strlen(Xml.szMaskViolationPercent)>0)
	else if(strlen(Xml.szMaskViolationPercent)>0)
	{
		sprintf_s(szTemp,sizeof(szTemp),"WiFiMask-F%s-A%s-R%s-BW%s_%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Xml.szMaskViolationPercent);	
		Sfcs_Result=Sfcs_Result+szTemp+"@";
	}
	else if(strlen(Xml.szRxPER)>0)
	//else if(m_eTestType<=WIFI_RXPER_TEST)
	{
		sprintf_s(szTemp,sizeof(szTemp),"WiFiRxPer-F%s-A%s-R%s-BW%s_%s",
			Xml.szFreq, 
			GetAntennaQuantity(Xml.Antenna),
			Xml.szDataRate, 
			Xml.szBandWidth,
			Xml.szRxPER);	
		Sfcs_Result=Sfcs_Result+szTemp+"@";
	}
	return ;
}
CString CIQfact_Test::AntennaToXml(XML_ANTENNA* Antenna,LPSTR Action){//Check Antenna
	CString n=CCommFunc::TraceStringMid(Action,":","");

	if(atoi(n)>0){
		if(strstr(Action,"TX1")!=NULL || strstr(Action,"RX1")!=NULL){
			strCopy(Antenna->A,"1");
		}
		else if(strstr(Action,"TX2")!=NULL || strstr(Action,"RX2")!=NULL){
			strCopy(Antenna->B,"1");
		}
		else if(strstr(Action,"TX3")!=NULL || strstr(Action,"RX3")!=NULL){
			strCopy(Antenna->C,"1");
		}
		else if(strstr(Action,"TX4")!=NULL || strstr(Action,"RX4")!=NULL){
			strCopy(Antenna->D,"1");
		}

		//xml
		if(strstr(Action,"TX")!=NULL){
			n=CCommFunc::TraceStringMid(Action,"TX",":");
			n=AddXmlHeader("ANTENNA",n.GetBuffer())+"\n";
		}
		else{
			n=CCommFunc::TraceStringMid(Action,"RX",":");
			n=AddXmlHeader("ANTENNA",n.GetBuffer())+"\n";
		}
	}
	else{
		n="";
	}

	return n;
}
CString CIQfact_Test::AddXmlHeader(LPSTR Header,LPSTR Data)
{// XML Format
	CString str="";
	str.Format("<%s>%s</%s>\n",Header,Data,Header);
	return str;
}
CString CIQfact_Test::GetAntennaQuantity(XML_ANTENNA Antenna){
	CString str="";
	if(strcmp(Antenna.A,"1") == 0) 
		str=str+"1";
	if(strcmp(Antenna.B,"1") == 0) 
		str=str+"2";
	if(strcmp(Antenna.C,"1") == 0) 
		str=str+"3";
	if(strcmp(Antenna.D,"1") == 0) 
		str=str+"4";

	return str;
}
