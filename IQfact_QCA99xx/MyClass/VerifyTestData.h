#pragma once

#include "../CommFunc.h"

class CVerifyTestData
{
private:
	enum CHECK_ITEM{
		//CONSOLE_CHECK_MAC=0,
		PIPE_CHECK_DUT_MAC=0,
		PIPE_CHECK_DUT_SN,
		PIPE_CHECK_COUNTRY_CODE,
		PIPE_CHECK_DUT_ENV,
		PIPE_CHECK_DUT_XTAL,
		PIPE_CHECK_DUT_INTERFACE,
		PIPE_CHECK_DUT_COUNTRY,
	};
public:
	CVerifyTestData(void);
public:
	~CVerifyTestData(void);
public:
	typedef struct FLAG{
		
		//BOOL		CONSOLE_CHECK_MAC;
		BOOL		PIPE_CHECK_DUT_MAC;
		BOOL        PIPE_CHECK_DUT_SN;
		BOOL        PIPE_CHECK_COUNTRY_CODE;
		BOOL        PIPE_CHECK_DUT_ENV;
		BOOL		PIPE_CHECK_DUT_XTAL;
		BOOL		PIPE_CHECK_DUT_INTERFACE;
		BOOL		PIPE_CHECK_DUT_COUNTRY;
	}FLAG;

public:
	FLAG		Flag;
	TCHAR		szMessage[DLL_INFO_SIZE];
	TCHAR		EXPECT_VALUE[DLL_INFO_SIZE];
	TCHAR		DUT_VALUE[DLL_INFO_SIZE];
	CString		szCheckName;
	CString		Str_Tmp;//mac_tmp url
	
	TM_RETURN	res;
	int         work_flow;
	//TCHAR		szStart[16];
	//TCHAR		szEnd[16];
public:
	BOOL	Clean();
	TM_RETURN PASCAL EXPORT START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
public:
	TM_RETURN ConSole_Check_Mac(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_Dut_Mac(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_SN(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_Dut_Env(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_Country_Code(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_Xtal(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_InterFace(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Pipe_Check_Dut_Country(LPSTR szCmd,LPSTR EXPECT,LPSTR DUT);
};
