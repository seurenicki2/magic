#include "stdafx.h"																	// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.

#include "WindowIDEnum.h"															// ������ ���̵� �̳� ����� �ҷ��´�.

#include "../Input/Mouse.h"															// ���콺 Ŭ���� ����� �ҷ��´�.

#include "GameIn.h"

#include "ChatManager.h"															// ä�� �Ŵ��� Ŭ���� ����� �ҷ��´�.
#include "ObjectManager.h"															// ������Ʈ �Ŵ��� Ŭ���� ����� �ҷ��´�.
#include "QuestManager.h"															// ����Ʈ �Ŵ��� Ŭ���� ����� �ҷ��´�.

#include "./Interface/cScriptManager.h"												// ��ũ��Ʈ �Ŵ��� Ŭ���� ����� �ҷ��´�.
#include "./Interface/cWindowManager.h"												// ������ �Ŵ��� Ŭ���� ����� �ҷ��´�.

#include "../ItemManager.h"

#include "NpcScriptManager.h"														// npc ��ũ��Ʈ �Ŵ��� ����� �ҷ��´�.

#include "./Interface/cFont.h"														// ��Ʈ Ŭ���� ����� �ҷ��´�.
#include "./Interface/cIcon.h"														// ������ Ŭ���� ����� �ҷ��´�.
#include "./Interface/cIconGridDialog.h"											// ������ �׸��� ���̾�α� Ŭ���� ����� �ҷ��´�.
#include "./Interface/cListDialog.h"												// ����Ʈ ���̾�α� Ŭ���� ����� �ҷ��´�.
#include "./Interface/cPushupButton.h"												// Ǫ�þ� ��ư Ŭ���� ����� �ҷ��´�.
#include "./Interface/cStatic.h"													// ����ƽ Ŭ���� ����� �ҷ��´�.

#include "cDialogueList.h"															// ���̾�α� ����Ʈ ����� �ҷ��´�.
#include "cMsgBox.h"																// �޽��� �ڽ� Ŭ���� ����� �ҷ��´�.
#include "cPage.h"																	// ������ Ŭ���� ����� �ҷ��´�.
#include "QuestDialog.h"															// ����Ʈ ���̾�α� Ŭ���� ����� �ҷ��´�.
#include "Quest.h"																	// ����Ʈ Ŭ���� ����� �ҷ��´�.

#include "..\[CC]Quest\QuestExecute.h"												// ����Ʈ ���� Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestExecute_Item.h"											// ����Ʈ ���� ������ Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestInfo.h"													// ����Ʈ ���� Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestNpcScript.h"											// ����Ʈ npc ��ũ��Ʈ Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestString.h"												// ����Ʈ ��Ʈ�� Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestTrigger.h"												// ����Ʈ Ʈ���� Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\SubQuestInfo.h"												// ���� ����Ʈ ���� Ŭ���� ����� �ҷ��´�.
#include "..\[CC]Quest\QuestExecute_SelectItem.h"											// ����Ʈ ���� ������ Ŭ���� ����� �ҷ��´�.
#include "QuestQuickViewDialog.h"
#include "InventoryExDialog.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestDialog::CQuestDialog()														// ������ �Լ�.
{
	m_type	= WT_QUESTDIALOG;														// ����Ʈ ���̾�α��� Ÿ���� WT_QUESTDIALOG�� �����Ѵ�.
	m_SelectedQuestID	= 0;														// ���� �� ����Ʈ ���̵� -1�� �����Ѵ�.
	m_QListCount		= 0;														// ����Ʈ ����Ʈ ī��Ʈ�� 0���� �����Ѵ�.
	m_ChangedRow		= -1;														// ���� �� �� ��ȣ�� -1�� �����Ѵ�.
	m_QuestItemPage		= 0;														// ����Ʈ ������ �������� 0���� �����Ѵ�.
	m_TooltipIdx		= -1;														// ���� �ε����� -1�� �����Ѵ�.
	m_RequitalItemTooltipIdx = -1 ;
	memset(m_QuestList, 0, sizeof(CQuestString*)*MAX_PROCESSQUEST);					// ����Ʈ ����Ʈ�� �޸� ���Ѵ�.

	m_IndexGenerator.Init( 999, IG_QUESTITEM_STARTINDEX);							// ����Ʈ ������ ���� �ε����� �ʱ�ȭ �Ѵ�.

	m_pQuestCount		= NULL ;													// ����Ʈ �������� ����ϴ� ����ƽ �����͸� NULLó���Ѵ�.

	m_pNoticeBtn		= NULL ;													// ����Ʈ �˸��� ��ư �����͸� NULL ó�� �Ѵ�.
	m_pGiveupBtn		= NULL ;													// ����Ʈ ���� ��ư �����͸� NULLó�� �Ѵ�.

	m_pQuestDescList	= NULL ;													// ����Ʈ ���� ����Ʈ ���̾�α� �����͸� NULL ó�� �Ѵ�.

	m_pRequitalGrid		= NULL ;													// ����Ʈ ���� ������ ���̾�α� �����͸� NULL ó�� �Ѵ�.

	m_pRequitalGold		= NULL ;													// ��� ���� ����ƽ �����͸� NULL ó�� �Ѵ�.
	m_pExperience		= NULL ;													// ����ġ ���� ����ƽ �����͸� NULL ó�� �Ѵ�.

	m_nQuestCount = 0 ;

	m_dwNpcIdx = 0 ;
	m_dwScriptPage = 0 ;
	m_bRequitalType = BYTE(eQuestExecute_TakeItem);
}

CQuestDialog::~CQuestDialog()														// �Ҹ��� �Լ�.
{
	m_IndexGenerator.Release();														// �ε��� �����⸦ �����Ѵ�.

	{
		sQuestItem* pItem;

		PTRLISTPOS pos = NULL ;
		pos = m_QuestItemList.GetHeadPosition() ;

		while(pos)
		{
			pItem = NULL ;
			pItem = (sQuestItem*)m_QuestItemList.GetNext(pos) ;

			if( pItem )
			{
				m_QuestItemList.Remove(pItem) ;

				delete pItem ;
			}
		}

		m_QuestItemList.RemoveAll() ;
	}

	{
		PTRLISTPOS deletePos = m_QuestRequitalList.GetHeadPosition() ;

		while(deletePos)
		{
			RequitalItem* pDeleteItem = (RequitalItem*)m_QuestRequitalList.GetNext(deletePos) ;

			if( pDeleteItem )
			{
				m_QuestRequitalList.Remove(pDeleteItem) ;

				delete pDeleteItem ;
			}
		}

		m_QuestRequitalList.RemoveAll() ;
	}

	{
		// 100414 ONS ���ú��󸮽�Ʈ �޸� ����
		PTRLISTPOS deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
		while(deletePos)
		{
			RequitalItem* pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
			if( pDeleteItem )
			{
				m_QuestSelectRequitalList.Remove(pDeleteItem) ;
				delete pDeleteItem ;
			}
		}

		m_QuestSelectRequitalList.RemoveAll() ;
	}

	{
		QuestTree* pTree ;
		DWORD* pQuestIdx ;

		PTRLISTPOS treePos ;
		PTRLISTPOS idPos ;

		treePos = NULL ;
		treePos = m_QuestTree.GetHeadPosition() ;

		while(treePos)
		{
			pTree = NULL ;
			pTree = (QuestTree*)m_QuestTree.GetNext(treePos) ;

			if( !pTree ) continue ;

			idPos = NULL ;
			idPos = pTree->list.GetHeadPosition() ;

			while(idPos)
			{
				pQuestIdx = NULL ;
				pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;

				if( !pQuestIdx ) continue ;

				pTree->list.Remove(pQuestIdx) ;

				delete pQuestIdx ;
			}

			pTree->list.RemoveAll() ;

			m_QuestTree.Remove(pTree) ;

			delete pTree ;
			pTree = NULL ;
		}
		m_QuestTree.RemoveAll() ;
	}
}

void CQuestDialog::DeleteTree(QuestTree* pTree)
{
	if(!pTree)
	{
		return ;
	}

	CQuestString* pDeleteString ;

	PTRLISTPOS pos = NULL ;
	pos = pTree->list.GetHeadPosition() ;

	while(pos)
	{
		pDeleteString = NULL ;
		pDeleteString = (CQuestString*)pTree->list.GetNext(pos) ;

		if( pDeleteString )
		{
			pTree->list.Remove(pDeleteString) ;

			delete pDeleteString ;
			pDeleteString = NULL ;
		}
	}

	pTree->list.RemoveAll() ;

	m_QuestTree.Remove(pTree) ;

	delete pTree ;

	pTree = NULL ;
}

void CQuestDialog::Linking()														// ���� ��Ʈ�ѵ��� ��ũ�ϴ� �Լ�.
{
	for(int i=0; i<MAX_QUEST_PAGEBTN; ++i)											// �ִ� ����Ʈ ������ �� ��ŭ for���� ������.
	{
		m_pPageBtn[i] = (cPushupButton*)GetWindowForID(QUE_PAGE1BTN+i);				// ����Ʈ ������ ������ ������ ��ũ�Ѵ�.
	}

	m_pPageBtn[0]->SetPush(TRUE);													// ù ������ ��ư�� ���� ���·� �����Ѵ�.
	
//	m_pTitleBtn = (cButton*)GetWindowForID(QUE_TITLEBTN);
	m_pQuestListDlg = (cListDialog*)GetWindowForID(QUE_QUESTLIST);					// ����Ʈ ����Ʈ ������ ��ũ�Ѵ�.
	m_pIconGridDlg = (cIconGridDialog*)GetWindowForID(QUE_ITEM_GRID);				// ����Ʈ �������� ������ �׸��� ������ ��ũ�Ѵ�.

	m_pQuestCount = (cStatic*)GetWindowForID(QUE_COUNTSTATIC) ;						// ����Ʈ �������� ����ϴ� ����ƽ ������ ��ũ�Ѵ�.

	m_pNoticeBtn = (cButton*)GetWindowForID(QUE_NOTICEBTN) ;						// ����Ʈ �˸��� ��ư ������ ��ũ�Ѵ�.
	m_pGiveupBtn = (cButton*)GetWindowForID(QUE_GIVEUPBTN) ;						// ����Ʈ ���� ��ư ������ ��ũ�Ѵ�.

	m_pQuestDescList	= (cListDialog*)GetWindowForID(QUE_DESCDLG) ;				// ����Ʈ ���� ����Ʈ ���̾�α� ������ ��ũ�Ѵ�.

	m_pRequitalGrid		= (cIconGridDialog*)GetWindowForID(QUE_REQUITAL_GRID) ;		// ����Ʈ ���� ������ ���̾�α� ������ ��ũ�Ѵ�.

	m_pRequitalGold		= (cStatic*)GetWindowForID(QUE_REQUITAL_GOLD) ;				// ��� ���� ����ƽ �����͸� ������ ��ũ�Ѵ�.
	m_pExperience		= (cStatic*)GetWindowForID(QUE_REQUITAL_EXP) ;				// ����ġ ���� ����ƽ �����͸� ������ ��ũ�Ѵ�.
	
	// 100414 ONS ���ú��� ���� UI�߰�
	m_pSelectRequitalGrid	= (cIconGridDialog*)GetWindowForID(QUE_SELECT_REQUITAL_GRID) ;
	m_pRequitalBtn			= (cPushupButton*)GetWindowForID(QUE_BTN_REQUITAL_FIX);
	m_pSelectRequitalBtn	= (cPushupButton*)GetWindowForID(QUE_BTN_REQUITAL_SELECT);

	m_pItemSignalOn			= (cStatic*)GetWindowForID(QUE_STATIC_REQUITAL_ON);
	m_pItemSignalOff		= (cStatic*)GetWindowForID(QUE_STATIC_REQUITAL_OFF);
	m_pSelectItemSignalOn	= (cStatic*)GetWindowForID(QUE_STATIC_SELECTREQUITAL_ON);
	m_pSelectItemSignalOff	= (cStatic*)GetWindowForID(QUE_STATIC_SELECTREQUITAL_OFF);

	m_pRequitalBtn->SetPush(TRUE);
	m_pRequitalGrid->SetActive(TRUE);
	m_pSelectRequitalBtn->SetPush(FALSE);
	m_pSelectRequitalGrid->SetActive(FALSE);

}

DWORD CQuestDialog::ActionEvent(CMouse* mouseInfo)										// ����Ʈ ���̾�α��� �̺�Ʈ�� ó���ϴ� �Լ�.
{
	DWORD we = WE_NULL;																	// �̺�Ʈ�� ���� ������ �����ϰ� NULL ó���� �Ѵ�.

	if( !m_bActive )																	// ����Ʈ ���̾�αװ� �����ִ� ���°� �ƴ϶��, 
	{
		return we;																		// �̺�Ʈ�� �����Ѵ�.
	}

	we = cDialog::ActionEvent(mouseInfo);												// ���̾�α��� �̺�Ʈ�� �޴´�.

	int nMouseX = mouseInfo->GetMouseX() ;												// �̺�Ʈ�� �߻��ϴ� ���콺 X��ǥ�� �޴´�.
	int nMouseY = mouseInfo->GetMouseY() ;												// �̺�Ʈ�� �߻��ϴ� ���콺 Y��ǥ�� �޴´�.
    																					// Show QuestItem Tooltip
	if(m_pIconGridDlg->PtInWindow(nMouseX,nMouseY))										// ���콺�� ������ �׸��� �ȿ� ������,
	{
		WORD Pos = WORD(-1);

		if(m_pIconGridDlg->GetPositionForXYRef(nMouseX,nMouseY,Pos))					// ������ �׸��� �󿡼� ��ġ�� �޴´�. ��ġ�� ��ȿ�ϸ�,
		{
			WORD RealPos = WORD(Pos + m_QuestItemPage*VIEW_QUESTITEM_PERPAGE);

			if(m_TooltipIdx != RealPos)													// ���� �ε����� �������� ���� ������,
			{
				ShowItemToolTip(RealPos);												// ���� ��ġ�� ������ �����ش�.
			}

			return 0;																	// 0�� �����Ѵ�.
		}
	}
	// 100414 ONS �Ϲݺ��� / ���ú��� �������
	if( m_bRequitalType == (BYTE)eQuestExecute_TakeItem &&
		m_pRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))					// ������ �׸��� �󿡼� ��ġ�� �޴´�. ��ġ�� ��ȿ�ϸ�,
		{
			WORD RealPos = Pos ;														// �������� �����Ͽ� ���� ��ġ�� �޴´�.
			ShowRequitalItemToolTip(m_QuestRequitalList, RealPos);											// ���� ��ġ�� ������ �����ش�.
			return 0;																	// 0�� �����Ѵ�.
		}
	}


	if(  m_bRequitalType == (BYTE)eQuestExecute_TakeSelectItem &&
		m_pSelectRequitalGrid->PtInWindow(nMouseX,nMouseY))
	{
		WORD Pos = WORD(-1);

		if(m_pSelectRequitalGrid->GetPositionForXYRef(nMouseX,nMouseY,Pos))					// ������ �׸��� �󿡼� ��ġ�� �޴´�. ��ġ�� ��ȿ�ϸ�,
		{
			WORD RealPos = Pos ;														// �������� �����Ͽ� ���� ��ġ�� �޴´�.
			ShowRequitalItemToolTip(m_QuestSelectRequitalList, RealPos);											// ���� ��ġ�� ������ �����ش�.
			return 0;																	// 0�� �����Ѵ�.
		}
	}

	if( m_pQuestListDlg->PtInWindow(nMouseX, nMouseY) )									// ���콺�� ����Ʈ ����Ʈ �ȿ� ������,
	{
		int nSelectedLine = -1 ;														// ����Ʈ���� ���� �� ������ ���� ������ �����ϰ� -1�� �����Ѵ�.

		nSelectedLine = m_pQuestListDlg->GetCurSelectedRowIdx() ;						// ����Ʈ ����Ʈ���� ���� �� ������ �޴´�.

		m_ChangedRow = nSelectedLine ;													// ������ �ٲ� ���ι�ȣ�� �����Ѵ�.

		ITEM* pItem = NULL ;
		pItem = m_pQuestListDlg->GetItem(nSelectedLine) ;

		if( !pItem ) return we ;

		//if( pItem->rgb == RGBA_MAKE(200, 128, 0, 255) )									// ���� �� ���� ������ �� Ʈ�� �����̸�,
		if(pItem->nFontIdx == cFont::FONT1)
		{
			if( we & WE_LBTNDBLCLICK )													// ���� Ŭ���� �ߴٸ�,
			{
				QuestTreeStateCheck(nSelectedLine) ;
			}
		}
		else
		{
			if( nSelectedLine != -1 )													// ���� �� ������ -1�� ���� ������,
			{
				DWORD QuestIdx = NULL ;
			
				QuestIdx = m_dwQuestList[nSelectedLine] ;								// ���� �� ������ ����Ʈ ��Ʈ���� �޴´�.

				if( QuestIdx != 0xFFFFFFFF )											// ����Ʈ ��Ʈ�� ������ ��ȿ�ϸ�,
				{				
					if( we & WE_LBTNCLICK )												// Ŭ���� �ߴٸ�,
					{
						m_SelectedQuestID = QuestIdx ;									// ���� �� ����Ʈ �ε����� �����Ѵ�.

						for(int i = 0; i < m_pQuestListDlg->GetItemCount(); ++i)
						{
							if(ITEM* const item = m_pQuestListDlg->GetItem(i))
							{
								item->rgb = (m_ChangedRow == i ? RGBA_MAKE(255, 255, 0, 255) : RGBA_MAKE(255, 255, 255, 255));
							}
						}

						QuestDescView(
							QUESTMGR->GetQuestString(m_SelectedQuestID, 0));

					}
				}
			}
		}
	}

	return we;
}

void CQuestDialog::SetQuestCount()
{
	char tempBuf[256] = {0, } ;

	DWORD dwColor = RGBA_MAKE(255, 255, 255, 255) ;

	int nCount = 0 ;

	QuestTree* pTree ;
	DWORD* pQuestIdx ;

	PTRLISTPOS treePos ;
	PTRLISTPOS idPos ;

	treePos = NULL ;
	treePos = m_QuestTree.GetHeadPosition() ;

	while(treePos)
	{
		pTree = NULL ;
		pTree = (QuestTree*)m_QuestTree.GetNext(treePos) ;

		if( !pTree ) continue ;

		idPos = NULL ;
		idPos = pTree->list.GetHeadPosition() ;

		while(idPos)
		{
			pQuestIdx = NULL ;
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;

			if( !pQuestIdx ) continue ;

			++nCount ;
		}
	}

	if( nCount > MAX_QUEST_COUNT )
	{
		dwColor = RGBA_MAKE(255, 0, 0, 255) ;
	}

	sprintf(tempBuf, "(%d/%d)", nCount, MAX_QUEST_COUNT) ;

	m_pQuestCount->SetStaticText(tempBuf) ;
	m_pQuestCount->SetFGColor(dwColor) ;

	m_nQuestCount = nCount ;
}

void CQuestDialog::SetGold(DWORD dwGold) 
{
	char tempBuf[128] = {0, } ;

	sprintf(tempBuf, "%d", dwGold) ;

	m_pRequitalGold->SetStaticText(tempBuf) ;
}

void CQuestDialog::SetExp(DWORD dwExp)
{
	char tempBuf[128] = {0, } ;

	sprintf(tempBuf, "%d", dwExp) ;

	m_pExperience->SetStaticText(tempBuf) ;
}

void CQuestDialog::Render()															// ���� �Լ�.
{
	cDialog::Render();																// ���̾�α׸� �����Ѵ�.

	int Count = 0;																	// ī��Ʈ�� �����ϰ� 0���� �����Ѵ�.
	int	nStart = m_QuestItemPage*VIEW_QUESTITEM_PERPAGE;							// ��ŸƮ �ε����� �����Ѵ�.
	
	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// ����Ʈ ������ ����Ʈ���� ����Ʈ ������ ������ ã�´�.
		if(!pSQuest)				break;											// ����Ʈ ������ ������ ��ȿ���� ������, ����������.

		if(Count >= nStart)															// ī��Ʈ�� ��ŸƮ �ε��� �̻��̸�,
		{
																					// ���� ��ġ ��Ƽ� ��´�.
			if(pSQuest->Count > 1)													// ����Ʈ �������� �ϳ� �̻��̸�,
			{
				static char nums[4];
				int absX=0;	int absY=0;												// ��� ��ġ ������ �����ϰ� 0���� �����Ѵ�.

				wsprintf(nums,"%3d", pSQuest->Count);								// ���ڸ� �� ���� ������ ������ �����Ѵ�.

				if(m_pIconGridDlg->GetCellAbsPos(pSQuest->Pos, absX, absY))			// ������ �׸��忡�� ��ġ ������ ��ȿ�ϸ�,
				{
					// 070209 LYW --- QuestDialog : Modified render part.
					m_renderRect.left	= (LONG)absX+16 ;							// ��� ��ġ�� �����Ѵ�.
					m_renderRect.top	= (LONG)absY+24 ;
					m_renderRect.right	= 1 ;
					m_renderRect.bottom	= 1 ;
																					// ����Ʈ ������ ������ ����Ѵ�.
					CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
				}
			}
		}
	
		++Count;																	// ī��Ʈ�� ������Ų��.

		if(Count >= nStart+VIEW_QUESTITEM_PERPAGE)		break;						// ī��Ʈ�� ������ �� �������� ������ ���� �Ѿ��, �������´�.

	PTRLISTSEARCHEND																// ����Ʈ �˻��� ��ģ��.

	int Count2 = 0;
	int	nStart2 = 0;

	PTRLISTSEARCHSTART(m_QuestRequitalList, RequitalItem*, pSQuest2) ;
	if( !pSQuest2 || m_bRequitalType != eQuestExecute_TakeItem )			break ;

		if(Count2 >= nStart2)
		{
			if( pSQuest2->nItemCount > 1 )
			{
				static char nums[4] ;
				int absX=0; int absY=0;

				wsprintf(nums, "%3d", pSQuest2->nItemCount) ;

				if( m_pRequitalGrid->GetCellAbsPos(pSQuest2->wPos, absX, absY))
				{
					// 070209 LYW --- QuestDialog : Modified render part.
					m_renderRect.left	= (LONG)absX+16 ;							// ��� ��ġ�� �����Ѵ�.
					m_renderRect.top	= (LONG)absY+24 ;
					m_renderRect.right	= 1 ;
					m_renderRect.bottom	= 1 ;
																					// ����Ʈ ������ ������ ����Ѵ�.
					CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
				}
			}
		}

		++Count2;																	// ī��Ʈ�� ������Ų��.

		if(Count2 >= nStart2 + MAX_REQUITAL_ICON_CELL)		break;					// ī��Ʈ�� ������ �� �������� ������ ���� �Ѿ��, �������´�.
	PTRLISTSEARCHEND

	// 100414 ONS ���ú��� ������ ���� ���
	Count2 = 0;
	PTRLISTSEARCHSTART(m_QuestSelectRequitalList, RequitalItem*, pSQuest2) ;
	if( !pSQuest2 || m_bRequitalType != eQuestExecute_TakeSelectItem )			break ;

		if(Count2 >= nStart2)
		{
			if( pSQuest2->nItemCount > 1)
			{
				static char nums[4] ;
				int absX=0; int absY=0;

				wsprintf(nums, "%3d", pSQuest2->nItemCount) ;

				if( m_pSelectRequitalGrid->GetCellAbsPos(pSQuest2->wPos, absX, absY))
				{
					// 070209 LYW --- QuestDialog : Modified render part.
					m_renderRect.left	= (LONG)absX+16 ;							// ��� ��ġ�� �����Ѵ�.
					m_renderRect.top	= (LONG)absY+24 ;
					m_renderRect.right	= 1 ;
					m_renderRect.bottom	= 1 ;
																					// ����Ʈ ������ ������ ����Ѵ�.
					CFONT_OBJ->RenderFontShadow(0, 1, nums,strlen(nums),&m_renderRect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
				}
			}
		}

		++Count2;																	// ī��Ʈ�� ������Ų��.

		if(Count2 >= nStart2 + MAX_REQUITAL_ICON_CELL)		break;					// ī��Ʈ�� ������ �� �������� ������ ���� �Ѿ��, �������´�.
	PTRLISTSEARCHEND
}

void CQuestDialog::SubQuestAdd(CQuestString* pQuestString)							// ���� ����Ʈ �߰� �Լ�.
{
	if(!pQuestString)		return;													// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������, ����ó���� �Ѵ�.

	QuestTree* pData = new QuestTree;												// ����Ʈ Ʈ�� �޸𸮸� �Ҵ��Ѵ�.
	pData->QuestIdx = pQuestString->GetQuestID();									// ����Ʈ �ε����� �����Ѵ�.
	pData->State = eQTree_Close;													// ����Ʈ Ʈ�� ���¸� ������ ���·� �����Ѵ�.
	pData->list.AddHead(pQuestString);												// ����Ʈ ��Ʈ���� Ʈ�� ����Ʈ�� �߰��Ѵ�.
	++pData->Count;																	// ����Ʈ Ʈ���� ī��Ʈ�� �����Ѵ�.
	
	m_QuestTree.AddTail(pData);														// ����Ʈ Ʈ�� ����Ʈ�� Ʈ�� ������ �߰��Ѵ�.
}

void CQuestDialog::QuestTreeStateCheck(int RowIdx)									// ����Ʈ Ʈ�� ���¸� üũ�ϴ� �Լ�.
{
	if(RowIdx<0)			return;													// �� �ε����� 0�̻��̸�, ���� ó���� �Ѵ�.

	int mapNum = int(m_dwQuestList[RowIdx]);
	PTRLISTPOS pos = m_QuestTree.GetHeadPosition() ;

	while(pos)
	{
		QuestTree* pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;

		if( !pTree ) continue ;

		if( pTree->nMapNum != mapNum ) continue ;

		if(pTree->State == eQTree_Open)									// ����Ʈ Ʈ���� ���� ���¶��,
		{
			pTree->State = eQTree_Close;									// Ʈ�� ���¸� �������� �����Ѵ�.
		}
		else if(pTree->State == eQTree_Close)								// ����Ʈ Ʈ���� �����ִ� ���¶��,
		{
			pTree->State = eQTree_Open;									// ����Ʈ Ʈ���� ���� ���·� �����Ѵ�.
		}

		break;																// Ʈ�� ����Ʈ �˻��� ����������.
	}

	RefreshQuestList();	
}

DWORD CQuestDialog::GetQuestIdx(int index)
{
	if( index < 0 || index >= m_QListCount )
	{
		return NULL ;
	}

	return m_dwQuestList[index] ;
}

void CQuestDialog::QuestListAdd(CQuestString* pQuestString, int SubID)				// ����Ʈ ����Ʈ�� ��Ʈ���� �߰��ϴ� �Լ�.
{	
	if(!pQuestString)			return;												// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������, ���� ó���� �Ѵ�.

	ITEM* pQStr = 0;
	char buf[512] = {0};
	PTRLISTPOS spos = pQuestString->GetTitle()->GetHeadPosition();

	while(spos)																		// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pQStr = (ITEM*)pQuestString->GetTitle()->GetNext(spos);					// ��ġ ������ �ش��ϴ� ����Ʈ ��Ʈ�� ������ �޴´�.

		if(pQStr)																	// ��Ʈ�� ������ ��ȿ�ϸ�,
		{
			if( pQuestString->IsTreeTitle() )
			{
				pQStr->rgb = RGBA_MAKE(200, 128, 0, 255) ;
				pQStr->nFontIdx = cFont::FONT1 ;
			}
			else
			{
				if( m_SelectedQuestID == int(pQuestString->GetQuestID()))
				{
					pQStr->rgb = RGBA_MAKE(0, 0, 255, 255) ;
				}
				else
				{
					pQStr->rgb = RGBA_MAKE(10, 10, 10, 255) ;
				}
				pQStr->nFontIdx = cFont::FONT0 ;
			}

			if(pQuestString->IsTreeTitle())										// ��Ʈ���� Ʈ�� Ÿ��Ʋ�̶��,
			{
				sprintf(buf, "* ") ;												// �ӽ� ���۸� *�� �����Ѵ�.
			}
			else																	// ���� ���̵� ������,
			{
				BOOL bResult = FALSE ;
				CQuestQuickViewDialog* pQuickView = NULL ;
				pQuickView = GAMEIN->GetQuestQuickViewDialog() ;

				if( !pQuickView ) continue ;

				DWORD* pCheckIdx = NULL ;

				cPtrList* pCheckList = NULL ;
				pCheckList = pQuickView->GetViewList() ;

				if( !pCheckList ) continue ;
				
				PTRLISTPOS checkPos = NULL ;
				checkPos = pCheckList->GetHeadPosition() ;

				while(checkPos)
				{
					pCheckIdx = NULL ;
					pCheckIdx = (DWORD*)pCheckList->GetNext(checkPos) ;

					if( !pCheckIdx ) continue ;

					if( *pCheckIdx == pQuestString->GetQuestID() )
					{
						bResult = TRUE ;
						break ;
					}
				}



				if( bResult == FALSE )
				{
					sprintf(buf, "	- ");												// �ӽ� ���۸� -�� �����Ѵ�.
				}
				else
				{
					sprintf(buf, "	��");												// �ӽ� ���۸� -�� �����Ѵ�.
				}
			}

			strcat(buf, pQStr->string);

			int nSubQuestCount = 0 ;
			CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(pQuestString->GetQuestID()) ;				// ����Ʈ ��ȣ�� ����Ʈ ������ �޴´�.

			if( pQuestInfo )
			{
				nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// ���� ����Ʈ ���� �޴´�.
			}

			if( pQuestString->IsComplete() || nSubQuestCount == 2 )
			{
				strcat(buf, CHATMGR->GetChatMsg(1463)) ;
			}

			m_pQuestListDlg->AddItem(buf, pQStr->rgb) ;
			memset(buf, 0, 512);													// �ӽ� ���۸� �޸� �� �Ѵ�.
			
			// ListDialog 
			m_QuestList[m_QListCount] = pQuestString;								// ����Ʈ ����Ʈ�� ����Ʈ ��Ʈ�� ������ �����Ѵ�.
			++m_QListCount;															// ����Ʈ ����Ʈ ī��Ʈ�� �����Ѵ�.
		}
	}
}

void CQuestDialog::UpdateQuestDescView(DWORD dwQuestIdx, DWORD dwSubQuestIdx)		// ����Ʈ ����â�� ������Ʈ �ϴ� �Լ�.
{
	int nQuestStringKey = 0 ;														// ����Ʈ ��Ʈ�� Ű ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	COMBINEKEY(dwQuestIdx, dwSubQuestIdx, nQuestStringKey) ;						// ���� ����Ʈ, ���� ����Ʈ �ε�����, ��ũ�� Ű ���� �޴´�.

	CQuestString* pQuestString = NULL ;												// ����Ʈ ��Ʈ���� ���� �����͸� �����ϰ� NULL ó���� �Ѵ�.
		
	pQuestString = QUESTMGR->GetQuestString(dwQuestIdx, dwSubQuestIdx);				// ����Ʈ �Ŵ������� Ű ������ ����Ʈ ��Ʈ���� �޾ƿ´�.

	if( pQuestString )
	{
		QuestDescView(pQuestString) ;												// ����Ʈ ����â�� ������Ʈ �Ѵ�.
	}
}

void CQuestDialog::SetQuestPrevInfo(CQuestString* pQuestString)
{
	char tempBuf[256] = {0, } ;

	if( !pQuestString )
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʾ�, ����Ʈ �⺻ ������ �������� ���߽��ϴ�.") ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwQuestIdx = pQuestString->GetQuestID() ;

	// - ����Ʈ ���� ���� - 
	CQuestString* pTitleString = NULL ;												// ����Ʈ ���� ��Ʈ�� ������ ���� �����͸� �����ϰ� nulló�� �Ѵ�.
	pTitleString = QUESTMGR->GetQuestString(dwQuestIdx, 0) ;						// ����Ʈ ���� ��Ʈ���� �޴´�.

	if( !pTitleString )
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)�� ����Ʈ ��Ʈ���� ���� ���� ����Ʈ ���� ó���� ���� ���߽��ϴ�.", pQuestString->GetQuestID()) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	ITEM* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = pTitleString->GetTitle()->GetHeadPosition();								// ����Ʈ ��Ʈ���� �ص� ��ġ�� �����Ѵ�.

	while(pos)																		// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pItem = NULL ;
		pItem = (ITEM*)pTitleString->GetTitle()->GetNext(pos);						// ��ġ ������ �ش��ϴ� ����Ʈ ��Ʈ�� ������ �ް�, ��ġ�� �������� �����Ѵ�.

		if(!pItem) continue ;														// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

		sprintf(tempBuf, "< %s >", pItem->string) ;									// �ӽ� ���ۿ� ����Ʈ ������ �����Ѵ�.
		m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 150, 0, 255));					// ����Ʈ ����Ʈ ���̾�α׿� �߰��Ѵ�.
	}

	m_pQuestDescList->AddItem(" ", 0);												// ����Ʈ ����Ʈ�� ��ĭ�� �߰��Ѵ�.

	CQuest* pQuest = NULL ;
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;

	if( !pQuest )
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)�� ����Ʈ�� ���� ���ؼ� ����Ʈ �Ƿ��ڸ� �������� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwCurSubQuestIdx = pQuest->GetCurSubQuestIdx() ;							// ���� �������� ���� ����Ʈ �ε����� �޴´�.

	if( dwCurSubQuestIdx != 0 )
	{
		--dwCurSubQuestIdx ;
	}

	CSubQuestInfo* pSubQuestInfo = NULL ;											// ���� ����Ʈ ������ ���� �����͸� �����Ѵ�.
	pSubQuestInfo = pQuest->GetSubQuestInfo(dwCurSubQuestIdx) ;

	if( !pSubQuestInfo ) 
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)�� ����Ʈ�� (%d)�� ���� ����Ʈ ������ ���� ���ؼ� �Ƿ��ڸ� �������� ���߽��ϴ�.", dwQuestIdx, dwCurSubQuestIdx-1) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	// 100408 ShinJS --- �ð����� ����Ʈ�̰� ���� ����Ұ����� �ð��ΰ�� ���� �ð��� �˷��ش�.
	if( pQuest->HasDateLimit() && !pQuest->IsValidDateTime() )
	{
		memset(tempBuf, 0, sizeof(tempBuf)) ;									// �ӽ� ���۸� �ʱ�ȭ �Ѵ�.
		__time64_t nextTime = pQuest->GetNextDateTime();
		struct tm nextTimeWhen = *_localtime64( & nextTime );

		sprintf(tempBuf, CHATMGR->GetChatMsg( 2215 ), nextTimeWhen.tm_mon + 1, nextTimeWhen.tm_mday, nextTimeWhen.tm_hour, nextTimeWhen.tm_min );		// "���� ���� ���� �ð� : %d�� %d�� %02d:%02d"
		m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255));		// ����Ʈ ���� ����Ʈ�� �߰��Ѵ�.
		m_pQuestDescList->AddItem(" ", 0);
	}

	// 100518 ShinJS --- ����Ʈ�� ScriptData ����
	m_dwNpcIdx = 0;
	CQuestNpcScript* pQuestNpcScript = pSubQuestInfo->GetQuestNpcScript();
	if( pQuestNpcScript )
	{
		m_dwNpcIdx = pQuestNpcScript->GetNpcIdx();
		m_dwScriptPage = pQuestNpcScript->GetNpcScriptPage();
		m_dwDialogType = pQuestNpcScript->GetDialogType();

		stNPCIDNAME* pNpcIdName = QUESTMGR->GetNpcIdName( m_dwNpcIdx );		// ����Ʈ �Ŵ������� npc ���̵�, �̸��� �޴´�.
		if( !pNpcIdName ) 
			return;

		memset(tempBuf, 0, sizeof(tempBuf)) ;									// �ӽ� ���۸� �ʱ�ȭ �Ѵ�.
		sprintf(tempBuf, CHATMGR->GetChatMsg( 1446 ), pNpcIdName->name) ;
		m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255));		// ����Ʈ ���� ����Ʈ�� �߰��Ѵ�.
	}

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;					// ����Ʈ ��ȣ�� ����Ʈ ������ �޴´�.

	if( !pQuestInfo )																// ����Ʈ ������ ��ȿ���� �ʴٸ�,
	{
		/*sprintf(tempBuf, "[SetQuestPrevInfo] - (%d)�� ����Ʈ ������ �޾ƿ��� ���� ����Ʈ ���� ��ġ�� �������� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// ���� ó���� �Ѵ�.
	}

	MAPTYPE nMapNum = MAPTYPE(pQuestInfo->GetMapNum());
	memset(tempBuf, 0, sizeof(tempBuf)) ;											// �ӽ� ���۸� �ʱ�ȭ �Ѵ�.
	char* pMapName = GetMapName(nMapNum) ;											// �� ��ȣ�� ���� �� �̸��� �޴´�.	
	sprintf(tempBuf, CHATMGR->GetChatMsg( 1447 ), pMapName) ;
	m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255));				// ����Ʈ ���� �߰��Ѵ�.
}





void CQuestDialog::RequestInfo(CQuestString* pQuestString)
{
	char tempBuf[256] = {0, } ;

	if( !pQuestString )
	{
		/*sprintf(tempBuf, "[RequestInfo] - ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʾ�, �Ƿڳ����� �������� ���߽��ϴ�.") ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwQuestIdx = pQuestString->GetQuestID() ;

	CQuest* pQuest = NULL ;
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;				// ����Ʈ ��ȣ�� ����Ʈ ������ �޴´�.

	if( !pQuestInfo )																// ����Ʈ ������ ��ȿ���� �ʴٸ�,
	{
		/*sprintf(tempBuf, "[QuestDescView] - (%d)�� ����Ʈ ������ �޾ƿ��� ���� ����Ʈ ����â�� �������� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// ���� ó���� �Ѵ�.
	}

	CSubQuestInfo* pSubQuestInfo = NULL ;											// ���� ����Ʈ ������ ���� �����͸� �����Ѵ�.
	CQuestString* pQString ;

	PTRLISTPOS pos ;

	cPtrList* pTitleList ;
	cPtrList* pDescList ;

	int QuestStringKey = 0 ;

	int nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// ���� ����Ʈ ���� �޴´�.

    for( int count = 0 ; count < nSubQuestCount ; ++count )							// ���� ����Ʈ �� ��ŭ for���� ������.
	{
		pSubQuestInfo = NULL ;													// ���� ����Ʈ ������ ���� �����͸� nulló�� �Ѵ�.
		pSubQuestInfo = pQuest->GetSubQuestInfoArray(count) ;					// ī��Ʈ�� �ش��ϴ� ���� ����Ʈ�� �޴´�.

		if( !pSubQuestInfo ) continue ;											// ���� ����Ʈ ������ ��ȿ���� ������ continue�Ѵ�.
		
		COMBINEKEY(dwQuestIdx, count, QuestStringKey) ;							// ����Ʈ, ���� ����Ʈ �ε����� ����Ʈ ��Ʈ���� ���Ѵ�.

		SUBQUEST* pSubQuest = pQuest->GetSubQuest(count) ;							// ���� ����Ʈ ������ �޴´�.

		pQString = NULL ;
		pQString = QUESTMGR->GetQuestString(QuestStringKey);						// ���� ����Ʈ ��Ʈ���� �޴´�.

		if( !pQString )	continue ;													// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������ continue�Ѵ�.

		// - ���� ����Ʈ Ÿ��Ʋ ���� -
		pTitleList = NULL ;
		pTitleList = pQString->GetTitle() ;											// ���� ����Ʈ�� Ÿ��Ʋ ����Ʈ�� �޴´�.

		if( !pTitleList ) continue ;												// Ÿ��Ʋ ����Ʈ�� ��ȿ���� ������ continue�Ѵ�.

		m_pQuestDescList->AddItem(" ", 0);											// ����Ʈ ����Ʈ�� ��ĭ�� �߰��Ѵ�.

		int nTitleCount = 0 ;														// ���� ����Ʈ�� Ÿ��Ʋ ī��Ʈ�� ���� ������ �����ϰ� 0���� �����Ѵ�.

		nTitleCount = pTitleList->GetCount() ;										// ���� ����Ʈ�� Ÿ��Ʋ�� �����Ѵ�.

		for( int count2 = 0 ; count2 < nTitleCount ; ++count2 )						// ���� Ÿ��Ʋ ī��Ʈ ��ŭ for���� ������.
		{
			pos = NULL ;															// ��ġ ������ ���� �����͸� nulló���� �Ѵ�.
			pos = pTitleList->FindIndex(count2) ;									// Ÿ��Ʋ�� ��ġ�� �޴´�.

			if( !pos ) continue ;													// ��ġ ������ ��ȿ���� ������ continue�Ѵ�.

			ITEM* pTitleLine = (ITEM*)pTitleList->GetAt(pos) ;						// Ÿ��Ʋ ��Ʈ���� �޴´�.

			if( !pTitleLine ) continue ;											// Ÿ��Ʋ ��Ʈ���� ��ȿ���� ������, continue�Ѵ�.

			if( !pSubQuest ) continue ;												// ���� ����Ʈ ������ ��ȿ���� ������ continue�Ѵ�.

			memset(tempBuf, 0, sizeof(tempBuf)) ;									// �ӽ� ���۸� �ʱ�ȭ �Ѵ�.

			if( pQString->IsComplete() )											// ����Ʈ�� �Ϸ� �Ǿ�����,
			{
				//sprintf(tempBuf, "%s (�Ϸ�)", pTitleLine->string) ;					// ���� �ϷḦ �߰��Ѵ�.
				sprintf(tempBuf, "%s", pTitleLine->string) ;
				strcat(tempBuf, CHATMGR->GetChatMsg(1463)) ;

				m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255)) ;	// ����� �ϷḦ ����Ѵ�.
			}
			else																	// �Ϸ��� ����Ʈ�� �ƴϸ�,
			{	
				if( pSubQuest->dwMaxCount == 0 )									// ���� ����Ʈ�� ��Ż ī��Ʈ�� 0�� ������,
				{
					if( pSubQuest->dwData == 0 )									// ���� ����Ʈ�� ���� ī��Ʈ�� 0�� ������,
					{
						sprintf(tempBuf, "%s", pTitleLine->string) ;				// ����Ʈ ���� �ӽù��ۿ� �����Ѵ�.
					}
					else															// ���� ����Ʈ�� ���� ī��Ʈ�� 0�� �ƴϸ�,
					{
						sprintf(tempBuf, "%s (%d)", pTitleLine->string, pSubQuest->dwData) ; // ���� ����Ʈ�� �����, ������ �ӽ� ���ۿ� �����Ѵ�.
					}
				}
				else																// ���� ����Ʈ�� ��Ż ī��Ʈ�� 0�� �ƴϸ�,
				{
					sprintf(tempBuf, "%s (%d/%d)", pTitleLine->string, pSubQuest->dwData, pSubQuest->dwMaxCount) ; // ���� ����Ʈ�� ����� ���� ī��Ʈ�� �ӽ� ���ۿ� �����Ѵ�.
				}										

				m_pQuestDescList->AddItem(tempBuf, RGBA_MAKE(255, 255, 0, 255)) ;		// ���õ� ��Ʈ���� ����Ʈ ���� ����Ʈ ���̾�α׿� �߰��Ѵ�.
			}
		}

        // - ���� ����Ʈ ���� ����
		pDescList = pQString->GetDesc() ;										// ���� ����Ʈ ���� ����Ʈ�� �޴´�.

		if( !pDescList ) continue ;												// ���� ����Ʈ ������ ��ȿ���� ������, continue�Ѵ�.

		int nDescCount = 0 ;													// ���� ������ ���� ������ �����ϰ� 0���� �����Ѵ�.

		nDescCount = pDescList->GetCount() ;									// ���� ����Ʈ�� ������ ���� �޴´�.

		for( int count2 = 0 ; count2 < nDescCount ; ++count2 )					// ���� ���� ��ŭ for���� ������.
		{
			pos = NULL ;														// ��ġ ������ ���� �����͸� nulló���� �Ѵ�.
			pos = pDescList->FindIndex(count2) ;								// ī��Ʈ�� �ش��ϴ� ��ġ ������ �޴´�.

			if( !pos ) continue ;												// ��ġ ������ ��ȿ���� ������ continue�Ѵ�.

			ITEM* pDescLine = (ITEM*)pDescList->GetAt(pos) ;  					// ���� ��Ʈ���� �޴´�.

			if( pDescLine )														// ���� ��Ʈ�� ������ ��ȿ�ϴٸ�,						
			{
				m_pQuestDescList->AddItem(pDescLine->string, pDescLine->rgb) ;	// ����Ʈ ���� ����Ʈ�� �߰��� �Ѵ�.
			}
		}

		pTitleList = NULL ;														// ���� ����Ʈ �����͸� nulló���� �Ѵ�.
		pDescList  = NULL ;														// ���� ����Ʈ �����͸� nulló���� �Ѵ�.

		pQString = NULL ;														// ��Ʈ�� �����͸� nulló���� �Ѵ�.			
	}

	m_pQuestDescList->AddItem(" ", 0);												// ����Ʈ ����Ʈ�� ��ĭ�� �߰��Ѵ�.
	m_pQuestDescList->AddItem(CHATMGR->GetChatMsg( 1448 ), RGBA_MAKE(255, 255, 0, 255));				// <�Ƿڳ���>�� �߰��Ѵ�.
	if( m_dwNpcIdx != 0 )
	{
		m_pQuestDescList->AddItem(" ", 0);												// ����Ʈ ����Ʈ�� ��ĭ�� �߰��Ѵ�.
	}

	DWORD scriptPage = m_dwScriptPage;

	if(0 < m_dwDialogType)
	{
		if(CSubQuestInfo* pSubQuestInfo = pQuestInfo->GetSubQuestInfo(0))
		{
			scriptPage = pSubQuestInfo->GetNpcScriptPage(m_dwNpcIdx);
		}
	}

	while(cPage* pPage = NPCSCRIPTMGR->GetPage(m_dwNpcIdx, scriptPage))
	{
		m_pQuestDescList->AddItemAutoLine(
			NPCSCRIPTMGR->GetDialogueList()->GetDialogue(pPage->GetDialogue()),
			RGBA_MAKE(255, 255, 255, 255));
		m_pQuestDescList->AddItem(
			"",
			0);

		if(HYPERLINK* const pHyperLink = pPage->GetHyperText(0))
		{
			scriptPage = pHyperLink->dwData;
		}
		else
		{
			break;
		}
	}
}




void CQuestDialog::RequitalInfo(CQuestString* pQuestString)
{
	SetGold(0) ;
	SetExp(0) ;
	// 100414 ONS ������� UI�ʱ�ȭ
	InitRequitalDlg();

	if( !pQuestString )
	{
		/*sprintf(tempBuf, "[RequitalInfo] - ����Ʈ ��Ʈ�� ������ ��ȿ���� �ʾ�, �Ƿڳ����� �������� ���߽��ϴ�.") ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	DWORD dwQuestIdx = pQuestString->GetQuestID() ;

	CQuest* pQuest = NULL ;
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;

	CQuestInfo* pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;				// ����Ʈ ��ȣ�� ����Ʈ ������ �޴´�.

	if( !pQuestInfo )																// ����Ʈ ������ ��ȿ���� �ʴٸ�,
	{
		/*sprintf(tempBuf, "[RequitalInfo] - (%d)�� ����Ʈ ������ �޾ƿ��� ���� ����Ʈ ����â�� �������� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// ���� ó���� �Ѵ�.
	}

	RequitalItem* pDeleteItem = NULL ;

	PTRLISTPOS deletePos = NULL ;
	deletePos = m_QuestRequitalList.GetHeadPosition() ;

	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestRequitalList.GetNext(deletePos) ;

		if( !pDeleteItem ) continue ;

		m_pRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;

		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;

		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;

		m_QuestRequitalList.Remove(pDeleteItem) ;

		delete pDeleteItem ;
	}

	m_QuestRequitalList.RemoveAll() ;

	// 100314 ONS ���ú��� ����Ʈ�� �����Ѵ�.
	deletePos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(deletePos)
	{
		pDeleteItem = NULL ;
		pDeleteItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(deletePos) ;
		if( !pDeleteItem ) continue ;

		m_pSelectRequitalGrid->DeleteIcon( pDeleteItem->wPos, &pDeleteItem->pIcon ) ;
		pDeleteItem->pIcon->SetActive(FALSE) ;
		WINDOWMGR->AddListDestroyWindow(pDeleteItem->pIcon) ;
		m_IndexGenerator.ReleaseIndex( pDeleteItem->pIcon->GetID() ) ;
		m_QuestSelectRequitalList.Remove(pDeleteItem) ;
		delete pDeleteItem ;
	}
	m_QuestSelectRequitalList.RemoveAll() ;

	RefreshRequitalItem() ;
	RefreshSelectRequitalItem() ;

	int nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// ���� ����Ʈ ���� �޴´�.

	if( nSubQuestCount < 1 )
	{
		/*sprintf(tempBuf, "[RequitalInfo] - (%d)�� ����Ʈ�� ���� ����Ʈ ������ ����, ���� ������ ó���� ���� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// ���� ó���� �Ѵ�.
	}

	CSubQuestInfo* pSubQuestInfo = pQuestInfo->GetSubQuestInfo(nSubQuestCount-1) ;					// ������ ���� ����Ʈ ������ �޴´�.

	if( !pSubQuestInfo )																// ���� ����Ʈ ������ ��ȿ�ϴٸ�,
	{
		/*sprintf(tempBuf, "[RequitalInfo] - (%d)�� ����Ʈ�� ���� ����Ʈ ������ ����, ���� ������ ó���� ���� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;																	// ���� ó���� �Ѵ�.
	}

	cPtrList& pTriggerList = pSubQuestInfo->GetTriggerList();
	int nTriggerCount = pTriggerList.GetCount();
	CQuestTrigger* pTrigger = 0;
	PTRLISTPOS pos = 0;
	PTRLISTPOS exePos = 0;

	for( int count = 0 ; count < nTriggerCount ; ++count )					// Ʈ���� ī��Ʈ �� ��ŭ for���� ������.
	{
		pos = pTriggerList.FindIndex(count);

		if( !pos ) continue ;												// ��ġ ������ ��ȿ�ϴٸ�,

		pTrigger = (CQuestTrigger*)pTriggerList.GetAt(pos);

		if( !pTrigger )	 continue ;										// Ʈ���� ������ ��ȿ�ϴٸ�,

		cPtrList* pExeList = NULL ;
		pExeList = pTrigger->GetExeList() ;								// Ʈ���� ���� ����Ʈ�� �޴´�.

		if( !pExeList )	continue ;										// Ʈ���� ���� ����Ʈ ������ ��ȿ�ϴٸ�,

		int nExeCount = pExeList->GetCount() ;					// ���� ����Ʈ ī��Ʈ�� �޴´�.

		CQuestExecute* pExecute = NULL ;						// ����Ʈ ���� ������ ���� �����͸� �����ϰ� nulló���� �Ѵ�.

		for(int count2 = 0 ; count2 < nExeCount ; ++count2 )	// ���� ī��Ʈ ��ŭ for���� ������.
		{
			exePos = NULL ;										// ���� ��ġ �����͸� null ó���� �Ѵ�.
			exePos = pExeList->FindIndex(count2) ;				// ī��Ʈ�� �ش��ϴ� ���� ��ġ ������ �޴´�.									

			if( !exePos ) continue ;										// ����Ʈ ���� ��ġ�� ��ȿ�ϸ�,

			pExecute = (CQuestExecute*)pExeList->GetAt(exePos) ;	// ����Ʈ ���� ������ �޴´�.

			if( !pExecute ) continue ;								// ����Ʈ ���� ������ ��ȿ�ϸ�,

			CQuestExecute_Item* pExecute_Item = ((CQuestExecute_Item*)pExecute) ;
			DWORD dwExeKind = pExecute->GetQuestExecuteKind() ;	// ���� Ÿ���� �޴´�.

			switch(dwExeKind)							// ���� Ÿ���� Ȯ���Ѵ�.
			{
			case eQuestExecute_TakeItem :				// ���� Ÿ���� �������� �޴� Ÿ���̶��,
				{
					ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(pExecute_Item->GetItemKind());

					if( pItemInfo )
					{
						AddRequitalItem(pItemInfo, pExecute_Item->GetItemNum(), BYTE(eQuestExecute_TakeItem)) ;
					}
				}
				break ;

			// 100414 ONS ���ú�������� ó�� �߰�.
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
				}
				break;

			case eQuestExecute_TakeMoney :				// ���� Ÿ���� ��带 �޴� Ÿ���̶��,
				{
					SetGold( pExecute_Item->GetItemNum() ) ;
				}
				break ;

			case eQuestExecute_TakeExp :				// ���� Ÿ���� ����ġ�� �޴� Ÿ���̶��,
				{
					SetExp( pExecute_Item->GetItemNum() ) ;
				}
				break ;

			}
		}
	}

	// 100414 ONS �����ܹ�ư�� ���� Ȱ��ȭ/��Ȱ��ȭó��
	if( m_QuestRequitalList.GetCount() > 0 )
	{
		m_pItemSignalOff->SetActive(FALSE);
		m_pItemSignalOn->SetActive(TRUE);
	}

	if( m_QuestSelectRequitalList.GetCount() > 0 )
	{
		m_pSelectItemSignalOff->SetActive(FALSE);
		m_pSelectItemSignalOn->SetActive(TRUE);
	}
}





void CQuestDialog::QuestDescView(CQuestString* pQuestString)						// ����Ʈ �� ������ ó�� �ϴ� �Լ�.
{
	if( !m_bActive ) return ;														// ����Ʈ â�� ���� ���°� �ƴ϶��, ���� ó���� �Ѵ�.

	if(!pQuestString)																// ���ڷ� �Ѿ�� ����Ʈ ��Ʈ�� ������ �������� ������ ����ó���Ѵ�.
	{
		m_pQuestDescList->RemoveAll() ;												// ���� ����Ʈ ���̾�α׸� ��� ����.
		{
			int nCellCount = m_pRequitalGrid->GetCellNum() ;							// ������ ���� �׸����� �� ī��Ʈ�� �޴´�.

			cIcon* pIcon ;																// ������ ������ ���� �����͸� �����Ѵ�.

			for(WORD count = 0 ; count < nCellCount ; ++count)
			{
				pIcon = m_pRequitalGrid->GetIconForIdx(count) ;							// ī��Ʈ�� �ش��ϴ� ������ ������ ������ �޴´�.

				if( pIcon )																// ������ ������ ��ȿ�ϸ�,
				{
					m_pRequitalGrid->DeleteIcon(pIcon) ;								// ���� ������ �׸��忡�� �������� �����Ѵ�.
				}
			}

			RequitalItem* item ;														// ���� �������� ������ ���� �����͸� �����Ѵ�.

			PTRLISTPOS pos = m_QuestRequitalList.GetHeadPosition() ;					// ����Ʈ ���� ������ ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

			while( pos )																// ��ġ ������ ��ȿ�� ���� while���� ������.
			{
				item = NULL ;															// ���� ������ ������ ���� �����͸� nulló�� �Ѵ�.
				item = ( RequitalItem* )m_QuestRequitalList.GetNext( pos ) ;			// ��ġ�� �ش��ϴ� ��������� ������ �ް�, ��ġ�� �������� �����Ѵ�.

				if( item )																// ������ ������ ��ȿ�ϸ�,
				{
					WINDOWMGR->AddListDestroyWindow( item->pIcon );						// ������ �Ŵ����� ������ ������� ���� �������� �������� ����Ѵ�.
					SAFE_DELETE( item ) ;												// ������ ������ �����ϰ� �����Ѵ�.
				}
			}

			m_QuestRequitalList.RemoveAll() ;											// ���� ������ ����Ʈ�� ��� ����.
		}
		// 100414 ONS ���ú������� ����
		{
			int nCellCount = m_pSelectRequitalGrid->GetCellNum();

			for( WORD count = 0 ; count < nCellCount ; ++count )
			{
				cIcon* pIcon = m_pSelectRequitalGrid->GetIconForIdx(count) ;			 
				if( pIcon )																
				{
					m_pSelectRequitalGrid->DeleteIcon(pIcon) ;							
				}
			}

			PTRLISTPOS pos = m_QuestSelectRequitalList.GetHeadPosition() ;				
			while( pos )																
			{
				RequitalItem* item = ( RequitalItem* )m_QuestSelectRequitalList.GetNext( pos ) ;			
				if( item )														
				{
					WINDOWMGR->AddListDestroyWindow( item->pIcon );				
					SAFE_DELETE( item ) ;										
				}
			}
			m_QuestSelectRequitalList.RemoveAll() ;							
		}

		SetGold(0) ;																// ���� ��� ��ġ�� 0���� �����Ѵ�.
		SetExp(0) ;																	// ���� ����ġ ��ġ�� 0���� �����Ѵ�.

		return;																		// return ó�� �Ѵ�.
	}

	m_pQuestDescList->RemoveAll() ;													// ���� ����Ʈ ���̾�α׸� ��� ����.

	SetQuestPrevInfo(pQuestString) ;

	AddRemandQuestExecuteString( pQuestString->GetQuestID() );

	RequestInfo(pQuestString) ;

	RequitalInfo(pQuestString) ;

	m_pQuestDescList->ResetGuageBarPos() ;
}

void CQuestDialog::AddRequitalItem(ITEM_INFO* pItemInfo, DWORD Count, BYTE bReauitalType)
{
	if( !pItemInfo ) return ;

	RequitalItem* pSQuest = new RequitalItem;										// ����Ʈ ������ �޸𸮸� �Ҵ��Ѵ�.
	memcpy(&pSQuest->itemInfo, pItemInfo, sizeof(ITEM_INFO)) ;						// ����Ʈ ������ ������ ���ڷ� �Ѿ�� ������ �����Ѵ�.
	pSQuest->nItemCount = (int)Count;												// ����Ʈ �������� ī��Ʈ�� �����Ѵ�.
	pSQuest->wPos = 0;																// ����Ʈ �������� ��ġ�� -1�� �����Ѵ�.

	cIcon* pIcon = new cIcon;														// ������ �޸𸮸� �Ҵ��Ѵ�.
	cImage Image;																	// �̹����� �����Ѵ�.

	SCRIPTMGR->GetImage(pSQuest->itemInfo.Image2DNum, &Image, PFT_ITEMPATH);		// ����Ʈ ������ ������ �̹����� �̹����� �����Ѵ�.
	pIcon->SetBasicImage(&Image);													// �������� �⺻ �̹����� ������ ������ �̹����� �����Ѵ�.
	pIcon->Init(0, 0, 40, 40, &Image, m_IndexGenerator.GenerateIndex() );			// �������� �ʱ�ȭ �Ѵ�.
	pIcon->SetMovable(FALSE);														// �̵� ���θ� FALSE�� �����Ѵ�.
	pIcon->SetActive(FALSE);														// Ȱ��ȭ ���θ� FALSE�� �����Ѵ�.

	pSQuest->pIcon = pIcon ;

	// 100414 ONS �Ϲݺ���� ���ú����� �����ؼ� ����Ʈ�� �����Ѵ�.
	if( bReauitalType == BYTE(eQuestExecute_TakeSelectItem) )
		m_QuestSelectRequitalList.AddTail(pSQuest);
	else
		m_QuestRequitalList.AddTail(pSQuest);
	
	WINDOWMGR->AddWindow(pIcon);													// ������ �Ŵ����� ������ ������ �߰��Ѵ�.

	RefreshRequitalItem() ;
	RefreshSelectRequitalItem();
}

void CQuestDialog::GiveupQuestDelete(DWORD QuestIdx)								// ����Ʈ ���� ���� �Լ�.
{
	PTRLISTSEARCHSTART(m_QuestTree, QuestTree*, pQTree);							// ����Ʈ Ʈ�� ����Ʈ�� �˻��Ѵ�.
	{
		if( pQTree )																// ����Ʈ Ʈ�� ������ ��ȿ���� üũ�Ѵ�.
		{
			if(pQTree->QuestIdx == QuestIdx)										// ����Ʈ Ʈ�� �ε����� ������ �ε����� ������,
			{
				pQTree->list.RemoveAll();											// Ʈ�� ������ ����Ʈ�� ��� ����.
				pQTree->State = eQTree_Close;										// Ʈ�� ������ ���¸� �����ִ� ���·� �����Ѵ�.
				pQTree->Count = 0;													// Ʈ�� ������ ī��Ʈ�� 0���� �����Ѵ�.
				pQTree->QuestIdx = 0;												// Ʈ�� ������ ����Ʈ �ε������� 0���� �����Ѵ�.
				m_QuestTree.Remove(pQTree);											// ����Ʈ Ʈ�� ����Ʈ���� Ʈ�� ������ ����.
				delete pQTree;														// Ʈ�� ������ �����Ѵ�.
				break;																// �˻��� �������´�.
			}
		}
	}
	PTRLISTSEARCHEND																// �˻��� �����Ѵ�.

	DWORD Idx = 0;																	// �ε��� ������ �����ϰ� 0���� �����Ѵ�.
	cIcon* pIcon = NULL;															// ������ �����͸� �����ϰ� nulló���� �Ѵ�.

	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// ����Ʈ ������ ����Ʈ�� �˻��Ѵ�.
		if(!pSQuest)			break;												// ����Ʈ ������ ������ ��ȿ���� ������ �˻��� ����������.

		Idx = pSQuest->pQuestItemInfo->QuestKey/100;								// ����Ʈ ������ �ε����� �޴´�.

		if(Idx == QuestIdx)															// �ε����� �����Ϸ��� ����Ʈ �ε����� ������,
		{
			m_pIconGridDlg->DeleteIcon(pSQuest->Pos, &pIcon);						// ����Ʈ ������ �׸��忡�� �������� �����Ѵ�.

			if(!pIcon)			break;												// ������ ������ ��ȿ���� ������ �˻��� ����������.

			WINDOWMGR->AddListDestroyWindow(pIcon);									// ������ �Ŵ����� �ı��� ������� �������� �߰��Ѵ�.
			m_IndexGenerator.ReleaseIndex(pIcon->GetID());							// �ε��� �����⿡�� �������� ���� �Ѵ�.
			m_QuestItemList.Remove(pSQuest);										// ����Ʈ ������ ����Ʈ����, �������� �����Ѵ�.
		}	
	PTRLISTSEARCHEND																// �˻��� �����Ѵ�.

	RefreshQuestItem(m_QuestItemPage);												// ����Ʈ ������ �׸��带 ���ΰ�ħ�Ѵ�.
}

void CQuestDialog::QuestItemAdd(QUEST_ITEM_INFO* pQuestItemInfo, DWORD Count)		// ����Ʈ �������� �߰��ϴ� �Լ�.
{
	if(!pQuestItemInfo)			return;												// ����Ʈ ������ ������ ��ȿ���� ������ ���� ó���� �Ѵ�.

	PTRLISTSEARCHSTART(m_QuestItemList,sQuestItem*,pQuestItem)						// ����Ʈ ������ ����Ʈ�� �˻��Ѵ�.
		if( pQuestItem->pQuestItemInfo->ItemIdx == pQuestItemInfo->ItemIdx )		// ����Ʈ ������ ������ ������ �ε�����, ���ڷ� �Ѿ�� ������ �ε����� ������,
		{
			pQuestItem->Count = Count ;												// ����Ʈ ������ ����Ʈ�� ������ ī��Ʈ�� �����Ѵ�.
			return ;																// ���� ó���� �Ѵ�.
		}
	PTRLISTSEARCHEND																// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.

	sQuestItem* pSQuest = new sQuestItem;											// ����Ʈ ������ �޸𸮸� �Ҵ��Ѵ�.
	pSQuest->pQuestItemInfo = pQuestItemInfo;										// ����Ʈ ������ ������ ���ڷ� �Ѿ�� ������ �����Ѵ�.
	pSQuest->Count = Count;															// ����Ʈ �������� ī��Ʈ�� �����Ѵ�.
	pSQuest->Pos = WORD(-1);

	cIcon* pIcon = new cIcon;														// ������ �޸𸮸� �Ҵ��Ѵ�.
	cImage Image;																	// �̹����� �����Ѵ�.

	SCRIPTMGR->GetImage(pSQuest->pQuestItemInfo->Image2DNum, &Image, PFT_ITEMPATH);	// ����Ʈ ������ ������ �̹����� �̹����� �����Ѵ�.
	pIcon->SetBasicImage(&Image);													// �������� �⺻ �̹����� ������ ������ �̹����� �����Ѵ�.
	pIcon->Init(0, 0, 40, 40, &Image, m_IndexGenerator.GenerateIndex() );			// �������� �ʱ�ȭ �Ѵ�.
	pIcon->SetMovable(FALSE);														// �̵� ���θ� FALSE�� �����Ѵ�.
	pIcon->SetActive(FALSE);														// Ȱ��ȭ ���θ� FALSE�� �����Ѵ�.

	pSQuest->pIcon = pIcon;															// ����Ʈ ������ ������ �������� ������ ������ ���������� �����Ѵ�.

	m_QuestItemList.AddTail(pSQuest);												// ����Ʈ ������ ������ ����Ʈ ������ ����Ʈ�� �߰��Ѵ�.

	WINDOWMGR->AddWindow(pIcon);													// ������ �Ŵ����� ������ ������ �߰��Ѵ�.
	RefreshQuestItem(m_QuestItemPage);												// ����Ʈ ������ �������� ���ΰ�ħ�Ѵ�.
}

void CQuestDialog::QuestItemDelete(DWORD ItemIdx)									// ����Ʈ �������� �����ϴ� �Լ�.
{
	sQuestItem* pItem;

	PTRLISTPOS pos = NULL ;
	pos = m_QuestItemList.GetHeadPosition() ;

	while(pos)
	{
		pItem = NULL ;
		pItem = (sQuestItem*)m_QuestItemList.GetNext(pos) ;

		if( pItem )
		{
			if( pItem->pQuestItemInfo->ItemIdx == ItemIdx )
			{
				m_pIconGridDlg->DeleteIcon(pItem->Pos, &pItem->pIcon) ;

				pItem->pIcon->SetActive(FALSE);									
				WINDOWMGR->AddListDestroyWindow(pItem->pIcon);					
				m_IndexGenerator.ReleaseIndex(pItem->pIcon->GetID());			

				m_QuestItemList.Remove(pItem) ;

				delete pItem ;
			}
		}
	}

	RefreshQuestItem(m_QuestItemPage);
}

DWORD CQuestDialog::QuestItemUpdate(DWORD type, DWORD ItemIdx, DWORD data)			// ����Ʈ �������� ������Ʈ �Ѵ�.
{
	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// ����Ʈ ������ ����Ʈ���� ������ ������ �˻��Ѵ�.

		if(!pSQuest)			break;												// ����Ʈ ������ ������ ��ȿ���� ������, �˻��� ����������.

		if(pSQuest->pQuestItemInfo->ItemIdx == ItemIdx)								// ����Ʈ ������ ����Ʈ ���� ������ �ε�����, ���ڷ� ���� ������ �ε����� ������,
		{
			switch(type)															// ������Ʈ Ÿ���� Ȯ���Ѵ�.
			{
			case eQItem_AddCount:													// ����Ʈ ������ ī��Ʈ�� �߰��ϴ� Ÿ���̸�,
				if(pSQuest)		pSQuest->Count += data;								// ����Ʈ ������ ������ ��ȿ�ϸ�, ����Ʈ ������ ������ ī��Ʈ�� �����͸� ���Ѵ�.
				break;
			case eQItem_SetCount:													// ����Ʈ ������ ī��Ʈ�� �����ϴ� Ÿ���̸�,
				if(pSQuest)		pSQuest->Count = data;								// ����Ʈ ������ ������ ��ȿ�ϸ�, ����Ʈ ������ ������ ī��Ʈ�� ������ ������ �����Ѵ�.
				break;
			case eQItem_GetCount:													// ����Ʈ ������ ī��Ʈ�� ��ȯ�ϴ� Ÿ���̸�,
				if(pSQuest)		return pSQuest->Count;								// ����Ʈ ������ ������ ��ȿ�ϸ�, ����Ʈ ������ ������ ī��Ʈ�� ��ȯ�Ѵ�.
				break;
			}
			break;																	// �˻��� ����������.
		}
	PTRLISTSEARCHEND																// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.
	
	return 0;																		// 0�� �����Ѵ�.
}

void CQuestDialog::RefreshRequitalItem()
{
	cIcon* pIcon = 0;
	int Count = 0;
	WORD Pos = 0;
	int	nStart = 0;

	for(WORD i=0; i<m_pRequitalGrid->GetCellNum(); ++i)
	{
		m_pRequitalGrid->DeleteIcon(i, &pIcon);										// ������ �׸��忡�� �������� �����Ѵ�.
	}														// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.

	RequitalItem* pItem = 0;
	PTRLISTPOS pos = m_QuestRequitalList.GetHeadPosition();

	while(pos)
	{
		pItem = (RequitalItem*)m_QuestRequitalList.GetNext(pos) ;

		if( pItem )
		{
			if( Count >= nStart && Count < 6 )
			{
				pItem->wPos = Pos ;
				m_pRequitalGrid->AddIcon( pItem->wPos, pItem->pIcon ) ;

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

void CQuestDialog::RefreshQuestItem(DWORD Page)										// ����Ʈ ������ �������� ���ΰ�ħ�ϴ� �Լ�.
{	
	cIcon* pIcon = 0;
	int Count = 0;
	WORD Pos = 0;
	int	nStart = Page*VIEW_QUESTITEM_PERPAGE;

	for(WORD i=0; i<MAX_QUEST_PAGEBTN; ++i)
	{
		m_pPageBtn[i]->SetPush(FALSE);												// ������ ��ư�� ������ ���� ���·� �����Ѵ�.
	}
	m_pPageBtn[Page]->SetPush(TRUE);												// ���ڷ� �Ѿ�� ������ ��ư�� ���� ���·� �����Ѵ�.

	for(WORD i=0; i<m_pIconGridDlg->GetCellNum(); ++i)
	{
		m_pIconGridDlg->DeleteIcon(i, &pIcon);										// ������ �׸��忡�� �������� �����Ѵ�.
	}
	
	
	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// ����Ʈ ������ ����Ʈ���� ����Ʈ ������ ������ �˻��Ѵ�.

		if(!pSQuest)			break;												// ����Ʈ ������ ������ ��ȿ���� ������, �˻��� �������´�.

		if(Count>=nStart && Count < nStart+VIEW_QUESTITEM_PERPAGE)					// ���� �μ��� ī��Ʈ ������ �۰�, ������ �� ���� �μ��� ī��Ʈ ���� ������,
		{
			// Add
			pSQuest->Pos = Pos;														// ����Ʈ ������ ������ ��ġ ������ �����Ѵ�.
			m_pIconGridDlg->AddIcon(pSQuest->Pos, pSQuest->pIcon);					// ����Ʈ ������ ������ �׸��忡, ������ ������ �����Ѵ�.
			pSQuest->pIcon->SetActive(TRUE);										// ����Ʈ ������ �������� Ȱ��ȭ �Ѵ�.
			++Pos;																	// ��ġ ������ �����Ѵ�.
		}
		else																		// ���� �μ� ������ ���� ������,
		{
			pSQuest->Pos = WORD(-1);
			pSQuest->pIcon->SetActive(FALSE);										// ����Ʈ �������� �������� ��Ȱ��ȭ ��Ų��.
		}

		++Count;																	// ī��Ʈ ���� ������Ų��.

	PTRLISTSEARCHEND																// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.

	m_QuestItemPage = Page;															// ����Ʈ ������ �������� �����Ѵ�.
}

void CQuestDialog::ShowRequitalItemToolTip(cPtrList& RequitalList, DWORD Index)
{
	char buf[128] = {0};
	DWORD dwCount = 0;

	PTRLISTSEARCHSTART(RequitalList, RequitalItem*, pSQuest);				// ����Ʈ ������ ����Ʈ����, ����Ʈ ������ ������ �˻��Ѵ�.

		if(!pSQuest)			break;												// ����Ʈ ������ ������ ��ȿ���� ������, �˻��� ����������.

		if(int(dwCount) == m_RequitalItemTooltipIdx)										// ���� �ε����� ī��Ʈ ���� ������,
		{
			pSQuest->pIcon->SetToolTip("");											// ����Ʈ ������ ������ ������ �������� �����Ѵ�.
		}

		if(dwCount == Index)														// ī��Ʈ ����, ���ڷ� �Ѿ�� �ε����� ������,
		{
			cImage Image;															// �̹����� �����Ѵ�.
			SCRIPTMGR->GetImage(0, &Image, PFT_HARDPATH);							// �̹����� �����Ѵ�.
			pSQuest->pIcon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &Image, TTCLR_ITEM_CANEQUIP );	// ������ ������ �������� �����Ѵ�.

			wsprintf(buf, "[%s]", pSQuest->itemInfo.ItemName);				// �ӽ� ���ۿ� ������ �̸��� �����Ѵ�.
			pSQuest->pIcon->AddToolTipLine(buf, ITEMMGR->GetItemNameColor( pSQuest->itemInfo ));						// ����Ʈ �������� ���� ���ο� �ӽ� ���۸� �߰��Ѵ�.
			
			ITEMMGR->AddToolTip( pSQuest->pIcon, pSQuest->itemInfo.ItemTooltipIdx );
		}

		++dwCount;																	// ī��Ʈ ���� �����Ѵ�.

	PTRLISTSEARCHEND																// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.
		
	m_RequitalItemTooltipIdx = Index;												// ���� �ε����� ���ڷ� �Ѿ�� �ε����� �����Ѵ�.
}

void CQuestDialog::ShowItemToolTip(DWORD Index)										// ������ ������ �����ִ� �Լ�.
{
	char buf[128] = {0};
	DWORD dwCount = 0;

	PTRLISTSEARCHSTART(m_QuestItemList, sQuestItem*, pSQuest);						// ����Ʈ ������ ����Ʈ����, ����Ʈ ������ ������ �˻��Ѵ�.

		if(!pSQuest)			break;												// ����Ʈ ������ ������ ��ȿ���� ������, �˻��� ����������.

		if(int(dwCount) == m_TooltipIdx)													// ���� �ε����� ī��Ʈ ���� ������,
		{
			pSQuest->pIcon->SetToolTip("");											// ����Ʈ ������ ������ ������ �������� �����Ѵ�.
		}

		if(dwCount == Index)														// ī��Ʈ ����, ���ڷ� �Ѿ�� �ε����� ������,
		{
			cImage Image;															// �̹����� �����Ѵ�.
			SCRIPTMGR->GetImage(0, &Image, PFT_HARDPATH);							// �̹����� �����Ѵ�.
			pSQuest->pIcon->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &Image, TTCLR_ITEM_CANEQUIP );	// ������ ������ �������� �����Ѵ�.

			wsprintf(buf, "[%s]", pSQuest->pQuestItemInfo->ItemName);				// �ӽ� ���ۿ� ������ �̸��� �����Ѵ�.
			pSQuest->pIcon->AddToolTipLine(buf, TTTC_DEFAULT);						// ����Ʈ �������� ���� ���ο� �ӽ� ���۸� �߰��Ѵ�.
			pSQuest->pIcon->AddToolTipLine("");										// ����Ʈ �������� ���� ���ο� ������ �߰��Ѵ�.
			AddTooltipInfo(pSQuest->pQuestItemInfo->ItemDesc, pSQuest->pIcon);		// ����Ʈ ������ ������ ������ �������� ������ �߰��Ѵ�.
		}

		++dwCount;																	// ī��Ʈ ���� �����Ѵ�.

	PTRLISTSEARCHEND																// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.
		
	m_TooltipIdx = Index;															// ���� �ε����� ���ڷ� �Ѿ�� �ε����� �����Ѵ�.
}

void CQuestDialog::AddTooltipInfo(char* pString, cIcon* pIcon)						// ���� ������ �߰��ϴ� �Լ�.
{
	if(strlen(pString) > MAX_ITEMDESC_LINE)											// �ִ� ������ ���� ���κ��� ��Ʈ�� ���̰� ũ��,
	{
		char* pStr = pString;														// ���ڿ� �����͸� �����ϰ� ��Ʈ�� ������ �޴´�.
		char tb[MAX_ITEMDESC_LINE+2] = { 0,  };										// �ӽ� ���۸� �����Ѵ�.
		int len = 0;																// ���̸� 0���� �����Ѵ�.
		
		while(*pStr)																// ���ڿ� ������ ��ȿ�� ���� while���� ������.
		{						
			tb[len]=*pStr;															// �ӽ� ������ ���̿� ���ڸ� ��´�.

			if( pStr + 1 != CharNext( CharPrev( pStr, pStr + 1 ) ) )				// ���� ������ ���� ���ڸ� üũ�Ͽ� 2����Ʈ ���ڸ�, 
				tb[++len] = *(++pStr);												// �� ����Ʈ�� �� ��´�.

			++len;																	// ���� ���� �����Ѵ�.
			++pStr;																	// ���� �����͸� �����Ѵ�.
			
			// �ϴ��� �׳� ����.. ��.��;;
			if(tb[0] == ' ')		{	len = 0;	}								// �ӽ� ������ ù ����Ʈ�� ��ĭ�̸�, ���̸� 0���� �����Ѵ�.
			
			if(strlen(tb) >= MAX_ITEMDESC_LINE)										// �ӽ� ���� ���̰� �ִ� ������ ���� ���κ��� ũ��,
			{
				pIcon->AddToolTipLine(tb, TTTC_DESCRIPTION);						// �ӽ� ���۸� ���� ���ο� �߰��Ѵ�.
				memset(tb, 0, MAX_ITEMDESC_LINE+2);									// �ӽ� ���۸� �޸� ���Ѵ�.
				len = 0;															// ���̸� 0���� �����Ѵ�.
			}
		}

		if(len)																		// ���� ������ ������,
		{
			pIcon->AddToolTipLine(tb, TTTC_DESCRIPTION);							// �����ܿ� ������ �߰��Ѵ�.
			memset(tb, 0, MAX_ITEMDESC_LINE+2);										// �ӽ� ���۸� �޸� ���Ѵ�.
			len = 0;																// ���� ���� 0���� �����Ѵ�.
		}
	}
	else																			// �ִ� ������ ���� ���κ��� ��Ʈ�� ���̰� ������,
		pIcon->AddToolTipLine(pString, TTTC_DESCRIPTION);							// �����ܿ� ������ �߰��Ѵ�.
}

void CQuestDialog::AddQuestTree(int nMapNum)
{
	QuestTree* pData = new QuestTree;									// Ʈ���� �����Ѵ�.

	CQuestString* pQuestString = new CQuestString;						// Ʈ���� ���� ����Ʈ ��Ʈ���� �����Ѵ�.

	int nTitleLine = 0;
	pQuestString->AddLine(
		GetMapName(MAPTYPE(nMapNum)),
		nTitleLine,
		TRUE);
	pQuestString->SetIndex(nMapNum, 0);									// Ʈ�� �ε����� �� ��ȣ�� 0���� �����Ѵ�.
	pQuestString->SetTreeTitle(TRUE) ;									// Ʈ���� �������� �����Ѵ�.
	pData->list.AddHead(pQuestString);									// ���� ������ Ʈ���� �� Ÿ��Ʋ�� �߰��Ѵ�.
	++pData->Count;														// Ʈ���� ��Ʈ�� ī��Ʈ�� �����Ѵ�.
	pData->State = eQTree_Open ;										// Ʈ���� ���¸� ���� ���·� �����Ѵ�.
	pData->nMapNum = nMapNum ;											// Ʈ���� �� ��ȣ�� �����Ѵ�.

	m_QuestTree.AddTail(pData) ;
}

void CQuestDialog::AddQuestTitle(CQuestString* pQuestString, int nMapNum)
{
	if( !pQuestString )
	{
		return ;
	}

	QuestTree* pTree ;

	PTRLISTPOS pos = NULL ;
	pos = m_QuestTree.GetHeadPosition() ;

	while(pos)
	{
		pTree = NULL ;
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;

		if( pTree )
		{
			if(pTree->nMapNum == nMapNum)
			{
				CQuestString* pNewQuestString = new CQuestString ;

				DWORD dwMainQuestIdx = pQuestString->GetQuestID() ;
				DWORD dwSubQuestIdx  = pQuestString->GetSubID() ;

				pNewQuestString->SetQuestID( dwMainQuestIdx ) ;
				pNewQuestString->SetSubID( dwSubQuestIdx ) ;
				pNewQuestString->SetIndex( dwMainQuestIdx, dwSubQuestIdx ) ;

				pNewQuestString->SetComplete( pQuestString->IsComplete() ) ;

				pNewQuestString->SetCount( pQuestString->GetCount() ) ;
				pNewQuestString->SetTotalCount( pQuestString->GetTotalCount() ) ;


				ITEM* pItem = NULL ;

				cPtrList* titleList = NULL ;
				titleList = pQuestString->GetTitle() ;

				if( titleList )
				{
					PTRLISTPOS titlePos = NULL ;
					titlePos = titleList->GetHeadPosition() ;

					while(titlePos)
					{
						pItem = NULL ;
						pItem = (ITEM*)titleList->GetNext(titlePos) ;

						if( pItem )
						{
							ITEM* pNewItem = new ITEM ;

							//memcpy(pNewItem, pItem, sizeof(ITEM)) ;

							memcpy(pNewItem->string, pItem->string, sizeof(pItem->string)) ;
							pNewItem->line		= pItem->line ;
							pNewItem->nAlpha	= pItem->nAlpha ;
							pNewItem->nFontIdx	= pItem->nFontIdx ;
							pNewItem->rgb		= pItem->rgb ;

							pNewQuestString->GetTitle()->AddTail(pNewItem) ;
						}
					}
				}

				cPtrList* descList = NULL ;
				descList = pQuestString->GetDesc() ;

				if( descList )
				{
					PTRLISTPOS descPos = NULL ;
					descPos = descList->GetHeadPosition() ;

					while(descPos)
					{
						pItem = NULL ;
						pItem = (ITEM*)descList->GetNext(descPos) ;

						if( pItem )
						{
							ITEM* pNewItem = new ITEM ;

							//memcpy(pNewItem, pItem, sizeof(ITEM)) ;

							memcpy(pNewItem->string, pItem->string, sizeof(pItem->string)) ;
							pNewItem->line		= pItem->line ;
							pNewItem->nAlpha	= pItem->nAlpha ;
							pNewItem->nFontIdx	= pItem->nFontIdx ;
							pNewItem->rgb		= pItem->rgb ;

							pNewQuestString->GetDesc()->AddTail(pNewItem) ;
						}
					}
				}

				pNewQuestString->SetTreeTitle( FALSE ) ;

				pTree->list.AddTail(pNewQuestString) ;
				++pTree->Count;
			}
		}
	}
}






void CQuestDialog::StartQuest(DWORD dwQuestIdx)
{
	CQuestInfo* pQuestInfo =NULL ;														// ����Ʈ ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;									// ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if(!pQuestInfo)																		// ����Ʈ ������ ��ȿ���� ������,
	{
		/*char tempBuf[128] = {0, } ;
		sprintf(tempBuf, "[CheckMapTree] - (%d)�� ����Ʈ ������ ���� ���ؼ� ����Ʈ�� �߰����� ���߽��ϴ�.", dwQuestIdx) ;
		CHATMGR->AddMsg(CTC_QUEST, tempBuf) ;*/
		return ;
	}

	int nMapNum = pQuestInfo->GetMapNum() ;												// ����Ʈ ������ �ִ� �� ��ȣ�� �޴´�.

	QuestTree* pTree ;																	// ����Ʈ Ʈ�� ������ ���� �����͸� �����Ѵ�.
	
	PTRLISTPOS pos = NULL ;																// Ʈ���� ��ġ�� ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pos = m_QuestTree.GetHeadPosition() ;												// ����Ʈ Ʈ�� ����Ʈ�� ���� �����ϰ� ��ġ�� �޴´�.

	while(pos)																			// Ʈ�� ��ġ ������ ��ȿ�� ���� whlie���� ������.
	{
		pTree = NULL ;																	// Ʈ�� ������ ���� �����͸� null ó���� �Ѵ�.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// ���� ��ġ�� Ʈ�� ������ �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pTree ) continue ;															// Ʈ�� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

		if( pTree->nMapNum != nMapNum ) continue ;										// ���� Ʈ���� �� ��ȣ��, �񱳴���� �� ��ȣ�� �ٸ���, continue ó���� �Ѵ�.

		BOOL b_SameQuestIdx = FALSE ;													// Ʈ�� ������ ���� �ε����� �ִ��� ������� ���� ������ �����ϰ� false�� �����Ѵ�.

		DWORD* pQuestIdx ;																// ����Ʈ �ε��� ��ȣ�� ���� �����͸� �����Ѵ�.

		PTRLISTPOS idPos = NULL ;														// ����Ʈ �ε��� ��ġ�� ���� �����͸� �����ϰ� null ó���� �Ѵ�.
		idPos = pTree->list.GetHeadPosition() ;											// ���� Ʈ���� ����Ʈ�� ���� �����ϰ�, �ε��� ��ġ�� �޴´�.

		while(idPos)																	// �ε��� ��ġ ������ ��ȿ�� ���� while���� ������.
		{
			pQuestIdx = NULL ;															// ����Ʈ �ε����� ���� �����͸� null ó���� �Ѵ�.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// ���� ��ġ�� ����Ʈ �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

			if( !pQuestIdx ) continue ;													// ����Ʈ �ε��� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

			if( *pQuestIdx == dwQuestIdx )												// ����Ʈ �ε����� �� ����� ����Ʈ �ε����� ������,
			{
				b_SameQuestIdx = TRUE ;													// ��� ���� true�� �����Ѵ�.
				break ;																	// ���� while���� Ż���Ѵ�.
			}
		}

		if( b_SameQuestIdx )															// ��� ���� true �̸�,
		{
			return ;
		}
		else																			// ���� ����Ʈ�� ���ٸ�,
		{
			DWORD* pNewQuest = new DWORD ;												// ����Ʈ �ε����� ���� ������ �����Ѵ�.
			*pNewQuest = dwQuestIdx ;													// ����Ʈ �ε��� ���� �����Ѵ�.

			pTree->list.AddTail(pNewQuest) ;											// ���� �� Ʈ���� ����Ʈ �ε����� �߰��Ѵ�.

			++m_nQuestCount ;
			return ;
		}
	}

	AddMapTree(nMapNum, dwQuestIdx) ;													// ���ο� �� Ʈ���� �߰��ϰ�, ����Ʈ�� �߰��Ѵ�.

	//SetSelectedQuestIdx(dwQuestIdx); // 080304 NYJ --- ����Ʈ�ڵ��˸��� ó���� ���� �߰� (���� ������ ����)
	RefreshQuestList() ;																// ����Ʈ ����Ʈ�� ���ΰ�ħ�Ѵ�.
}





void CQuestDialog::EndQuest(DWORD dwQuestIdx)
{
	CQuestInfo* pQuestInfo =NULL ;														// ����Ʈ ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pQuestInfo = QUESTMGR->GetQuestInfo(dwQuestIdx) ;									// ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if(!pQuestInfo)																		// ����Ʈ ������ ��ȿ���� ������,
	{
		return ;
	}

	int nMapNum = pQuestInfo->GetMapNum() ;												// ����Ʈ ������ �ִ� �� ��ȣ�� �޴´�.

	QuestTree* pTree ;																	// ����Ʈ Ʈ�� ������ ���� �����͸� �����Ѵ�.
	
	PTRLISTPOS pos = NULL ;																// Ʈ���� ��ġ�� ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pos = m_QuestTree.GetHeadPosition() ;												// ����Ʈ Ʈ�� ����Ʈ�� ���� �����ϰ� ��ġ�� �޴´�.

	while(pos)																			// Ʈ�� ��ġ ������ ��ȿ�� ���� whlie���� ������.
	{
		pTree = NULL ;																	// Ʈ�� ������ ���� �����͸� null ó���� �Ѵ�.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// ���� ��ġ�� Ʈ�� ������ �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pTree ) continue ;															// Ʈ�� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

		if( pTree->nMapNum != nMapNum ) continue ;										// ���� Ʈ���� �� ��ȣ��, �񱳴���� �� ��ȣ�� �ٸ���, continue ó���� �Ѵ�.

		DWORD* pQuestIdx ;																// ����Ʈ �ε��� ��ȣ�� ���� �����͸� �����Ѵ�.

		PTRLISTPOS idPos = NULL ;														// ����Ʈ �ε��� ��ġ�� ���� �����͸� �����ϰ� null ó���� �Ѵ�.
		idPos = pTree->list.GetHeadPosition() ;											// ���� Ʈ���� ����Ʈ�� ���� �����ϰ�, �ε��� ��ġ�� �޴´�.

		while(idPos)																	// �ε��� ��ġ ������ ��ȿ�� ���� while���� ������.
		{
			pQuestIdx = NULL ;															// ����Ʈ �ε����� ���� �����͸� null ó���� �Ѵ�.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// ���� ��ġ�� ����Ʈ �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

			if( !pQuestIdx ) continue ;													// ����Ʈ �ε��� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

			if( *pQuestIdx == dwQuestIdx )												// ����Ʈ �ε����� �� ����� ����Ʈ �ε����� ������,
			{
				pTree->list.Remove(pQuestIdx) ;
				delete pQuestIdx ;
			}
		}

		if( pTree->list.GetCount() < 1 ) DeleteMapTree(nMapNum) ;
	}

	RefreshQuestList() ;																// ����Ʈ ����Ʈ�� ���ΰ�ħ�Ѵ�.

	// 081112 LYW --- QuestDialog : ����Ʈ ��Ͽ��� �ٸ� ����Ʈ�� ������ ��µǴ� ����.
	// ����Ʈ ����� �Ϸ� �� ���� ó���� �߻��Ǵ� ����Ʈ ������ ��Ŀ����
	// ���� �������� �����ǰ� �ִ� ���� ���� -> �Ϸ� �� ���� ó���� ����Ʈ ����
	// ��Ŀ���� Blank ���·� ó�� ���.(�賲��)
	QuestDescView(NULL) ;

	m_SelectedQuestID = GetQuestIdx(1) ;
	SetSelectedQuestIdx(m_SelectedQuestID) ;

	// 100414 ONS ������̾�α� �ʱ�ȭ
	InitRequitalDlg();
}





void CQuestDialog::AddMapTree(int nMapNum, DWORD dwQuestIdx) 
{
	QuestTree* pNewQuestTree = new QuestTree ;
	pNewQuestTree->nMapNum = nMapNum ;
	pNewQuestTree->State = eQTree_Open ;

	DWORD* pNewQuest = new DWORD ;
	*pNewQuest = dwQuestIdx ;

	pNewQuestTree->list.AddTail(pNewQuest) ;

	m_QuestTree.AddTail(pNewQuestTree) ;
	++m_nQuestCount ;
}





void CQuestDialog::DeleteMapTree(int nMapNum)
{
	QuestTree* pTree ;																	// ����Ʈ Ʈ�� ������ ���� �����͸� �����Ѵ�.
	
	PTRLISTPOS pos = NULL ;																// Ʈ���� ��ġ�� ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pos = m_QuestTree.GetHeadPosition() ;												// ����Ʈ Ʈ�� ����Ʈ�� ���� �����ϰ� ��ġ�� �޴´�.

	while(pos)																			// Ʈ�� ��ġ ������ ��ȿ�� ���� whlie���� ������.
	{
		pTree = NULL ;																	// Ʈ�� ������ ���� �����͸� null ó���� �Ѵ�.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// ���� ��ġ�� Ʈ�� ������ �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pTree ) continue ;															// Ʈ�� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

		if( pTree->nMapNum != nMapNum ) continue ;										// ���� Ʈ���� �� ��ȣ��, �񱳴���� �� ��ȣ�� �ٸ���, continue ó���� �Ѵ�.

		DWORD* pQuestIdx ;																// ����Ʈ �ε��� ��ȣ�� ���� �����͸� �����Ѵ�.

		PTRLISTPOS idPos = NULL ;														// ����Ʈ �ε��� ��ġ�� ���� �����͸� �����ϰ� null ó���� �Ѵ�.
		idPos = pTree->list.GetHeadPosition() ;											// ���� Ʈ���� ����Ʈ�� ���� �����ϰ�, �ε��� ��ġ�� �޴´�.

		while(idPos)																	// �ε��� ��ġ ������ ��ȿ�� ���� while���� ������.
		{
			pQuestIdx = NULL ;															// ����Ʈ �ε����� ���� �����͸� null ó���� �Ѵ�.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// ���� ��ġ�� ����Ʈ �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

			if( !pQuestIdx ) continue ;													// ����Ʈ �ε��� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

			pTree->list.Remove(pQuestIdx) ;

			delete pQuestIdx ;
		}

		pTree->list.RemoveAll() ;

		m_QuestTree.Remove(pTree) ;

		delete pTree ;
	}
}






void CQuestDialog::RefreshQuestList()
{
	if( !IsActive() )
		return;

	char buf[512] = { 0, };																// �ӽ� ���۸� �����Ѵ�.	

	int BackupLine = m_pQuestListDlg->GetTopListItemIdx();							// ��� ������ �����Ѵ�.

	CQuestQuickViewDialog* pQuickView = NULL ;
	pQuickView = GAMEIN->GetQuestQuickViewDialog() ;

	if( !pQuickView )
	{
		/*sprintf(buf, "����Ʈ �˸�â ������ �޾ƿ��� ���� ����Ʈ ����Ʈ�� ���ΰ�ħ ���� ���߽��ϴ�.") ;
		CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
		return ;
	}

	DWORD* pCheckIdx ;
	PTRLISTPOS checkPos ;																// ����Ʈ �˸�â�� ��ġ ������ ���� �����͸� �����Ѵ�.

	cPtrList* pCheckList = NULL ;
	pCheckList = pQuickView->GetViewList() ;

	if( !pCheckList )
	{
		/*sprintf(buf, "����Ʈ �˸�â�� ����Ʈ ������ �޾ƿ��� ���� ����Ʈ ����Ʈ�� ���ΰ�ħ ���� ���߽��ϴ�.") ;
		CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
		return ;
	}

	m_pQuestListDlg->RemoveAll();														// ����Ʈ ����Ʈ ���̾�α׸� ��� ����.

	m_QListCount = 0 ;
	memset(m_dwQuestList, 0, sizeof(CQuestString*)*MAX_PROCESSQUEST);					// ����Ʈ ����Ʈ�� �޸� �� �Ѵ�.
	
	DWORD Color = 0;																	// ���� ���� ���� ������ �����ϰ� 0���� �����Ѵ�.

	QuestTree* pTree ;																	// ����Ʈ Ʈ�� ������ ���� �����͸� �����Ѵ�.
	PTRLISTPOS pos ;																	// Ʈ���� ��ġ�� ���� �����͸� �����Ѵ�.

	char* pMapName ;																	// �� �̸��� ���� �����͸� �����Ѵ�.

	DWORD* pQuestIdx ;																	// ����Ʈ �ε����� ���� �����͸� �����Ѵ�.
	CQuestInfo* pQuestInfo ;															// ����Ʈ ������ ���� �����͸� �����Ѵ�.

	PTRLISTPOS idPos ;																	// �ε��� ��ġ ������ ���� �����͸� �����Ѵ�.
	CQuestString* pQuestString ;														// ����Ʈ ��Ʈ�� ������ ���� �����͸� �����Ѵ�.
	
	pos = NULL ;																		// Ʈ���� ��ġ�� ���� �����͸� null ó���� �Ѵ�.
	pos = m_QuestTree.GetHeadPosition() ;												// ����Ʈ Ʈ�� ����Ʈ�� ���� �����ϰ� ��ġ�� �޴´�.

	while(pos)																			// Ʈ�� ��ġ ������ ��ȿ�� ���� whlie���� ������.
	{
		pTree = NULL ;																	// Ʈ�� ������ ���� �����͸� null ó���� �Ѵ�.
		pTree = (QuestTree*)m_QuestTree.GetNext(pos) ;									// ���� ��ġ�� Ʈ�� ������ �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pTree ) continue ;															// Ʈ�� ������ ��ȿ���� ������ continue ó���� �Ѵ�.

		if( pTree->list.GetCount() < 1 )
		{
			/*sprintf(buf, "(%d)�� ���� ��ϵ� ����Ʈ�� ���, (%d)�� �� ����Ʈ�� �߰����� ���߽��ϴ�.", pTree->nMapNum) ;
			CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
			continue ;
		}

		pMapName = GetMapName(
			MAPTYPE(pTree->nMapNum));

		if( !pMapName )
		{
			/*sprintf(buf, "(%d)�� �� �̸��� �޾ƿ��� ���� (%d)�� �� ����Ʈ�� �߰����� ���߽��ϴ�.", pTree->nMapNum) ;
			CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
			continue ;
		}

		Color = RGBA_MAKE(255, 150, 0, 255) ;											// �� Ʈ���� ������ �����Ѵ�.	
		sprintf(buf, "* ") ;															// �ӽ� ���۸� *�� �����Ѵ�.
		strcat(buf, pMapName) ;															// �ӽ� ���ۿ� �� �̸��� �߰��Ѵ�.

		m_pQuestListDlg->AddItemWithFont(buf, Color, cFont::FONT1) ;							// ����Ʈ ����Ʈ�� �� �̸��� �߰��Ѵ�.
		//m_pQuestListDlg->AddItem(buf, Color) ;											// ����Ʈ ����Ʈ�� �� �̸��� �߰��Ѵ�.

		m_dwQuestList[m_QListCount] = pTree->nMapNum ;									// ����Ʈ ����Ʈ�� ����Ʈ ��Ʈ�� ������ �����Ѵ�.
		++m_QListCount;																	// ����Ʈ ����Ʈ ī��Ʈ�� �����Ѵ�.

		if( pTree->State == eQTree_Close ) continue ;									// Ʈ���� ������ ���¶��, continue ó���� �Ѵ�.

		idPos = NULL ;																	// �ε��� ��ġ ������ ���� ������ null ó���� �Ѵ�.
		idPos = pTree->list.GetHeadPosition() ;											// ���� �� Ʈ���� ����Ʈ�� ���� �����ϰ� ��ġ�� �޴´�.

		while(idPos)																	// ��ġ ������ ��ȿ�� ���� while���� ������.
		{
			pQuestIdx = NULL ;															// ����Ʈ �ε����� ���� �����͸� null ó���� �Ѵ�.
			pQuestIdx = (DWORD*)pTree->list.GetNext(idPos) ;							// ���� ��ġ�� ����Ʈ �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

			if( !pQuestIdx ) continue ;													// ����Ʈ �ε����� ��ȿ���� ������ continue ó���� �Ѵ�.

			memset(buf, 0, sizeof(buf)) ;												// �ӽ� ���۸� �޸� ���Ѵ�.

			pQuestString = NULL ;														// ����Ʈ ��Ʈ�� ������ ���� �����͸� null ó���� �Ѵ�.
			pQuestString = QUESTMGR->GetQuestString(*pQuestIdx, 0) ;					// ���� ����Ʈ�� ���� ��Ʈ���� �޾ƿ´�.

			if( !pQuestString )															// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������,
			{
				/*sprintf(buf, "(%d)�� ����Ʈ ������ �޾ƿ��� ���� (%d)�� ����Ʈ ������ �߰����� ���߽��ϴ�.", pTree->nMapNum) ;
				CHATMGR->AddMsg(CTC_QUEST, buf) ;*/
				continue ;	
			}

			if( m_SelectedQuestID == int(pQuestString->GetQuestID()))
			{
				Color = RGBA_MAKE(255, 255, 0, 255) ;
			}
			else
			{
				Color = RGBA_MAKE(255, 255, 255, 255) ;
			}

			checkPos = NULL ;
			checkPos = pCheckList->GetHeadPosition() ;

			BOOL bResult = FALSE ;

			while(checkPos)
			{
				pCheckIdx = NULL ;
				pCheckIdx = (DWORD*)pCheckList->GetNext(checkPos) ;

				if( !pCheckIdx ) continue ;

				if( *pCheckIdx == pQuestString->GetQuestID() )
				{
					bResult = TRUE ;
					break ;
				}
			}

			if( bResult == FALSE )
			{
				sprintf(buf, "	- ");												// �ӽ� ���۸� -�� �����Ѵ�.
			}
			else
			{
				sprintf(buf, "	��");												// �ӽ� ���۸� -�� �����Ѵ�.
			}


			CQuest* pQuest = QUESTMGR->GetQuest(pQuestString->GetQuestID());

			// 100408 ShinJS --- �ð����� ����Ʈ�̰� ���� ����Ұ����� �ð��ΰ��
			if( pQuest->HasDateLimit() && !QUESTMGR->IsValidDateTime( pQuestString->GetQuestID() ) )
			{
				strcat( buf, CHATMGR->GetChatMsg( 2216 ) );		// "[����Ұ�] "
			}

			ITEM* pItem ;

			cPtrList* pTitleList = NULL ;
			pTitleList = pQuestString->GetTitle() ; 

			PTRLISTPOS titlePos = NULL ;
			titlePos = pTitleList->GetHeadPosition() ;

			while(titlePos)
			{
				pItem = NULL ;
				pItem = (ITEM*)pTitleList->GetNext(titlePos) ;

				if( !pItem ) continue ;

				strcat(buf, pItem->string) ;
			}

			int nSubQuestCount = 0 ;
			pQuestInfo = QUESTMGR->GetQuestInfo(pQuestString->GetQuestID()) ;				// ����Ʈ ��ȣ�� ����Ʈ ������ �޴´�.

			if( pQuestInfo )
			{
				nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// ���� ����Ʈ ���� �޴´�.
			}

			// 100422 ShinJS --- ����Ʈ �䱸������ �Ϸ�Ǿ����� �Ǵ��Ѵ�.
			if( nSubQuestCount - 1 == int(pQuest->GetCurSubQuestIdx()) &&
				pQuest->IsCompletedDemandQuestExecute() )
			{
				strcat(buf, CHATMGR->GetChatMsg(1463)) ;
			}

			m_pQuestListDlg->AddItemWithFont(buf, Color, cFont::FONT0) ;							// ����Ʈ ����Ʈ�� �� �̸��� �߰��Ѵ�.
			//m_pQuestListDlg->AddItem(buf, Color) ;											// ����Ʈ ����Ʈ�� �� �̸��� �߰��Ѵ�.

			m_dwQuestList[m_QListCount] = *pQuestIdx ;										// ����Ʈ ����Ʈ�� ����Ʈ ��Ʈ�� ������ �����Ѵ�.
			++m_QListCount;																	// ����Ʈ ����Ʈ ī��Ʈ�� �����Ѵ�.
		}
	}

	m_pQuestListDlg->ResetGuageBarPos() ;
	m_pQuestListDlg->SetTopListItemIdx(BackupLine);								// �������� ���� �ε����� �����Ѵ�.

	SetQuestCount() ;

	CQuestString* pCurString = NULL ;
	pCurString = QUESTMGR->GetQuestString(m_SelectedQuestID, 0) ;
	// 071012 LYW --- QuestDialog : Modified.
	//QuestDescView(pCurString) ;	// ����Ʈ�� ������, ���������� ǥ�� �ߴ� ������ ��������� null�� �߰��Ѵ�.
	if( m_nQuestCount <= 0 ) 
	{
		QuestDescView(NULL) ;
	}
	else
	{
		QuestDescView(pCurString) ;
	}
}

// 100414 ONS ���ú��� �������� �����Ѵ�.
void CQuestDialog::RefreshSelectRequitalItem()
{
	cIcon* pIcon = NULL;
	WORD Count = 0;
	WORD Pos = 0;

	for(WORD i=0; i<m_pSelectRequitalGrid->GetCellNum(); ++i)
	{
		m_pSelectRequitalGrid->DeleteIcon(i, &pIcon);								// ������ �׸��忡�� �������� �����Ѵ�.
	}																				// ����Ʈ ������ ����Ʈ �˻��� �����Ѵ�.

	PTRLISTPOS pos = m_QuestSelectRequitalList.GetHeadPosition() ;
	while(pos)
	{
		RequitalItem* pItem = (RequitalItem*)m_QuestSelectRequitalList.GetNext(pos) ;
		if( pItem ) 
		{
			if( Count >= 0 && Count < MAX_REQUITAL_ICON_CELL )
			{
				pItem->wPos = Pos ;
				m_pSelectRequitalGrid->AddIcon( pItem->wPos, pItem->pIcon ) ;
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

// 100414 ONS �Ϲݺ���� ���ú����ư ���ý� Ȱ��ȭó��.
void CQuestDialog::SetActiveSelectRequital( BOOL bVal )
{
	m_pSelectRequitalGrid->SetActive(bVal);
	m_pRequitalGrid->SetActive(!bVal);

	m_pRequitalBtn->SetPush(bVal);
	m_pSelectRequitalBtn->SetPush(!bVal);
}

// 100414 ONS �Ϲݺ���/���ú������ UI�ʱ�ȭ
void CQuestDialog::InitRequitalDlg()
{
	m_bRequitalType = eQuestExecute_TakeItem;

	m_pSelectRequitalGrid->SetActive(FALSE);
	m_pRequitalGrid->SetActive(TRUE);

	m_pRequitalBtn->SetPush(FALSE);
	m_pSelectRequitalBtn->SetPush(TRUE);

	m_pItemSignalOn->SetActive(FALSE);
	m_pItemSignalOff->SetActive(TRUE);
	m_pSelectItemSignalOn->SetActive(FALSE);
	m_pSelectItemSignalOff->SetActive(TRUE);
}

void CQuestDialog::AddRemandQuestExecuteString( DWORD dwQuestIdx )
{
	CQuest* pQuest = QUESTMGR->GetQuest(dwQuestIdx);
	if( !pQuest )
		return;

	BOOL bAdded = FALSE;

	const CQuest::DemandQuestExecuteKeyMap& demandQuestExecuteMap = pQuest->GetDemandQuestExecuteMap();
	for( CQuest::DemandQuestExecuteKeyMap::const_iterator iter = demandQuestExecuteMap.begin() ; iter != demandQuestExecuteMap.end() ; ++iter )
	{
		CQuestExecute* pQuestExecute = iter->second;
		switch( pQuestExecute->GetQuestExecuteKind() )
		{
		case eQuestExecute_GiveItem:
			{
				CQuestExecute_Item* pQuestExecute_Item = (CQuestExecute_Item*)pQuestExecute;

				const DWORD dwItemIdx = pQuestExecute_Item->GetItemKind();
				const DWORD dwNeedCnt = pQuestExecute_Item->GetItemNum();

				ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( dwItemIdx );
				if( !pItemInfo )
					continue;

				if( !bAdded )
				{
					bAdded = TRUE;
					m_pQuestDescList->AddItem( " ", 0 );
				}

				char buf[512] = {0,};

				DWORD dwInvenCnt = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwItemIdx );
				sprintf( buf, CHATMGR->GetChatMsg( 2220 ), pItemInfo->ItemName, dwInvenCnt, dwNeedCnt );

				m_pQuestDescList->AddItem( buf, RGBA_MAKE(255, 255, 0, 255) );
			}
			break;
		}
	}
}

// 100611 ONS ����Ʈ���̾�α� ����Ʈ�� ������ ����Ʈ�� �����ϴ��� �˻��Ѵ�.
BOOL CQuestDialog::IsExistInQuestList( DWORD dwQuestIdx )
{
	if( 0 >= dwQuestIdx )
	{
		return FALSE;
	}

	PTRLISTPOS pos = m_QuestTree.GetHeadPosition();
	while(pos)										
	{
		QuestTree* pTree = (QuestTree*)m_QuestTree.GetNext(pos);
		if( NULL == pTree || 
			pTree->list.GetCount() < 1)
		{
			continue;
		}
		
		PTRLISTPOS idPos = pTree->list.GetHeadPosition();
		while(idPos)
		{
			const DWORD* pQuestIdx = (DWORD*)pTree->list.GetNext(idPos);
			if( *pQuestIdx == dwQuestIdx )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

// 100617 ONS ����Ʈ Ʈ���κ��� ������ �׸��� �ε����� ��ȯ�Ѵ�.
DWORD CQuestDialog::GetSelectedQuestIDFromTree( DWORD dwSelectIndex )
{
	DWORD dwCount = 0;
	PTRLISTPOS pos = m_QuestTree.GetHeadPosition();
	while(pos)										
	{
		const QuestTree* pTree = (QuestTree*)m_QuestTree.GetNext(pos);
		if( 0 == pTree || 
			pTree->list.GetCount() < 1)
		{
			return 0;
		}

		PTRLISTPOS idPos = pTree->list.GetHeadPosition();
		while(idPos)
		{
			const DWORD* pQuestIdx = (DWORD*)pTree->list.GetNext(idPos);
			if( *pQuestIdx > 0 && dwCount == dwSelectIndex )
			{
				return *pQuestIdx;
			}
			dwCount++;
		}
	}
	return 0;
}