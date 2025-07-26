#include "stdafx.h"
#include "WindowIDEnum.h"
#include "GameIn.h"
#include "Quest.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "ChatManager.h"
#include "QuestManager.h"
#include "NpcScriptManager.h"
#include "Interface/cWindowManager.h"
#include "Interface/cStatic.h"
#include "Input/UserInput.h"
#include "cMsgBox.h"
#include "cDialogueList.h"
#include "cHyperTextList.h"
#include "NpcImageDlg.h"
#include "NpcScriptDialog.h"
#include "QuestDialog.h"
#include "./Interface/GameNotifyManager.h"
#include "FishingPointDialog.h"
#include "interface/cFont.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "SiegeWarfareMgr.h"
#include "MHMap.h"
#include "cMapMoveDialog.h"
#include "cResourceManager.h"
#include "./NpcRecallMgr.h"
#include "../[CC]Header/GameResourceManager.h"
#include "./Interface/cIcon.h"														// ������ Ŭ���� ����� �ҷ��´�.
#include "./Interface/cIconGridDialog.h"											// ������ �׸��� ���̾�α� Ŭ���� ����� �ҷ��´�.
#include "./Interface/cScriptManager.h"												// ��ũ��Ʈ �Ŵ��� Ŭ���� ����� �ҷ��´�.
#include "./Interface/cFont.h"														// ��Ʈ Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestString.h"												// ����Ʈ ��Ʈ�� Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestTrigger.h"
#include "..\[CC]Quest\QuestExecute_Item.h"											// ����Ʈ ���� ������ Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestExecute_SelectItem.h"											// ����Ʈ ���� ������ Ŭ���� ����� �ҷ��´�.
#include "itemmanager.h"
#include "QuestQuickViewDialog.h"
#include "ChattingDlg.h"
#include "cPage.h"
#include "cListDialogEx.h"
#include "StreetTournamentMgr.h"

extern HWND _g_hWnd;

#define MAX_FIX_REQUITAL_ICON_CELL 5
#define MAX_SELECT_REQUITAL_ICON_CELL 10

cNpcScriptDialog::cNpcScriptDialog()													// ������ �Լ�.
{
	m_type = WT_NPCSCRIPDIALOG;
	m_nHyperCount = 0;																	// ������ ī��Ʈ�� 0���� �����Ѵ�.
	m_nConversationHyperCount = 0 ;		
	m_dwCurNpc = 0;
	m_wNpcChxNum = 0;
	m_pListDlg = NULL;
	m_pLinkListDlg = NULL;
	m_pQuestBtn = NULL;
	m_pTitleText = NULL;
	m_dwQuestIdx = 0;
	m_dwMainLinkPageId = 0;

	// 100414 ONS ����Ʈ������� ���� �ʱ�ȭ
	m_pRequitalInfoDlg			= NULL;
	m_pFixRequitalGrid			= NULL;
	m_pSelectRequitalGrid		= NULL;
	m_pRequitalQuestName		= NULL;
	m_pRequitalGold				= NULL;
	m_pRequitalExp				= NULL;
	m_RequitalItemTooltipIdx	= -1 ;
	m_RequitalIndex				= 0;
	m_RequitalCount				= 0;

	for(int i=0 ; i< MAX_REGIST_HYPERLINK;++i)											// �ִ� ��� ������ ��ũ �� ��ŭ for���� ������.
	{
		m_sHyper[i].Init();																// ī��Ʈ�� ���� ������ ��ũ ������ �ʱ�ȭ �Ѵ�.
	}

	for(int i=0 ; i< MAX_REGIST_HYPERLINK;++i)											// �ִ� ��� ������ ��ũ �� ��ŭ for���� ������.
	{
		m_sConversationHyper[i].Init() ;												// ī��Ʈ�� ���� ������ ��ũ ������ �ʱ�ȭ �Ѵ�.
	}


}

cNpcScriptDialog::~cNpcScriptDialog()													// �Ҹ��� �Լ�.
{
	m_pListDlg->RemoveAll() ;
	m_pLinkListDlg->RemoveAll() ;

	// 100414 ONS ����Ʈ ������� �޸� ���� 
	RequitalItem* pDeleteItem = NULL ;
	PTRLISTPOS deletePos = NULL ;
	deletePos = m_QuestFixRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestFixRequitalList.GetNext(deletePos) ;
		if( pDeleteItem )
		{
			m_QuestFixRequitalList.Remove(pDeleteItem) ;
			delete pDeleteItem ;
		}
	}
	m_QuestFixRequitalList.RemoveAll() ;

	deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
		if( pDeleteItem )
		{
			m_QuestSelectRequitalList.Remove(pDeleteItem) ;
			delete pDeleteItem ;
		}
	}
	m_QuestSelectRequitalList.RemoveAll() ;

	m_IndexGenerator.Release();
}

void cNpcScriptDialog::Linking()														// ��ũ �Լ�.
{
	m_pListDlg = (cListDialogEx*)this->GetWindowForID( NSI_LISTDLG );
	m_pLinkListDlg = (cListDialogEx*)this->GetWindowForID(NSI_LISTDLG2);
	m_pQuestBtn = (cButton*)GetWindowForID(NSI_QUESTBTN);

	// 100414 ONS ����Ʈ���������� ���̾�α׷� �޾ƿ´�.
	m_pRequitalInfoDlg		= (cDialog*)this->GetWindowForID( NSI_DLG_SELECTITEM );
	m_pRequitalInfoDlg->SetDisable(TRUE);

	// �Ϲݺ���� ���ú��� �׸��� ����.
	m_pFixRequitalGrid		= (cIconGridDialog*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_GRID) ;
	m_pSelectRequitalGrid	= (cIconGridDialog*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_SELECT_GRID) ;

	m_pRequitalQuestName	= (cStatic*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_QUESTNAME);
	m_pRequitalGold			= (cStatic*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_GOLD);
	m_pRequitalExp			= (cStatic*)m_pRequitalInfoDlg->GetWindowForID(NSI_REQUITAL_EXP);

	m_IndexGenerator.Init( 20, IG_SHOPITEM_STARTINDEX );

	m_pTitleText = new cStatic;															// Ÿ��Ʋ �ؽ�Ʈ��ŭ �޸𸮸� �Ҵ��Ѵ�.

	if( m_pTitleText )																	// �޸� �Ҵ� ������ ��ȿ�ϴٸ�,
	{
		m_pTitleText->Init( 15, 15, 0, 0, NULL, -1 );									// Ÿ��Ʋ �ؽ�Ʈ�� �ʱ�ȭ �Ѵ�.

		m_pTitleText->SetShadow( TRUE );												// �׸��� ���θ� TRUE�� �����Ѵ�.

		m_pTitleText->SetFontIdx( 5 );													// 2�� ��Ʈ�� �����Ѵ�.

		m_pTitleText->SetAlign( TXT_LEFT );												// �ؽ�Ʈ ������ ���� ���ķ� �����Ѵ�.
		
		m_pTitleText->SetStaticText( "" );												// �ؽ�Ʈ�� ""�� �����Ѵ�.

		m_pTitleText->SetFGColor( RGBA_MAKE( 170, 236, 4, 255 ) );
		
		Add( m_pTitleText );															// ���̾�α׿� Ÿ��Ʋ�� �߰��Ѵ�.
	}
}

DWORD cNpcScriptDialog::ActionEvent(CMouse* mouseInfo)	
{
	// 100414 ONS ���� �׸����� ������ ������ ǥ���Ѵ�.
	DWORD we = WE_NULL;	
	if( !m_bActive )	
	{
		return we;		
	}

	we = cDialog::ActionEvent(mouseInfo);

	int nMouseX = mouseInfo->GetMouseX() ;
	int nMouseY = mouseInfo->GetMouseY() ;

	if( m_pFixRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pFixRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))
		{
			ShowRequitalItemToolTip(m_QuestFixRequitalList, Pos);		
			return 0;													
		}
	}
	if( m_pSelectRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pSelectRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))
		{
			ShowRequitalItemToolTip(m_QuestSelectRequitalList, Pos);	
			return 0;													
		}
	}

	return we;
}

void cNpcScriptDialog::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch( lId )
	{
	case NSI_TOPBTN :
		{
			OpenDialog(m_dwCurNpc, m_wNpcChxNum) ;
		}
		break ;

	case NSI_ENDBTN :
		{
			EndDialog() ;
			SetActive(FALSE) ;
		}
		break ;

	// 090924 ShinJS --- ����Ʈ ����/�Ϸ� ��ư�� ���� ���
	case NSI_QUESTBTN :
		{
			CQuest* pQuest = QUESTMGR->GetQuest( m_dwQuestIdx ); //alemuri
			if (m_dwQuestIdx >= 9000 && m_dwQuestIdx < 9068)
			{
				questLinkRedirect(m_dwQuestIdx);
				break;
			}

			if( !pQuest )
			{
				// ����Ʈ�� ������ �ȵ� ���¿��� ��οϷ��ư�� ���� ��� ���� �������� ����Ʈ �߿��� �Ϸᰡ���� ����Ʈ�� ��� �Ϸ��Ų��.
				if( m_QuestBtnType == eQuestBtnType_CompleteAll )
				{
					DWORD dwPageId = 0 ;
					if( m_dwMainLinkPageId != 0 )
						dwPageId = m_dwMainLinkPageId ;
					else
						dwPageId = m_dwCurPageId ;

					cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// ���� npc�� ������ ���̵�� ������ ������ ��´�.
					if( pPage == NULL ) break;

					int nLinkCount = pPage->GetHyperLinkCount();										// ��ũ ī��Ʈ�� �����Ѵ�.
					for( int i = 0; i < nLinkCount; ++i )												// ��ũ ī��Ʈ �� ��ŭ for���� ������.
					{
						HYPERLINK* pLink = pPage->GetHyperText(i);										// ������ ��ũ ������ �޴´�.
						if( pLink == NULL ) continue;

						CQuest* pQuest = QUESTMGR->GetQuest( pLink->dwData );							// ����Ʈ ������ �޴´�.
						if( !pQuest ) continue;
						 
						if( pQuest->IsQuestState( m_dwCurNpc ) == eQuestState_Executed &&				// �������� ����Ʈ�� ���
							pQuest->IsPossibleQuestComplete() )											// �Ϸ� ������ ����Ʈ�� ���
						{
							// 100414 ONS ���ú���������� ���� ��� �Ϸ�.
							if( !CanTakeSelectRequital(pLink->dwData) )
							{
								// ����Ʈ�� �Ϸ��Ų��.
								QUESTMGR->SendNpcTalkEvent( m_dwCurNpc, pLink->dwData );
							}
							else
							{
								// ����������� ������ �ش�����Ʈ�� ������������ �����Ѵ�.
								WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214) );

								m_pLinkListDlg->SetActive( FALSE );
								m_pRequitalInfoDlg->SetDisable( FALSE );
								m_pRequitalInfoDlg->SetActive( TRUE );

								OpenRequitalPage( pLink->dwData );

								DWORD dwPage = pQuest->GetNpcScriptPage( m_dwCurNpc );
								OpenLinkPage( dwPage );	

								m_dwQuestIdx = pLink->dwData;
								return;								
							}
						}
					}

					// ����Ʈ Link Page Update
					RefreshQuestLinkPage();

					// �Ϸ� ��ư�� ��Ȱ��ȭ ��Ų��
					m_pQuestBtn->SetActive( FALSE );
				}

				break;
			}
			
			// 100414 ONS �Ϸ��ư Ŭ���� ���ú��� ���ÿ��� �Ǵ�
			if(	!IsRequitalSelected( m_dwQuestIdx )	)
			{
				WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214) );
				return;
			}

			// 100524 ONS ����Ʈ����Ʈ �ִ밳�� üũó�� �߰�.
			CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;
			if( pQuestDialog )
			{
				pQuestDialog->SetQuestCount();
				if( pQuestDialog->GetQuestCount() >= MAX_QUEST_COUNT )
				{
					// �ִ밹���ϰ��, ����Ʈ�� ��Ͽ� ����(������, �Ϸ�)�ϸ� �����Ű����,
					// �׷������� ���(����)�� ����ƮFULL�޼����� ����ϰ� �����Ѵ�.
					if( FALSE == pQuestDialog->IsExistInQuestList( m_dwQuestIdx ) )
					{
						WINDOWMGR->MsgBox( MBI_QUEST_FULL, MBT_OK, CHATMGR->GetChatMsg( 1315 ) );
						return;
					}
				}
			}

			if( m_QuestBtnType == eQuestBtnType_Progress )
			{
				// ����Ʈ�����ư Ŭ���� ��� ������ Open
				OpenQuestLinkPage( m_dwMainLinkPageId );
			}

			// ������ ������ Open
			OpenLastPageOfQuest();

			// ����Ʈ ����/�Ϸ� ��û
			if( m_dwQuestIdx )
			{
				QUESTMGR->SendNpcTalkEvent( m_dwCurNpc, m_dwQuestIdx, m_RequitalIndex, m_RequitalCount );
				m_dwQuestIdx = 0;
			}
			// ����/�Ϸ� ��ư�� ��Ȱ��ȭ ��Ų��
			m_pQuestBtn->SetActive( FALSE );

			// 100414 ONS ������̾�α׸� ��Ȱ��ȭ��Ű�� ����Ʈ����Ʈ�� Ȱ��ȭ�Ѵ�.
			m_pRequitalInfoDlg->SetActive( FALSE );
			m_pLinkListDlg->SetActive( TRUE );
		}
		break;
	}
}

void cNpcScriptDialog::ResetConversationList(cDialogueList* pList, DWORD dwMsg)			// ��ȭâ�� �ٽ� ���� �ϴ� �Լ�.
{
	if( !pList )
	{
		return ;
	}

	m_pListDlg->RemoveAll();

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
}

void cNpcScriptDialog::ResetMainLinkList(cPage* pMainPage, cHyperTextList* pHyper, cListDialogEx* pListDlg)	// ��ũ �������� �ٽ� �����ϴ� �Լ�.
{
	for(int i=0 ; i< MAX_REGIST_HYPERLINK;++i)											// �ִ� ��� ���� ������ ��ũ �� ��ŭ for���� ������.
	{
		m_sHyper[i].Init();																// ī��Ʈ�� �ش��ϴ� ������ ��ũ ������ �ʱ�ȭ �Ѵ�.
	}
	m_pLinkListDlg->RemoveAll() ;														// ��ũ ����Ʈ�� ��� ����.
	m_nHyperCount = 0;																	// ������ ��ũ ī��Ʈ�� 0���� �����Ѵ�.
	DIALOGUE* temp = NULL;																// ���̾�α� ������ ���� ������ ������ �����ϰ� NULLó���� �Ѵ�.
	LINKITEM* pItem = NULL;																// ��ũ ������ ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.
	HYPERLINK* pLink;																	// ��ũ ������ ���� �����͸� �����ϰ� NULLó���� �Ѵ�.
	int nLinkCount = pMainPage->GetHyperLinkCount();									// �������� ��ũ ī��Ʈ�� �޴´�.

	BOOL bQuest = QUESTMGR->IsNpcRelationQuest( m_dwCurNpc );							// ����Ʈ ���θ� �޴´�.

	for( int j = 0 ; j < nLinkCount ; ++j )												// ��ũ ī��Ʈ ��ŭ for���� ������.
	{
		pLink = pMainPage->GetHyperText(j);												// ī��Ʈ�� �ش��ϴ� ��ũ ������ �޴´�.

		if( pLink == NULL )																// ��ũ ������ ��ȿ���� ������ 
		{
			break;																		// for���� Ż���Ѵ�.
		}
		if( pLink->wLinkType == emLink_Quest && !bQuest )								// ��ũ Ÿ���� ����Ʈ�� ����, bQuest�� FALSE�� ������,
		{
			continue;																	// ����Ѵ�.
		}
		temp = pHyper->GetHyperText( pLink->wLinkId );									// ��ũ ���̵�� ���̾�α� ������ �޴´�.
		if( temp )																		// ���̾�α� ������ ��ȿ�ϸ�,
		{
			LINKITEM* pItem = new LINKITEM;												// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
			strcpy(pItem->string, temp->str);											// ������ ��Ʈ���� ���̾�α� ��Ʈ������ �����Ѵ�.
			pItem->dwType = pLink->wLinkType;											// ������ ��ũ Ÿ���� �����Ѵ�.
			pListDlg->cListItem::AddItem( pItem );										// ��ũ ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.
			m_sHyper[m_nHyperCount].bUse = TRUE;										// ������ ��ũ ������ ��뿩�θ� TRUE�� �����Ѵ�.
			m_sHyper[m_nHyperCount].dwListItemIdx = pListDlg->GetItemCount()-1;			// ������ ��ũ ������ ����Ʈ ������ �ε����� �����Ѵ�.
			m_sHyper[m_nHyperCount].sHyper = *pLink;									// ������ ��ũ ������ ��ũ ������ �����Ѵ�.
			++m_nHyperCount;															// ������ ī��Ʈ�� �����Ѵ�.
		}
	}

	pListDlg->ResetGuageBarPos() ;														// ����Ʈ�� �������� ��ġ�� �ٽ� ��´�.

	if(nLinkCount > 0)
		pListDlg->SetFocus(TRUE);
}

void cNpcScriptDialog::ResetConversationLinkList(cPage* pMainPage, cHyperTextList* pHyper)
{
	for(int i=0 ; i< MAX_REGIST_HYPERLINK;++i)											// �ִ� ��� ���� ������ ��ũ �� ��ŭ for���� ������.
	{
		m_sConversationHyper[i].Init();													// ī��Ʈ�� �ش��ϴ� ������ ��ũ ������ �ʱ�ȭ �Ѵ�.
	}

	m_nConversationHyperCount = 0 ;														// ��ȭ ����Ʈ�� ��ũ ī��Ʈ�� 0���� �����Ѵ�.
	DIALOGUE* temp = NULL;																// ���̾�α� ������ ���� ������ ������ �����ϰ� NULLó���� �Ѵ�.
	LINKITEM* pItem = NULL;																// ��ũ ������ ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.
	HYPERLINK* pLink;																	// ��ũ ������ ���� �����͸� �����ϰ� NULLó���� �Ѵ�.

	int nLinkCount = pMainPage->GetHyperLinkCount();									// �������� ��ũ ī��Ʈ�� �޴´�.

	// ���� ������ ���� ����
	BOOL bQuest = QUESTMGR->IsNpcRelationQuest( m_dwCurNpc );							// ����Ʈ ���θ� �޴´�.

	pItem = new LINKITEM;																// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
	strcpy( pItem->string, "" );														// ������ ��Ʈ���� ""�� �����Ѵ�.
	m_pListDlg->cListItem::AddItem( pItem );											// ���� �������� �߰��Ѵ�.

	for( int j = 0 ; j < nLinkCount ; ++j )												// ��ũ ī��Ʈ ��ŭ for���� ������.
	{
		pLink = pMainPage->GetHyperText(j);												// ī��Ʈ�� �ش��ϴ� ��ũ ������ �޴´�.

		if( pLink == NULL )																// ��ũ ������ ��ȿ���� ������ 
		{
			break;																		// for���� Ż���Ѵ�.
		}

		if( pLink->wLinkType == emLink_Quest && !bQuest )								// ��ũ Ÿ���� ����Ʈ�� ����, bQuest�� FALSE�� ������,
		{
			continue;																	// ����Ѵ�.
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// ��ũ ���̵�� ���̾�α� ������ �޴´�.

		if( temp )																		// ���̾�α� ������ ��ȿ�ϸ�,
		{
			LINKITEM* pItem = new LINKITEM;												// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.

			strcpy(pItem->string, temp->str);											// ������ ��Ʈ���� ���̾�α� ��Ʈ������ �����Ѵ�.

			pItem->dwType = pLink->wLinkType;											// ������ ��ũ Ÿ���� �����Ѵ�.
			
			m_pListDlg->cListItem::AddItem( pItem );									// ��ũ ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.

			m_sConversationHyper[m_nConversationHyperCount].bUse = TRUE;										// ������ ��ũ ������ ��뿩�θ� TRUE�� �����Ѵ�.
			m_sConversationHyper[m_nConversationHyperCount].dwListItemIdx = m_pListDlg->GetItemCount()-1;		// ������ ��ũ ������ ����Ʈ ������ �ε����� �����Ѵ�.
			m_sConversationHyper[m_nConversationHyperCount].sHyper = *pLink;									// ������ ��ũ ������ ��ũ ������ �����Ѵ�.

			++m_nConversationHyperCount;												// ������ ī��Ʈ�� �����Ѵ�.
		}
	}

	m_pListDlg->ResetGuageBarPos() ;													//��ȭ ����Ʈ�� �������� ��ġ�� �ٽ� ��´�.

	if(nLinkCount > 0)
		m_pListDlg->SetFocus(TRUE);	
}

void cNpcScriptDialog::SettingNpcImage(cPage* pMainPage, WORD npcChxNum)				// NPC �̹����� ����� �����ϴ� �Լ�.											
{
   	GAMEIN->GetNpcImageDlg()->SetNpc( npcChxNum );										// NPC �̹��� ���̾�α׿� ���� NPC�� �����Ѵ�.
 	GAMEIN->GetNpcImageDlg()->SetEmotion( pMainPage->GetEmotion() );					// NPC�� �̸���� �����Ѵ�.
	GAMEIN->GetNpcImageDlg()->SetActive( TRUE );										// NPC �̹��� ���̾�α׸� Ȱ��ȭ �Ѵ�.
	GAMEIN->GetNpcImageDlg()->SetDisable( TRUE );										// NPC �̹��� ���̾�α׸� �𽺿��̺� ��Ų��.

	CNpc* pObj = NPCSCRIPTMGR->GetSelectedNpc();										// ���� �� NPC ������ �޴´�.

	if( pObj )																			// NPC ������ ��ȿ�ϸ�,
	{
		m_pTitleText->SetStaticText( pObj->GetObjectName() );
	}
	else																				// NPC ������ ��ȿ���� ������,
	{	
 		m_pTitleText->SetStaticText( CHATMGR->GetChatMsg(42) );
		
		CSHFarmRenderObj* pFarmObj = g_csFarmManager.CLI_GetTargetObj();

		if (pFarmObj &&
			pFarmObj->GetInfo()->nKind == CSHFarmManager::RENDER_STEWARD_KIND)
		{
			m_pTitleText->SetStaticText( pFarmObj->GetObjectName() );
 			GAMEIN->GetChattingDlg()->SetActive( TRUE );
 			SetFocus(TRUE);
		}
	}

	SetActive( TRUE );																	// NPC ��ũ��Ʈ ���̾�α׸� Ȱ��ȭ �Ѵ�.
}

BOOL cNpcScriptDialog::OpenDialog( DWORD dwNpcId, WORD npcChxNum )						// ���̾�α׸� ���� �Լ�.
{
	m_dwCurNpc = dwNpcId;																// ���� NPC ���̵� �����Ѵ�.
	m_wNpcChxNum = npcChxNum ;															// ���� NPC CHX ��ȣ�� �����Ѵ�.

	cPage* pMainPage = NPCSCRIPTMGR->GetMainPage( m_dwCurNpc );							// NPC ��ũ��Ʈ �Ŵ����� ���� ���� NPC�� ���� ������ ������ �޴´�.

	if( pMainPage == NULL ) return FALSE;												// ������ ������ ��ȿ���� ������, FALSE ���� ó���� �Ѵ�.

	cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();								// ��Ʈ��Ʈ �Ŵ����� ���� ���̾�α� ������ �޴´�.

	if( pList == NULL ) return FALSE;													// ���̾�α� ������ ��ȿ���� ������ FALSE ���� ó���� �Ѵ�.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// ��ũ��Ʈ �Ŵ����� ���� ������ �ؽ�Ʈ ������ �޴´�.

	if( pHyper == NULL ) return FALSE;

	ResetConversationList(
		pList,
		pMainPage->GetRandomDialogue());
	ResetMainLinkList(
		pMainPage,
		pHyper,
		m_pLinkListDlg);
	MoveCursorToLink();
	SettingNpcImage(
		pMainPage,
		npcChxNum);

	// 100414 ONS ������̾�α׸� ��Ȱ��ȭ�Ѵ�.
	m_pRequitalInfoDlg->SetActive( FALSE );
	m_pRequitalInfoDlg->SetDisable( TRUE );

	return TRUE;																		// TRUE�� �����Ѵ�.
}

BOOL cNpcScriptDialog::OpenLinkPage( DWORD dwPageId )									// ��ũ �������� ���� �Լ�.
{
	m_dwCurPageId = dwPageId;															// �ӽ÷� ���� ǥ�� �������� ������.	

	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// ���� NPC�� ���� ������ ���̵�� ������ ������ �޴´�.

	if( pPage == NULL ) return FALSE;													// ������ ������ ��ȿ���� ���ϸ� FALSE ���� ó���� �Ѵ�.

	cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();								// �޼��� ��ȣ�� �̿��Ͽ� ����� �޼����� �����´�.

	if( pList == NULL ) return FALSE;													// ���̾�α� ������ ��ȿ���� ������, FALSE ���� ó���� �Ѵ�.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// ������ �ؽ�Ʈ ����Ʈ ������ �޴´�.

	if( pHyper == NULL ) return FALSE;													// ������ �ؽ�Ʈ ����Ʈ ������ ��ȿ���� ������ FALSE ���� ó���� �Ѵ�.

	DWORD dwMsg = pPage->GetRandomDialogue();											// ������ ������ �̿��Ͽ� �������� �����Ѵ�. // �޼����� ��ȣ

	ResetConversationList(pList, dwMsg) ;												// ��ȭ ����Ʈ�� �ٽ� �����Ѵ�.
	
	ResetConversationLinkList(pPage, pHyper) ;											// ��ȭ ����Ʈ�� ��ũ�� �����Ѵ�.
	
	GAMEIN->GetNpcImageDlg()->SetEmotion( pPage->GetEmotion() );						// NPC �̹��� ���̾�α��� �̸���� �����Ѵ�.

	return TRUE;																		// TRUE�� �����Ѵ�.
}

void cNpcScriptDialog::EndDialog()														// ���̾�α׸� �����ϴ� �Լ�.
{
	m_pListDlg->RemoveAll();
	m_nHyperCount = 0;
	m_dwCurNpc = 0;																		// ���� npc�� 0���� �����Ѵ�.
	m_wNpcChxNum = 0 ;																	// ���� npc chx ��ȣ�� 0���� �����Ѵ�.

	GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );										// NPC �̹��� ���̾�α��� Disable�� �����Ѵ�.
	GAMEIN->GetNpcImageDlg()->SetActive( FALSE );										// NPC �̹��� ���̾�α׸� ��Ȱ��ȭ �Ѵ�.

	NPCSCRIPTMGR->SetSelectedNpc( NULL );												// ������ NPC�� �����Ѵ�.
	
	//�����Ŀ� ���� ���� �ִ�.
	if( HERO->GetState() != eObjectState_Die )											// HERO�� ���� ���°� �ƴϸ�,
	{
		if(FALSE == ((cDialog*)GAMEIN->GetStorageDialog())->IsActive() &&
			FALSE == ((cDialog*)GAMEIN->GetDealDialog())->IsActive() &&
			FALSE == ((cDialog*)GAMEIN->GetGuildWarehouseDlg())->IsActive())
		{
			OBJECTSTATEMGR->EndObjectState(
				HERO,
				eObjectState_Deal);
		}
	}
}

//void cNpcScriptDialog::HyperLinkParser(LPCTSTR selectedText,DWORD selectedData)
void cNpcScriptDialog::HyperLinkParser( DWORD dwIdx, BOOL bConversation )
{
	//const HYPERLINK& hyperLink = GetHyperLink(selectedText, selectedData);
	if( bConversation )
	{
		if( m_nConversationHyperCount == 0 ) return;										// ������ ī��Ʈ�� 0�� ������ ���� ó���� �Ѵ�.
	}
	else
	{
		if( m_nHyperCount == 0 ) return;													// ������ ī��Ʈ�� 0�� ������ ���� ó���� �Ѵ�.
	}

	int nType = -1;																			// Ÿ���� -1�� �����Ѵ�.

	DWORD dwData = 0 ;																		// ������ ������ �����Ѵ�.

	int	hypertext = 0;																		// ������ �ؽ�Ʈ ������ 0���� �����Ѵ�.


	float fXpos = 0.0f ;
	float fZpos = 0.0f ;

	if( bConversation )
	{
		for( int i=0 ; i<m_nConversationHyperCount ; ++i)									// ������ ī��Ʈ �� ��ŭ for���� ������.
		{
			if( m_sConversationHyper[i].dwListItemIdx == dwIdx )							// �ε����� ������,
			{
				nType		= m_sConversationHyper[i].sHyper.wLinkType;						// ��ũ Ÿ���� �����Ѵ�.	
				dwData		= m_sConversationHyper[i].sHyper.dwData;						// ��ũ �����͸� �����Ѵ�.
				hypertext	= m_sConversationHyper[i].sHyper.wLinkId;						// ��ũ ���̵� �����Ѵ�.
				fXpos		= m_sConversationHyper[i].sHyper.fXpos ;						// �� �̵��� ����� x ��ǥ�� �޴´�.
				fZpos		= m_sConversationHyper[i].sHyper.fZpos ;						// �� �̵��� ����� y ��ǥ�� �޴´�.

				break;
			}
		}
	}
	else
	{
		for( int i=0 ; i<m_nHyperCount ; ++i)												// ������ ī��Ʈ �� ��ŭ for���� ������.
		{
			if( m_sHyper[i].dwListItemIdx == dwIdx )										// �ε����� ������,
			{
				nType		= m_sHyper[i].sHyper.wLinkType;									// ��ũ Ÿ���� �����Ѵ�.	
				dwData		= m_sHyper[i].sHyper.dwData;									// ��ũ �����͸� �����Ѵ�.
				hypertext	= m_sHyper[i].sHyper.wLinkId;									// ��ũ ���̵� �����Ѵ�.
				fXpos		= m_sHyper[i].sHyper.fXpos ;									// �� �̵��� ����� x ��ǥ�� �޴´�.
				fZpos		= m_sHyper[i].sHyper.fZpos ;									// �� �̵��� ����� y ��ǥ�� �޴´�.

				break;
			}
		}
	}

	//switch(hyperLink.wLinkType)
	switch(nType)
	{
	case emLink_Page:																	// ��ũ ���������,
		{
			OpenLinkPage(dwData);
			m_pListDlg->ResetGuageBarPos();
		}
		break;

	case emLink_Open:																	// ��ũ �����̸�,
		{
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2008.01.23
			// ..���� ���׷��̵� ó��
			if (m_dwCurNpc == 999)
			{		
				if (m_dwCurPageId == 90	&& g_csFarmManager.CLI_ProcUpgradeDlg(dwData))
				{
					return;
				}
				if (hypertext == 691 && dwData == 6 && g_csFarmManager.CLI_ProcPayTaxDlg())
				{
					return;
				}
			}

			// �����ʸ�Ʈ �߰�
			if(m_dwCurNpc == 125)
			{
				MSGBASE msg;
				MSG_WORD msgGTTableSyn;
				MSG_DWORD msgMapSyn;

				switch(hypertext)
				{
				case 734:	//��ʸ�Ʈ ��û�� �Ѵ�
					// ���̾�α׷� ����
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGIST_SYN;
					msg.dwObjectID = HEROID;
					NETWORK->Send( &msg, sizeof(msg) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				case 735:	//������ ����� �����Ѵ�
					msg.Category = MP_GTOURNAMENT;
					msg.Protocol = MP_GTOURNAMENT_REGISTPLAYER_SYN;
					msg.dwObjectID = HEROID;
					NETWORK->Send( &msg, sizeof(msg) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				case 736:	//��ʸ�Ʈ ����ǥ�� Ȯ���Ѵ�
					// ���̾�α׷� ����
					msgGTTableSyn.Category = MP_GTOURNAMENT;
					msgGTTableSyn.Protocol = MP_GTOURNAMENT_STANDINGINFO_SYN;
					msgGTTableSyn.dwObjectID = HEROID;
					//msgGTTableSyn.wData = 1;
					NETWORK->Send( &msgGTTableSyn, sizeof(msgGTTableSyn) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				case 737:	//��ʸ�Ʈ ��������� �̵��� �Ѵ�
					msgMapSyn.Category = MP_GTOURNAMENT;
					msgMapSyn.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
					msgMapSyn.dwObjectID = HEROID;
					msgMapSyn.dwData = 0;	// �������ϴ� ��Ʋ��ȣ
					NETWORK->Send( &msgMapSyn, sizeof(msgMapSyn) );

					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				//case 738:	//��ʸ�Ʈ ������ �޴´�
				//	GAMEIN->GetNpcScriptDialog()->EndDialog();
				//	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				//	return;
				default:
					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					return;
				}
			}

			// 080514 KTH -- �ּ� ó��
			//NPCSCRIPTMGR->StartNpcBusiness( dwData );									// �ŷ�â�� ����.
			// 080514 KTH -- Hack Check
			CObject* pObject = OBJECTMGR->GetSelectedObject();

			// 090601 LUJ, ������ ������ ���� ������ ������ ���, Ŭ���̾�Ʈ�� ���� ����ȴ�
			if( 0 == pObject )
			{
				break;
			}

			MSG_DWORD3	msg;
			msg.Protocol = MP_NPC_CHECK_HACK_SYN;
			msg.Category = MP_NPC;
			msg.dwObjectID = HEROID;
			msg.dwData1 = pObject->GetID() ;//m_dwCurNpc;
			msg.dwData2 = (DWORD)((CNpc*)pObject)->GetNpcUniqueIdx();
			msg.dwData3 = dwData;
			NETWORK->Send(&msg, sizeof(msg));
		}
		break;

	case emLink_End:																	// ��ũ �����,
		{
			EndDialog();																// ���̾�α׸� �����Ѵ�.
			DisActiveEx();																// Disable�Ѵ�.
		}
		break;

	case emLink_Quest:																	// ��ũ ����Ʈ��,
		{
			if( m_dwCurNpc == 135 || m_dwCurNpc == 136 )
			{
				//������ ���̶�� 
				if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
				{
					GAMEIN->GetNpcScriptDialog()->EndDialog();
					GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
					NPCSCRIPTMGR->SetSelectedNpc(NULL);
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
					return;
				}
			}
			// 090924 ShinJS --- ����Ʈ�� ������ �ȵ� ��츦 �Ǵ��ϱ� ���� �ʱ�ȭ �߰�
			m_dwQuestIdx = 0;			
			m_dwMainLinkPageId = dwData;

			OpenQuestLinkPage(dwData);
		}
		break;

	case emLink_QuestLink:																// ��ũ ����Ʈ ��ũ��,
		{
			m_dwQuestIdx = 0;															// ����Ʈ �ε����� 0���� �����Ѵ�.

			CQuest* pQuest = QUESTMGR->GetQuest(dwData);

			if(0 == pQuest)
			{
				break;
			}

			DWORD dwPage = pQuest->GetNpcScriptPage( m_dwCurNpc );

			if( dwPage == 0 )
			{
				dwPage = 30;
			}

			// 100414 ONS ����Ʈ����Ʈ�� ��Ȱ��ȭ��Ű��, ������̾�α׸� Ȱ��ȭ�Ѵ�.
			m_pLinkListDlg->SetActive( FALSE );
			m_pRequitalInfoDlg->SetDisable( FALSE );
			m_pRequitalInfoDlg->SetActive( TRUE );

			OpenRequitalPage(dwData);
			OpenLinkPage(dwPage);

			m_dwQuestIdx = dwData;

			switch(pQuest->IsQuestState( m_dwCurNpc ))
			{
				// ���� ���� ����Ʈ
			case eQuestState_Possible:
				{
					m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1158 ), m_pQuestBtn->GetBasicColor() );				// "����Ʈ ����"
					m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1160 ), RGB_HALF(255,255,255) );					// "���� ���õ� ����Ʈ�� �����մϴ�"
					m_pQuestBtn->SetActive( TRUE );
					m_pQuestBtn->SetRenderArea();
					m_QuestBtnType = eQuestBtnType_Allow;
				}
				break;

				// �������� ����Ʈ
			case eQuestState_Executed:
				{
					// ����Ʈ�� �Ϸ� ������ ���
					if( pQuest->IsPossibleQuestComplete() )
					{
						m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1159 ), m_pQuestBtn->GetBasicColor() );			// "����Ʈ �Ϸ�"
						m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1161 ), RGB_HALF(255,255,255) );				// "���� ���õ� ����Ʈ�� �Ϸ��մϴ�"
						m_pQuestBtn->SetActive( TRUE );
						m_pQuestBtn->SetRenderArea();
						m_QuestBtnType = eQuestBtnType_Complete;
					}
					else
					{
						// 100525 ONS ����Ʈ�� ���� ������ ���
						m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1449 ), m_pQuestBtn->GetBasicColor() );			// "����Ʈ �Ϸ�"
						m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1450 ), RGB_HALF(255,255,255) );				// "���� ���õ� ����Ʈ�� �Ϸ��մϴ�"
						m_pQuestBtn->SetActive( TRUE );
						m_pQuestBtn->SetRenderArea();
						m_QuestBtnType = eQuestBtnType_Progress;
					}
				}
				break;
			}
		}
		break;

	case emLink_QuestStart:																// ����Ʈ�� �����ߴٸ�,
		{
			// 100414 ONS ����Ʈ �Ϸḵũ�� Ŭ�������� ������ ���õǾ��ִ��� üũ
			if( !IsRequitalSelected( m_dwQuestIdx ) )
			{
				WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214) );
				return;
			}

			// 100524 ONS ����Ʈ����Ʈ �ִ밳�� üũó�� �̵� (��ũ�������� ���������� üũ�ϵ��� ����)
			CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;
			if( pQuestDialog )
			{
				pQuestDialog->SetQuestCount();
				if( pQuestDialog->GetQuestCount() >= MAX_QUEST_COUNT )
				{
					// �ִ밹���ϰ��, ����Ʈ�� ��Ͽ� ����(������, �Ϸ�)�ϸ� �����Ű����,
					// �׷������� ���(����)�� ����ƮFULL�޼����� ����ϰ� �����Ѵ�.
					if( FALSE == pQuestDialog->IsExistInQuestList( m_dwQuestIdx ) )
					{
						WINDOWMGR->MsgBox( MBI_QUEST_FULL, MBT_OK, CHATMGR->GetChatMsg( 1315 ) );
						return;
					}
				}
			}

			if( m_dwQuestIdx == 45 || m_dwQuestIdx == 46 || m_dwQuestIdx == 47 )		// ����Ʈ �ε����� 45, 46, 47�̸�,
			{
				if(CQuest* pQuest = QUESTMGR->GetQuest( m_dwQuestIdx ))
				{
					if(FALSE == pQuest->IsTimeCheck())
					{
						if( hypertext == 347 || hypertext == 349 )
						{
							HERO->StartSocietyActForQuest( 259, -1, -1, TRUE );			// ����Ʈ�� ���� �׼��� �����Ѵ�.
						}

						OpenLinkPage(dwData);
					}
				}
			}
			else																		// ����Ʈ �ε����� 45, 46, 47�� �ƴϸ�,
			{
				OpenLinkPage(dwData);
			}

			if( m_dwQuestIdx )															// ����Ʈ �ε��������� ��ȿ�ϴٸ�,
			{
				QUESTMGR->SendNpcTalkEvent( m_dwCurNpc, m_dwQuestIdx, m_RequitalIndex, m_RequitalCount );					// NPC ��ȭ �̺�Ʈ�� ������.
			}
			
			if(NPCSCRIPTMGR->GetSelectedNpc())											// ������ NPC ������ ��ȿ�ϴٸ�,
			{
				if(NPCSCRIPTMGR->GetSelectedNpc()->GetNpcJob() == BOMUL_ROLE)			// NPC�� ������ ���� ���̶��,
				{
					MSG_DWORD msg;														// �޽��� ����ü�� �����Ѵ�.
					msg.Category = MP_NPC;												// ī�װ��� NPC�� �����Ѵ�.
					msg.Protocol = MP_NPC_DOJOB_SYN;									// ���������� ������ũ�� �����Ѵ�.
					msg.dwData = NPCSCRIPTMGR->GetSelectedNpc()->GetID();				// ������ NPC�� ���̵� �����Ѵ�.
					msg.dwObjectID = HEROID;											// HERO�� ���� �����Ѵ�.
					NETWORK->Send(&msg, sizeof(msg));									// �޽����� �����Ѵ�.
				}
			}

			// 090924 ShinJS --- ����Ʈ ����/�Ϸ� ��ư ��Ȱ��ȭ
			m_pQuestBtn->SetActive( FALSE );
			m_pRequitalInfoDlg->SetActive( FALSE );
			m_pLinkListDlg->SetActive( TRUE );
		}
		break;

	case emLink_EventQuestStart:														// ����Ʈ ���� �̺�Ʈ ��ũ���,
		{
			if(dwData)
			{
				QUESTMGR->SendNpcTalkEventForEvent(m_dwCurNpc, dwData);
			}

			EndDialog();																// ���̾�α׸� �ݴ´�.
			DisActiveEx();																// ��Ȱ��ȭ ��Ų��.
		}
		break;

	case emLink_QuestContinue:															// ����Ʈ�� ����ϴ� ��ũ���,
		{
			OpenLinkPage(dwData);
		}
		break;

	case emLink_QuestEnd:																// ����Ʈ�� �����ϴ� ��ũ���,
		{
			QUESTMGR->QuestAbandon(m_dwQuestIdx);
			OpenLinkPage(dwData);
		}
		break;

	case emLink_MapChange:																// �� ü���� ��ũ���,
		{
			g_UserInput.GetHeroMove()->MapChangeToNpc(dwData, fXpos, fZpos);
		}
		break;

	case emLink_ChangeMapPage :
		{
			cPage* pPage = NPCSCRIPTMGR->GetPage(m_dwCurNpc, dwData);

			if( pPage == NULL ) return ;												// ������ ������ ��ȿ���� ���ϸ� FALSE ���� ó���� �Ѵ�.

			cDialogueList* pList = NPCSCRIPTMGR->GetDialogueList();						// �޼��� ��ȣ�� �̿��Ͽ� ����� �޼����� �����´�.
			if( pList == NULL ) return ;												// ���̾�α� ������ ��ȿ���� ������, FALSE ���� ó���� �Ѵ�.

			cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();					// ������ �ؽ�Ʈ ����Ʈ ������ �޴´�.
			if( pHyper == NULL ) return ;												// ������ �ؽ�Ʈ ����Ʈ ������ ��ȿ���� ������ FALSE ���� ó���� �Ѵ�.

			DWORD dwMsg = pPage->GetRandomDialogue();									// ������ ������ �̿��Ͽ� �������� �����Ѵ�. // �޼����� ��ȣ
			ResetConversationList(pList, dwMsg) ;										// ��ȭ ����Ʈ�� �ٽ� �����Ѵ�.

			m_pListDlg->ResetGuageBarPos();												// �������� ��ġ�� ������ �Ѵ�.

			m_dwMainLinkPageId = dwData;

			OpenChangeMapLinkPage(dwData);
		}
		break ;

	case emLink_FishPointChange:
		{
			GAMEIN->GetNpcScriptDialog()->EndDialog();
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
			GAMEIN->GetFishingPointDlg()->ShowDealDialog(TRUE);
		}
		break;

		// 080818 LYW --- NpcScriptDialog : Add link type to recall npc.
	case emLink_GetFreeBuffIX:
		{
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( HEROID );
			
			if(!pPlayer)
			{
				EndDialog();
   				DisActiveEx();
				return;
			}
			//if(pPlayer->GetRebirth() > 6)
			//{
			//	CHATMGR->AddMsg(CTC_SYSMSG, "NPC buff is for character rebirth under 6x.");
			//	EndDialog();
   //				DisActiveEx();
			//	return;
			//}
			//if(pPlayer->GetRebirth() == 0)
			//{
				WINDOWMGR->MsgBox( MBI_GETNPCBUFF_AREYOUSURE, MBT_YESNO, "get free buff?" ) ;
			//} else if(pPlayer->GetRebirth() == 1)
			//{
			//	WINDOWMGR->MsgBox( MBI_GETNPCBUFF_AREYOUSURE, MBT_YESNO, "Are you sure pay 500.000 for get buff?" ) ;
			//} else if(pPlayer->GetRebirth() == 2)
			//{
			//	WINDOWMGR->MsgBox( MBI_GETNPCBUFF_AREYOUSURE, MBT_YESNO, "Are you sure pay 1.000.000 for get buff?" ) ;
			//} else if(pPlayer->GetRebirth() > 2)
			//{
			//	WINDOWMGR->MsgBox( MBI_GETNPCBUFF_AREYOUSURE, MBT_YESNO, "Are you sure pay 2.000.000 for get buff?" ) ;
			//}
			EndDialog();
   			DisActiveEx();
			return;
		} 
		break;
	case emLink_RegistStreetTournament:
		{
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( HEROID );
			
			if(!pPlayer)
			{
				EndDialog();
   				DisActiveEx();
				return;
			}

			if(STREETTOURNAMENTMGR->GetState() != 1)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "Street Tournament Registration is closed.");
				EndDialog();
   				DisActiveEx();
				return;
			}

			if(pPlayer->GetLevel() < 105)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "You can participate Street Tournament after level 105.");
				EndDialog();
   				DisActiveEx();
				return;
			}
			if(HERO->GetMoney() < 50000000)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "You don't have enough money, Registration fee is 50.000.000");
				EndDialog();
   				DisActiveEx();
				return;
			}

			if(pPlayer->IsRegisterStreetTournament())
			{
				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_STREETTOURNAMENT_REGIST;
				msg.dwObjectID = gHeroID;
				NETWORK->Send( &msg, sizeof(msg) );

				break;
			}

			WINDOWMGR->MsgBox( MBI_REGISTSTREETTOURNAMENT_AREYOUSURE, MBT_YESNO, "Are you sure use 50.000.000 gold for regist Street Tournament?" ) ;

			EndDialog();
   			DisActiveEx();
			return;
		} 
		break;
   	case emLink_RecallNpc :
   		{
			CNpcRecallBase* pBaseRecallInfo = NPCRECALLMGR->Get_RecallBase(dwData);

			if( ! pBaseRecallInfo )
				return;

			DWORD dwParentNpcIdx  = pBaseRecallInfo->Get_ParentNpcIndex() ;
			DWORD dwParentNpcType = (DWORD) pBaseRecallInfo->Get_ParentNpcType() ;

			if(	dwParentNpcType == CNpcRecallBase::eParentNpcType_Static )
			{
				if( dwParentNpcIdx != m_dwCurNpc )
					return;
			}
			else if( dwParentNpcType == CNpcRecallBase::eParentNpcType_Recall )
			{
				if( NPCRECALLMGR->Get_RecallBase( dwParentNpcIdx ) == NULL )
					return;
			}

			//�������д��ϰ�� 
			if( m_dwCurNpc == 130 )
			{
   				if( !SIEGEWARFAREMGR->UseCandleStand() )
   				{
   					// Error Message " ����� �������� �带 �� �ƹ� ������ �����ϴ�."
   					WINDOWMGR->MsgBox(MBI_RECALLPORTAL_NOTUSE, MBT_OK, CHATMGR->GetChatMsg( 1727 ) ) ;
   					EndDialog();
   					DisActiveEx();
   					return;
   				}
			}
	
   			MSG_DWORD msg;
   			msg.Category = MP_RECALLNPC;
   			msg.Protocol = MP_RECALLNPC_CTOM_RECALL_SYN;
   			msg.dwObjectID = gHeroID;
   			msg.dwData = dwData;
   			NETWORK->Send(&msg, sizeof(MSG_DWORD));
	
   			EndDialog() ;
   			DisActiveEx() ;

   		}
   		break;
   	case emLink_SpellAllBuff : 
   		{
			WORD wSiegeState = SIEGEWARFAREMGR->GetState() ;

   			// �����߿��� ���� ������ ���� �� ���ٴ� �޽����� ����Ѵ�.
			// 081009 LYW --- NpcScriptDialog : ���� �ҽ� ����
			// 081106 LYW --- NpcScriptDialog : ���� ���� ��, �ٷ� ���������� ������ ���� �� ���� ���� ����.
   			//if( wSiegeState > eSiegeWarfare_State_Before )//>= eSiegeWarfare_Notify_Begin && wSiegeState < eSiegeWarfare_Notify_End )
			if( wSiegeState > eSiegeWarfare_State_Before && wSiegeState != eSiegeWarfare_State_End )
			{
				WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, CHATMGR->GetChatMsg(1717) ) ;
			}
   			else
   			{
   				MSG_DWORD msg;
   				msg.Category = MP_SIEGEWARFARE;
   				msg.Protocol = MP_SIEGEWARFARE_BUFF_SYN;
   				msg.dwObjectID = gHeroID;
   					msg.dwData = dwData;
   					NETWORK->Send(&msg, sizeof(MSG_DWORD));
   			}
   
   			EndDialog() ;
   			DisActiveEx() ;
   		}
   		break ;
   
   		// 080904 LYW --- NpcScriptDialog : Add link type to change map by castle flag.
   	case emLink_ChangeMapRecallNpc :
   		{
   			MSG_NPCRECALL_CHANGEMAP msg;
			ZeroMemory(
				&msg,
				sizeof(msg));
   			msg.Category = MP_RECALLNPC;
   			msg.Protocol = MP_RECALLNPC_CTOAM_CHANGEMAP_SYN;
   			msg.dwObjectID = gHeroID;
   			msg.CurMapNum = MAP->GetMapNum();
   			msg.ChangeMapNum = (MAPTYPE)dwData;
   			msg.ChannelNum = (BYTE)gChannelNum;
   			NETWORK->Send(
				&msg,
				sizeof(msg));
   		}
   		break ;
   
   		// 080919 LYW --- NpcScriptDialog : �罨 ���������� ���̵� â ���� ó�� �߰�.
   	case emLink_MapMoveList_Rushen :
   		{
			//������ ���̶�� 
			if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
				return;
			}

   			EndDialog() ;
   			DisActiveEx() ;
   
   			cMapMoveDialog* pDlg = NULL ;
   			pDlg = GAMEIN->GetMapMoveDlg() ;
   
   			if( !pDlg )
   			{
   #ifdef _GMTOOL_
   				MessageBox(NULL, "Failed to receive MapMoveListDlg_Rushen.", __FUNCTION__, MB_OK) ;
   				return ;
   #else
   				return ;
   #endif //_GMTOOL_
   			}
   			
   			pDlg->SetUseCase(eCastle_Rushen) ;
   			pDlg->SetActive(TRUE) ;
   		}
   		break ;
   
   		// 080919 LYW --- NpcScriptDialog : �罨 ���������� ���̵� â ���� ó�� �߰�.
   	case emLink_MapMoveList_Zevyn :
   		{
			//������ ���̶�� 
			if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
				return;
			}

   			EndDialog() ;
   			DisActiveEx() ;
   
   			cMapMoveDialog* pDlg = NULL ;
   			pDlg = GAMEIN->GetMapMoveDlg() ;
   
   			if( !pDlg )
   			{
   #ifdef _GMTOOL_
   				MessageBox(NULL, "Failed to receive MapMoveListDlg_Zevyn.", __FUNCTION__, MB_OK) ;
   				return ;
   #else
   				return ;
   #endif //_GMTOOL_
   			}
   
   			pDlg->SetUseCase(eCastle_Zevyn) ;
   			pDlg->SetActive(TRUE) ;
   		}
   		break ;
   	case emLink_MoveToDungeon :
   		{
			//������ ���̶�� 
			if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
				return;
			}

   			EndDialog() ;
   			DisActiveEx() ;
   
   			MSGBASE msg ;
   
   			msg.Category	= MP_SIEGEWARFARE ;
   			msg.Protocol	= MP_SIEGEWARFARE_MOVETODUNGEON_SYN ;
   
   			msg.dwObjectID	= HEROID ;
   
   			NETWORK->Send( &msg, sizeof(msg) ) ;
   		}
   		break ;
	}

	MoveCursorToLink();
}

void cNpcScriptDialog::SetActive(BOOL val)
{
	if(FALSE == val)
	{
		if(CNpc* const pNpc = NPCSCRIPTMGR->GetSelectedNpc())
		{			
			if( pNpc->GetNpcJob() == BOMUL_ROLE)										// NPC ������ �ް�, ������ ���� �Ѱ� ������,
			{
				MSG_DWORD msg;															// �޽��� ����ü�� �����Ѵ�.
				msg.Category = MP_NPC;													// ī�װ��� NPC�� �����Ѵ�.
				msg.Protocol = MP_NPC_CLOSEBOMUL_SYN;									// ���������� MP_NPC_CLOSEBOMUL_SYN�� �����Ѵ�.
				msg.dwObjectID = gHeroID;											// HERO�� ���̵� �����Ѵ�.
				msg.dwData = NPCSCRIPTMGR->GetSelectedNpc()->GetID();					// ���� ���� �� NPC ���̵� �����Ѵ�.
				NETWORK->Send(&msg, sizeof(msg));										// �޽����� �����Ѵ�.
			}
		}

		if(IsActive())
		{
			EndDialog();
		}

		if(CQuestQuickViewDialog* const pQuickDlg = GAMEIN->GetQuestQuickViewDialog())
		{
			pQuickDlg->SetHide( FALSE );
			pQuickDlg->UpdateQuickView();
		}
	}
	else
	{
		CQuestQuickViewDialog* pQuickDlg = GAMEIN->GetQuestQuickViewDialog();
		if( pQuickDlg					&& 
			NULL == pQuickDlg->IsActive() )
		{
			pQuickDlg->SetActive( TRUE );
			pQuickDlg->SetHide( TRUE );
		}
	}

	cDialog::SetActiveRecursive( val );

	m_pQuestBtn->SetActive( FALSE );
}

// �ٸ������� ���� ȣ������ ���ÿ�!!
void cNpcScriptDialog::DisActiveEx()													// ��Ȱ��ȭ Ȯ�� �Լ�.
{
	m_pListDlg->RemoveAll();

	m_dwCurNpc = 0;
	m_wNpcChxNum = 0;
	m_nHyperCount = 0;	

	NPCSCRIPTMGR->SetSelectedNpc(NULL);
	cDialog::SetActiveRecursive(FALSE);
	WINDOWMGR->SetMouseInputProcessed();
}

void cNpcScriptDialog::RefreshQuestLinkPage()
{
	m_pLinkListDlg->RemoveAll();

	DWORD dwPageId = 0 ;
	m_nHyperCount = 0;	

	if( m_dwMainLinkPageId != 0 )
	{
		dwPageId = m_dwMainLinkPageId ;
	}
	else
	{
		dwPageId = m_dwCurPageId ;
	}

	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// ���� npc�� ������ ���̵�� ������ ������ ��´�.

	if( pPage == NULL ) return;															// ������ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// ������ �ؽ�Ʈ ������ �޴´�.

	if( pHyper == NULL ) return;														// ������ �ؽ�Ʈ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	DIALOGUE* temp = NULL;																// �ӽ� ���̾�α� ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.

	LINKITEM* pItem = NULL;																// ��ũ ������ ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.

	HYPERLINK* pLink;																	// ��ũ ������ ���� �����͸� �����Ѵ�. // ��ũ ������ �����ͼ� �����Ѵ�.

	int nLinkCount = pPage->GetHyperLinkCount();										// ��ũ ī��Ʈ�� �����Ѵ�.

	pItem = new LINKITEM;																// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
	strcpy( pItem->string, CHATMGR->GetChatMsg(239) );									// ������ ��Ʈ������ [ ����Ʈ ��� ]�� �����Ѵ�.
	pItem->rgb = RGBA_MAKE	( 170, 236, 4, 255 );											// ������ ������ �����Ѵ�.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// ��ũ ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.

	// ���� ����
	pItem = new LINKITEM;																// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
	strcpy( pItem->string, "" );														// ������ ��Ʈ���� ""�� �����Ѵ�.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// ���� �������� �߰��Ѵ�.

	BOOL bCheck = FALSE;																// üũ ���θ� FALSE�� �����Ѵ�.

	// ���డ���� ����
	for( int j = 0; j < nLinkCount; ++j )												// ��ũ ī��Ʈ ��ŭ for���� ������.
	{
		pLink = pPage->GetHyperText(j);													// ������ �ؽ�Ʈ ������ �޴´�.

		if( pLink == NULL )																// ������ �ؽ�Ʈ ������ ��ȿ���� ������,
		{
			break;																		// for���� Ż���Ѵ�.
		}

		CQuest* pQuest = QUESTMGR->GetQuest( pLink->dwData );							// ����Ʈ ������ �޴´�.

		if( pQuest )																	// ����Ʈ ������ ��ȿ�ϸ�,
		{
			if( pQuest->IsQuestState( m_dwCurNpc ) != eQuestState_Possible )			// ����Ʈ ���°��� üũ�Ѵ�.
			{
				continue;																// ����Ѵ�.
			}
		}
		else																			// ����Ʈ ������ ��ȿ���� ������,
			continue;																		// ����Ѵ�.

		if( !bCheck )																	// üũ ���ΰ� FALSE�� ������,
		{		
			LINKITEM* pSimbol = new LINKITEM;
			if( pSimbol == NULL ) break;

			strcpy( pSimbol->string, "!  ");
			pSimbol->rgb = RGBA_MAKE( 157, 246, 254, 255 );
			
			pItem = new LINKITEM;														// ��ũ ������ �޸𸮸� �����Ѵ�.
			strcpy( pItem->string, CHATMGR->GetChatMsg(243) );												// ���� ������ ����Ʈ�� �����Ѵ�.
			pItem->rgb = RGBA_MAKE( 255, 204, 0, 255 );									// ������ ������ �����Ѵ�.

			pSimbol->NextItem = pItem;
			m_pLinkListDlg->cListItem::AddItem( pSimbol );								// ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.

			bCheck = TRUE;																// üũ ���θ� TRUE�� �����Ѵ�.
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// ������ �ؽ�Ʈ ������ �޴´�.

		if( temp )																		// ������ ��ȿ�ϸ�,
		{
			pItem = new LINKITEM;														// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
			strcpy( pItem->string, temp->str );											// ��ũ ��Ʈ���� ������ ��Ʈ���� �����Ѵ�.
			pItem->dwType = pLink->wLinkType;											// ������ Ÿ���� ��ũ Ÿ������ �����Ѵ�.
			pItem->dwData = pLink->dwData;
			
			m_pLinkListDlg->cListItem::AddItem( pItem );

			m_sHyper[m_nHyperCount].bUse = TRUE;										// ��뿩�θ� TRUE�� �����Ѵ�.
			m_sHyper[m_nHyperCount].dwListItemIdx = m_pLinkListDlg->GetItemCount()-1;	// ������ �ε����� �����Ѵ�.
			m_sHyper[m_nHyperCount].sHyper = *pLink;									// ������ ��ũ ������ �����Ѵ�.

			++m_nHyperCount;	
		}
	}

	if( bCheck )																		// üũ ���ΰ� TRUE�� ���ٸ�,
	{
		pItem = new LINKITEM;															// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
		strcpy( pItem->string, "" );													// ��Ʈ���� ""�� �����Ѵ�.
		m_pLinkListDlg->cListItem::AddItem( pItem );									// ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.
	}

	bCheck = FALSE;																		// üũ ���θ� FALSE�� �����Ѵ�.
	// �������� ����

	for( int j = 0; j < nLinkCount; ++j )													// ��ũ ī��Ʈ �� ��ŭ for���� ������.
	{
		pLink = pPage->GetHyperText(j);													// ������ ��ũ ������ �޴´�.

		if( pLink == NULL ) break;														// ������ ��ũ ������ ��ȿ���� ������ for���� Ż���Ѵ�.

		CQuest* pQuest = QUESTMGR->GetQuest( pLink->dwData );							// ����Ʈ ������ �޴´�.

		temp = pHyper->GetHyperText( pLink->wLinkId	);									// ������ �ؽ�Ʈ ������ �޴´�.
		if( pQuest )																	// ����Ʈ ������ ��ȿ�ϸ�,
		{
			if( pQuest->IsQuestState( m_dwCurNpc ) != eQuestState_Executed )			// ����Ʈ ���¸� üũ�Ѵ�.
			{
				continue;																// ����Ѵ�.
			}
		}
		else																			// ����Ʈ ������ ��ȿ���� ������,
			continue;																	// ����Ѵ�.

		if( !bCheck )																	// üũ���ΰ� FALSE�� ������,
		{
			LINKITEM* pSimbol = new LINKITEM;
			if( pSimbol == NULL ) break;

			strcpy( pSimbol->string, "? ");
			pSimbol->rgb = RGBA_MAKE( 157, 246, 254, 255 );

			pItem = new LINKITEM;														// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
			strcpy( pItem->string, CHATMGR->GetChatMsg(244) );							// ��Ʈ���� �������� ����Ʈ�� �����Ѵ�.
			pItem->rgb = RGBA_MAKE( 255, 204, 0, 255 );									// ������ ������ �����Ѵ�.

			pSimbol->NextItem = pItem;
			m_pLinkListDlg->cListItem::AddItem( pSimbol );								// ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.

			bCheck = TRUE;																// üũ ���θ� TRUE�� �����Ѵ�.
		}

		// 090924 ShinJS --- �Ϸ� ������ ����Ʈ�� ��� �Ϸ��ư�� Ȱ��ȭ��Ų��.
		if( pQuest->IsPossibleQuestComplete() )
		{
			m_pQuestBtn->SetText( RESRCMGR->GetMsg( 1159 ), m_pQuestBtn->GetBasicColor() );		// "����Ʈ �Ϸ�"
			m_pQuestBtn->SetToolTip( RESRCMGR->GetMsg( 1162 ), RGB_HALF(255,255,255) );			// "���� �Ϸ� ������ ����Ʈ�� ��� �Ϸ��մϴ�"
			m_pQuestBtn->SetActive( TRUE );
			m_pQuestBtn->SetRenderArea();
			m_QuestBtnType = eQuestBtnType_CompleteAll;
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// ������ �ؽ�Ʈ ������ �޴´�.

		if( temp )																		// ������ �ؽ�Ʈ ������ ��ȿ�ϸ�,
		{
			pItem = new LINKITEM;														// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
			strcpy( pItem->string, temp->str );											// ������ ��Ʈ���� ��ũ ��Ʈ������ �����Ѵ�.

			pItem->dwType = pLink->wLinkType;											// ������ Ÿ���� ��ũ Ÿ������ �����Ѵ�.
			pItem->dwData = pLink->dwData;
			
			m_pLinkListDlg->cListItem::AddItem( pItem );

			m_sHyper[m_nHyperCount].bUse = TRUE;										// ��뿩�θ� TRUE�� �����Ѵ�.
			m_sHyper[m_nHyperCount].dwListItemIdx = m_pLinkListDlg->GetItemCount()-1;	// ������ �ε����� �����Ѵ�.
			m_sHyper[m_nHyperCount].sHyper = *pLink;									// ������ ��ũ ������ �����Ѵ�.

			++m_nHyperCount;
		}
	}

	if( m_pLinkListDlg->GetItemCount() <= 2 )
	{
		m_pLinkListDlg->RemoveAll() ;
	}

	m_pLinkListDlg->ResetGuageBarPos();
	MoveCursorToLink();
}

// 070917 LYW --- NpcScriptDialog : Add function to refresh page to change map.
void cNpcScriptDialog::RefreshChangeMapLinkPage()
{
	m_pLinkListDlg->RemoveAll();

	DWORD dwPageId = 0 ;
	m_nHyperCount = 0;	

	if( m_dwMainLinkPageId != 0 )
	{
		dwPageId = m_dwMainLinkPageId ;
	}
	else
	{
		dwPageId = m_dwCurPageId ;
	}

	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );						// ���� npc�� ������ ���̵�� ������ ������ ��´�.

	if( pPage == NULL ) return;															// ������ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();							// ������ �ؽ�Ʈ ������ �޴´�.

	if( pHyper == NULL ) return;														// ������ �ؽ�Ʈ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	DIALOGUE* temp = NULL;																// �ӽ� ���̾�α� ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.

	LINKITEM* pItem = NULL;																// ��ũ ������ ������ ���� ������ ������ �����ϰ� NULL ó���� �Ѵ�.

	HYPERLINK* pLink;																	// ��ũ ������ ���� �����͸� �����Ѵ�. // ��ũ ������ �����ͼ� �����Ѵ�.

	int nLinkCount = pPage->GetHyperLinkCount();										// ��ũ ī��Ʈ�� �����Ѵ�.

	pItem = new LINKITEM;																// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
	strcpy( pItem->string, CHATMGR->GetChatMsg(1472) );										// ������ ��Ʈ������ [ ����Ʈ ��� ]�� �����Ѵ�.
	pItem->rgb = RGBA_MAKE( 0, 0, 255, 255 );											// ������ ������ �����Ѵ�.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// ��ũ ����Ʈ ���̾�α׿� �������� �߰��Ѵ�.

	// ���� ����
	pItem = new LINKITEM;																// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
	strcpy( pItem->string, "" );														// ������ ��Ʈ���� ""�� �����Ѵ�.
	m_pLinkListDlg->cListItem::AddItem( pItem );										// ���� �������� �߰��Ѵ�.

	// ���డ���� ����
	for( int j = 0; j < nLinkCount; ++j )												// ��ũ ī��Ʈ ��ŭ for���� ������.
	{
		pLink = pPage->GetHyperText(j);													// ������ �ؽ�Ʈ ������ �޴´�.

		if( pLink == NULL )																// ������ �ؽ�Ʈ ������ ��ȿ���� ������,
		{
			break;																		// for���� Ż���Ѵ�.
		}

		temp = pHyper->GetHyperText( pLink->wLinkId );									// ������ �ؽ�Ʈ ������ �޴´�.

		if( temp )																		// ������ ��ȿ�ϸ�,
		{
			pItem = new LINKITEM;														// ��ũ ������ �޸𸮸� �Ҵ��Ѵ�.
			strcpy( pItem->string, temp->str );											// ��ũ ��Ʈ���� ������ ��Ʈ���� �����Ѵ�.
			pItem->dwType = pLink->wLinkType;											// ������ Ÿ���� ��ũ Ÿ������ �����Ѵ�.
			pItem->dwData = pLink->dwData;
			
			m_pLinkListDlg->cListItem::AddItem( pItem );
			m_sHyper[m_nHyperCount].bUse = TRUE;										// ��뿩�θ� TRUE�� �����Ѵ�.
			m_sHyper[m_nHyperCount].dwListItemIdx = m_pLinkListDlg->GetItemCount()-1;	// ������ �ε����� �����Ѵ�.
			m_sHyper[m_nHyperCount].sHyper = *pLink;									// ������ ��ũ ������ �����Ѵ�.

			++m_nHyperCount;
		}
	}

	m_pLinkListDlg->ResetGuageBarPos() ;												// ��ũ ����Ʈ�� �������� ��ġ�� �ٽ� ��´�.
}


void cNpcScriptDialog::OpenQuestLinkPage( DWORD dwPageId )								// ����Ʈ ��ũ �������� ���� �Լ�.
{
	m_dwCurPageId = dwPageId;															// ������ ������ ���̺� �Ѵ�.

	RefreshQuestLinkPage() ;															// ����Ʈ ��ũ ����Ʈ�� ���ΰ�ħ �Ѵ�.
}

// 070917 LYW --- NpcScriptDialog : Add function to open page to change map.
void cNpcScriptDialog::OpenChangeMapLinkPage( DWORD dwPageId )							// �� ���� ��ũ �������� ���� �Լ�.
{
	m_dwCurPageId = dwPageId;															// ������ ������ ���̺� �Ѵ�.

	//RefreshQuestLinkPage() ;															// ����Ʈ ��ũ ����Ʈ�� ���ΰ�ħ �Ѵ�.
	RefreshChangeMapLinkPage() ;
}

void cNpcScriptDialog::OpenLastPageOfQuest()
{
	// ���� ���õ� ����Ʈ�� ���Ѵ�.
	CQuest* pQuest = QUESTMGR->GetQuest( m_dwQuestIdx );
	if( !pQuest )
		return;

	// ����Ʈ�� Page�� ���Ѵ�.
	DWORD dwPageId = pQuest->GetNpcScriptPage( m_dwCurNpc );
	cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );
	if( !pPage )
		return;

	int nHyperCnt = pPage->GetHyperLinkCount();
	if( nHyperCnt <= 0 || nHyperCnt > 1 )
		return;

	// ����Ʈ�� ������ Page�� ã�´�.
	while( nHyperCnt > 0 )
	{
		pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );
		if( !pPage )
			return;

		HYPERLINK* pLink = pPage->GetHyperText( 0 );
		if( !pLink )
			return;

		dwPageId = pLink->dwData;
		cPage* pPage = NPCSCRIPTMGR->GetPage( m_dwCurNpc, dwPageId );
		if( !pPage )
			return;

		nHyperCnt = pPage->GetHyperLinkCount();
	}

	// ������ Page Open
	OpenLinkPage( dwPageId );
}

void cNpcScriptDialog::MoveCursorToLink()
{
	cListDialogEx* const listDialog = (m_pRequitalInfoDlg->IsActive() ? m_pListDlg : m_pLinkListDlg);
	int selectedRow = listDialog->GetCurSelectedRowIdx() + 1;

	if(listDialog->GetTopListItemIdx() + listDialog->GetVisibleLineSize() <= selectedRow)
	{
		selectedRow	= 0;
	}

	int firstLinkedRow = INT_MAX;

	for(int i = 0; i < listDialog->GetVisibleLineSize(); ++i)
	{
		const int row = listDialog->GetTopListItemIdx() + i;
		const LINKITEM* const item = (LINKITEM*)listDialog->GetItem(
			row);

		if(0 == item)
		{
			break;
		}
		else if(emLink_Null == item->dwType)
		{
			continue;
		}

		firstLinkedRow = min(
			firstLinkedRow,
			row);
		const int linkedRow = row;

		if(selectedRow == linkedRow)
		{
			break;
		}
		else if(selectedRow < linkedRow)
		{
			selectedRow = linkedRow;
			break;
		}
	}

	const LINKITEM* listItem = (LINKITEM*)listDialog->GetItem(
		selectedRow);

	if(0 == listItem ||
		emLink_Null == listItem->dwType)
	{
		listItem = (LINKITEM*)listDialog->GetItem(
			firstLinkedRow);

		if(0 == listItem)
		{
			return;
		}

		selectedRow = firstLinkedRow;
	}

	POINT point = {LONG(listDialog->GetAbsX()), LONG(listDialog->GetAbsY())};
	ClientToScreen(
		_g_hWnd,
		&point);

	const LONG textWidth = CFONT_OBJ->GetTextExtentEx(
		listDialog->GetFontIdx(),
		LPTSTR(listItem->string),
		_tcslen(listItem->string));
	SetCursorPos(
		point.x + textWidth,
		point.y + (selectedRow - listDialog->GetTopListItemIdx()) * listDialog->GetLineHeight() + listDialog->GetLineHeight() / 2);
}

DWORD cNpcScriptDialog::ActionKeyboardEvent(CKeyboard* keyboard)
{
	if(keyboard->GetKeyDown(KEY_TAB))
	{
		MoveCursorToLink();
	}
	/*
	else if(keyboard->GetKeyDown(KEY_SPACE))
	{
		if(m_pRequitalInfoDlg->IsActive())
		{
			if(ITEM* const item = m_pListDlg->GetItem(m_pListDlg->GetCurSelectedRowIdx()))
			{
				HyperLinkParser(item->string, item->dwData);
			}
		}
		else
		{
			if(ITEM* const item = m_pLinkListDlg->GetItem(m_pLinkListDlg->GetCurSelectedRowIdx()))
			{
				HyperLinkParser(item->string, item->dwData);
			}
		}
	}

	return WE_NULL;
	*/
	return WE_NULL;
}

void cNpcScriptDialog::Render()														
{
	cDialog::Render();																
	if( !m_pRequitalInfoDlg->IsActive() ) return;

	int Count = 0;

	PTRLISTSEARCHSTART(m_QuestFixRequitalList, RequitalItem*, pSQuest) ;
		if( !pSQuest )			break ;

		if( pSQuest->nItemCount > 1 )
		{
			static char nums[4] ;
			int absX=0; int absY=0;

			wsprintf(nums, "%3d", pSQuest->nItemCount) ;

			if( m_pFixRequitalGrid->GetCellAbsPos(pSQuest->wPos, absX, absY))
			{
				// 070209 LYW --- QuestDialog : Modified render part.
				m_renderRect.left	= (LONG)absX+16 ;				
				m_renderRect.top	= (LONG)absY+24 ;
				m_renderRect.right	= 1 ;
				m_renderRect.bottom	= 1 ;
																	
				CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
			}
		}
		++Count;														
		if(Count >= MAX_FIX_REQUITAL_ICON_CELL)		break;	
	PTRLISTSEARCHEND

	// ���� ����������� ������ ����Ѵ�.
	Count	= 0;
	PTRLISTSEARCHSTART(m_QuestSelectRequitalList, RequitalItem*, pSQuest);
		if(!pSQuest)				break;								

		if(pSQuest->nItemCount > 1)									
		{
			static char nums[4];
			int absX=0;	int absY=0;									

			wsprintf(nums,"%3d", pSQuest->nItemCount);				

			if(m_pSelectRequitalGrid->GetCellAbsPos(pSQuest->wPos, absX, absY))
			{
				// 070209 LYW --- QuestDialog : Modified render part.
				m_renderRect.left	= (LONG)absX+16 ;						
				m_renderRect.top	= (LONG)absY+24 ;
				m_renderRect.right	= 1 ;
				m_renderRect.bottom	= 1 ;
																			
				CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
			}
		}
		++Count;
		if(Count >= MAX_SELECT_REQUITAL_ICON_CELL)		break;	
	PTRLISTSEARCHEND		

}

// 100412 ONS ��������� �������� ����.
void cNpcScriptDialog::OpenRequitalPage( DWORD dwQuestIdx )
{
	// ����Ʈ ���� ��Ʈ���� �޴´�.
	CQuestString* pTitleString = QUESTMGR->GetQuestString(dwQuestIdx, 0) ;
	if( !pTitleString )
	{
		return ;
	}

	char tempBuf[128] = {0, } ;
	PTRLISTPOS pos2 = pTitleString->GetTitle()->GetHeadPosition();				
	while(pos2)																	
	{
		ITEM* pItem = (ITEM*)pTitleString->GetTitle()->GetNext(pos2);				
		if(!pItem) continue ;														

		sprintf(tempBuf, "%s", pItem->string) ;									
	}	
	m_pRequitalQuestName->SetStaticText(tempBuf);

	m_pRequitalGold->SetStaticText("0");
	m_pRequitalExp->SetStaticText("0");
	m_pSelectRequitalGrid->SetCurSelCellPos(-1);
	m_RequitalIndex = 0;
	m_RequitalCount = 0;
	
	PTRLISTPOS deletePos = m_QuestFixRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		RequitalItem* pDeleteItem = (RequitalItem*)m_QuestFixRequitalList.GetNext(deletePos) ;
		if( !pDeleteItem ) continue ;

	    m_pFixRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;
		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;
		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;
		m_QuestFixRequitalList.Remove(pDeleteItem) ;
		delete pDeleteItem ;
	}
	m_QuestFixRequitalList.RemoveAll() ;

	deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		RequitalItem* pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
		if( !pDeleteItem ) continue ;

	    m_pSelectRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;
		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;
		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;
		m_QuestSelectRequitalList.Remove(pDeleteItem) ;
		delete pDeleteItem ;
	}
	m_QuestSelectRequitalList.RemoveAll() ;

	const cPtrList* pTriggerList = QUESTMGR->GetLastSubQuestTrigger( dwQuestIdx );
	if( !pTriggerList ) return;
	
	int nTriggerCount = pTriggerList->GetCount() ;							
	for( int count = 0 ; count < nTriggerCount ; ++count )
	{
		PTRLISTPOS pos = pTriggerList->FindIndex(count) ;
		if( !pos ) continue ;							
		
		// �ش� Ʈ���� ������ �޴´�.
		CQuestTrigger* pTrigger = (CQuestTrigger*)pTriggerList->GetAt(pos) ;			
		if( !pTrigger )	 continue ;	

		// Ʈ���� ���� ����Ʈ�� �޴´�.
		cPtrList* pExeList = pTrigger->GetExeList() ;								
		if( !pExeList )	continue ;

		int nExeCount = pExeList->GetCount() ;
		for(int count2 = 0 ; count2 < nExeCount ; ++count2 )
		{
			PTRLISTPOS exePos = pExeList->FindIndex(count2) ;									
			if( !exePos ) continue ;

			// ����Ʈ ���� ������ �޴´�.
			CQuestExecute* pExecute = (CQuestExecute*)pExeList->GetAt(exePos) ;	
			if( !pExecute ) continue ;

			CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute) ;
			DWORD dwExeKind = pExecute->GetQuestExecuteKind() ;

			switch(dwExeKind)
			{
			case eQuestExecute_TakeItem :				// ���� Ÿ���� �������� �޴� Ÿ���̶��,
				{
					ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());
					if( pItemInfo )
					{
						AddRequitalItem( pItemInfo, pExecute_Item->GetItemNum(), BYTE(eQuestExecute_TakeItem) );
						RefreshRequitalGrid( BYTE(eQuestExecute_TakeItem) );
					}
				}
				break ;
				
			case eQuestExecute_TakeSelectItem:
				{
					CQuestExecute_SelectItem* pExecute_selectItem = ((CQuestExecute_SelectItem*)pExecute) ;

					DWORD dwMaxCount = pExecute_selectItem->GetMaxItemCount();
					for( DWORD i = 0; i < dwMaxCount; i++ )
					{
						ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pExecute_selectItem->GetItemKind(i));
						if( !pItemInfo ) continue;

						AddRequitalItem( pItemInfo, pExecute_selectItem->GetItemNum(i), BYTE(eQuestExecute_TakeSelectItem) );
					}
					RefreshRequitalGrid( BYTE(eQuestExecute_TakeSelectItem) );
				}
				break;

			case eQuestExecute_TakeMoney :				// ���� Ÿ���� ��带 �޴� Ÿ���̶��,
				{
					m_pRequitalGold->SetStaticText( AddComma(pExecute_Item->GetItemNum()) );
				}
				break ;

			case eQuestExecute_TakeExp :				// ���� Ÿ���� ����ġ�� �޴� Ÿ���̶��,
				{
					m_pRequitalExp->SetStaticText( AddComma(pExecute_Item->GetItemNum()) );
				}
				break ;
			}
		}
	}
}

// ��������� �׸��带 �����Ѵ�.
void cNpcScriptDialog::RefreshRequitalGrid( BYTE bRequitalType )
{
	cIconGridDialog*	pRequitalGrid = NULL;
	cPtrList*			pRequitalList = NULL;
	DWORD				dwMaxCellNum = 0;

	if( bRequitalType == eQuestExecute_TakeItem )
	{
		pRequitalList = &m_QuestFixRequitalList;
		pRequitalGrid = m_pFixRequitalGrid;
		dwMaxCellNum = MAX_FIX_REQUITAL_ICON_CELL;
	}
	else if( bRequitalType == eQuestExecute_TakeSelectItem )
	{
		pRequitalList = &m_QuestSelectRequitalList;
		pRequitalGrid = m_pSelectRequitalGrid;
		dwMaxCellNum = MAX_SELECT_REQUITAL_ICON_CELL;
	}
	else
	{
		return;
	}

	DWORD Count = 0;	
	WORD Pos = 0;	
	PTRLISTPOS pos = pRequitalList->GetHeadPosition() ;
	while(pos)
	{
		RequitalItem* pItem = (RequitalItem*)pRequitalList->GetNext(pos) ;
		if( pItem )
		{
			if( Count >= 0 && Count < dwMaxCellNum )
			{
				pItem->wPos = Pos ;
				pRequitalGrid->AddIcon( pItem->wPos, pItem->pIcon ) ;
				pItem->pIcon->SetActive(TRUE) ;
				++Pos ;
			}
			else
			{
				pItem->wPos = WORD(-1);
				pItem->pIcon->SetActive(FALSE) ;
			}
			++Count ;
		}
	}				
}

// ����������� ����Ʈ�� �߰��Ѵ�.
void cNpcScriptDialog::AddRequitalItem( ITEM_INFO* pItemInfo, DWORD Count, BYTE bType )
{
	if( !pItemInfo ) return ;

	RequitalItem* pSQuest = new RequitalItem;										// ����Ʈ ������ �޸𸮸� �Ҵ��Ѵ�.
	memcpy(&pSQuest->itemInfo, pItemInfo, sizeof(ITEM_INFO)) ;						// ����Ʈ ������ ������ ���ڷ� �Ѿ�� ������ �����Ѵ�.
	pSQuest->nItemCount = (int)Count;												// ����Ʈ �������� ī��Ʈ�� �����Ѵ�.
	pSQuest->wPos = 0;

	cImage Image;																	// �̹����� �����Ѵ�.
	SCRIPTMGR->GetImage(pItemInfo->Image2DNum, &Image, PFT_ITEMPATH);				// ����Ʈ ������ ������ �̹����� �̹����� �����Ѵ�.

	cIcon* pIcon = new cIcon;														// ������ �޸𸮸� �Ҵ��Ѵ�.
	pIcon->SetBasicImage(&Image);													// �������� �⺻ �̹����� ������ ������ �̹����� �����Ѵ�.
	pIcon->Init(0, 0, 40, 40, &Image, m_IndexGenerator.GenerateIndex());			// �������� �ʱ�ȭ �Ѵ�.
	pIcon->SetMovable(FALSE);														// �̵� ���θ� FALSE�� �����Ѵ�.
	
	pSQuest->pIcon = pIcon ;
	
	if( bType == eQuestExecute_TakeItem )
		m_QuestFixRequitalList.AddTail(pSQuest);
	else
		m_QuestSelectRequitalList.AddTail(pSQuest);

	WINDOWMGR->AddWindow(pIcon);													// ������ �Ŵ����� ������ ������ �߰��Ѵ�.
}

// ��������������� ǥ���Ѵ�.
void cNpcScriptDialog::ShowRequitalItemToolTip( cPtrList& RequitalList, DWORD Index )
{
	char buf[64] = { 0, };															
	DWORD dwCount = 0;																
	PTRLISTSEARCHSTART(RequitalList, RequitalItem*, pSQuest);						
		if(!pSQuest)			break;												

		if(dwCount == Index)														
		{
			cImage Image;															
			SCRIPTMGR->GetImage(0, &Image, PFT_HARDPATH);							
			pSQuest->pIcon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &Image, TTCLR_ITEM_CANEQUIP );

			wsprintf(buf, "[%s]", pSQuest->itemInfo.ItemName);
			pSQuest->pIcon->AddToolTipLine(buf, ITEMMGR->GetItemNameColor( pSQuest->itemInfo ));
			
			ITEMMGR->AddToolTip( pSQuest->pIcon, pSQuest->itemInfo.ItemTooltipIdx );
		}

		++dwCount;																	

	PTRLISTSEARCHEND																
}

// ���ú���������� ���õǾ����� ���θ� �Ǵ��Ѵ�.
BOOL cNpcScriptDialog::IsRequitalSelected( DWORD dwQuestIdx )
{
	CQuest* pQuest = QUESTMGR->GetQuest( dwQuestIdx );
	if( !pQuest ) return TRUE;

	// ����Ʈ�� �����Ҷ��� ó������ �ʴ´�.
	if( pQuest->IsQuestState( m_dwCurNpc ) != eQuestState_Executed	&& !pQuest->IsPossibleQuestComplete() )
		return TRUE;

	m_RequitalIndex = 0;
	m_RequitalCount = 0;

	DWORD dwCount = 0;

	PTRLISTSEARCHSTART(m_QuestSelectRequitalList, RequitalItem*, pSQuest);
		if(!pSQuest)			break;									

		// ���ú�������� �׸���κ��� ������ ����������� �ε����� ������ �����´�.
		if(dwCount == (DWORD)m_pSelectRequitalGrid->GetCurSelCellPos())	
		{
			m_RequitalIndex = pSQuest->itemInfo.ItemIdx;
			m_RequitalCount = pSQuest->nItemCount;
			break;
		}
		++dwCount;	
	PTRLISTSEARCHEND

	// �������� ���� ����
	if( m_QuestSelectRequitalList.GetCount() > 0 &&	m_RequitalIndex == 0 )											
	{
		return FALSE;
	}

	return TRUE;
}

// �ش� ����Ʈ�� ���ú���������� �����ϴ��� ���θ� �Ǵ��Ѵ�.
BOOL cNpcScriptDialog::CanTakeSelectRequital( DWORD dwQuestIdx )
{
	const cPtrList* pTriggerList = QUESTMGR->GetLastSubQuestTrigger( dwQuestIdx );
	if( !pTriggerList ) return FALSE;

	int nTriggerCount = pTriggerList->GetCount();		

	for( int count = 0; count < nTriggerCount; ++count )
	{
		PTRLISTPOS pos = pTriggerList->FindIndex(count);
		if( !pos ) continue;							

		CQuestTrigger* pTrigger = (CQuestTrigger*)pTriggerList->GetAt(pos);
		if( !pTrigger )	 continue;

		cPtrList* pExeList = pTrigger->GetExeList();
		if( !pExeList )	continue;

		int nExeCount = pExeList->GetCount();
		for( int count2 = 0; count2 < nExeCount; ++count2 )
		{
			PTRLISTPOS exePos = pExeList->FindIndex(count2);
			if( !exePos ) continue;

			CQuestExecute* pExecute = (CQuestExecute*)pExeList->GetAt(exePos);
			if( !pExecute ) continue;

			DWORD dwExeKind = pExecute->GetQuestExecuteKind();
			if( dwExeKind == eQuestExecute_TakeSelectItem )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

WORD cNpcScriptDialog::GetConversationListFontIdx() const
{
	return m_pListDlg->GetFontIdx();
}

LONG cNpcScriptDialog::GetConversationListTextWidth() const
{
	return m_pListDlg->GetTextWidth();
}

const HYPERLINK& cNpcScriptDialog::GetHyperLink(LPCTSTR text,DWORD data)
{
	cHyperTextList* pHyper = NPCSCRIPTMGR->GetHyperTextList();

	typedef std::list< cPage* > PageContainer;
	PageContainer pageContainer;
	pageContainer.push_back(
		NPCSCRIPTMGR->GetPage(m_dwCurNpc, m_dwCurPageId));
	pageContainer.push_back(
		NPCSCRIPTMGR->GetMainPage(m_dwCurNpc));

	for(PageContainer::const_iterator iterator = pageContainer.begin();
		pageContainer.end() != iterator;
		++iterator)
	{
		cPage* const page = *iterator;

		if(0 == page)
		{
			continue;
		}

		for(int i = 0; i < page->GetHyperLinkCount(); ++i)
		{
			const HYPERLINK* const hyperlink = page->GetHyperText(
				i);

			if(0 == hyperlink )
			{
				break;
			}

			if( hyperlink->dwData != data)
			{
				continue;
			}

			const DIALOGUE* const dialogue = pHyper->GetHyperText(hyperlink->wLinkId);

			if(0 == dialogue)
			{
				continue;
			}

			if(0 == _tcscmp(dialogue->str, text))
			{
				return *hyperlink;
			}
		}
	}

	static HYPERLINK emptyHyperLink = {0};

	return emptyHyperLink;
}
void cNpcScriptDialog::questLinkRedirect(DWORD questIdx)
{
	// 100414 ONS ����Ʈ �Ϸḵũ�� Ŭ�������� ������ ���õǾ��ִ��� üũ
	if (!IsRequitalSelected(m_dwQuestIdx))
	{
		WINDOWMGR->MsgBox(MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(2214));
		return;
	}

	// 100524 ONS ����Ʈ����Ʈ �ִ밳�� üũó�� �̵� (��ũ�������� ���������� üũ�ϵ��� ����)
	CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog();
	if (pQuestDialog)
	{
		pQuestDialog->SetQuestCount();
		if (pQuestDialog->GetQuestCount() >= MAX_QUEST_COUNT)
		{
			// �ִ밹���ϰ��, ����Ʈ�� ��Ͽ� ����(������, �Ϸ�)�ϸ� �����Ű����,
			// �׷������� ���(����)�� ����ƮFULL�޼����� ����ϰ� �����Ѵ�.
			if (FALSE == pQuestDialog->IsExistInQuestList(m_dwQuestIdx))
			{
				WINDOWMGR->MsgBox(MBI_QUEST_FULL, MBT_OK, CHATMGR->GetChatMsg(1315));
				return;
			}
		}
	}

	if (m_dwQuestIdx)															// ����Ʈ �ε��������� ��ȿ�ϴٸ�,
	{
		QUESTMGR->SendNpcTalkEvent(m_dwCurNpc, m_dwQuestIdx, m_RequitalIndex, m_RequitalCount);					// NPC ��ȭ �̺�Ʈ�� ������.
	}

	if (NPCSCRIPTMGR->GetSelectedNpc())											// ������ NPC ������ ��ȿ�ϴٸ�,
	{
		if (NPCSCRIPTMGR->GetSelectedNpc()->GetNpcJob() == BOMUL_ROLE)			// NPC�� ������ ���� ���̶��,
		{
			MSG_DWORD msg;														// �޽��� ����ü�� �����Ѵ�.
			msg.Category = MP_NPC;												// ī�װ��� NPC�� �����Ѵ�.
			msg.Protocol = MP_NPC_DOJOB_SYN;									// ���������� ������ũ�� �����Ѵ�.
			msg.dwData = NPCSCRIPTMGR->GetSelectedNpc()->GetID();				// ������ NPC�� ���̵� �����Ѵ�.
			msg.dwObjectID = HEROID;											// HERO�� ���� �����Ѵ�.
			NETWORK->Send(&msg, sizeof(msg));									// �޽����� �����Ѵ�.
		}
	}

	// 090924 ShinJS --- ����Ʈ ����/�Ϸ� ��ư ��Ȱ��ȭ
	m_pQuestBtn->SetActive(FALSE);
	m_pRequitalInfoDlg->SetActive(FALSE);
	m_pLinkListDlg->SetActive(TRUE);

}