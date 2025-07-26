// MiniNoteDialog.h: interface for the CMiniNoteDialogWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MININOTEDIALOG_H__11F671E3_42AB_4140_945A_71A1FEABEDDA__INCLUDED_)
#define AFX_MININOTEDIALOG_H__11F671E3_42AB_4140_945A_71A1FEABEDDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"
#include "./Interface/cImage.h"
#include "./input/Mouse.h"

class cStatic;
class cTextArea;
class cButton;
class cEditBox;
class cIconDialog;
class CItem;
class CVirtualItem;

class CMiniNoteDialogWrite : public cDialog  
{
public:

	cTextArea * m_pWNoteText;
	cButton *	m_pSendOkBtn;
	cButton *	m_pSendCancelBtn;
	
	cEditBox *	m_pTitleEdit;
	cEditBox *	m_pMReceiverEdit;
	cStatic *	m_pReceiver;
	cStatic *	m_pPackMoney;
	cButton *	m_pAddMoney;
	cStatic *	m_pTaxMoney;

	cPtrList m_MinNoteCtlListArray;

private:
	int m_CurMiniNoteMode;
	MONEYTYPE m_sendMoneyNya;
	MONEYTYPE m_sendPajakNya;

	//CMouse*			MOUSE;
	cIconDialog*	mIconDialog;
	CVirtualItem*	mSourceItem;
	DWORD			itemDbidxnya;

public:
	CMiniNoteDialogWrite(void);
	virtual ~CMiniNoteDialogWrite(void);
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void SetActive(BOOL val);
	void OnActionEvent( LONG lId, void* p, DWORD we );
	void OnActionSend();
	static void OnInputMoney( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 );
	static void OnInputCancel( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 );
	void SetMoney(MONEYTYPE);
	void MasukinDuit();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );
	void Linking();
	void SetMiniNote(char* Sender, char* Note, WORD ItemIdx);
	int GetCurMode();
};

#endif // !defined(AFX_MININOTEDIALOG_H__11F671E3_42AB_4140_945A_71A1FEABEDDA__INCLUDED_)
