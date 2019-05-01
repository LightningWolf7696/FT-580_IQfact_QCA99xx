#pragma once
#include "../CommFunc.h"

class CBtWriteTestData
{
public:
	CBtWriteTestData(void);
public:
	~CBtWriteTestData(void);

public:
	TM_RETURN Run(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport,int nRetryCnt);
	void Clear();
public:
	enum WRITE_ITEM{
		WRITE_BT_MAC =0,
		WRITE_BT_POWER_LEVEL
	};
public:
	struct FLAG{
		BOOL WRITE_BT_MAC;
		BOOL WRITE_BT_POWER_LEVEL;
	};
	FLAG Flag;
	
public:
	int work_flow;
	TM_RETURN res;
	CString szWorkName;
	TCHAR	szMessage[DLL_INFO_SIZE];
	TCHAR EXPECT_VALUE[DLL_INFO_SIZE];
	TCHAR DUT_VALUE[DLL_INFO_SIZE];
	//TCHAR INI_STR[DLL_INFO_SIZE];

public:
	TM_RETURN Write_Bt_Mac(LPSTR EXPECT,LPSTR DUT);
	TM_RETURN Write_Bt_Power_Level(LPSTR PowerLevel,LPSTR EXPECT,LPSTR DUT);
};
