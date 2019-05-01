#pragma once

#include "../CommFunc.h"


class CLoadDutRequired
{
public:
	CLoadDutRequired(void);
public:
	~CLoadDutRequired(void);

private:
	enum ENUM_ITEM{
		ENV_PRE_SET,
		GEN_TESTSCRIPT,
		TFTPD32_INIT,
		COLSE_OEM_SOFT,
		PLINK_INIT,
		REG_DELETE,
		PIPE_ARP,
		SEND_MSG,
		PASER_CABLELOSS,
	};
public:
	typedef  struct FLAG{
		BOOL	ENV_PRE_SET;
		BOOL	GEN_TESTSCRIPT;
		BOOL	COLSE_OEM_SOFT;
		BOOL	TFTPD32_INIT;
		BOOL	PLINK_INIT;
		BOOL	REG_DELETE;
		BOOL    PIPE_ARP;
		BOOL    SEND_MSG;
		BOOL	PASER_CABLELOSS;
	};
public:
	CString szWorkName;
	BOOL	res;
	FLAG	Flag;
	TCHAR   szMessage[DLL_INFO_SIZE];
	
public:
	void Clean();
	BOOL START(BOOL bRetry, LPSTR lpszRunInfo);

private:
	void CopyAllFolders(LPSTR szSrcPath, LPSTR szDestPath);
	void ReplaceFileString(LPSTR szFileName, LPSTR szSrc, LPSTR szDest);
	void ReplaceFileStringData(LPSTR szFileName,LPSTR szItem, LPSTR szData);
public:
	BOOL PreSetting();
	//BOOL Gen_TestScript(LPSTR Mac_Rule);
	BOOL Gen_TestScript();
	BOOL CheckAlreadyXtal(LPSTR szCmd,LPSTR szKeyWord);
	BOOL PipePlinkInit(int &Pipeid,LPSTR szCmd,LPSTR UserName,LPSTR PassWord,LPSTR szKeyWord);
	BOOL RegDelete(LPSTR Key);
	BOOL Pipe_ARP();
	BOOL Send_Msg();
	BOOL Get_Oem_Type();
	BOOL Paser_CableLoss(LPSTR FileName);
};
