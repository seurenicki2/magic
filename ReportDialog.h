#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"
#include "./Interface/cImage.h"
#include "./input/Mouse.h"


class cStatic;
class cTextArea;
class cEditBox;

class CReportDialog : public cDialog
{
private:
	cTextArea * m_pWReportText;
	cEditBox *	m_pSubjectEdit;
	cEditBox *	m_pTitleEdit;

public:
	CReportDialog();
	virtual ~CReportDialog();
	
	void Linking();
	virtual void SetActive( BOOL val );
	//virtual DWORD ActionEvent(CMouse*);
	virtual void OnActionEvent(LONG id, LPVOID, DWORD event );

private:
	DWORD mPlastsentReport;
};