#include "StdAfx.h"
#include "WindowIDEnum.h"
#include "interface\cFont.h"
#include "Interface\cListDialog.h"
#include "interface\cWindowManager.h"
#include "Audio/MHAudioManager.h"
#include "TutorialManager.h"
#include "Tutorial.h"
#include "Tutorial_Msg.h"
#include "Tutorialdlg.h"
#include "cImageSelf.h"
#include "MHFile.h"

const DWORD tutorialFontIndex = 7;

cTutorialDlg::cTutorialDlg(void) :
mCharacterSizePerLine(0),
mInvisibleTick(UINT_MAX),
mWishedAlpha(255),
m_NpcImage(new cImageSelf)
{
	SHORT_RECT rect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&rect,
		&value,
		&value);
	m_nDispWidth  = rect.right;
	m_nDispHeight = rect.bottom;

	m_nBtnStartXpos = 0 ;													// ��ư ���� x��ǥ�� 0���� �����Ѵ�.
	m_nBtnStartYpos = 0 ;													// ��ư ���� y��ǥ�� 0���� �����Ѵ�.

	m_vImagePos.x = 0 ;														// ��� ��ġ x �� 0���� �����Ѵ�.
	m_vImagePos.y = 0 ;														// ��� ��ġ y �� 0���� �����Ѵ�.
	m_pSpeechList	= NULL ;												// Ʃ�丮�� ��� ����Ʈ ���̾�α׸� null ó���Ѵ�.

	m_pOkBtn		= NULL ;												// Ȯ�� ��ư �����͸� null ó���Ѵ�.
	m_pPrevBtn		= NULL ;												// ���� ��ư �����͸� null ó���Ѵ�.
	m_pNextBtn		= NULL ;												// ���� ��ư �����͸� null ó���Ѵ�.
	m_pPassBtn		= NULL ;												// �н� ��ư �����͸� null ó���Ѵ�.

	m_pSpeechBack	= NULL ;												// ��� ��� ����ƽ �����͸� null ó���Ѵ�.

	m_pTutorial = NULL ;													// ��� Ʃ�丮�� �����͸� null ó���� �Ѵ�.
	m_pCurSubTutorial = NULL ;												// ��� ���� ���� Ʃ�丮�� �����͸� null ó���� �Ѵ�.
	m_nCurSubIndex = 0 ;													// ���� ���� Ʃ�丮�� �ε����� 0���� �����Ѵ�.
	m_nTotalSubCount = 0 ;													// �� ���� Ʃ�丮�� ī��Ʈ�� 0���� �����Ѵ�.
}

cTutorialDlg::~cTutorialDlg(void)
{
	SAFE_DELETE(m_NpcImage);
}

void cTutorialDlg::Linking() 												// ���̾�α� ���� ��Ʈ�ѵ��� ��ũ�ϴ� �Լ�.
{
	m_pSpeechList = (cListDialog*)GetWindowForID(TTR_SPEECH_LIST) ;			// ��� ����Ʈ ���̾�α׸� ��ũ�Ѵ�.
	m_pSpeechList->SetStyle(LDS_FONTSHADOW) ;								// �׸��� ��µǴ� ����Ʈ ��Ÿ�Ϸ� �����Ѵ�.
	m_pSpeechList->Set_ShadowFontIndex(tutorialFontIndex) ;					// ��� ��Ʈ�� �����Ѵ�.
	m_pSpeechList->Set_ShadowDistance(2) ;									// �׸��� �Ÿ��� 2�� �����Ѵ�.

	m_pOkBtn = GetWindowForID(TTR_OKBTN);
	m_pOkBtn->SetActive(FALSE);

	m_pPrevBtn = GetWindowForID(TTR_PREVBTN);
	m_pPrevBtn->SetActive(FALSE);

	m_pNextBtn = GetWindowForID(TTR_NEXTBTN);
	m_pNextBtn->SetActive(FALSE);

	m_pPassBtn = GetWindowForID(TTR_PASSBTN);
	m_pPassBtn->SetActive(FALSE);

	m_pSpeechBack = GetWindowForID(TTR_SP_BACK);
	m_pSpeechBack->SetActive(FALSE);

	// 100111 LUJ, ���κ� ǥ�� ������ ���� ������ ��´�
	cImageRect* const backImageRect = m_pSpeechBack->GetBasicImage()->GetImageRect();
	mCharacterSizePerLine = abs(backImageRect->right - backImageRect->left) / CFONT_OBJ->GetTextExtentWidth(tutorialFontIndex, "?", 1);
}





//=========================================================================
//		Ʃ�丮�� ���̾�α� ���� �Լ�.
//=========================================================================
void cTutorialDlg::Render()													// ���� �Լ�.
{
	if( !IsActive() ) return ;												// Ȱ��ȭ ���°� �ƴϸ�, return ó���� �Ѵ�.

	if(mWishedAlpha < m_alpha)
	{
		SetAlpha(
			BYTE(max(int(mWishedAlpha), int(m_alpha) - 5)));
		m_pSpeechList->SetAlpha(
			BYTE(max(int(mWishedAlpha), int(m_alpha) - 5)));
	}
	else if(mWishedAlpha > m_alpha)
	{
		SetAlpha(
			BYTE(min(int(mWishedAlpha), int(m_alpha) + 5)));
		m_pSpeechList->SetAlpha(
			BYTE(min(int(mWishedAlpha), int(m_alpha) + 5)));
	}
	else if(mWishedAlpha == 0)
	{
		SetActive(
			FALSE);
		return;
	}

	m_NpcImage->RenderSprite(
		0,
		0,
		0,
		&m_vImagePos,
		RGBA_MAKE(255,255,255, m_alpha));

	cDialog::RenderWindow();												// �����츦 �����Ѵ�.
	cDialog::RenderComponent();												// ���� ��ü���� �����Ѵ�.

	if(mInvisibleTick < gCurTime)
	{
		mWishedAlpha = 0;
	}
}

void cTutorialDlg::OnActionEvent(LONG lId, void* p, DWORD we) 				// ���� �� ��Ʈ�� �̺�Ʈ�� ó�� �� �Լ�.
{
	switch(lId)																// ��Ʈ�� ���̵� Ȯ���Ѵ�.
	{
	case TTR_OKBTN :														// Ȯ�� ��ư�� �����ٸ�,
		{
			if(0 == m_pTutorial)
			{
				SetActive(FALSE);
				break;
			}

			m_pTutorial->Set_Complete(TRUE) ;
			UpdateTutorial() ;												// Ʃ�丮���� �����ϰ�, ������Ʈ �Ѵ�.
			// 090417 ONS Ʃ�丮�� ���̾�α� Off���� 
			TUTORIALMGR->SetOnOffTutorial(FALSE);							
		}
		break ;

	case TTR_PREVBTN :														// ���� ��ư�� �����ٸ�,
		{
			if( m_nCurSubIndex <= 0 )
			{
				// ����ó�� �Ѵ�~~~!!!
				// 0�� ���� ������ ���� ��ư�� ��??!!!
				return ;
			}

			SUB_TUTORIAL* pPrevSubTutorial = NULL ;
			pPrevSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex-1) ;		// ���� ���� Ʃ�丮�� ������ �޴´�.

			if( pPrevSubTutorial )											// ���� ���� Ʃ�丮���� �������,
			{
				--m_nCurSubIndex ;											// ���� Ʃ�丮�� �ε����� �����Ѵ�.
				Prepare_Tutorial() ;										// Ʃ�丮�� ������ �غ��Ѵ�.
			}
		}
		break ;

	case TTR_NEXTBTN :
		{
			SUB_TUTORIAL* pNextSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex+1) ;

			if( pNextSubTutorial )											// ���� ���� Ʃ�丮���� �������,
			{
				++m_nCurSubIndex ;											// ���� Ʃ�丮�� �ε����� �����Ѵ�.
				Prepare_Tutorial() ;										// Ʃ�丮�� ������ �غ��Ѵ�.
			}
		}
		break ;

	case TTR_PASSBTN :														// ��� ��ư�� �����ٸ�,
		{
			if(0 == m_pTutorial)
			{
				SetActive(FALSE);
				break;
			}

			m_pTutorial->Set_Complete(TRUE) ;
			UpdateTutorial() ;												// Ʃ�丮���� �����ϰ�, ������Ʈ �Ѵ�.
			// 090417 ONS Ʃ�丮�� ���̾�α� Off���� 
			TUTORIALMGR->SetOnOffTutorial(FALSE);							

		}
		break ;
	}
}

void cTutorialDlg::Set_TutorialInfo(cTutorial* pTutorial)
{
	m_pTutorial = pTutorial;
}

void cTutorialDlg::Start_Tutorial() 										// Ʃ�丮�� �����Լ�.
{
	m_nCurSubIndex = 0 ;													// ���� ���� Ʃ�丮�� �ε����� 0���� �����Ѵ�.
	m_nTotalSubCount = m_pTutorial->Get_SubTutorial_Count() ;				// ���� Ʃ�丮���� ���� Ʃ�丮�� ī��Ʈ�� �޴´�.

	Prepare_Tutorial();
	SetActive(
		TRUE);
	m_pSpeechBack->SetActive(
		TRUE);

	if(cDialog* dialog = WINDOWMGR->GetWindowForID(NSI_SCRIPTDLG))
	{
		dialog->SetActive(FALSE);
	}
}

void cTutorialDlg::LoadNpcImage()
{
	if(0 == m_pCurSubTutorial)
	{
		return;
	}

	m_NpcImage->Release();

	CMHFile file;
	file.Init("Data/Interface/Windows/NpcImageList.bin","rb");

	while(FALSE == file.IsEOF())
	{
		WORD wIndex  = file.GetWord();											// �ε����� �о���δ�.
		BYTE emotion = file.GetByte();											// ����� �о���δ�.
		char*	szFileName	=	file.GetString();							// ���ϸ��� �����Ѵ�.

		if( strlen(szFileName) == 0 )
		{
			break;
		}

		if( wIndex != m_pCurSubTutorial->nNpc_Idx ) continue ;				// npc �ε����� ���� ������ continue ó���� �Ѵ�.

		if( emotion != m_pCurSubTutorial->nFace_Idx ) continue ;			// emotion �ε����� ���� ������, continue ó���� �Ѵ�.

		char buf[1024] = {0};
		sprintf( buf, "%s%s", "data/interface/2dImage/npcImage/", szFileName);
		m_NpcImage->LoadSprite(buf);

		m_vImagePos.x = (float)(m_nDispWidth - m_NpcImage->GetImageRect()->right);
		m_vImagePos.y = (float)(m_nDispHeight - m_NpcImage->GetImageRect()->bottom);
		break;
	}
}





//=========================================================================
//		Ʃ�丮�� ������ ���� �غ� ó�� �Լ�.	
//=========================================================================
void cTutorialDlg::Prepare_Tutorial() 										// Ʃ�丮�� ������ �̸� �غ��ϴ� �Լ�.
{
	m_pCurSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex) ;		// ���� ���� Ʃ�丮�� ������ �޴´�.

	if( !m_pTutorial )														// Ʃ�丮�� ������ ��ȿ���� ������,
	{
		return;															// return ó���� �Ѵ�.
	}
	else if( !m_pCurSubTutorial )												// ���� Ʃ�丮�� ������ �޴µ� ���� �ߴٸ�,
	{
		#ifdef _GMTOOL_														// GM���� ���,
		char tempBuf[128] = {0, } ;											// ���� ��¿� �ӽ� ���۸� �����Ѵ�.
		sprintf(tempBuf, "Failed to receive subtutorial info : %d", m_pTutorial->Get_Index()) ;			// ���� Ʃ�丮�� �ޱ� ����~!!
		MessageBox(NULL, tempBuf, "cTutorialDlg::Prepare_Tutorial", MB_OK);	// ���� �޽����� �޽��� �ڽ��� ����.
		#endif
		return ;															// return ó���� �Ѵ�.
	}

	cTutorial_Msg* pMsg = TUTORIALMGR->Get_TutorialMsg( m_pCurSubTutorial->nSpeech_Idx );

	if( !pMsg )																// �޽����� �޴µ� �����ߴٸ�,
	{
		return ;															// return ó���� �Ѵ�.
	}

	m_pSpeechList->RemoveAll() ;											// ��� ��� ����Ʈ�� ��� ����.

	for( int count = 0 ; count < pMsg->Get_LineCount() ; ++count )						// �޽��� ���� �� ��ŭ for���� ������.
	{
		MSG_LINE* pMsgLine = pMsg->Get_MsgLine(count) ;								// ī��Ʈ�� �ش��ϴ� �޽��� ������ �޴´�.

		if( !pMsgLine )
		{
			continue ;														// continue ó���� �Ѵ�.
		}

		m_pSpeechList->AddItem(pMsgLine->msg, RGB(255, 255, 255), count) ;	// Ʃ�丮�� ��縦 �߰��Ѵ�.

        if( count != (pMsg->Get_LineCount() - 1) ) continue ;							// ���� ī��Ʈ��, ������ ī��Ʈ�� �ƴϸ� continue ó���� �Ѵ�.

		m_nBtnStartXpos = int(m_pSpeechList->GetAbsX() + 
						  CFONT_OBJ->GetTextWidth(tutorialFontIndex) + 
					      CFONT_OBJ->GetTextExtentWidth(tutorialFontIndex, pMsgLine->msg, strlen(pMsgLine->msg)) ) ;			// ��ư ����� ���� x��ǥ�� ���Ѵ�.

		m_nBtnStartYpos = int(m_pSpeechList->GetAbsY() + 6 + 
						  (CFONT_OBJ->GetTextHeight(tutorialFontIndex) * (pMsg->Get_LineCount() - 1)));
	}

	DeActiveAllBtn();

	switch(m_pCurSubTutorial->nComplete_Condition1)
	{
	case e_CLICK_OKBTN:
		{
			ActiveBtns();
			break;
		}
	case e_OPEN_DIALOG:
		{
			cWindow* pWindow = WINDOWMGR->GetWindowForID(IDSEARCH(m_pCurSubTutorial->szCompleteCondition2)) ;

			if( pWindow )
			{
				pWindow->SetActive(FALSE) ;
			}

			break;
		}
	}

	LoadNpcImage();

	AUDIOMGR->StopAll();
	AUDIOMGR->Play(
		m_pCurSubTutorial->nSound_Idx,
		gHeroID);
}

void cTutorialDlg::DeActiveAllBtn() 										// ��� ��ư�� ��Ȱ��ȭ �ϴ� �Լ�.
{
	m_pOkBtn->SetActive(FALSE) ;											// Ȯ�� ��ư�� ��Ȱ��ȭ �Ѵ�.
	m_pPrevBtn->SetActive(FALSE) ;											// ���� ��ư�� ��Ȱ��ȭ �Ѵ�.
	m_pNextBtn->SetActive(FALSE) ;											// ���� ��ư�� ��Ȱ��ȭ �Ѵ�.
	m_pPassBtn->SetActive(FALSE) ;											// ��� ��ư�� ��Ȱ��ȭ �Ѵ�.
}

void cTutorialDlg::ActiveBtns() 											// ��Ȳ�� �´� ��ư�� Ȱ��ȭ �ϴ� �Լ�.
{
	int nCurSubIndex = m_pCurSubTutorial->nIndex ;							// ���� ���� Ʃ�丮���� �ε����� ���Ѵ�.
	int nLastSubIndex = m_nTotalSubCount-1 ;								// ������ ���� Ʃ�丮���� �ε����� ���Ѵ�.

	int nBtnDistance = 5 ;													// ��ư ���� �Ÿ� ���� 5�� �����Ѵ�.

	if( nCurSubIndex == 0 )													// ���� ���� Ʃ�丮�� �ε����� 0�� ������,
	{
		if( nCurSubIndex == nLastSubIndex )									// ó������ �������̸�,
		{
			m_pOkBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;			// ok ��ư�� ��ġ�� ���,
			m_pOkBtn->SetActive(TRUE) ;										// Ȱ��ȭ ��Ų��.
		}
		else																// �ټ� ������ �� ó���̸�,
		{
			m_pNextBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;		// ok ��ư�� ��ġ�� ���,
			m_pNextBtn->SetActive(TRUE) ;									// Ȱ��ȭ ��Ų��.

			int nSecontX = m_nBtnStartXpos + m_pNextBtn->GetWidth() + nBtnDistance ;	// ���� ��ư�� ���� ��ǥ�� ���Ѵ�.

			m_pPassBtn->SetAbsXY(nSecontX, m_nBtnStartYpos) ;				// ���� ��ư�� ��ġ�� ���,
			m_pPassBtn->SetActive(TRUE) ;									// Ȱ��ȭ ��Ų��.
		}
	}
	else if( nCurSubIndex == nLastSubIndex )								// ���� ���� Ʃ�丮���� ������ Ʃ�丮���̸�,
	{
		if(m_nTotalSubCount > 1)											// ���� Ʃ�丮�� �� �������� 1������ �̻��̸�,
		{
			m_pPrevBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;		// ���� ��ư�� ��ġ�� ���,
			m_pPrevBtn->SetActive(TRUE) ;									// Ȱ��ȭ ��Ų��.

			int nSecontX = m_nBtnStartXpos + m_pPrevBtn->GetWidth() + nBtnDistance ;	// Ȯ�� ��ư�� ��ǥ�� ���Ѵ�.

			m_pOkBtn->SetAbsXY(nSecontX, m_nBtnStartYpos) ;					// Ȯ�� ��ư�� ��ġ�� ���,
			m_pOkBtn->SetActive(TRUE) ;										// Ȱ��ȭ ��Ų��.
		}
		else																// ���� ���� Ʃ�丮�� �������� ������ ���������,
		{
			m_pOkBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;			// Ȯ�� ��ư�� ��ġ�� ���,
			m_pOkBtn->SetActive(TRUE) ;										// Ȱ��ȭ ��Ų��.
		}
	}
	else																	// ù�������� ������ �������� �ƴϸ�,
	{		
		m_pPrevBtn->SetAbsXY(m_nBtnStartXpos, m_nBtnStartYpos) ;			// ���� ��ư�� ��ġ�� ��´�.
		m_pPrevBtn->SetActive(TRUE) ;										// Ȱ��ȭ ��Ų��.
	
		int nSecontX = m_nBtnStartXpos + m_pPrevBtn->GetWidth() + nBtnDistance ;	// ���� ��ư�� ���� ��ǥ�� ���Ѵ�.

		m_pNextBtn->SetAbsXY(nSecontX, m_nBtnStartYpos) ;					// ���� ��ư�� ��ġ�� ���,
		m_pNextBtn->SetActive(TRUE) ;										// Ȱ��ȭ ��Ų��.

		int nThirdX = nSecontX + m_pNextBtn->GetWidth() + nBtnDistance ;	// ��� ��ư�� ���� ��ǥ�� ���Ѵ�.

		m_pPassBtn->SetAbsXY(nThirdX, m_nBtnStartYpos) ;					// ��� ��ư�� ��ġ�� ���,
		m_pPassBtn->SetActive(TRUE) ;										// Ȱ��ȭ ��Ų��.
	}
}





//=========================================================================
//		Ʃ�丮�� ���� ���ǰ� ���� npc ���� üũ�ϴ� �Լ�.
//=========================================================================
BOOL cTutorialDlg::IsSameEndNpc(DWORD dwNpcIdx) 							// Ʃ�丮�� ���� ���ǰ� ���� npc ���� üũ�ϴ� �Լ�.
{
	if( !m_pCurSubTutorial ) return FALSE ;

	int nEndCondition = m_pCurSubTutorial->nComplete_Condition1 ;				// ù ���� �Ϸ� ������ �޴´�.

	if( nEndCondition != e_NPC_TALK )										// �Ϸ� ���� ù��°�� npc�� ��ȭ�� �ƴϸ�,
	{
		return FALSE ;														// false return ó���� �Ѵ�.
	}

	return dwNpcIdx == DWORD(m_pCurSubTutorial->nComplete_Condition2);
}

BOOL cTutorialDlg::IsHaveNextSubTutorial()									// ���� ���� Ʃ�丮���� �ִ��� ���θ� üũ�ϴ� �Լ�.
{
	return 0 < m_pTutorial->Get_SubTutorial(m_nCurSubIndex+1);
}





//=========================================================================
//		���� ���� Ʃ�丮���� �����ϴ� �Լ�.
//=========================================================================
void cTutorialDlg::StartNextSubTutorial() 									// ���� ���� Ʃ�丮���� �����ϴ� �Լ�.
{
	SUB_TUTORIAL* pNextSubTutorial = m_pTutorial->Get_SubTutorial(m_nCurSubIndex+1) ;		// ���� ���� Ʃ�丮�� ������ �޴´�.

	if( pNextSubTutorial )													// ���� ���� Ʃ�丮���� �������,
	{
		++m_nCurSubIndex ;													// ���� Ʃ�丮�� �ε����� �����Ѵ�.
		Prepare_Tutorial() ;												// Ʃ�丮�� ������ �غ��Ѵ�.
	}
}





//=========================================================================
//		���� ���� Ʃ�丮���� �����ϴ� �Լ�.
//=========================================================================
void cTutorialDlg::UpdateTutorial() 										// Ʃ�丮���� �Ϸ��ϰ� ������Ʈ �ϴ� �Լ�.
{
	SetActive(
		FALSE);
	TUTORIALMGR->UpdateTutorial(
		m_pTutorial->Get_Index());
}

SUB_TUTORIAL* cTutorialDlg::GetCurSubTutorial()
{
	return m_pCurSubTutorial;
}

void cTutorialDlg::SetActive(BOOL isActive)
{
	if(FALSE == isActive)
	{
		mInvisibleTick = UINT_MAX;
		mWishedAlpha = 255;

		SetAlpha(
			255);
	}

	cDialog::SetActive(isActive);

	cDialog* const tutorialNoticeDialog = WINDOWMGR->GetWindowForID(TUTORIALBTN_DLG);

	if(tutorialNoticeDialog)
	{
		tutorialNoticeDialog->SetActive(! isActive);
	}
}

void cTutorialDlg::SetNpcImage(LPCTSTR fileName)
{
	TCHAR path[MAX_PATH] = {0};
	_stprintf(
		path,
		"data/interface/2dImage/npcImage/%s",
		fileName);

	m_NpcImage->Release();
	m_NpcImage->LoadSprite(
		path);
	m_vImagePos.x = (float)(m_nDispWidth - m_NpcImage->GetImageRect()->right);
	m_vImagePos.y = (float)(m_nDispHeight - m_NpcImage->GetImageRect()->bottom);	
}

void cTutorialDlg::SetText(LPCTSTR text)
{
	if(0 == _tcslen(text))
	{
		mWishedAlpha = 0;
		return;
	}

	m_pSpeechList->RemoveAll();

	// 110111 LUJ, ������ �������� ������ �޸� ��Ʈ�� �߻��Ѵ�
	TCHAR itemText[MAX_LISTITEM_SIZE / 2] = {0};

	do
	{
		ZeroMemory(
			itemText,
			sizeof(itemText));
		_mbsnbcpy(
			PUCHAR(itemText),
			PUCHAR(text),
			mCharacterSizePerLine);
		m_pSpeechList->AddItem(
			LPTSTR(itemText),
			RGB(255, 255, 255));
		text += _tcslen(itemText);
	} while (*text);

	if(FALSE == IsActive())
	{
		SetAlpha(0);
	}

	m_pSpeechBack->SetActive(TRUE);
	mWishedAlpha = 255;
	
	DeActiveAllBtn();

	m_pOkBtn->SetAbsXY(
		LONG(m_pSpeechList->GetAbsX() + CFONT_OBJ->GetTextWidth(tutorialFontIndex) + CFONT_OBJ->GetTextExtentWidth(tutorialFontIndex, LPTSTR(itemText), strlen(itemText))),
		LONG(m_pSpeechList->GetAbsY() + 6 + CFONT_OBJ->GetTextHeight(tutorialFontIndex) * (m_pSpeechList->GetItemCount() - 1)));
	m_pOkBtn->SetActive(TRUE);
}

void cTutorialDlg::SetInvisibleSecond(DWORD second)
{
	mInvisibleTick = gCurTime + second * 1000;
}