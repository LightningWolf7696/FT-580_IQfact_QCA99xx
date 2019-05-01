#pragma once

#include "../CommFunc.h"


class CWriteTestData
{
public:
	CWriteTestData(void);
public:
	~CWriteTestData(void);
	enum WRITE_ITEM{
		PIPE_WRITE_IDL_MAC=0,
		PIPE_WRITE_LICENSE_KEY,
		PIPE_WRITE_DUT_SN,
		PIPE_WRITE_DUT_MAC,
		PIPE_WRITE_COUNTRY_CODE,
		PIPE_WRITE_DUT_ENV,
		
	};
public:
	typedef struct FLAG{
		BOOL		PIPE_WRITE_IDL_MAC;
		BOOL        PIPE_WRITE_LICENSE_KEY;
		BOOL        PIPE_WRITE_DUT_SN;
		BOOL        PIPE_WRITE_DUT_MAC;
		BOOL        PIPE_WRITE_COUNTRY_CODE;
		BOOL        PIPE_WRITE_DUT_ENV;
		
	}FLAG;
public:
	FLAG		Flag;
	CString		szWorkName;
	TM_RETURN	res;
	TCHAR		EXPECT_VALUE[DLL_INFO_SIZE];
	TCHAR		DUT_VALUE[DLL_INFO_SIZE];
	int			work_flow;
	TCHAR		szMessage[DLL_INFO_SIZE];
	TCHAR		INI_STR[DLL_INFO_SIZE];
	TCHAR       Config_Name[32];
	
public:
	void Clean();
	TM_RETURN START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
public:
	TM_RETURN Pipe_Write_Idl_Mac(LPSTR Config,LPSTR Country,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Write_License_Key(LPSTR szTcl,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Write_SN(LPSTR szTcl,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Write_Dut_Mac(int PipeId,LPSTR szTcl,int Mac_Count,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Write_Country_Code(LPSTR szTcl,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Write_Dut_Env(LPSTR szCmd,LPSTR szItem,int Mac_Count ,LPSTR EXPECT,LPSTR DUT);
};
