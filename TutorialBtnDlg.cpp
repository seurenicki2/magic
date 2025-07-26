#include "stdafx.h"
#include "tutorialbtndlg.h"
#include "../WindowIDEnum.h"
#include "Interface/cButton.h"
#include "TutorialDlg.h"
#include "GameIn.h"
#include "Audio/MHAudioManager.h"
#include "TutorialManager.h"
#include "OptionManager.h"

cTutorialBtnDlg::cTutorialBtnDlg(void)
{
	m_pButton = NULL ;														// ��ư �����͸� null ó���� �Ѵ�.

	m_nAlpha = 255 ;														// ��ư ���� ���� 255�� �����Ѵ�.
}

cTutorialBtnDlg::~cTutorialBtnDlg(void)
{}

void cTutorialBtnDlg::Linking()
{
	m_pButton = (cButton*)GetWindowForID(TTR_NOTICEBTN);
}

void cTutorialBtnDlg::OnActionEvent(LONG lId, void* p, DWORD we )
{
	if( lId != TTR_NOTICEBTN ) return ;										// ���� ��ư�� �ƴϸ� return ó���� �Ѵ�.

	cTutorialDlg* pDlg = GAMEIN->GetTutorialDlg() ;							// Ʃ�丮�� ���̾�α׸� ��´�.

	if( !pDlg ) return ;													// Ʃ�丮�� ���̾�αװ� ��ȿ���� ������, 

	pDlg->Start_Tutorial() ;												// Ʃ�丮���� �����Ѵ�.

	SetActive(FALSE) ;														// ���� ��ư ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	m_pButton->SetActive(FALSE) ;											// ��ư�� ��Ȱ��ȭ �Ѵ�.
}

void cTutorialBtnDlg::Render()
{
	if( !IsActive() ) return ;												// Ȱ��ȭ ���°� �ƴϸ�, return ó���� �Ѵ�.

	DWORD dwCurTick = GetTickCount() ;

	DWORD dwResultTime = dwCurTick - m_dwStartTime ;

	if( dwResultTime >= 1000 )
	{
		m_dwStartTime = GetTickCount() ;
	}

	m_nAlpha = (int)(2.5f * (dwResultTime/10)) ;

	m_pButton->SetAlpha((BYTE)m_nAlpha) ;									// ��ư�� ���� ���� �����Ѵ�.

	cDialog::RenderWindow();												// �����츦 �����Ѵ�.
	cDialog::RenderComponent();												// ���� ��ü���� �����Ѵ�.

	//m_pButton->Render() ;													// ��ư�� �����Ѵ�.
}

void cTutorialBtnDlg::Active_NoticeBtn()
{
	SetActive(TRUE) ;														// ���̾�α׸� Ȱ��ȭ ��Ų��.

	m_nAlpha = 255 ;														// ��ư ���� ���� 255�� �����Ѵ�.

	m_dwStartTime = GetTickCount() ;										// ���� �ð��� �޴´�.

	m_pButton->SetActive(
		TRUE);
	AUDIOMGR->Play(
		TUTORIALMGR->Get_ShowBtnSndIdx(),
		gHeroID);
}