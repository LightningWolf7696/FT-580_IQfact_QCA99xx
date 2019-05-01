#pragma once

#include "../CommFunc.h"


class CParserCableLoss
{
public:
	CParserCableLoss(void);
public:
	~CParserCableLoss(void);

public:
	BOOL ParserIQfactCableLoss(LPSTR lpszRunInfo);
	BOOL ParserART2CableLoss(LPSTR lpszRunInfo);
};
