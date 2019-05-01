#pragma once

#include "../CommFunc.h"


class CBtInitialize
{
public:
	CBtInitialize(void);
public:
	~CBtInitialize(void);

public:
	enum _ITEM{
		PIPE_INPUT_TEST_CMD =0,
		CONSOLE_KEYWORD,
		CONSOLE_INPUT_TEST_CMD,

	};
public:
	typedef struct FLAG{
		BOOL PIPE_INPUT_TEST_CMD;
		BOOL CONSOLE_KEYWORD;
		BOOL CONSOLE_INPUT_TEST_CMD;
	
	}FLAG;

public:
	TCHAR szMessage[DLL_INFO_SIZE];
	BOOL res;
	FLAG Flag;
	CString szWorkName;


public:
	void Clean();
	BOOL START(BOOL bRetry, LPSTR lpszRunInfo);
};
