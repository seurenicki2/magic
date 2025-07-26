#include "stdafx.h"
#include "ReportDialog.h"
#include "cWindowManager.h"
#include "WindowIDEnum.h"
#include ".\interface\cTextArea.h"
#include ".\interface\cEditBox.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "FilteringTable.h"
#include "MHTimemanager.h"
#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"
#include "../Interface/cPushupButton.h"
#include "cStatic.h"
#include "cScriptManager.h"
#include "hero.h"
#include "GameIn.h"

CReportDialog::CReportDialog()
{
	m_type = WT_REPORT_DLG;
	mPlastsentReport = 0;

}

CReportDialog::~CReportDialog()
{
	
}

void CReportDialog::Linking()
{
	m_pSubjectEdit = (cEditBox*)GetWindowForID(REPORT_MWSUBJECT);
	m_pTitleEdit = (cEditBox*)GetWindowForID(REPORT_MWTITLE);
	m_pWReportText = (cTextArea*)GetWindowForID(REPORT_TEXTAREA_MSG);
	m_pSubjectEdit->SetEditText("");
	m_pTitleEdit->SetEditText("");
	m_pWReportText->SetEnterAllow(FALSE);
	mPlastsentReport = 0;
}

void CReportDialog::SetActive(BOOL val)
{
	//if( m_bDisable ) return;

	if(val == TRUE)
	{
		cDialog::SetActive(val);

		m_pSubjectEdit->SetEditText("");
		m_pTitleEdit->SetEditText("");
		m_pWReportText->SetScriptText("");
	} else {
		m_pSubjectEdit->SetFocusEdit(FALSE);
		m_pTitleEdit->SetFocusEdit(FALSE);
		m_pWReportText->SetFocusEdit(FALSE);
		cDialog::SetActive(val);
	}
}

/*
DWORD CReportDialog::ActionEvent(CMouse * mouseInfo)
{
	if(IsDisable())
	{
		return WE_NULL;
	}

	const DWORD we = cDialog::ActionEvent( mouseInfo );

	return we ;
}
*/

void CReportDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	if( event & WE_BTNCLICK )
	{
		switch(id)
		{
		case REPORT_BTN_OK:
			{
				if (mPlastsentReport > gCurTime && mPlastsentReport != 0)
				{
					CHATMGR->AddMsg(CTC_SYSMSG, "Wait 5 minutes, to sent another report!");
					return;
				}

				char psubject[MAX_NAME_LENGTH+1] = {0};
				SafeStrCpy(psubject, m_pSubjectEdit->GetEditText(), MAX_NAME_LENGTH+1);
				if(strlen(psubject) == 0)
				return;

				char ptitle[MAX_NOTE_TITLE+1] = {0,};
				SafeStrCpy(ptitle, m_pTitleEdit->GetEditText(), MAX_NOTE_TITLE+1);
				if(strlen(ptitle) == 0)
				return;

				char FullText[512];
				m_pWReportText->GetScriptText(FullText);

				char reportText[MAX_NOTE_LENGTH];
				SafeStrCpy(reportText, FullText, MAX_NOTE_LENGTH - 1);
				reportText[MAX_NOTE_LENGTH - 1] = 0;

				if(strlen(reportText) == 0)
				return;

				//NOTEMGR->WriteNote(Note, ptitle, pname, itemDbidxnya, positionnya, kirimduit);
				MSG_REPORT_SEND_TOGM msg;
				msg.Category = MP_NOTE;
				msg.Protocol = MP_NOTE_SENDREPORT_SYN_TOGM;
				msg.dwObjectID = HERO->GetID();
				SafeStrCpy(msg.Subject, psubject, MAX_NOTE_TITLE+1);
				SafeStrCpy(msg.Title, ptitle, MAX_NOTE_TITLE - 1);
				SafeStrCpy(msg.Report, reportText, MAX_NOTE_LENGTH - 1);	

				NETWORK->Send(&msg, msg.GetMsgLength());

				mPlastsentReport = gCurTime+300000;

				CReportDialog::SetActive(FALSE);

			} break;
		case REPORT_BTN_CANCEL:
			{
				CReportDialog::SetActive(FALSE);
			} break;
		case REPORT_CLOSEBTN:
			{
				CReportDialog::SetActive(FALSE);
			} break;
		}
	}
}
