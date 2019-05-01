#pragma once

#include "../CommFunc.h"


class CPreLoadImage
{
public:
	CPreLoadImage(void);
public:
	~CPreLoadImage(void);

public:
	TM_RETURN START(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport);
	TM_RETURN QUANTENNA(funcGetMessage cbListMessage, funcGetMessage cbXmlMessage, funcGetMessage cbSfcsSpecMessage, funcGetMessage cbSfcsResultMessage, ERRORREPORT &ErrorReport);
	static DWORD ThreadForLedTurnOn(LPVOID pParam);
	static BOOL	BreakThread;
	BOOL	IsPsuCtrl;

private:
	BOOL ReadAutoLedParam(int &nAutoCheckLed, int &nLedComPort, int &nLedTimeOutMsec, LPSTR lpszRunInfo);
	BOOL GetPrivateProfileStringNoComment(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpComment, LPCTSTR lpDefault, LPSTR lpszValue, int nValueSize, LPCTSTR lpFileName);

public:
	TM_RETURN Check_Load_Fw(UINT TimeOut);
};
