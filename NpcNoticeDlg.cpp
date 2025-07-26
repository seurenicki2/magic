#include "stdafx.h"
#include "NpcNoticeDlg.h"
#include "WindowIDEnum.h"
#include "NpcScriptManager.h"
#include "NpcImageDlg.h"
#include "cDialogueList.h"
#include "cPage.h"
#include "GameIn.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"

cNpcNoticeDlg::cNpcNoticeDlg()
{
	m_pListDlg = NULL;
}

cNpcNoticeDlg::~cNpcNoticeDlg()
{
}

void cNpcNoticeDlg::Linking()
{
	m_pListDlg = (cListDialogEx*)this->GetWindowForID( NND_LISTDLG );
}

BOOL cNpcNoticeDlg::OpenDialog( DWORD dwNpcId, WORD npcChxNum )
{
	cPage* pMainPage = NPCSCRIPTMGR->GetMainPage( dwNpcId );
	if( pMainPage == NULL ) return FALSE;

	cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();
	if( pList == NULL ) return FALSE;

	DWORD dwMsg = pMainPage->GetRandomDialogue();

	m_pListDlg->RemoveAll();															// ��� ����Ʈ�� ��� ����.

	WORD wIdx = 0;																		// �ε����� ���� ������ �����ϰ� 0���� �����Ѵ�.

	WORD LineInfo = 0;																	// ���� ������ ���� ������ �����ϰ� 0���� �����Ѵ�.

	DIALOGUE* temp = NULL;																// ���̾�α� ������ ���� ������ ������ �����ϰ� NULLó���� �Ѵ�.

	LINKITEM* pItem = NULL;																// ��ũ ������ ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.
	LINKITEM* Prev = NULL;																// ���� ��ũ ������ ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.

	while(1)																			// while���� ������.
	{
		temp = pList->GetDialogue( dwMsg, wIdx );										// �����ϰ� ���� �������� ���̾�α� ������ �޴´�.

		if( temp == NULL ) break;														// ������ ��ȿ���� ������, while���� ����������.

		pItem = new LINKITEM;															// ������ �޸𸮸� �Ҵ��Ͽ� �����ͷ� �ѱ��.

		strcpy(pItem->string, temp->str);												// �ӽ� ������ ��Ʈ���� ������ ��Ʈ���� ���縦 �Ѵ�.

		pItem->rgb = temp->dwColor;														// �ӽ� ������ ������ ������ �������� �����Ѵ�.

		//m_pConversationList.AddTail(pItem) ;

		if( Prev == NULL )																// ���� ������ ������ ���ٸ�,
		{
			m_pListDlg->cListItem::AddItem(pItem);										// ������ ������ ��� ����Ʈ�� �߰��Ѵ�.

			LineInfo = temp->wLine;														// ���������� �ӽ� ������ �������� �����Ѵ�.
		}
		else																			// ���� ������ ������ �ִٸ�,
		{
			if( LineInfo == temp->wLine )												// ���� ������ �ӽ� ���ΰ� ������,
			{
				Prev->NextItem = pItem;													// ���� �������� ���� ���������� ���� �������� �����Ѵ�.
			}
			else																		// ���� ������ �ӽ� ���ΰ� ���� ������,
			{
				m_pListDlg->cListItem::AddItem(pItem);									// ��� ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.

				LineInfo = temp->wLine;													// ���� ������ �ӽ� �������� �����Ѵ�.
			}
		}
		
		Prev = pItem;																	// ���� ������ ������ ���� ������ ������ �����Ѵ�.

		++wIdx;																			// �ε��� ������ �����Ѵ�.
	}

	m_pListDlg->ResetGuageBarPos() ;													// ��ȭ ����Ʈ�� �������� ��ġ�� �ٽ� ��´�.

	GAMEIN->GetNpcImageDlg()->SetNpc( npcChxNum );										// NPC �̹��� ���̾�α׿� ���� NPC�� �����Ѵ�.
 	GAMEIN->GetNpcImageDlg()->SetEmotion( pMainPage->GetEmotion() );					// NPC�� �̸���� �����Ѵ�.
	GAMEIN->GetNpcImageDlg()->SetActive( TRUE );										// NPC �̹��� ���̾�α׸� Ȱ��ȭ �Ѵ�.
	GAMEIN->GetNpcImageDlg()->SetDisable( TRUE );										// NPC �̹��� ���̾�α׸� �𽺿��̺� ��Ų��.

	SetActive( TRUE );

	return TRUE;
}

void cNpcNoticeDlg::SetActive( BOOL val )
{
	if( !val )
	{
		BOOL chk = FALSE;

		chk = ( chk || ( ( cDialog* )GAMEIN->GetStorageDialog() )->IsActive() );
		chk = ( chk || ( ( cDialog* )GAMEIN->GetDealDialog() )->IsActive() );
		chk = ( chk || ( ( cDialog* )GAMEIN->GetGuildWarehouseDlg() )->IsActive() );

		if( !chk )
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);						// ������Ʈ ���¸� �����Ѵ�.
		}

		NPCSCRIPTMGR->SetSelectedNpc( NULL );												// ������ NPC�� �����Ѵ�.

		m_pListDlg->RemoveAll();															// ����Ʈ ���̾�α׸� ��� ����.

		GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );										// NPC �̹��� ���̾�α��� Disable�� �����Ѵ�.
		GAMEIN->GetNpcImageDlg()->SetActive( FALSE );										// NPC �̹��� ���̾�α׸� ��Ȱ��ȭ �Ѵ�.
	}
	else
	{
	}

	cDialog::SetActiveRecursive( val );
}