
#pragma once
#include "./Interface/cDialog.h"
#include "ImageNumber.h"

class cStatic;

class CServerTimeDlg : public cDialog
{
protected:
	cStatic*		m_DateText;

public:
	CServerTimeDlg() ;
	~CServerTimeDlg() ;

	void		 	Linking();
	virtual void 	SetActive( BOOL val );
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void SetTime(DWORD year, DWORD month, DWORD day, DWORD hour, DWORD Minute);
	virtual void 	Render();

};