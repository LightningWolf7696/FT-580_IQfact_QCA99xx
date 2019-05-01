#pragma once
#include "../CommFunc.h"

class CBtVerifyTestData
{
public:
	CBtVerifyTestData(void);
public:
	~CBtVerifyTestData(void);

public:
	TM_RETURN Run(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
	void Clear();
public:
	enum CHECK_ITEM{
		CHECK_BT_MAC =0,
		CHECK_BT_POWER_LEVEL
	};
public:
	struct FLAG{
		BOOL CHECK_BT_MAC;
		BOOL CHECK_BT_POWER_LEVEL;
	};
	FLAG Flag;
public:
	int work_flow;
	TM_RETURN res;
	CString szWorkName;
	CString Str_Tmp;
	TCHAR	szMessage[DLL_INFO_SIZE];
	TCHAR EXPECT_VALUE[DLL_INFO_SIZE];
	TCHAR DUT_VALUE[DLL_INFO_SIZE];
public:
	TM_RETURN Check_Bt_Mac(LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Check_Bt_Power_level(LPSTR EXPECT,LPSTR DUT);
};
