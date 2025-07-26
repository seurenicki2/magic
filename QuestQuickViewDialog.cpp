#include "stdafx.h"																	// ǥ�� �ý��۰�, ������Ʈ�� ������, ���־��� �ش����� ���� �ش������� �ҷ��´�.

#include ".\questquickviewdialog.h"													// ����Ʈ �˸��� Ŭ���� ����� �ҷ��´�.

#include "WindowIDEnum.h"															// ������ ���̵� �̳� ����� �ҷ��´�.

#include "./Interface/cWindowManager.h"												// ������ �Ŵ��� Ŭ���� ����� �ҷ��´�.
#include "./Interface/cListDialog.h"												// ����Ʈ ���̾�α� Ŭ���� ����� �ҷ��´�.
#include "QuestManager.h"															// ����Ʈ �Ŵ��� Ŭ���� ����� �ҷ��´�.

#include "ChatManager.h"

#include "Quest.h"																	// ����Ʈ Ŭ���� ����� �ҷ��´�.

#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\QuestString.h"												// ����Ʈ ��Ʈ�� Ŭ���� ����� �ҷ��´�.

#include "cMsgBox.h"																// �˸� �޽��� �ڽ� Ŭ���� ����� �ҷ��´�.
#include "QuestDialog.h"															// ����Ʈ ���̾�α� Ŭ���� ����� �ҷ��´�.
#include "../[CC]Quest/SubQuestInfo.h"
#include "../[CC]Quest/QuestExecute.h"
#include "../[CC]Quest/QuestExecute_Item.h"
#include "ItemManager.h"
#include "GameIn.h"
#include "InventoryExDialog.h"

#include "../Input/Mouse.h"															// ���콺 Ŭ���� ����� �ҷ��´�.
#include "./Interface/cStatic.h"
#include "./Interface/cButton.h"
#include "NpcScriptDialog.h"

// 080304 ggomgrak --- ����Ʈ�ڵ��˸��� ó���� ���� �߰� (���� ������ ����)
//#include "ChatManager.h"

CQuestQuickViewDialog::CQuestQuickViewDialog(void)									// ������ �Լ�.
{
	m_type = WT_QUESTQUICKVIEWDIALOG ;												// ������ Ÿ���� �˸��� ���̾�α׷� �����Ѵ�.

	m_QuestTreeList.RemoveAll() ;													// ����Ʈ Ʈ�� ����Ʈ�� ��� ����.

	m_pViewList = NULL ;															// Ʈ�� ����Ʈ ���̾�α� �����͸� null�� �����Ѵ�.
	m_bIsUpdate = FALSE;
}

CQuestQuickViewDialog::~CQuestQuickViewDialog(void)									// �Ҹ��� �Լ�.
{
	Release() ;																		// ���� �Լ��� ȣ���Ѵ�.
}

void CQuestQuickViewDialog::Release()												// ����� ���ҽ��� �����ϴ� �Լ�.
{
	DWORD* pDeleteIdx ;																// ������ ����Ʈ �ε����� ���� �����͸� �����Ѵ�.

	PTRLISTPOS pos = NULL ;															// ��ġ ������ ���� �����͸� �����ϰ� null ó�� �Ѵ�.
	pos = m_QuestTreeList.GetHeadPosition() ;										// ����Ʈ ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

	while(pos)																		// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pDeleteIdx = NULL ;															// ������ �ε����� ���� �����͸� null ó���Ѵ�.
		pDeleteIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// ��ġ�� �ش��ϴ� �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pDeleteIdx ) continue ;												// ����Ʈ �ε����� ��ȿ���� ������, continue �Ѵ�.

		m_QuestTreeList.Remove( pDeleteIdx ) ;										// ����Ʈ ����Ʈ���� ���� �ε����� ������.
	
		delete pDeleteIdx ;															// ���� ����Ʈ �ε����� �����Ѵ�.
	}

	m_QuestTreeList.RemoveAll() ;													// ����Ʈ ����Ʈ�� ��� ����.
}

void CQuestQuickViewDialog::Linking()												// ��ũ �Լ�.
{
	m_pViewList = (cListDialog*)GetWindowForID(QUE_QUICKVIEWLIST) ;					// Ʈ�� ����Ʈ ���̾�α׸� ��ũ�Ѵ�.
	m_pViewList->SetStyle( LDS_FONTSHADOW, 0 ) ;										// ����Ʈ ���̾�α׸� �׸��� ��� ��Ÿ�Ϸ� �����Ѵ�.
	// 071018 LYW --- QuestQuickViewDialog : Setting shadow distance of shadow font to use render shadowfont.
	m_pViewList->Set_ShadowDistance(1) ;

	// 100601 ONS �˸��̵��â ��� UI�߰�
    m_pQuickViewTop		= (cStatic*)GetWindowForID(QUE_QUICKVIEW_TOP);
	m_pQuickViewMiddle	= (cStatic*)GetWindowForID(QUE_QUICKVIEW_MIDDLE);
	m_pQuickViewBottom	= (cStatic*)GetWindowForID(QUE_QUICKVIEW_BOTTOM);

	m_LineHeight		= m_pQuickViewMiddle->GetHeight();

	for( int i = 0; i < MAX_QUICKVIEW_QUEST; i++ )
	{
		m_pQuickViewRemoveBtn[i] = (cButton*)GetWindowForID(QUE_QUICKVIEW_CLOSE_1 + i);
	}

	m_pQuickViewRemoveAllBtn = (cButton*)GetWindowForID(QUE_QUICKVIEW_CLOSE_ALL);
	
}

BOOL CQuestQuickViewDialog::CheckSameQuest(DWORD dwQuestIdx)						// ����Ʈ ����Ʈ ���� ���� ����Ʈ�� �ִ��� ���θ� ��ȯ�ϴ� �Լ�.
{
	BOOL b_Result = FALSE ;															// ��� ���� ���� ������ �����ϰ� ���� false�� �����Ѵ�.

	DWORD* pCheckIdx ;																// ����Ʈ �ε��� ������ ���� �����͸� �����Ѵ�.

	PTRLISTPOS pos = NULL ;															// ��ġ ������ ���� �����͸� �����ϰ� null ó�� �Ѵ�.
	pos = m_QuestTreeList.GetHeadPosition() ;										// ����Ʈ ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

	while(pos)																		// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pCheckIdx = NULL ;															// ����Ʈ �ε��� �����͸� null ó�� �Ѵ�.
		pCheckIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// ��ġ�� �ش��ϴ� ����Ʈ �ε��� ������ �ް�, ���� ��ġ�� �����Ѵ�.

		if( !pCheckIdx ) continue ;													// ����Ʈ �ε��� ������ ��ȿ���� ������, continue �Ѵ�.

		if( *pCheckIdx != dwQuestIdx ) continue ;									// ���ڷ� �Ѿ�� �ε�����, ���� ����Ʈ �ε����� �ٸ���, continue �Ѵ�.

		b_Result = TRUE ;															// ��� ���� true�� �����Ѵ�.

		break ;																		// while���� ����������.
	}

	return b_Result ;																// ��� ���� return ó�� �Ѵ�.
}

// 080304 ggomgrak --- ����Ʈ�ڵ��˸��� ó���� ���� �߰� (���� ������ ����)
void CQuestQuickViewDialog::RegistQuest(DWORD dwQuestIdx)							// �˸��� â�� ����Ʈ�� ��� ��Ű�� �Լ�.
{
	CQuest* pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;								// ���ڷ� �Ѿ�� ����Ʈ �ε����� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;															// ����Ʈ ������ ��ȿ���� ������, return ó���Ѵ�.

	m_bIsUpdate = TRUE;

	if( pQuest->GetSubQuestFlag() == 0 )
	{
		DeleteQuest(dwQuestIdx) ;

		return ;
	}

	if( CheckSameQuest(dwQuestIdx) )												// �˸��� â�� ���ڷ� �Ѿ�� ����Ʈ�� ���� ����Ʈ�� ������,
	{
		DeleteQuest(dwQuestIdx) ;													// ����Ʈ�� �����Ѵ�.
		return ;																	// return ó���Ѵ�.
	}

	int nRegistedCount = 0 ;														// ���� ��� �� ī��Ʈ ���� ���� ������ �����ϰ� 0���� �����Ѵ�.
	nRegistedCount = m_QuestTreeList.GetCount() ;									// ����Ʈ Ʈ�� ����Ʈ�� ī��Ʈ ���� �޴´�.

	if( nRegistedCount >= MAX_QUICKVIEW_QUEST )										// ��� �� ����Ʈ ���� �ִ� ��� ���ɼ� �̻��̸�,
	{
		char tempBuf[256] = {0} ;													// ����Ʈ ���Ͽ� ����� �ӽ� ���۸� �����Ѵ�.

		sprintf(tempBuf, CHATMGR->GetChatMsg(1465), 
			MAX_QUICKVIEW_QUEST) ;													// ��� ���� �޽����� �����Ѵ�.

		WINDOWMGR->MsgBox( MBI_QUESTQUICKVIEW_FULL, MBT_OK, tempBuf) ;				// ���� �޽����� ����Ѵ�.

		return ;																	// return ó���Ѵ�.
	}

	DWORD* pQuestIdx = new DWORD ;													// DWORD �� �����͸� �����ϰ� �����Ϳ� ��´�.

	*pQuestIdx = dwQuestIdx ;														// ����Ϸ��� ����Ʈ �ε����� �����Ѵ�.

	m_QuestTreeList.AddTail(pQuestIdx) ;											// ����Ʈ ����Ʈ�� �߰��Ѵ�.
}

void CQuestQuickViewDialog::DeleteQuest(DWORD dwQuestIdx) 							// �˸��� â�� �ִ� ����Ʈ�� ���� �ϴ� �Լ�.
{
	DWORD* pDeleteIdx ;																// ������ ����Ʈ �ε����� ���� �����͸� �����Ѵ�.

	PTRLISTPOS pos = NULL ;															// ��ġ ������ ���� �����͸� �����ϰ� null ó�� �Ѵ�.
	pos = m_QuestTreeList.GetHeadPosition() ;										// ����Ʈ ����Ʈ�� ���� �����ϰ� ��ġ ������ �޴´�.

	while(pos)																		// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pDeleteIdx = NULL ;															// ������ �ε����� ���� �����͸� null ó���Ѵ�.
		pDeleteIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// ��ġ�� �ش��ϴ� �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pDeleteIdx ) continue ;												// ����Ʈ �ε����� ��ȿ���� ������, continue �Ѵ�.

		if( *pDeleteIdx != dwQuestIdx ) continue ;									// ���ڷ� �Ѿ�� �ε����� ���� �ε����� �ٸ���, continue �Ѵ�.

		m_QuestTreeList.Remove( pDeleteIdx ) ;										// ����Ʈ ����Ʈ���� ���� �ε����� ������.
	
		delete pDeleteIdx ;															// ���� ����Ʈ �ε����� �����Ѵ�.

		break ;																		// while���� Ż���Ѵ�.
	}

	ResetQuickViewDialog() ;														// �˸��� â�� ���ΰ�ħ �Ѵ�.
}

void CQuestQuickViewDialog::ResetQuickViewDialog()									// ����Ʈ ����Ʈ�� ���ΰ�ħ�ϴ� �Լ�.
{
	int nItemCount = 0 ;															// �˸��� ����Ʈ�� ������ ī��Ʈ�� ���� ������ �����ϰ� 0���� �����Ѵ�.
	nItemCount = m_pViewList->GetItemCount() ;										// �˸��� ����Ʈ�� ������ ī��Ʈ�� �޴´�.

	m_pViewList->RemoveAll() ;														// ����Ʈ ����Ʈ ���̾�α׸� ��� ����.

	for( int count = 0 ; count < nItemCount ; ++count )								// �˸��� ����Ʈ�� ������ ī��Ʈ ��ŭ for���� ������.
	{
		m_pViewList->SetExtendReduction(LD_REDUCTION) ;								// ����Ʈ ���̾�α׸� ����Ѵ�.
	}

	for( int count = 0 ; count < MAX_QUICKVIEW_QUEST ; ++count )
	{
		m_pQuickViewRemoveBtn[count]->SetActive( FALSE );
	}

	PTRLISTPOS pos = NULL ;															// ��ġ ������ ���� �����͸� �����ϰ� null ó���Ѵ�.
	pos = m_QuestTreeList.GetHeadPosition() ;										// ����Ʈ ����Ʈ�� ���� �����ϰ�, ��ġ���� �����Ѵ�.
	if( NULL == pos )
	{
		SetActive(FALSE);
		return;
	}

	DWORD* pCheckIdx ;																// �ε��� ������ ���� �����͸� �����Ѵ�.
	CQuest* pQuest ;																// ����Ʈ ������ ���� �����͸� �����Ѵ�.
	CSubQuestInfo* pSubQuestInfo ;													// ���� ����Ʈ ������ ���� �����͸� �����Ѵ�.
	SUBQUEST* pSubQuest ;															// ���� ����Ʈ�� ���� �����͸� �����Ѵ�.
	DWORD dwIndex = 0;
	while(pos)																		// ��ġ ������ ��ȿ�� ���� while���� ������.
	{
		pCheckIdx = NULL ;															// �ε����� ���� �����͸� null ó���Ѵ�.
		pCheckIdx = (DWORD*)m_QuestTreeList.GetNext(pos) ;							// ��ġ�� �ش��ϴ� �ε����� �ް�, ��ġ�� �������� �����Ѵ�.

		if( !pCheckIdx ) continue ;													// �ε��� ������ ��ȿ���� ������, continue �Ѵ�.

		pQuest = NULL ;																// ����Ʈ ������ ���� �����͸� null ó���Ѵ�.
		pQuest = QUESTMGR->GetQuest(*pCheckIdx) ;									// ���� �ε����� ����Ʈ �Ŵ������� ����Ʈ ������ �޴´�.

		if( !pQuest ) continue ;													// ����Ʈ ������ ��ȿ���� ������, continue �Ѵ�.

		if( pQuest->IsQuestComplete() || !pQuest->GetSubQuest(0) )					// ���� �Ϸ��߰ų�, ���� ����Ʈ 0�� ������ ������,
		{
			DeleteQuest(*pCheckIdx) ;												// ����Ʈ�� Ʈ������ �����Ѵ�.
		}

		// 100708 ONS �˸��̴��̾�α��� ��������Ʈ�� ������ư�� �߰��Ѵ�.
		m_pQuickViewRemoveBtn[dwIndex]->SetAbsY(LONG(m_pViewList->GetAbsY() + m_pViewList->GetItemCount() * 15) );
		m_pQuickViewRemoveBtn[dwIndex]->SetRelXY( LONG(m_pQuickViewRemoveBtn[dwIndex]->GetRelX()), 
												  LONG(m_pViewList->GetRelY() + m_pViewList->GetItemCount() * 15) );
		dwIndex++;

		AddMainString( *pCheckIdx, 0 ) ;											// ����Ʈ ������ �߰��Ѵ�.

		for( int count = 0 ; count < MAX_SUBQUEST ; ++count )						// �� ����Ʈ�� ���� �� �ִ� �ִ� �� ��ŭ for���� ������.
		{
			pSubQuestInfo = NULL ;													// ���� ����Ʈ ������ ���� �����͸� null ó���Ѵ�.
			pSubQuestInfo = pQuest->GetSubQuestInfoArray(count) ;					// ī��Ʈ�� �ش��ϴ� ���� ����Ʈ ������ �޴´�.

			if( !pSubQuestInfo ) continue ;											// ���� ����Ʈ ������ ��ȿ���� ������, continue �Ѵ�.

			pSubQuest = NULL ;														// ���� ����Ʈ ������ ���� �����͸� nulló�� �Ѵ�.
			pSubQuest = pQuest->GetSubQuest(count) ;								// ī��Ʈ�� �ش��ϴ� ���� ����Ʈ ������ �޴´�.

			if(!pSubQuest) continue ;												// ���� ����Ʈ ������ ��ȿ���� ������, continue �Ѵ�.

			int nSubQuestType = e_SQT_NORMAL ;										// ����Ʈ Ÿ���� ���� ������ �����ϰ�, �⺻���� �����Ѵ�.

			if( pSubQuest->dwMaxCount > 0 )											// ��ǥ ī��Ʈ�� ������,
			{
				nSubQuestType = e_SQT_HUNT ;										// ��� ����Ʈ�̴�.
			}
			else																	// ��ǥ ī��Ʈ�� ������,
			{
				if( pSubQuest->dwData > 0 )											// ���� ī��Ʈ�� ������,
				{
					nSubQuestType = e_SQT_COLLECTION ;								// ���� ����Ʈ�̴�.
				}
				else																// ���� ī��Ʈ�� ������,
				{
					nSubQuestType = e_SQT_COMPLETED ;								// �Ϸ� ����Ʈ �̴�.
				}
			}

			AddRemandQuestExecuteString( *pCheckIdx );

			AddSubString( *pCheckIdx, count, nSubQuestType ) ;						// ���� ����Ʈ ��Ʈ���� �߰��Ѵ�.
		}
	}

	UpdateQuickView();
}

void CQuestQuickViewDialog::AddMainString(DWORD dwQuestIdx, DWORD dwSubQuestIdx)	// ����Ʈ ������ �߰��ϴ� �Լ�.
{
	CQuestString* pString = NULL ;													// ����Ʈ ��Ʈ�� ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pString = QUESTMGR->GetQuestString(dwQuestIdx, dwSubQuestIdx) ;					// ����Ʈ �ε�����, ����Ʈ ��Ʈ�� ������ �޾ƿ´�.

	if( !pString ) return ;															// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������, return ó�� �Ѵ�.	

	char tempBuf[512] = {0,} ;														// �ӽ� ���۸� �����Ѵ�.
	GetTitleString( tempBuf, pString ) ;											// ����Ʈ ��Ʈ���� ������ �޴´�.

	if( strlen(tempBuf) <= 4 ) return ;												// ������ ��ȿ���� ������ return ó���Ѵ�.

	m_pViewList->AddItem( tempBuf, RGB(0, 255, 255) ) ;								// ����Ʈ ������ �߰��Ѵ�.
	m_pViewList->SetExtendReduction(LD_EXTEND) ;									// ����Ʈ ���̾�α׸� Ȯ���Ѵ�.
}

void CQuestQuickViewDialog::AddSubString(DWORD dwQuestIdx, DWORD dwSubQuestIdx, int nType)		// ���� ����Ʈ ������ �߰��ϴ� �Լ�.
{
	CQuestString* pString = NULL ;													// ����Ʈ ��Ʈ�� ������ ���� �����͸� �����ϰ� null ó���� �Ѵ�.
	pString = QUESTMGR->GetQuestString(dwQuestIdx, dwSubQuestIdx) ;					// ����Ʈ �ε�����, ����Ʈ ��Ʈ�� ������ �޾ƿ´�.

	if( !pString ) return ;															// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������, return ó�� �Ѵ�.	

	CQuest* pQuest = NULL ;															// ����Ʈ ������ ���� �����͸� �����ϰ� null ó���Ѵ�.
	pQuest = QUESTMGR->GetQuest(dwQuestIdx) ;										// �ε����� �ش��ϴ� ����Ʈ ������ �޴´�.

	if( !pQuest ) return ;															// ����Ʈ ������ ��ȿ���� ������, return ó�� �Ѵ�.

	SUBQUEST* pSubQuest ;															// ���� ����Ʈ ������ ���� �����͸� �����Ѵ�.
	pSubQuest = pQuest->GetSubQuest(dwSubQuestIdx) ;								// ���� ����Ʈ ������ �޴´�.

	if( !pSubQuest ) return ;														// ���� ����Ʈ ������ ��ȿ���� ������, return ó�� �Ѵ�.

	CQuestString* pQuestString ;														// ����Ʈ ��Ʈ�� ������ ���� �����͸� �����Ѵ�.

	pQuestString = NULL ;														// ����Ʈ ��Ʈ�� ������ ���� �����͸� null ó���� �Ѵ�.
	pQuestString = QUESTMGR->GetQuestString(dwQuestIdx, 0) ;					// ���� ����Ʈ�� ���� ��Ʈ���� �޾ƿ´�.

	if( !pQuestString ) return ;

	CQuestInfo* pQuestInfo ;															// ����Ʈ ������ ���� �����͸� �����Ѵ�.

	int nSubQuestCount = 0 ;
	pQuestInfo = QUESTMGR->GetQuestInfo(pQuestString->GetQuestID()) ;				// ����Ʈ ��ȣ�� ����Ʈ ������ �޴´�.

	if( !pQuestInfo ) return ;

	char tempBuf[512] = {0,} ;														// �ӽ� ���۸� �����Ѵ�.
	strcat(tempBuf, "	- ") ;														// �������� �߰��Ѵ�.
	GetTitleString( tempBuf, pString ) ;											// ����Ʈ ��Ʈ���� ������ �޴´�.

	if( strlen(tempBuf) <= 4 ) return ;												// ������ ��ȿ���� ������ return ó���Ѵ�.

	char typeBuf[64] = {0, } ;														// Ÿ�Կ� ���� ó���� ���� Ÿ�� ���۸� �����Ѵ�.

	switch(nType)																	// ���� ����Ʈ Ÿ���� Ȯ���Ѵ�.
	{
	case e_SQT_NORMAL : break ;														// �⺻ Ÿ���ΰ��,
	case e_SQT_HUNT :																// ��� Ÿ���� ���,
		{
			sprintf(typeBuf, "(%d/%d)", pSubQuest->dwData, pSubQuest->dwMaxCount) ; // ����/��ǥ ī��Ʈ�� �߰��� �ش�.
		}
		break ;

	case e_SQT_COLLECTION :															// ���� Ÿ���� ���,
		{
			sprintf(typeBuf, "(%d)", pSubQuest->dwData) ;							// ���� ī��Ʈ�� �߰��� �ش�.
		}
		break ;

	case e_SQT_COMPLETED :															// �Ϸ� Ÿ���� ���,
		{
		}
		break ;
	}

	strcat(tempBuf, typeBuf) ;

	nSubQuestCount = pQuestInfo->GetSubQuestCount() ;							// ���� ����Ʈ ���� �޴´�.

	// 100422 ShinJS --- ����Ʈ �䱸������ �Ϸ�Ǿ����� �Ǵ��Ѵ�.
	if( nSubQuestCount - 1 == int(pQuest->GetCurSubQuestIdx()) &&
		pQuest->IsCompletedDemandQuestExecute() )
	{
		strcat(tempBuf, CHATMGR->GetChatMsg(1463)) ;
	}

	// 100726 ONS ���� ����Ʈ ������ �߰��Ѵ�.
	cListDialog::ToolTipTextList toolTipList;
	cPtrList* pDescList = pString->GetDesc();
	if( pDescList )
	{
		char line[MAX_PATH] = {0};
		for( int count = 0; count < pDescList->GetCount(); ++count )
		{
			PTRLISTPOS pos = pDescList->FindIndex(count);
			if( !pos ) continue;						

			ITEM* pDescLine = (ITEM*)pDescList->GetAt(pos);
			if( pDescLine )									
			{
				sprintf(line, "%s", pDescLine->string);
				toolTipList.push_back(line);
			}
		}		
	}

	const WORD TOOLTIP_BACK_IMAGE_INDEX = 149;
	m_pViewList->AddItem(tempBuf, RGB(255, 255, 255), toolTipList, -1, 0, TOOLTIP_BACK_IMAGE_INDEX );

	m_pViewList->SetExtendReduction(LD_EXTEND) ;									// ����Ʈ ���̾�α׸� Ȯ���Ѵ�.
	m_bIsUpdate = TRUE;
}

void CQuestQuickViewDialog::GetTitleString( char* pBuf, CQuestString* pQuestString )// ����Ʈ ��Ʈ���� ���� ��Ʈ���� ��� �Լ�.
{
	if( !pQuestString ) return ;													// ����Ʈ ��Ʈ�� ������ ��ȿ���� ������, null�� return �Ѵ�.

	if( !pBuf ) return ;

	cPtrList* pList = NULL ;
	pList = pQuestString->GetTitle() ;

	if( !pList ) return ;

	ITEM* pItem ;

	PTRLISTPOS pos = NULL ;
	pos = pList->GetHeadPosition() ;

	while( pos )
	{
		pItem = NULL ;
		pItem = (ITEM*)pList->GetNext(pos) ;

		if( !pItem ) continue ;

		strcat(pBuf, pItem->string) ;
	}
}

void CQuestQuickViewDialog::AddRemandQuestExecuteString( DWORD dwQuestIdx )
{
	CQuest* pQuest = QUESTMGR->GetQuest(dwQuestIdx);
	if( !pQuest )
		return;

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

				char buf[MAX_PATH] = {0,};
				char fmt[MAX_PATH] = {0,};

				const DWORD dwInvenCnt = GAMEIN->GetInventoryDialog()->GetTotalItemDurability( dwItemIdx );
				sprintf( fmt, "	- %s", CHATMGR->GetChatMsg( 2220 ) );
				sprintf( buf, fmt, pItemInfo->ItemName, dwInvenCnt, dwNeedCnt );

				if( dwInvenCnt >= dwNeedCnt )
					continue;

				m_pViewList->AddItem( buf, RGBA_MAKE(255, 255, 255, 255) );
				m_pViewList->SetExtendReduction(LD_EXTEND);
			}
			break;
		}
	}
}

DWORD CQuestQuickViewDialog::ActionEvent(CMouse* mouseInfo)							
{
	DWORD we = WE_NULL;																

	if( !m_bActive )																
	{
		return we;																	
	}

	we = cDialog::ActionEvent(mouseInfo);											

	int nMouseX = mouseInfo->GetMouseX() ;											
	int nMouseY = mouseInfo->GetMouseY() ;											

	// ĸ�ǿ����� ������ ������ Ŭ���Ұ�� �׶��� Ŭ������ ó���Ǿ���ϹǷ� �����Ų��.
	BOOL bOnCaptionRect = m_pQuickViewTop->PtInWindow(nMouseX,nMouseY);
	BOOL bOnDrawRect = m_pQuickViewMiddle->PtInWindow(nMouseX,nMouseY);
	BOOL bActiveFlag = bOnDrawRect || bOnCaptionRect;

	if( m_pQuickViewMiddle->IsActive() != bActiveFlag	|| 
		m_bIsUpdate										)
	{
		// �˸���â�� Active���°� �ٲ𶧸� �����Ѵ�. 
		m_pQuickViewTop->SetActive( bActiveFlag );
		m_pQuickViewMiddle->SetActive( bActiveFlag );
		m_pQuickViewBottom->SetActive( bActiveFlag );

		// ����Ʈ�� ���ŵǸ� ��ư���¸� �����Ѵ�.
		m_pQuickViewRemoveAllBtn->SetActive( bActiveFlag );
		for( int i = 0; i < m_QuestTreeList.GetCount(); i++ )
		{
			m_pQuickViewRemoveBtn[i]->SetActive( bActiveFlag );
		}

		m_bIsUpdate = FALSE;
	}
    	
	if( (we & WE_LBTNCLICK)		&& 
		!bOnCaptionRect			&& 
		bOnDrawRect				)
	{
		// ����Ʈ �������� ��ư�� Ŭ������ ���, ����ó���� �����Ѵ�.
		for( int i = 0; i < m_QuestTreeList.GetCount(); i++ )
		{
			if( m_pQuickViewRemoveBtn[i]->PtInWindow(nMouseX,nMouseY) )
			{
				return we;
			}
		}

		// ��ư�̿��� ������ Ŭ���������, �̵�ó���� �����Ѵ�.
		return NULL;
	}

	return we;
}

// 100601 ONS ����Ʈ�˸���â�� ������Ʈ�Ѵ�.
void CQuestQuickViewDialog::UpdateQuickView()
{
	const WORD ItemCount = (WORD)m_pViewList->GetItemCount();
	if( ItemCount == 0 )
	{
		// �˸��̰� ������ ���̾�α׸� ��Ȱ��ȭ.
		SetActive(FALSE);
		return;
	}
	
	if( !IsActive() )
	{
		m_pQuickViewTop->SetActive(FALSE);
		m_pQuickViewMiddle->SetActive(FALSE);
		m_pQuickViewBottom->SetActive(FALSE);

		for( int i = 0; i < m_QuestTreeList.GetCount(); i++ )
		{
			m_pQuickViewRemoveBtn[i]->SetActive(FALSE);
		}
		m_pQuickViewRemoveAllBtn->SetActive(FALSE);

		SetActive( TRUE );
	}
	// �˸���â ����� ����Ʈ�� �߰�/�����ɶ� �̹�������� ����ؼ� �������� �����ؼ� ǥ���Ѵ�.
	VECTOR2 vScale = {0};
	vScale.x = 1.0f;
	vScale.y = (float)ItemCount;
	m_pQuickViewMiddle->SetScale( &vScale );

	m_pQuickViewMiddle->SetWH(		m_pQuickViewMiddle->GetWidth(),  
									m_LineHeight * ItemCount );	
	
	m_pViewList->SetWH( GetWidth(), m_LineHeight * ItemCount );

	// ����Ʈ������ ����ʿ����� BOTTOM�̹����� ��ġ�� ���Ž�Ų��.
	m_pQuickViewBottom->SetAbsXY(	(LONG)m_pQuickViewBottom->GetAbsX(), 
									(LONG)(m_pQuickViewMiddle->GetAbsY() + m_LineHeight * ItemCount) );

	m_pQuickViewBottom->SetRelXY(	(LONG)m_pQuickViewBottom->GetRelX(),  
									(LONG)(m_pQuickViewMiddle->GetRelY() + m_LineHeight * ItemCount) );
}

void CQuestQuickViewDialog::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( lId >= QUE_QUICKVIEW_CLOSE_1	&& 
		lId <= QUE_QUICKVIEW_CLOSE_10 )
	{
		// ����Ʈ �˸��� ���� ���� ó��
		PTRLISTPOS pos = m_QuestTreeList.FindIndex( int(lId - QUE_QUICKVIEW_CLOSE_1) );
		if( pos )
		{
			DWORD* pQuestIndex = (DWORD*)m_QuestTreeList.GetAt(pos);
			if( 0 == pQuestIndex )
			{
				return;
			}

			DeleteQuest(*pQuestIndex);
			// �˸��� ����̹����� ũ�⸦ �����Ѵ�.
			UpdateQuickView();
			// ����Ʈ���̾�α��� ����Ʈ�� �����Ѵ�.
			GAMEIN->GetQuestDialog()->RefreshQuestList();
		}
	}
	else if( lId == QUE_QUICKVIEW_CLOSE_ALL )
	{
		// ����Ʈ �˸��� �ϰ� ���� ó��
		PTRLISTPOS pos = m_QuestTreeList.GetHeadPosition() ;
		while(pos)
		{
			DWORD* pDeleteIdx = (DWORD*)m_QuestTreeList.GetNext(pos);
			if( 0 == pDeleteIdx )
			{
				continue;
			}

			m_QuestTreeList.Remove( pDeleteIdx );
			delete pDeleteIdx;
		}
		UpdateQuickView();
		ResetQuickViewDialog();
		GAMEIN->GetQuestDialog()->RefreshQuestList();
	}
}

