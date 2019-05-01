// IQfact_QCA99xx.h : IQfact_QCA99xx DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"		// 主要符號
#include "CommFunc.h"

#include ".\MyClass\LoadDutRequired.h"
#include ".\MyClass\WiFiInitialize.h"
#include ".\MyClass\WiFiTerminate.h"
#include ".\MyClass\IQfact_Test.h"
#include ".\MyClass\PreLoadImage.h"
#include ".\MyClass\WriteTestData.h"
#include ".\MyClass\ParserCableLoss.h"
#include ".\MyClass\VerifyTestData.h"
#include ".\MyClass\ART2_Test.h"

//BT

#include ".\MyClass\BtInitialize.h"
#include ".\MyClass\BtStartFrameModulatedTx.h"
#include ".\MyClass\BtStopFrameModulatedTx.h"
#include ".\MyClass\BtStartContModulatedTx.h"
#include ".\MyClass\BtStopContModulatedTx.h"
#include ".\MyClass\BtGetTargetPower.h"
#include ".\MyClass\BtVerifyTestData.h"
#include ".\MyClass\BtWriteTestData.h"

#include ".\MyClass\StartContModulatedTx.h"
#include ".\MyClass\StopContModulatedTx.h"


// CIQfact_QCA99xxApp
// 這個類別的實作請參閱 IQfact_QCA99xx.cpp
//

class CIQfact_QCA99xxApp : public CWinApp
{
public:
	CIQfact_QCA99xxApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
public:
	CPreLoadImage		m_PreLoadImage;
	CWiFiInitialize		m_WiFiInitialize;
	CLoadDutRequired	m_LoadDutRequired;
	CIQfact_Test		m_IQfact_Test;
	CWriteTestData		m_WriteTestData;
	CParserCableLoss	m_ParserCableLoss;
	CVerifyTestData     m_VerifyTestData;
	CART2_Test			m_ART2_Test;
	CWiFiTerminate		m_WiFiTerminate;
	//bt
	CBtInitialize				m_BtInitialize;
	CBtStartFrameModulatedTx	m_BtStartFrameModulatedTx;
	CBtStopFrameModulatedTx		m_BtStopFrameModulatedTx;
	CBtStartContModulatedTx     m_BtStartContModulatedTx;
	CBtStopContModulatedTx      m_BtStopContModulatedTx;
	CBtGetTargetPower			m_BtGetTargetPower;
	CBtVerifyTestData			m_BtVerifyTestData;
	CBtWriteTestData			m_BtWriteTestData;
	
	CStartContModulatedTx		 m_StartContModulatedTx;
	CStopContModulatedTx		 m_StopContModulatedTx;

public:
	POWER_TABLE		m_PwrTable;
	long	m_Sleep;
};
