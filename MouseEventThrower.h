// MouseEventThrower.h: interface for the CMouseEventThrower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOUSEEVENTTHROWER_H__CA64761F_AD68_448B_AA54_5394A0467267__INCLUDED_)
#define AFX_MOUSEEVENTTHROWER_H__CA64761F_AD68_448B_AA54_5394A0467267__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "input/Mouse.h"

class CMouseEventReceiver;

class CMouseEventThrower  
{
	CMouseEventReceiver* m_pDefaultReceiver;
	CMouseEventReceiver* m_pCurrentReceiver;
	// 100121 ShinJS --- ���콺 Ŭ�� ó�� ��� ���� �߰�
	BOOL	m_bAllowMouseClick;

//KES 031017
protected:
	BOOL	m_bCancelSelect;
	
public:
	CMouseEventThrower();
	virtual ~CMouseEventThrower();

	void SetDefaultReceiver(CMouseEventReceiver* pDefaultReceiver);
	void SetCurrentReceiver(CMouseEventReceiver* pCurrentReceiver);
	void SetReceiverToDefault();

	void Process(CMouse* Mouse);
//---KES ���󰡱�
	void Follow();
	BOOL IsAllowMouseClick() const { return m_bAllowMouseClick; }
	void SetAllowMouseClick( BOOL bAllow ) { m_bAllowMouseClick = bAllow; }
};

#endif // !defined(AFX_MOUSEEVENTTHROWER_H__CA64761F_AD68_448B_AA54_5394A0467267__INCLUDED_)
