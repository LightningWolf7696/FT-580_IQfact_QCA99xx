#pragma once

#include "../CommFunc.h"


class CWiFiInitialize
{
public:
	CWiFiInitialize(void);
public:
	~CWiFiInitialize(void);
private:
	enum ENUM_ITEM{
		START_DUT_SERVICE,
		ART2_OEM_TOOL,
	};

public:
	typedef  struct FLAG{
		BOOL	ART2_OEM_TOOL;
		BOOL	START_DUT_SERVICE;
		BOOL	ONLY_CART;
	};
public:
	FLAG	Flag;
	CString szWorkName;
	BOOL	res;

public:
	void Clean();
	BOOL START(BOOL bRetry, LPSTR lpszRunInfo);
public:
	BOOL Start_Dut_Service(LPSTR szCmd,LPSTR szKeyWord,UINT TimeOut);
	BOOL Art2_Oem_Tool(BOOL Only_Cart,LPSTR lpszRunInfo);
};
