
#include "../StdAfx.h"
#include "LoadDutRequired.h"
#include <io.h>


CLoadDutRequired::CLoadDutRequired(void)
{
}

CLoadDutRequired::~CLoadDutRequired(void)
{

}
void CLoadDutRequired::Clean(){

	zeroIt(Flag);zeroIt(Net_Info);zeroIt(Test_Info);zeroIt(szMessage);zeroIt(g_GlobalInfo.INI_STR);
	Iqfact_Script.Mac_Rules.RemoveAll();
	res=TRUE;
}
BOOL CLoadDutRequired::START(BOOL bRetry, LPSTR lpszRunInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	szWorkName="";
	for(int work_flow=0;work_flow<=(int)PASER_CABLELOSS;work_flow++){

		if (szWorkName.GetLength()>0) break;

		switch(work_flow){
		case ENV_PRE_SET:
			if (!Flag.ENV_PRE_SET) continue;
			szWorkName="ENV_PRE_SET";
			ExportTestLog("==========%s==========",szWorkName);
			res=PreSetting();//External Folder(Source) to OEM(Destination) Folder
			break;
		case GEN_TESTSCRIPT:
			if (!Flag.GEN_TESTSCRIPT) continue;
			szWorkName="GEN_TESTSCRIPT";
			ExportTestLog("==========%s==========",szWorkName);
			res=Gen_TestScript();// Edit Test Script, Sample to Test_ipq4019_qc9984
			break;
		case TFTPD32_INIT:
			if(!Flag.TFTPD32_INIT) continue;
			szWorkName="TFTPD32_INIT";
			ExportTestLog("==========%s==========",szWorkName);
			res=CCommFunc::OpenExecSoft("tftp","tftpd32");
			break;
		case COLSE_OEM_SOFT:
			if (!Flag.COLSE_OEM_SOFT) continue;
			szWorkName="COLSE_OEM_SOFT";
			ExportTestLog("==========%s==========",szWorkName);
			res=ProcessKill(g_GlobalInfo.INI_STR,szMessage);
			if(res)
				ExportTestLog("CLOSE_OEM_SOFT_PASS = %s",g_GlobalInfo.INI_STR);
			else
				ExportTestLog("CLOSE_OEM_SOFT_FAIL = %s",g_GlobalInfo.INI_STR);
			break;
		case PLINK_INIT:
			if(!Flag.PLINK_INIT) continue;
			szWorkName="PLINK_INIT";
			ExportTestLog("==========%s==========",szWorkName);
			res=CCommFunc::PipePlinkInit(g_GlobalInfo.Pipe_OEM_ID,Test_Info.szCmd,Net_Info.UserName,Net_Info.PassWord,Test_Info.szKeyWord);//Use CommFunc
			break;
		case REG_DELETE:
			if(!Flag.REG_DELETE) continue;
			szWorkName="REG_DELETE";
			ExportTestLog("==========%s==========",szWorkName);
			res=CCommFunc::RegDelete(g_GlobalInfo.INI_STR);//Use CommFunc
			break;
		case PIPE_ARP:
			if(!Flag.PIPE_ARP) continue;
			szWorkName="PIPE_ARP";
			ExportTestLog("==========%s==========",szWorkName);
			res=Pipe_ARP();
			break;
		case SEND_MSG:
			if(!Flag.SEND_MSG) continue;
			szWorkName="SEND_MSG";
			ExportTestLog("==========%s==========",szWorkName);
			res=Send_Msg();
			break;
		case PASER_CABLELOSS:
			if(!Flag.PASER_CABLELOSS) continue;
			szWorkName="PASER_CABLELOSS";
			ExportTestLog("==========%s==========",szWorkName);
			res=Paser_CableLoss(g_GlobalInfo.INI_STR);//Copy pathloss.ini to CableLoss.ini 
			break;
			}//switch
	}//for loop

	if (szWorkName.GetLength()>0){
		ExportTestLog("==========%s==========\r\n",szWorkName);
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"%s_%s",szWorkName.GetBuffer(),(res)?"PASS":"FAIL");
	}
	else{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"WIFI_LOAD_DUT_REQUIRED_PASS");
	}

	return res;
}

void CLoadDutRequired::CopyAllFolders(LPSTR szSrcPath, LPSTR szDestPath)
{
	/*
	/S 複製每個目錄及其包含的子目錄
	/E 複製每個目錄及其包含的子目錄，也複製空目錄
	/D 複製發生變更的檔案
	/Y 所有覆蓋的詢問都回答 YES
	*/

	char szCmd[MAX_PATH]={0};
	sprintf_s(szCmd,sizeof(szCmd),"xcopy \"%s\" \"%s\" /e /r /y",szSrcPath,szDestPath);	
	WinExec(szCmd,SC_MINIMIZE);

	return;
}
BOOL CLoadDutRequired::PreSetting(){
	char szMessage[DLL_INFO_SIZE]={0};	
	char szSourcePath[MAX_PATH]={0};
	char szDestinationPath[MAX_PATH]={0};

	//GetLocalDirectory(g_GlobalInfo.szLocalPath,szMessage);
	if(g_GlobalInfo.IsArt2){
		sprintf_s(szSourcePath,MAX_PATH,"%sART2\\%s", g_GlobalInfo.szLocalPath, g_GlobalInfo.szPN);
		sprintf_s(szDestinationPath,MAX_PATH,"%sOEM\\ART2\\%s", g_GlobalInfo.szLocalPath, g_GlobalInfo.CUSTOMER);

		if(!CheckFileExist(szSourcePath,szMessage))
			return FALSE;	
		if(!CheckFileExist(szDestinationPath,szMessage))
		{
			if(!MakeDirectory(szDestinationPath, szMessage))
				return FALSE;
		}
	}
	else{
		sprintf_s(szSourcePath,MAX_PATH,"%s%s\\%s", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER, g_GlobalInfo.szPN);
		sprintf_s(szDestinationPath,MAX_PATH,"%sOEM\\%s", g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
	}
	ExportTestLog("szSourcePath = %s",szSourcePath);
	ExportTestLog("szDestinationPath = %s",szDestinationPath);
	CopyAllFolders(szSourcePath, szDestinationPath);//Copy file source path to destination path 

	return TRUE;
}
BOOL CLoadDutRequired::Gen_TestScript(){

	BOOL res=TRUE,szFindKeyWord=FALSE,isDakota=FALSE,CheckChipSet=FALSE,isFindXtal=FALSE;
	TCHAR FilePath[MAX_PATH];zeroIt(FilePath);
	TCHAR szTemp[MAX_PATH];zeroIt(szTemp);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	CStdioFile Test_Script,Sample_Script;
	CString str;
	int Mac_Count=0;

	sprintf_s(FilePath,"%sOEM\\%s\\",g_GlobalInfo.szLocalPath,g_GlobalInfo.CUSTOMER);
	sprintf_s(szTemp,sizeof(szTemp),"%s%s",FilePath,Iqfact_Script.Sample_Script);

	if(strlen(Test_Info.szCmd)>0){//Jerry Add
		res=(!CheckAlreadyXtal(Test_Info.szCmd,Test_Info.szKeyWord));//Check OTP XTAL
	}

	if(!CheckFileExist(szTemp,szMessage)){
		ExportTestLog("%s Not Found",szTemp);
		return FALSE;
	}else{
		Sample_Script.Open(szTemp,CFile::modeReadWrite);
		sprintf_s(szTemp,sizeof(szTemp),"%s%s",FilePath,Test_Info.szScript_Name);
		Test_Script.Open(szTemp,CFile::modeCreate|CFile::modeWrite);

		while(Sample_Script.ReadString(str)){

			if(strstr(str.GetBuffer(),"INSERT_DUT")!=NULL){
				CheckChipSet=TRUE;
			}
			else if(CheckChipSet){
				if(strstr(str.GetBuffer(),"OPTION_STRING")!=NULL){
					if(strstr(str.GetBuffer(),"ipq")!=NULL){
						isDakota=TRUE;//ipq 4019 & 4018
						CheckChipSet=FALSE;
					}
					else{
						isDakota=FALSE;
						CheckChipSet=FALSE;
					}
				}
			}
			else if(strstr(str.GetBuffer(),">MAC_ADDRESS")!=NULL){
                if(Iqfact_Script.Mac_Rules.GetCount()>0){//SCRIPT_MAC_RULES
				str=str+CCommFunc::MacAddrToCalcPlus(g_GlobalInfo.szBarcode,atoi(Iqfact_Script.Mac_Rules.GetAt(Mac_Count)));
				Mac_Count++;
				ExportTestLog("MAC = %s",str);
                }
			}
			
			else if(isDakota){//ipq4018 XTAL
				if(!isFindXtal){
					if(szFindKeyWord){
						str.Replace("ALWAYS_SKIP","SKIP_ON_FAIL");
						szFindKeyWord=FALSE;//Prevent loss data
						isDakota=FALSE;
						isFindXtal=TRUE;
					}
					else if(!res){
						if(strstr(str.GetBuffer(),"XTAL_CALIBRATION")!=NULL){
							str.Replace("ALWAYS_SKIP","SKIP_ON_FAIL");
							szFindKeyWord=TRUE;// For OTP XTAL CAL
						}
					}
				}//isFindXtal
			}
			str=str+"\n";
			Test_Script.Write(str,str.GetLength());
		}//while
		Sample_Script.Close();
		Test_Script.Close();
	}
	return CheckFileExist(szTemp,szMessage);
}
BOOL CLoadDutRequired::CheckAlreadyXtal(LPSTR szCmd,LPSTR szKeyWord){

	BOOL res=FALSE;
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);

	//CCommFunc::ConSoleWithLog(szCmd,"#",szMessage,5);
	CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,szCmd,"#",szMessage,5);
	
	if(strstr(szMessage,szKeyWord)!=NULL){
		res=TRUE;
	}
	return res;
}
BOOL CLoadDutRequired::PipePlinkInit(int &Pipeid,LPSTR szCmd,LPSTR UserName,LPSTR PassWord,LPSTR szKeyWord){

	TCHAR szMessage[DLL_INFO_SIZE];
	TCHAR szPath[10000];
	BOOL res=FALSE;
	BOOL flag=FALSE;
	CString str="";
	//DWORD dbRead=0;
	if (!CCommFunc::PipeCalibration(Pipeid,"OEM\\plink",szPath))
		return FALSE;
	if(!CCommFunc::PipeLineMsgClear(Pipeid))
		return FALSE;
	double StartTime=GetTickCount();

	while(((GetTickCount()-StartTime)/1000)<Test_Info.TimeOut){

		//loop to check connect
		if(strstr(szCmd,"telnet")!=NULL){//telnet work
			if(CCommFunc::PipeWithLog(Pipeid,szCmd,"login",szMessage,5)){
				flag=TRUE;
			}
		}
		else{//ssh work
			if(CCommFunc::PipeWithLog(Pipeid,szCmd,"(y/n)",szMessage,5)){
				CCommFunc::mydelay(200);
				zeroIt(szPath);
				sprintf_s(szPath,sizeof(szPath),"y\n",UserName);
				if(PipeLineWriteData(Pipeid,szPath,szMessage)){
					flag=TRUE;
				}
			}
		}//if ssh

		if(flag){
			if(strlen(UserName)>0){
				CCommFunc::mydelay(500);
				//CCommFunc::PipeLineMsgClear(Pipeid);
				zeroIt(szPath);
				sprintf_s(szPath,sizeof(szPath),"%s\r\n",UserName);
				if(!PipeLineWriteData(Pipeid,szPath,szMessage)){
					break;
				}
			}
			if(strlen(PassWord)>0){
				CCommFunc::mydelay(500);
				//CCommFunc::PipeLineMsgClear(Pipeid);
				zeroIt(szPath);
				sprintf_s(szPath,sizeof(szPath),"%s\r\n",PassWord);
				if(!PipeLineWriteData(Pipeid,szPath,szMessage)){
					break;
				}
			}
			CCommFunc::mydelay(500);zeroIt(szPath);
			PipeLineReadData(Pipeid,szPath,sizeof(szPath),szMessage);
			
			if(strstr(szPath,Test_Info.szKeyWord)!=NULL){
				
				res=TRUE;
				break;
			}
		}// if flag
		if(!res){
			CCommFunc::CloseExecSoft("plink.exe");
		}

	}//while

	for(int i=0;i<2;i++){
		PipeLineWriteData(Pipeid,"\n",szMessage);
		CCommFunc::PipeLineMsgClear(Pipeid);
	}


	return res;
}
BOOL CLoadDutRequired::RegDelete(LPSTR Key){

	LONG lResult;
	HKEY hKeyRoot;
	CString str = CCommFunc::TraceStringMid(Key,"","\\");

	ExportTestLog("Address = %s",Key);

	if(strstr("HKEY_CURRENT_USER",str.GetBuffer())){
		hKeyRoot=HKEY_CURRENT_USER;
	}
	else if(strstr("HKEY_CLASSES_ROOT",str.GetBuffer())){
		hKeyRoot=HKEY_CLASSES_ROOT;
	}
	else if(strstr("HKEY_LOCAL_MACHINE",str.GetBuffer())){
		hKeyRoot=HKEY_LOCAL_MACHINE;
	}
	else if(strstr("HKEY_USERS",str.GetBuffer())){
		hKeyRoot=HKEY_USERS;
	}
	else if(strstr("HKEY_CURRENT_CONFIG",str.GetBuffer())){
		hKeyRoot=HKEY_CURRENT_CONFIG;
	}
	else{
		return FALSE;
	}

	str=CCommFunc::TraceStringMid(Key,str.GetBuffer(),"");
	str=CCommFunc::TraceStringMid(str.GetBuffer(),"\\","");

	lResult = RegDeleteKey(hKeyRoot, str.GetBuffer());


	if(lResult==ERROR_SUCCESS || lResult==ERROR_FILE_NOT_FOUND){
		return TRUE;
	}else{
		return FALSE;
	}
}
BOOL CLoadDutRequired::Pipe_ARP(){

	TCHAR szCmd[MAX_PATH];zeroIt(szCmd);
	TCHAR szMessage[DLL_INFO_SIZE];zeroIt(szMessage);
	TCHAR szKeyWord[MAX_PATH];zeroIt(szKeyWord);


	if (!CCommFunc::PipeCalibration(g_GlobalInfo.Pipe_SYS_ID,"",szKeyWord))
		return FALSE;
	if (!CCommFunc::PipeLineMsgClear(g_GlobalInfo.Pipe_SYS_ID))
		return FALSE;

	sprintf_s(szCmd,sizeof(szCmd),"%s","arp -d");
	ExportTestLog("Input szCmd = {%s}",szCmd);

	for(int i=0;i<2;i++){
		if(!CCommFunc::PipeLineQueryData(g_GlobalInfo.Pipe_SYS_ID,szCmd,szKeyWord,sizeof(szCmd),szMessage))
			return FALSE;
	}

	ExportTestLog("Output szMessage = {%s}",szMessage);
	return TRUE;
}

BOOL CLoadDutRequired::Send_Msg(){

	if(strlen(Test_Info.Console_Msg)>0){
		if(!CCommFunc::ConSoleWithLog(Test_Info.szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut))
			return FALSE;
	}else
		if(!CCommFunc::PipeWithLog(g_GlobalInfo.Pipe_OEM_ID,Test_Info.szCmd,Test_Info.szKeyWord,szMessage,Test_Info.TimeOut))
		return FALSE;
	return TRUE;
}

BOOL CLoadDutRequired::Paser_CableLoss(LPSTR FileName){
	TCHAR szMessage[DLL_INFO_SIZE]={0};	
	//TCHAR szTemp[MAX_PATH];
	TCHAR CableLossFilePath[MAX_PATH];
	void *hndl;
	CString str="";

	/*ConfigStation.ini*/ 
	sprintf_s(CableLossFilePath,MAX_PATH,"%s%s",g_GlobalInfo.szLocalPath,CONFIG_STATION_FILE);//...bin\Env\ConfigStation.ini
	if(!CreateIni(&hndl, CableLossFilePath, szMessage)) //read ini
		return FALSE;
	if(!GetIniKeyNameResult(hndl, "GLOBAL_FILE_PATH", "CABLE_LOSS_PATH", INI_TYPE_STRING, &CableLossFilePath[0], szMessage)) 
		return FALSE;
	if(!DestroyIni(hndl,szMessage)) //close ini
		return FALSE;
	//Load ConfigStation.ini CableLoss path.

	if(!CheckFileExist(CableLossFilePath,szMessage)){
		//if Not Found D:\TestProgramEnvSet\cablesloss.ini 
		//Get local ..\TestProgramEnvSet\cablesloss.ini
		sprintf(CableLossFilePath,"%sTestProgramEnvSet\\CableLoss.ini",g_GlobalInfo.szLocalPath);
	}
	str=CableLossFilePath;
	str.Replace("CableLoss.ini",FileName);
	//pathloss replace to CableLoss.

	if(!CheckFileExist(str.GetBuffer(),szMessage))
		return FALSE;
    ExportTestLog("Replace to CableLoss Success and %s.",szMessage);
	//copy FileName(pathloss) to cableloss.ini
	return CopyFile(str.GetBuffer(),CableLossFilePath,FALSE);
}