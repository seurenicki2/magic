#pragma once

#include "./Interface/cDialog.h"


class cStatic;
class cButton;
class cImage;

class CSlotDialog : public cDialog
{
	cButton*	m_pSpinBtn;
	cButton*	m_pBetminBtn;
	cButton*	m_pBet2xBtn;
	cButton*	m_pBet3xBtn;
	cButton*	m_pBetmaxBtn;
	cButton*	m_pCancelBtn;
	cButton*	m_pCloseBtn;
	cStatic*	m_pDeposit;
	cStatic*	m_pWinmoney;
	cStatic*	m_pCharmoney;
	cStatic*	m_pInfowin;
	DWORD		m_Depositvalue;
	BOOL		m_pWait;

	cStatic*	m_pSpin1;
	cStatic*	m_pSpin2;
	cStatic*	m_pSpin3;
	cStatic*	m_pSpin4;

	cStatic*	m_pJackpot;

public:
	CSlotDialog();
	virtual ~CSlotDialog();
	
	void Linking();
	virtual void SetActive( BOOL val );
	virtual DWORD ActionEvent(CMouse*);
	virtual void OnActionEvent(LONG id, LPVOID, DWORD event );
	virtual void Prosses();
	void SetResultSpin(MONEYTYPE Moneyresult, WORD slot1, WORD slot2, WORD slot3, WORD slot4, MONEYTYPE totjackpot, WORD wincode);
	void UpdateJackpot(MONEYTYPE mJackpot);
};