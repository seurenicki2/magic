#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include ".\interface\cdialog.h"

class cButton ;
class cStatic ;
class cEditBox ;
class cTextArea ;

class CChaptchaDlg : public cDialog
{
protected :
	cButton*	m_pBtnOk ;
	cButton*	m_pBtnCancel ;
	cStatic*	m_pRanNum ;
	cStatic*	m_pTimer;
	cEditBox*	m_pEditBox ;
	cTextArea*	m_pText ;
	//cStatic*	m_pTitle ;
	int m_nRanNum  ;
	BOOL isGameCalled;
	DWORD m_secnumber;
	DWORD m_captchatype;
	char itemname[128];
	char fullmsg[256];
	//Alemuri----------
	int attempts;
	//-----------------

public:
	CChaptchaDlg(void);
	virtual ~CChaptchaDlg(void);
	void Linking() ;
	virtual void SetActive( BOOL val );

	void OnActionEvent( LONG lId, void* p, DWORD we ) ;
	void MakeRandomNumber() ;
	
	void SetTypeCaptcha( DWORD type ) { m_captchatype = type ; }
	void SetTypeGame( BOOL typegame ) { isGameCalled = typegame ; }
	void SetSecNumber( DWORD number ); // { m_secnumber = number ; }
	void SetTimer(WORD timex);
	void SetItemnameTodelete( char* itemnamex );// { itemname = itemnamex ; }
	void SetFullmsg( char* msg );
};