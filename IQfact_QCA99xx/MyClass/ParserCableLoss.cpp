#include "../StdAfx.h"
#include "ParserCableLoss.h"


CParserCableLoss::CParserCableLoss(void)
{
}

CParserCableLoss::~CParserCableLoss(void)
{

}

BOOL CParserCableLoss::ParserIQfactCableLoss(LPSTR lpszRunInfo)
{//Parser IQfact CableLoss
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	char szMessage[DLL_INFO_SIZE]={0};	
	FILE *stream = NULL;
	char lpszFileName[MAX_PATH]={0};
	char szLocalPath[MAX_PATH]={0};

	//Get local path
	GetLocalDirectory(szLocalPath,szMessage);
	strCopy(g_GlobalInfo.szLocalPath,szLocalPath);
	sprintf_s(lpszFileName,sizeof(lpszFileName),"%sOEM\\%s\\%s",szLocalPath,g_GlobalInfo.CUSTOMER,g_CableLoss.Iqfact_CableLoss);	
	if(CheckFileExist(lpszFileName,szMessage)) DeleteFile(lpszFileName);

	if((stream = fopen(lpszFileName, "w")) == NULL)
	{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't parser cableLoss from DUT.[Name:%s]",lpszFileName);
		return FALSE;
	}
	for (int i=0; i < (WIFI_CABLELOSS_ANTENNA*WIFI_CABLELOSS_MAX_ARY*WIFI_CABLELOSS_RESULT); i++)
	{
		char szCableLoss[64]={0};
		double dbAntLoss[4]={0};
		double dbFreq;
		if((dbFreq = g_CableLoss.TxPath[0][i][0]) == 0) break;		
		for(int j=0 ; j<4 ; j++)
			dbAntLoss[j] = g_CableLoss.TxPath[j][i][1];
		fprintf(stream,"%.0lf,%.1lf,%.1lf,%.1lf,%.1lf\n",dbFreq,dbAntLoss[0],dbAntLoss[1],dbAntLoss[2],dbAntLoss[3]);			
	}
	fclose(stream);


	/*sprintf_s(lpszFileName,sizeof(lpszFileName),"%sOEM\\%s\\%s",szLocalPath,g_GlobalInfo.CUSTOMER,"OEM.bat");	
	if(CheckFileExist(lpszFileName,szMessage)) DeleteFile(lpszFileName);

	if((stream = fopen(lpszFileName, "w")) == NULL)
	{
	sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't parser cableLoss from DUT.[Name:%s]",lpszFileName);
	return FALSE;
	}

	fprintf(stream,"cd %sOEM\\%s\\\n",szLocalPath,g_GlobalInfo.CUSTOMER);
	fprintf(stream,"%c:\n",szLocalPath[0]);
	fprintf(stream,"generate_path_loss2.exe\n");
	fclose(stream);

	WinExec(lpszFileName,SC_MINIMIZE);*/
	sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Success to Parser %s.",lpszFileName);
	return TRUE;
}
BOOL CParserCableLoss::ParserART2CableLoss(LPSTR lpszRunInfo){//Parser ART2 CableLoss
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	
	char szMessage[DLL_INFO_SIZE]={0};	
	FILE *stream = NULL, *streamTemp = NULL;
	TCHAR szFileName[MAX_PATH]={0},szTempFileName[MAX_PATH]={0};
	TCHAR szLocalPath[MAX_PATH]={0};
	errno_t err;
	TCHAR szLineBuf[DLL_INFO_SIZE/2]={0};
	TCHAR *pLine;
	TCHAR szFrequency[1024]={0}, szCableLoss[4][1024]={0};
	TCHAR szTemp[10]={0};
	CString strTypeName[5]={"=golden","=pm","=sa","=vsa","=vsg"};
	CString strChainName[4]={"chain=1","chain=2","chain=4","chain=8"};
	BOOL bTrigger[5][4]={FALSE};//[Type][Chain]

	//Get local path
	GetLocalDirectory(szLocalPath,szMessage);
	sprintf_s(szFileName,MAX_PATH,"%sART2\\%s\\bin\\start.art", szLocalPath, g_GlobalInfo.szPN);
	sprintf_s(szTempFileName,MAX_PATH,"%sART2\\%s\\bin\\startTemp.art", szLocalPath,  g_GlobalInfo.szPN);
	if(!CheckFileExist(szFileName,lpszRunInfo)) 
	{
		sprintf_s(szFileName,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\start.art", szLocalPath, g_GlobalInfo.CUSTOMER);
		sprintf_s(szTempFileName,MAX_PATH,"%sOEM\\ART2\\%s\\bin\\startTemp.art", szLocalPath, g_GlobalInfo.CUSTOMER);
		if(!CheckFileExist(szFileName,lpszRunInfo)) 
			return FALSE;
	}

	if(err = fopen_s(&streamTemp, szTempFileName,"w") != NULL)
	{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't parser cableLoss from DUT.[Name:%s]",streamTemp);
		return FALSE;
	}
	if((err = fopen_s(&stream, szFileName, "r")) != NULL)
	{
		sprintf_s(lpszRunInfo,DLL_INFO_SIZE,"Can't parser cableLoss from DUT.[Name:%s]",szFileName);
		return FALSE;
	}

	for(int i=0 ; i<WIFI_CABLELOSS_MAX_ARY ; i++)
	{
		if(g_CableLoss.TxPath[0][i][0] == 0) break;
		sprintf_s(szTemp,sizeof(szTemp),"%.0lf,",g_CableLoss.TxPath[0][i][0]);
		strcat_s(szFrequency,sizeof(szFrequency),szTemp);
		for(int j=0 ; j<4 ; j++)
		{
			sprintf_s(szTemp,sizeof(szTemp),"%.1lf,",g_CableLoss.TxPath[j][i][1]);
			strcat_s(szCableLoss[j],sizeof(szCableLoss[j]),szTemp);
		}
	}
	szFrequency[strlen(szFrequency)-1] = 0;
	for(int j=0 ; j<4 ; j++)
		szCableLoss[j][strlen(szCableLoss[j])-1] = 0;

	while(fgets(szLineBuf, sizeof(szLineBuf)-2, stream) != NULL)
	{
		pLine = szLineBuf;
		if(pLine == NULL) 
			continue;
		else if(strstr(pLine,_T("path device")) != NULL){
			for(int i=0 ; i<5 ; i++){
				if(strstr(pLine,strTypeName[i].GetBuffer()) != NULL){
					for(int j=0 ; j<4 ; j++){						
						if(strstr(pLine,strChainName[j].GetBuffer()) != NULL){
							if(!bTrigger[i][j]){
								sprintf_s(pLine,sizeof(szLineBuf),"path device%s;f=%s;%s;loss=%s;\n",strTypeName[i].GetBuffer(),szFrequency,strChainName[j].GetBuffer(),szCableLoss[j]);
								bTrigger[i][j] = TRUE;
							}
							break;
						}//if(strstr(pLine,strChainName[j].GetBuffer()) != NULL){
					}//for(int j=0 ; j<6 ; j++){
				}//if(strstr(pLine,strTypeName[i].GetBuffer()) != NULL){
			}//for(int i=0 ; i<3 ; i++){
		}//else if(strstr(pLine,_T("path device")) != NULL){
		fprintf(streamTemp, "%s", pLine);
	}
	fclose(stream);
	fclose(streamTemp);

	do 
	{
		DeleteFile(szFileName);
		Sleep(500);
	} while (CheckFileExist(szFileName,szMessage));	

	rename(szTempFileName,szFileName);	
	strcpy_s(lpszRunInfo,DLL_INFO_SIZE,"Success to ParserART2CableLoss from DUT.");
	return TRUE;
}
