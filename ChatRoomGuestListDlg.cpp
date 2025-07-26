#include "Stdafx.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "ChatRoomGuestListDlg.h"
#include "Input/Mouse.h"
#include "Interface/cResourceManager.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "Interface/cButton.h"
#include "Interface/cListDialog.h"
#include "ChatRoomMgr.h"
#include "ChatManager.h"

CChatRoomGuestListDlg::CChatRoomGuestListDlg(void)
{
	// 100107 ONS ��Ȯ������� �г���, ����, ������ ������ ����Ʈ�� ����
	m_pGuestNicknameList		= NULL ;
	m_pGuestStateList			= NULL ;
	m_pGuestLevelList			= NULL ;

	m_pBtn_ChangeOwner	= NULL ;
	m_pBtn_FreezeGuest	= NULL ;
	m_pBtn_KickGuest	= NULL ;
	m_pBtn_AddFriend	= NULL ;

	//// Ŭ���� ��ũ �ʱ�ȭ.
	//for(BYTE count = 0 ; count < MAX_ROW_GUESTLIST ; ++count)
	//{
	//	ZeroMemory(&m_MarkImg[count], sizeof(cImage)) ;
	//}

	// ���� �� ���� ���� �ʱ�ȭ.
	memset(&m_SelectedUser, 0, sizeof(ST_CR_USER)) ;

	// �� ���� �ʱ�ȭ.
	//memset(&m_MyUserInfo, 0, sizeof(ST_CR_USER)) ;

	m_plGuest = NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomGuestListDlg
//	DESC : �Ҹ��� �Լ�.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomGuestListDlg::~CChatRoomGuestListDlg(void)
{
	//ST_CR_USER* pInfo ;
	//PTRLISTPOS pos = NULL ;
	//pos = m_plGuest->GetHeadPosition() ;

	//BYTE byCount = 0 ;
	//while(pos)
	//{
	//	pInfo = NULL ;
	//	pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

	//	if(!pInfo) continue ;

	//	m_plGuest->Remove(pInfo) ;

	//	delete pInfo ;
	//}

	//m_plGuest->RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : ���� ��Ʈ�ѵ��� ��ũ�ϴ� �Լ�.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::Linking()
{
	// ���� ��Ʈ�� ��ũ.
	// 100107 ONS ��Ȯ������� �г���, ����, ������ ������ ����Ʈ�� ����
	m_pGuestNicknameList= (cListDialog*)GetWindowForID(CRGLD_LST_GUESTNICKNAME) ;
	m_pGuestStateList	= (cListDialog*)GetWindowForID(CRGLD_LST_GUESTSTATE) ;
	m_pGuestLevelList	= (cListDialog*)GetWindowForID(CRGLD_LST_GUESTLEVEL) ;

	m_pBtn_ChangeOwner	= (cButton*)GetWindowForID(CRGLD_BTN_CHANGEOWNER) ;
	m_pBtn_FreezeGuest	= (cButton*)GetWindowForID(CRGLD_BTN_FREEZEGUEST) ;
	m_pBtn_KickGuest	= (cButton*)GetWindowForID(CRGLD_BTN_KICKGUEST) ;
	m_pBtn_AddFriend	= (cButton*)GetWindowForID(CRGLD_BTN_ADDFRIEND) ;



	// ��ũ �� ��Ʈ�� ��ȿ�� üũ.
	ASSERT(m_pGuestNicknameList) ;
	ASSERT(m_pGuestStateList) ;
	ASSERT(m_pGuestLevelList) ;

	ASSERT(m_pBtn_ChangeOwner) ;
	ASSERT(m_pBtn_FreezeGuest) ;
	ASSERT(m_pBtn_KickGuest) ;
	ASSERT(m_pBtn_AddFriend) ;

	if( !m_pGuestNicknameList	|| 
		!m_pGuestStateList		|| 
		!m_pGuestLevelList		||
		!m_pBtn_ChangeOwner		|| 
		!m_pBtn_FreezeGuest		|| 
		!m_pBtn_KickGuest		||
		!m_pBtn_AddFriend)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ä�ù� �Ŵ����� ���.
	CHATROOMMGR->SetGuestListDlg(this) ;



	// ä�ù� �Ŵ����� �����ϰ� �ִ� ������ ����Ʈ�� �޴´�.
	m_plGuest = CHATROOMMGR->GetGuestList() ;
}





////-------------------------------------------------------------------------------------------------
////	NAME : Render
////	DESC : ���� ����Ʈ ��Ʈ�ѿ� Ŭ���� ��ũ�� ����Ѵ�.
////  DATE : APRIL 12, 2008 LYW
////-------------------------------------------------------------------------------------------------
//void CChatRoomGuestListDlg::Render()
//{
//	// Ȱ��ȭ ���� üũ.
//	if( !IsActive() ) return ;
//
//	// ���� ��Ʈ�� üũ.
//	ASSERT(m_pGuestList) ;
//
//	if(!m_pGuestList)
//	{
//		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
//		SetActive(FALSE) ;
//		return ;
//	}
//
//	// ���̾�α�/���� ��Ʈ�� ����.
//	cDialog::RenderWindow() ;
//	cDialog::RenderComponent() ;
//
//	// �̹��� ��Ʈ ����.
//	cImageRect* pRect ;
//
//	// ������ ����.
//	VECTOR2 vScale ;
//	memset(&vScale, 0, sizeof(VECTOR2)) ;
//
//	vScale.x = 1.0f ;
//	vScale.y = 1.0f ;
//
//	// ��ġ ����.
//	VECTOR2 vPos ;
//	memset(&vPos, 0, sizeof(VECTOR2)) ;
//
//	vPos.x = m_pGuestList->GetAbsX() ;
//	vPos.y = m_pGuestList->GetAbsY() ;//+ m_pGuestList->GetHeadLineHeight() + 4 ;
//
//	// ��� ���� ���� �ޱ�.
//	int nRows = 0 ;
//	//nRows = m_pGuestList->GetRowsNum() ;
//	nRows = m_pGuestList->GetMinMiddleNum() ;
//
//	// ��� ������ ���� �� ��ŭ for��.
//	for(int count = 0 ; count < nRows ; ++count)
//	{
//		// �̹��� ��ȿ�� üũ.
//		if(m_MarkImg[count].IsNull()) continue ;
//
//		// �̹��� ����.
//		m_MarkImg[count].RenderSprite(&vScale, 0, 0, &vPos, 0xffffffff) ;
//
//		// �̹��� ��Ʈ ����.
//		pRect = NULL ;
//		pRect = m_MarkImg[count].GetImageRect() ;
//
//		ASSERT(pRect) ;
//
//		if(!pRect)
//		{
//			CHATROOMMGR->Throw_Error("Invalid rect!!", __FUNCTION__) ;
//			SetActive(FALSE) ;
//			return ;
//		}
//
//		vPos.y += ((pRect->bottom-pRect->top)+6) ;
//	}
//}





//-------------------------------------------------------------------------------------------------
//	NAME : ActionEvent
//	DESC : ���콺 �̺�Ʈ�� ó���ϴ� �Լ�.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomGuestListDlg::ActionEvent( CMouse* mouseInfo ) 
{
	// �̺�Ʈ ���� ����.
	DWORD we = WE_NULL ;



	// ���� ��Ʈ�� üũ.
	ASSERT(m_pGuestNicknameList) ;
	ASSERT(m_pGuestStateList) ;
	ASSERT(m_pGuestLevelList) ;

	if(!m_pGuestNicknameList || !m_pGuestStateList || !m_pGuestLevelList)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		SetActive(FALSE) ;
		return we ;
	}



	// ���콺 ���� üũ.
	if( !mouseInfo ) return we ;



	// ���̾�α� �̺�Ʈ�� �޴´�.
	we = cDialog::ActionEvent( mouseInfo ) ;

	// ���� ����Ʈ �� ���콺�� ������,
	if(m_pGuestNicknameList->PtInWindow( (LONG)mouseInfo->GetMouseX(), (LONG)mouseInfo->GetMouseY() ))
	{
		// ������ ����Ʈ üũ.
		ASSERT(m_plGuest) ;

		if(!m_plGuest)
		{
			CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
			return we ;
		}
		else if( we & WE_LBTNCLICK )
		{
			// 100107 ONS ��Ȯ������� �г���, ����, ������ ������ ����Ʈ�� ����
			BYTE byCount = (BYTE)m_pGuestNicknameList->GetItemCount();
			BYTE bySelectRow = BYTE(m_pGuestNicknameList->GetSelectRowIdx() + m_pGuestNicknameList->GetTopListItemIdx());
			DWORD dwColor = RGBA_MAKE(255, 255, 255, 255);

			for(BYTE count = 0 ; count < byCount ; ++count)
			{
				ITEM* pNickItem = NULL ;
				ITEM* pStateItem = NULL;
				ITEM* pLevelItem = NULL;
					
				pNickItem	= (ITEM*)m_pGuestNicknameList->GetItem((LONG)count) ;
				pStateItem	= (ITEM*)m_pGuestStateList->GetItem((LONG)count) ;
				pLevelItem	= (ITEM*)m_pGuestLevelList->GetItem((LONG)count) ;

				if(!pNickItem || !pStateItem || !pLevelItem) continue ;

				// ������ �������̸�, 
				if(count == bySelectRow)
				{
					// ������ �������� ������Ʈ.
					dwColor = RGBA_MAKE(255, 0, 0, 255) ;
				}
				// �⺻ �������� ����.
				else
				{
					dwColor = RGBA_MAKE(255, 255, 255, 255) ;
				}
				
				pNickItem->rgb  = dwColor;
				pStateItem->rgb = dwColor;
				pLevelItem->rgb = dwColor;
			}

			ST_CR_USER* pInfo ;
			PTRLISTPOS pos = NULL ;
			pos = m_plGuest->GetHeadPosition() ;

			BYTE byCheckCount = 0 ;
			while(pos)
			{
				pInfo = NULL ;
				pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

				if(!pInfo) continue ;

				if(byCheckCount == bySelectRow)
				{
					memcpy(&m_SelectedUser, pInfo, sizeof(ST_CR_USER)) ;
					break ;
				}
				++byCheckCount ;
			}
		}
	}

	return we ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : EventFunc�� ���� �Ѿ�� Event�� ó���ϴ� �Լ�.
//  DATE : APRIL 12, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(p) ;	

	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// ���� ��Ʈ�� üũ.
	ASSERT(m_pBtn_ChangeOwner) ;
	ASSERT(m_pBtn_FreezeGuest) ;
	ASSERT(m_pBtn_KickGuest) ;
	ASSERT(m_pBtn_AddFriend) ;

	if( !m_pGuestNicknameList	||
		!m_pGuestStateList		|| 
		!m_pGuestLevelList		|| 
		!m_pBtn_ChangeOwner		|| 
		!m_pBtn_FreezeGuest		|| 
		!m_pBtn_KickGuest		|| 
		!m_pBtn_AddFriend)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ��Ʈ�� ���̵� üũ.
	switch(id)
	{
	case CRGLD_BTN_CHANGEOWNER : ChangeOwner() ; break ;
	case CRGLD_BTN_FREEZEGUEST : FreezeGuest() ; break ;
	case CRGLD_BTN_KICKGUEST :	 KickGuest() ;	 break ;
	case CRGLD_BTN_ADDFRIEND :	 AddFriend() ;	 break ;
	default : break ;
	}



	//������ ������ ����Ʈ ���̾�α׿� ����.
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : InitGuestList
//	DESC : ������ ����Ʈ�� �����ϴ� �Լ�.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::InitGuestList(ST_CR_USER* pUser, BYTE byCount)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pUser) ;

	if(!pUser)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// ������ ����Ʈ üũ.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ���� ���� ����Ʈ ���.
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		m_plGuest->Remove(pInfo) ;

		delete pInfo ;
	}
	m_plGuest->RemoveAll() ;

	// ����Ʈ ���̾�α� ����.
	//m_pGuestList->DeleteAllItems() ;
	m_pGuestNicknameList->RemoveAll() ;
	m_pGuestStateList->RemoveAll() ;
	m_pGuestLevelList->RemoveAll() ;




	// ������ ����Ʈ�� ���� ���� ���.
	for(BYTE count = 0 ; count < byCount ; ++count)
	{
		if(pUser[count].dwPlayerID == 0) continue ;

		//if(HEROID == pUser[count].dwPlayerID)
		//{
		//	m_MyUserInfo = pUser[count] ;
		//}

		ST_CR_USER* pGuest = NULL ;
		pGuest = new ST_CR_USER ;

		ASSERT(pGuest) ;

		if(!pGuest)
		{
			CHATROOMMGR->Throw_Error("Failed to create user!!", __FUNCTION__) ;
			return ;
		}

		memcpy(pGuest, &pUser[count], sizeof(ST_CR_USER)) ;
		m_plGuest->AddTail(pGuest) ;
	}



	// ������ ������ ����Ʈ ���̾�α׿� ����
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddGuest
//	DESC : ������ �߰� ó���� �ϴ� �Լ�.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
BOOL CChatRoomGuestListDlg::AddGuest(ST_CR_USER* pUser)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pUser) ;

	if(!pUser)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return FALSE ;
	}



	// ������ ����Ʈ üũ.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return FALSE ;
	}



	// ���� ���� ����.
	ST_CR_USER* pGuest = NULL ;
	pGuest = new ST_CR_USER ;

	// ���� ���� üũ.
	ASSERT(pGuest) ;

	if(!pGuest)
	{
		CHATROOMMGR->Throw_Error("Failed to create user!!", __FUNCTION__) ;
		return FALSE ;
	}

	// ���� ���� ���.
	memcpy(pGuest, pUser, sizeof(ST_CR_USER)) ;
	m_plGuest->AddTail(pGuest) ;



	// ������ ������ ����Ʈ ���̾�α׿� ����.
	SetClassMark() ;

	return TRUE ;
}




//-------------------------------------------------------------------------------------------------
//	NAME : DeleteGuest
//	DESC : ������ ���� ó���� �ϴ� �Լ�.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::DeleteGuest(DWORD dwPlayerID)
{
	// ������ ����Ʈ üũ.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ���� �� ��� ���� ó��.
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		if(pInfo->dwPlayerID != dwPlayerID) continue ;

		// 100427 ONS ������ ���������� �ʱ�ȭ�Ѵ�.
		if( m_SelectedUser.dwPlayerID == pInfo->dwPlayerID )
		{
			ZeroMemory(&m_SelectedUser, sizeof(ST_CR_USER)) ;
		}

		m_plGuest->Remove(pInfo) ;

		delete pInfo ;

		break ;
	}



	// ������ ������ ����Ʈ ���̾�α׿� ����
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetClassMark
//	DESC : ������ ������ ����Ʈ ���̾�α׿� �����ϴ� �Լ�.
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::SetClassMark()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pGuestNicknameList) ;
	ASSERT(m_pGuestStateList) ;
	ASSERT(m_pGuestLevelList) ;
	
	if( !m_pGuestNicknameList || !m_pGuestStateList || !m_pGuestLevelList )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		SetActive(FALSE) ;
		return ;
	}



	// ������ ����Ʈ üũ.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ���� ������ �� �ޱ�.
	int nMaxRow = 0 ;
	nMaxRow = m_plGuest->GetCount() ;

	// �ֻ��� �ε��� �ޱ�.
	LONG lTopIdx = 0 ;
	//lTopIdx = m_pGuestList->GetStartIdxOfPrintedItems() ;
	lTopIdx = m_pGuestNicknameList->GetTopListItemIdx() ;

	// �ε��� ��ȿ üũ.
	ASSERT(lTopIdx >= 0) ;

	if(lTopIdx < 0)
	{
		CHATROOMMGR->Throw_Error("Invalid Index!!", __FUNCTION__) ;
		SetActive(FALSE) ;
		return ;
	}

	//// Ŭ���� ��ũ �ʱ�ȭ.
	//for(BYTE count = 0 ; count < MAX_ROW_GUESTLIST ; ++count)
	//{
	//	ZeroMemory(&m_MarkImg[count], sizeof(cImage)) ;
	//}

	//m_pGuestList->DeleteAllItems() ;
	m_pGuestNicknameList->RemoveAll() ;
	m_pGuestStateList->RemoveAll() ;
	m_pGuestLevelList->RemoveAll() ;


	//char Level[4] ;
	//char State[5] ;

	// 080704 LYW --- ChatRoomGuestListDlg : �ӽ� ���� ������ Ȯ��.
	//char tempBuff[128] = {0,} ;
	//char szState[24] = {0, } ;

	char tempBuff[256] = {0,} ;
	char szState[256] = {0, } ;

	DWORD dwColor = 0 ;
	dwColor = RGBA_MAKE(255, 255, 255, 255) ;

	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	BYTE byCount = 0 ;
	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		memset(&tempBuff, 0, sizeof(tempBuff)) ;
		memset(&szState, 0, sizeof(szState)) ;

		// ���� ����.
		if(CHATROOMMGR->GetMyRoomOwnerIdx() == pInfo->dwPlayerID)
		{
			if(CHATROOMMGR->IsFreezed(pInfo->dwPlayerID))
			{
				strcat(szState, CHATMGR->GetChatMsg(1484)) ;
				strcat(szState, "/") ;
				strcat(szState, CHATMGR->GetChatMsg(1485)) ;
			}
			else
			{
				SafeStrCpy(szState, CHATMGR->GetChatMsg(1484), 5) ;
			}
		}
		else if(CHATROOMMGR->IsFreezed(pInfo->dwPlayerID))
		{
			SafeStrCpy(szState, CHATMGR->GetChatMsg(1485), 5) ;
		}
		else
		{
			SafeStrCpy(szState, CHATMGR->GetChatMsg(1486), 5) ;
		}

		// 100107 ONS ��Ȯ������� �г���, ����, ������ ������ ����Ʈ�� ����
		// �̸� ����.
		sprintf(tempBuff, "%-28s", pInfo->name) ;
		m_pGuestNicknameList->AddItem(tempBuff, dwColor, byCount) ;	

		// ���� ����
		memset(&tempBuff, 0, sizeof(tempBuff)) ;
		sprintf(tempBuff, "%-8s", szState) ;
		m_pGuestStateList->AddItem(tempBuff, dwColor, byCount) ;	

		// ���� ����
		memset(&tempBuff, 0, sizeof(tempBuff)) ;
		sprintf(tempBuff, "%5d", pInfo->byLevel) ;
		m_pGuestLevelList->AddItem(tempBuff, dwColor, byCount) ;	

		// 100427 ONS ��ȭ����� ����Ʈ�� ���ŵɶ� ���õ� ����� ������ �����Ѵ�.
		ITEM* pNickItem		= (ITEM*)m_pGuestNicknameList->GetItem((LONG)byCount);
		ITEM* pStateItem	= (ITEM*)m_pGuestStateList->GetItem((LONG)byCount);
		ITEM* pLevelItem	= (ITEM*)m_pGuestLevelList->GetItem((LONG)byCount);

		if(!pNickItem || !pStateItem || !pLevelItem)
		{
			++byCount;
			continue;
		}

		if( m_SelectedUser.dwPlayerID == pInfo->dwPlayerID )
		{
			dwColor = RGBA_MAKE(255, 0, 0, 255) ;
		}
		else
		{
			dwColor = RGBA_MAKE(255, 255, 255, 255) ;
		}

		pNickItem->rgb  = dwColor;
		pStateItem->rgb = dwColor;
		pLevelItem->rgb = dwColor;

		++byCount ;

	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDlgToDefault
//	DESC : ������ ����Ʈ�� �⺻ ���·� �����ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::SetDlgToDefault()
{
	// ������ ����Ʈ üũ.
	ASSERT(m_plGuest) ;

	if(!m_plGuest)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ����Ʈ ��Ʈ�� ����.
	//m_pGuestList->DeleteAllItems() ;
	m_pGuestNicknameList->RemoveAll() ;
	m_pGuestStateList->RemoveAll() ;
	m_pGuestLevelList->RemoveAll() ;

	// ����Ʈ ����.
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_plGuest->GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_plGuest->GetNext(pos) ;

		if(!pInfo) continue ;

		m_plGuest->Remove(pInfo) ;

		delete pInfo ;
	}
	m_plGuest->RemoveAll() ;

	//// Ŭ���� ��ũ �ʱ�ȭ.
	//for(BYTE count = 0 ; count < MAX_ROW_GUESTLIST ; ++count)
	//{
	//	ZeroMemory(&m_MarkImg[count], sizeof(cImage)) ;
	//}



	// ���� �� ���� ���� �ʱ�ȭ.
	memset(&m_SelectedUser, 0, sizeof(ST_CR_USER)) ;

	// �� ���� �ʱ�ȭ.
	//memset(&m_MyUserInfo, 0, sizeof(ST_CR_USER)) ;



	// ������ ������ ����Ʈ ���̾�α׿� ����
	SetClassMark() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ChangeOwner
//	DESC : �������� ��ư�� ������ �� ó���� �ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::ChangeOwner()
{
	// �������� üũ.
	if(HEROID != CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
		return ;
	}

	// ����� ���̵� üũ.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// ����� ��ȿ�� üũ.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// ���� �ð� üũ.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// ���� ���� ��û.
	MSG_DWORD msg ;
	memset(&msg, 0, sizeof(MSG_DWORD)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_CHANGE_OWNER_SYN ;
	msg.dwObjectID	= HEROID ;
	
	msg.dwData		= m_SelectedUser.dwPlayerID ;

	NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;



	// �ð� ������Ʈ.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : FreezeGuest
//	DESC : ���� ��ư�� ������ �� ó���� �ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::FreezeGuest()
{
	// ����� ���̵� üũ.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// ����� ��ȿ�� üũ.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// ����� ���¸� �����Ѵ�.
	if(CHATROOMMGR->IsFreezed(m_SelectedUser.dwPlayerID))
	{
		CHATROOMMGR->UnFreezeUser(m_SelectedUser.dwPlayerID) ;
	}
	else if(!CHATROOMMGR->IsFreezed(m_SelectedUser.dwPlayerID))
	{
		CHATROOMMGR->FreezeUser(m_SelectedUser.dwPlayerID) ;
	}
	else return ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : KickGuest
//	DESC : �������� ��ư�� ������ �� ó���� �ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::KickGuest()
{
	// �������� üũ.
	if(HEROID != CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
		return ;
	}

	// ����� ���̵� üũ.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// ����� ��ȿ�� üũ.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// ���� �ð� üũ.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// ���� ���� ��û.
	MSG_DWORD msg ;
	memset(&msg, 0, sizeof(MSG_DWORD)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_KICK_GUEST_SYN ;
	msg.dwObjectID	= HEROID ;
	
	msg.dwData		= m_SelectedUser.dwPlayerID ;

	NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;



	// �ð� ������Ʈ.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddFriend
//	DESC : ģ���߰� ��ư�� ������ �� ó���� �ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomGuestListDlg::AddFriend()
{
	// ����� ���̵� üũ.
	if(m_SelectedUser.dwPlayerID == HEROID)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1488), e_ChatMsg_System) ;
		return ;
	}

	// ����� ��ȿ�� üũ.
	if(m_SelectedUser.dwPlayerID == 0)
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1489), e_ChatMsg_System) ;
		return ;
	}

	// ���� �ð� üũ.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// ���� ���� ��û.
	MSG_DWORD msg ;
	memset(&msg, 0, sizeof(MSG_DWORD)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_REQUEST_FRIEND_SYN ;
	msg.dwObjectID	= HEROID ;
	
	msg.dwData		= m_SelectedUser.dwPlayerID ;

	NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;

	CHATROOMMGR->SetRequestFriendState(TRUE) ;



	// �ð� ������Ʈ.
	CHATROOMMGR->UpdateMyLastComTime() ;
}
