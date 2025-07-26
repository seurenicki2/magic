#pragma once

#include "interface/cDialog.h"
#include ".\interface\cStatic.h"

class cBrowser;
class cButton ;
class cStatic;

class cHousingWebDlg :
	public cDialog
{
protected:
	cStatic* m_pGambar;
	cStatic* m_pBody;
	cStatic* m_pTitle;
	cBrowser* m_pBrowser;
	WORD	m_typebrowser;
	cButton*		m_pBvote ;
	cButton*		m_pBedit ;
	cButton*		m_pBshop ;
	cButton*		m_pBforum ;
	cButton*		m_pBreport ;
	cButton*		m_pDonate ;
	cButton*		m_pGlobalchat ;

public:
	cHousingWebDlg(void);
	virtual ~cHousingWebDlg(void);

	void Linking();
	void SetActive( BOOL val );
	void SetType( WORD val );
	void Navigate( char* pURL );
	
	void OpenMiniHomePage(DWORD dwOwnerUserIndex, MAPTYPE Mapnum, DWORD ncharidx);
	void OpenBrowser(DWORDEX Code, char* Acccode);
	void OnActionEvent( LONG lId, void* p, DWORD we );
};
