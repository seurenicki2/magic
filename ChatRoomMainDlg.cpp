#include "StdAfx.h"
#include "../WindowIDEnum.h"

#include "./Input/Mouse.h"
#include "./Input/Keyboard.h"

#include "./FilteringTable.h"

#include "./Interface/cResourceManager.h"
#include "./Interface/cWindowManager.h"

#include "./Interface/cButton.h"
#include "./Interface/cComboBox.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cListDialog.h"
#include "./Interface/cStatic.h"

#include "./cMsgBox.h"
#include "./GameIn.h"

#include "./ObjectManager.h"
#include "./ChatManager.h"
#include "./ChatRoomMgr.h"

#include "./ChatRoomCreateDlg.h"
#include "./ChatRoomDlg.h"
#include "./ChatRoomGuestListDlg.h"
#include "./ChatRoomJoinDlg.h"
#include "./ChatRoomMainDlg.h"
#include "./ChatRoomOptionDlg.h"

CChatRoomMainDlg::CChatRoomMainDlg(void)
{
	// �ʱ�ȭ ó��.
	m_pRoomList			= NULL ;

	m_pSearchBtn		= NULL ;
	m_pSearchBox		= NULL ;
	m_pTitle			= NULL ;
	m_pName				= NULL ;

	m_pRoomType			= NULL ;

	m_pCreateBtn		= NULL ;
	m_pJoinBtn			= NULL ;
	m_pNumJoinBtn		= NULL ;
	m_pRefreshBtn		= NULL ;

	m_pPrevBtn			= NULL ;
	m_pNextBtn			= NULL ;

	m_pPage				= NULL ;

	//m_pLoad_Chatroom	= NULL ;

	//memset(m_SearchBuff, 0, sizeof(m_SearchBuff)) ;

	m_bySearchMode		= e_Search_Title ;

	//m_byRoomType		= e_RTM_Party ;

	m_byCurPage			= 0 ;
	m_byMaxPage			= 0 ;

	memset(&m_SelectRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	m_byRoomType		= e_RTM_AllLooK ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomMainDlg
//	DESC : �Ҹ��� �Լ�.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMainDlg::~CChatRoomMainDlg(void)
{
}


void CChatRoomMainDlg::Linking()
{
	// ���� ��Ʈ�� ������ ��ũ.
	m_pRoomList			= (cListDialog*)GetWindowForID(CRMD_LST_ROOMLIST) ;

	m_pSearchBtn		= (cButton*)GetWindowForID(CRMD_BTN_SEARCH) ;
	m_pSearchBox		= (cEditBox*)GetWindowForID(CRMD_EDB_SEARCH) ;
	m_pTitle			= (cPushupButton*)GetWindowForID(CRMD_CHK_TITLE) ;
	m_pName				= (cPushupButton*)GetWindowForID(CRMD_CHK_NAME) ;

	m_pRoomType			= (cComboBox*)GetWindowForID(CRMD_CMB_ROOMTYPE) ;

	m_pCreateBtn		= (cButton*)GetWindowForID(CRMD_BTN_CREATE) ;
	m_pJoinBtn			= (cButton*)GetWindowForID(CRMD_BTN_JOIN) ;
	m_pNumJoinBtn		= (cButton*)GetWindowForID(CRMD_BTN_NUMJOIN) ;
	m_pRefreshBtn		= (cButton*)GetWindowForID(CRMD_BTN_REFRESH) ;

	m_pPrevBtn			= (cButton*)GetWindowForID(CRMD_BTN_PREV) ;
	m_pNextBtn			= (cButton*)GetWindowForID(CRMD_BTN_NEXT) ;

	m_pPage				= (cStatic*)GetWindowForID(CRMD_STC_PAGE) ;

	if( !m_pRoomList	|| !m_pSearchBtn	|| !m_pSearchBox || !m_pTitle || !m_pName ||
		!m_pRoomType	|| !m_pCreateBtn	|| !m_pJoinBtn  ||
		!m_pNumJoinBtn	|| !m_pRefreshBtn	|| //!m_pLoad_Chatroom ||
		!m_pPrevBtn		|| !m_pNextBtn		|| !m_pPage )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ä�ù� �з� ����.
	//m_pRoomType->SetCurSelectedIdx(e_RTM_AllLooK) ;
	m_pRoomType->SetMaxLine(e_RT_Max) ;



	// �⺻ �˻� ��� ����.
	if(m_pTitle)		m_pTitle->SetPush(TRUE) ;
	if(m_pName)			m_pName->SetPush(FALSE) ;

	// �ε� ����ƽ ��Ȱ��ȭ.
	//m_pLoad_Chatroom->SetActive(FALSE) ;



	// ������ ����.
	//SetPage(1, 1) ;
	m_pPage->SetStaticText("") ;



	// ä�ù� �Ŵ����� ���.
	CHATROOMMGR->SetMainDlg(this) ;
}

DWORD CChatRoomMainDlg::ActionEvent( CMouse* mouseInfo ) 
{
	DWORD we = WE_NULL ;

	// �Լ� �Ķ���� üũ.
	ASSERT(mouseInfo) ;

	if( !mouseInfo )
	{
		CHATROOMMGR->Throw_Error("Invalid a mouse info!!", __FUNCTION__) ;
		return we ;
	}



	// ���̾�α� �̺�Ʈ�� �޴´�.
	we = cDialog::ActionEvent( mouseInfo ) ;



	// ���� ����Ʈ �� ���콺�� ������,
	if(m_pRoomList->PtInWindow( (LONG)mouseInfo->GetMouseX(), (LONG)mouseInfo->GetMouseY() ))
	{
		// Ŭ�� �̺�Ʈ�� �߻����� ���.
		if( we & WE_LBTNCLICK )
		{
			ITEM* pItem ;

			DWORD dwColor ;

			LONG lItemCount = 0 ;
			lItemCount = m_pRoomList->GetItemCount() ;

			int nSelectedIdx = 0 ;
			nSelectedIdx  = m_pRoomList->GetSelectRowIdx() ;
			
			for(LONG count = 0 ; count < lItemCount ; ++count)
			{
				pItem = NULL ;
				pItem = m_pRoomList->GetItem(count) ;

				if(!pItem) continue ;

				if(pItem->line == nSelectedIdx)
				{
					dwColor = RGBA_MAKE(255, 0, 0, 255) ;
				}
				else
				{
					dwColor = RGBA_MAKE(255, 255, 255, 255) ;
				}

				pItem->rgb = dwColor ;
			}

			LIST_ROOM::iterator it ;
			it = m_Room.begin() ;

			std::advance( it, nSelectedIdx ) ;

			if(it == m_Room.end())
			{
				memset(&m_SelectRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;
			}
			else
			{
				m_SelectRoom = *it ;
			}
		}
		else if( we & WE_LBTNDBLCLICK )
		{
			Join_Syn() ;
		}
	}

	return we ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : ������Ʈ�ѿ� �̺�Ʈ�� �߻����� �� ó���ϴ� �Լ�.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(p) ;

	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// ��Ʈ�� ���̵� Ȯ��.
	switch(id)
	{
	case CRMD_BTN_SEARCH :		Search_Syn() ;			break ;

	case CRMD_CHK_TITLE :		Check_Title() ;			break ;
	case CRMD_CHK_NAME :		Check_Name() ;			break ;

	case CRMD_BTN_CREATE :		Create_Syn() ;			break ;
	case CRMD_BTN_JOIN :		Join_Syn() ;			break ;
	case CRMD_BTN_NUMJOIN :		NumJoin_Syn() ;			break ;
	case CRMD_BTN_REFRESH :		Refresh_Syn() ;			break ;

	case CRMD_BTN_PREV :		Btn_Prev() ;			break ;
	case CRMD_BTN_NEXT :		Btn_Next() ;			break ;

	case CRMD_CMB_ROOMTYPE :	
	{
		if( event == WE_COMBOBOXSELECT )
		{
			//SortRoomByType() ;

			// ���� �ð� üũ.
			if(!CHATROOMMGR->IsEnableExecuteCom())
			{
				WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
				return ;
			}

			m_byRoomType = (BYTE)m_pRoomType->GetCurSelectedIdx() ;

			MSG_BYTE2 msg ;
			memset(&msg, 0, sizeof(MSG_BYTE2)) ;

			msg.Category		= MP_CHATROOM ;
			msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
			msg.dwObjectID		= HEROID ;

			msg.bData1			= m_byRoomType ;
			msg.bData2			= m_byCurPage ;

			NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;
			CHATROOMMGR->UpdateMyLastComTime() ;
		}
	}
	break ;

	default : break ;
	}
}

void CChatRoomMainDlg::Search_Syn()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pSearchBox) ;

	if(!m_pSearchBox)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// �˻� ���� üũ.
	char* word ;

	word = NULL ;
	word = m_pSearchBox->GetEditText() ;

	m_pSearchBox->SetEditText("") ;

	if(strlen(word) <= 1)
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1491)) ;
		return ;
	}

	// ���͸� üũ.
	if( FILTERTABLE->FilterChat(word) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		return ;
	}

	// �˻� ��� üũ.
	if(m_bySearchMode == e_Search_Title)
	{
		MSG_CR_SEARCH_TITLE msg ;
		memset(&msg, 0, sizeof(MSG_CR_SEARCH_TITLE)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_SEARCH_FOR_TITLE_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= 0 ;
		
		SafeStrCpy(msg.title, word, TITLE_SIZE) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_SEARCH_TITLE) ) ;
	}
	else
	{
		if( strcmp(word, HERO->GetObjectName()) == 0)
		{
			WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1538)) ;
			return ;
		}

		MSG_CR_SEARCH_NAME msg ;
		memset(&msg, 0, sizeof(MSG_CR_SEARCH_NAME)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_SEARCH_FOR_NAME_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= 0 ;
		
		SafeStrCpy(msg.name, word, MAX_NAME_LENGTH + 1) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_SEARCH_NAME) ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Check_Title
//	DESC : �˻� ��带 ������ �˻����� �����Ѵ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Check_Title() 
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pTitle) ;
	ASSERT(m_pName) ;

 	if(!m_pTitle || !m_pName) 
 	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
 	}

	m_pTitle->SetPush(TRUE) ;
 	m_pName->SetPush(FALSE) ;

 	m_bySearchMode = e_Search_Title ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Check_Name
//	DESC : �˻���带 �̸� �˻����� �����Ѵ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Check_Name()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pTitle) ;
	ASSERT(m_pName) ;

 	if(!m_pTitle || !m_pName) 
 	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
 	}

	m_pName->SetPush(TRUE) ;
 	m_pTitle->SetPush(FALSE) ;

 	m_bySearchMode = e_Search_Name ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Create_Syn
//	DESC : ä�ù� ���� ���̾�α׸� ����.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Create_Syn()
{
	if(CHATROOMMGR->Get_ChatRoomState() == e_State_In_ChatRoom) 
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1505)) ;
		return ;
	}

	// ä�ù� ���� â �ޱ�.
	CChatRoomCreateDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomCreateDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive create dialog!!", __FUNCTION__) ;
		return ;
	}

	pDlg->SetActive(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Join_Syn
//	DESC : ������ �濡 �����ϱ⸦ ��û�ϴ� �Լ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Join_Syn()
{
	if(CHATROOMMGR->Get_ChatRoomState() == e_State_In_ChatRoom) 
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1505)) ;
		return ;
	}
	
	// ������ ä�ù� üũ.
	if(m_SelectRoom.byIdx == 0)
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1516)) ;
		return ;
	}

	//MSG_CR_JOIN_SYN msg ;
	//memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

	//msg.Category	= MP_CHATROOM ;
	//msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
	//msg.dwObjectID	= HEROID ;

	//msg.byRoomIdx	= m_SelectRoom.byIdx ;

	//NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

	// ���������,
	if(m_SelectRoom.bySecretMode == e_RM_Open)
	{
		// ���� ��û�� �Ѵ�.
		MSG_CR_JOIN_SYN msg ;
		memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= m_SelectRoom.byIdx ;

		NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;
	}
	// ����� �����,
	else if(m_SelectRoom.bySecretMode == e_RM_Close)
	{
		// ���� â�� �޴´�.
		CChatRoomJoinDlg* pDlg = NULL ;
		pDlg = GAMEIN->GetChatRoomJoinDlg() ;

		if(!pDlg)
		{
			CHATROOMMGR->Throw_Error("Failed to receive join dialog!!", __FUNCTION__) ;
			return ;
		}

		// ��й�ȣ �Է� ���� Ȱ��ȭ ��Ų��.
		pDlg->SetInputMode(e_JoinDlg_Type_SecretCode) ;
		pDlg->SetActive(TRUE) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : NumJoin_Syn
//	DESC : ��ȣ �Է����� �����ϱ⸦ ������ �� ó���� �ϴ� �Լ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::NumJoin_Syn() 
{
	if(CHATROOMMGR->Get_ChatRoomState() == e_State_In_ChatRoom) 
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1505)) ;
		return ;
	}

	// ���� â �ޱ�.
	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive join dialog!!", __FUNCTION__) ;	
		return ;
	}

	// ���ȣ �Է� ���� ����â ����.
	pDlg->SetInputMode(e_JoinDlg_Type_RoonIndex) ;
	pDlg->SetActive(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Refresh_Syn
//	DESC : ���ΰ�ħ ��û�� ó���ϴ� �Լ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Refresh_Syn()
{
	//// ���� ��Ʈ�� üũ.
	//ASSERT(m_pLoad_Chatroom) ;

 //	if(!m_pLoad_Chatroom) 
 //	{
	//	CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
	//	return ;
 //	}

	//m_pLoad_Chatroom->SetActive(TRUE) ;

	// ���� �ð� üũ.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}

	MSG_BYTE2 msg ;
	memset(&msg, 0, sizeof(MSG_BYTE2)) ;

	msg.Category		= MP_CHATROOM ;
	msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
	msg.dwObjectID		= HEROID ;

	if(m_byRoomType == e_RTM_Searched)
	{
		msg.bData1			= e_RTM_AllLooK ;
		msg.bData2			= 0 ;
	}
	else
	{
		msg.bData1			= m_byRoomType ;
		msg.bData2			= m_byCurPage ;
	}

	NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;

	// �ð� ������Ʈ.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Prev
//	DESC : ���� ������ ��û�� �ϴ� �Լ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Btn_Prev() 
{
	// ������ Ȯ��.
	if(m_byCurPage == 0) return ;
	else
	{
		//// ���� ��Ʈ�� üũ.
		//ASSERT(m_pLoad_Chatroom) ;

 	//	if(!m_pLoad_Chatroom) 
 	//	{
		//	CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		//	return ;
 	//	}

		//m_pLoad_Chatroom->SetActive(TRUE) ;

		// ���� �ð� üũ.
		if(!CHATROOMMGR->IsEnableExecuteCom())
		{
			WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
			return ;
		}

		if( m_byCurPage == 0 ) return ;

		MSG_BYTE2 msg ;
		memset(&msg, 0, sizeof(MSG_BYTE2)) ;

		msg.Category		= MP_CHATROOM ;
		msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
		msg.dwObjectID		= HEROID ;

		msg.bData1			= m_byRoomType ;
		msg.bData2			= m_byCurPage-1 ;

		NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;
		CHATROOMMGR->UpdateMyLastComTime() ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Next
//	DESC : ���� ������ ��û�� �ϴ� �Լ�.
//  DATE : APRIL 8, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Btn_Next()
{
	// ������ Ȯ��.
	if(m_byCurPage == m_byMaxPage) return ;
	else
	{
		//// ���� ��Ʈ�� üũ.
		//ASSERT(m_pLoad_Chatroom) ;

 	//	if(!m_pLoad_Chatroom) 
 	//	{
		//	CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		//	return ;
 	//	}

		//m_pLoad_Chatroom->SetActive(TRUE) ;

		// ���� �ð� üũ.
		if(!CHATROOMMGR->IsEnableExecuteCom())
		{
			WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
			return ;
		}

		if(m_byCurPage >= m_byMaxPage) return ;

		MSG_BYTE2 msg ;
		memset(&msg, 0, sizeof(MSG_BYTE2)) ;

		msg.Category		= MP_CHATROOM ;
		msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
		msg.dwObjectID		= HEROID ;

		msg.bData1			= m_byRoomType ;
		msg.bData2			= m_byCurPage+1 ;

		NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;
		CHATROOMMGR->UpdateMyLastComTime() ;
	}
}

void CChatRoomMainDlg::Check_Room_To_Join(BYTE inputMode, char* pCode)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pCode) ;

	if(!pCode || strlen(pCode) == 0)
	{
		//CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1510)) ;
		return ;
	}

	// ����â �ޱ�.
	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive join dialog!!", __FUNCTION__) ;
		return ;
	}

	// ä�ù��� ��� Map�� ���� �ޱ�.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// ���� ������ �޽��� �ڽ� ���.
	if(it == m_Room.end())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1490)) ;

		m_pRoomList->RemoveAll() ;

		m_Room.clear() ;
		m_TempRoom.clear() ;

		return ;
	}

	// ���ȣ �Է� ����, 
	if(inputMode == e_JoinDlg_Type_RoonIndex)
	{
		BYTE byCheckCount = BYTE(m_Room.size());

		for(BYTE count = 0 ; count < byCheckCount ; ++count)
		{
			if(it->byIdx != (BYTE)(atoi(pCode)))
			{
				++it ;
				continue ;
			}

			// ������ �� ����.
			m_SelectRoom = *it ;

			// ����/����� Ȯ��.
			if(it->bySecretMode == e_RM_Close)
			{
				// ��й�ȣ â�� Ȱ��ȭ �Ѵ�.
				pDlg->SetInputMode(e_JoinDlg_Type_SecretCode) ;
				pDlg->SetActive(TRUE) ;
				return ;
			}
		}

		MSG_CR_JOIN_SYN msg ;
		memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= (BYTE)atoi(pCode) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

		pDlg->SetActive(FALSE) ;
	}
	// ��й�ȣ �Է� ����,
	else if(inputMode == e_JoinDlg_Type_SecretCode)
	{
		// ��й�ȣ�� ��� ���� ��û�� �Ѵ�.
		MSG_CR_JOIN_SYN msg ;
		memset(&msg, 0, sizeof(MSG_CR_JOIN_SYN)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_JOIN_ROOM_SYN ;
		msg.dwObjectID	= HEROID ;

		msg.byRoomIdx	= m_SelectRoom.byIdx ;
		SafeStrCpy(msg.code, pCode, SECRET_CODE_SIZE) ;

		NETWORK->Send( &msg, sizeof(MSG_CR_JOIN_SYN) ) ;

		pDlg->SetActive(FALSE) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Init_RoomList
//		 : 
//	DESC : ������ ���� �Ѿ� ��, ä�ù� ������ �����ϴ� �Լ�.
//		 : 
//		 : 1. ä�ù��� ��� �ִ� Map�� ����.
//		 : 2. �������� �޴´�.
//		 : 3. ������ ����.
//		 : 4. �ε� �ؽ�Ʈ ��Ȱ��ȭ.
//		 : 5. ä�ù� ����Ʈ ���̾�α� ���ΰ�ħ.
//		 : 
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Init_RoomList(void* pMsg)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// ���� �޽��� ��ȯ
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		CHATROOMMGR->Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// ���� ��Ʈ�� üũ.
	ASSERT(m_pRoomList) ;
	//ASSERT(m_pLoad_Chatroom) ;

	if(!m_pRoomList /*|| !m_pLoad_Chatroom*/)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ä�ù� Ÿ�� ����.
	m_pRoomType->SetCurSelectedIdx(pmsg->byRoomType) ;
	m_pRoomType->SelectComboText(pmsg->byRoomType) ;
	m_byRoomType = pmsg->byRoomType ;

	// ä�ù� ����Ʈ ���.
	m_Room.clear() ;

	// ä�ù� ���� ��ŭ ����.
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		if(pmsg->room[count].byIdx == 0) continue ;

		const ST_CR_ROOM_CLT& room = pmsg->room[ count ] ;

		m_Room.push_back(room) ;
	}

	SetPage(pmsg->byCurPage, pmsg->byTotalPage);
	Refresh_RoomList();
}


void CChatRoomMainDlg::Refresh_RoomList()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pRoomList) ;

	if(!m_pRoomList)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// Ȱ��ȭ ���� üũ.
	if(!IsActive()) return ;



	// ����Ʈ ���̾�α� ����.
	m_pRoomList->RemoveAll() ;

	// �� ��ġ �ޱ�.
	LIST_ROOM::iterator it = m_Room.begin();

	if(it == m_Room.end()) return;

	char tempBuf[256] = {0,};
	char tempNum[256] = {0,};
	BYTE byCount = BYTE(m_Room.size());
	BYTE byStartCount = 0 ;
	byStartCount = m_byCurPage*ROOM_COUNT_PER_PAGE ;



	// �� ���� ��ŭ ����.
	for(BYTE count = 0 ; count < byCount ; ++count)
	{
		if(it == m_Room.end()) return ;

		memset(tempBuf, 0, sizeof(tempBuf)) ;
		memset(tempNum, 0, sizeof(tempNum)) ;

		// �ε��� ����.
		if( it->byIdx / 100 <= 0 )
		{
			if( it->byIdx / 10 <= 0 )
			{
				strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
				strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
			}
			else
			{
				strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
			}
		}

		strcat(tempBuf, itoa(it->byIdx, tempNum, 10)) ;
		strcat(tempBuf, ".") ;

		// ����/����� ����.
		strcat(tempBuf, "[") ;
		if( it->bySecretMode == e_RM_Open )
		{
			strcat(tempBuf, RESRCMGR->GetMsg(892)) ;
		}
		else
		{
			strcat(tempBuf, RESRCMGR->GetMsg(893)) ;
		}
		strcat(tempBuf, "]") ;

		// ä�ù� �з� ����.
		strcat(tempBuf, "[") ;
		switch(it->byRoomType)
		{
		case e_RT_Party :			strcat(tempBuf, RESRCMGR->GetMsg(880)) ;	break ;
		case e_RT_Deal :			strcat(tempBuf, RESRCMGR->GetMsg(881)) ;	break ;
		case e_RT_Normal_Chat :		strcat(tempBuf, RESRCMGR->GetMsg(882)) ;	break ;
		case e_RT_Local_Chat :		strcat(tempBuf, RESRCMGR->GetMsg(883)) ;	break ;
		case e_RT_Make_Friend :		strcat(tempBuf, RESRCMGR->GetMsg(884)) ;	break ;
		default : break ;
		}
		strcat(tempBuf, "]") ;

		// ä�ù� ���� ����.
		strcat(tempBuf, it->title) ;

		// ���� ������ ����.
		strcat(tempBuf, "(") ;

		if(it->byCurGuestCount % 10 < 0)
		{
			strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
		}

		memset(tempNum, 0, sizeof(tempNum)) ;
		strcat(tempBuf, itoa(it->byCurGuestCount, tempNum, 10)) ;

		strcat(tempBuf, "/") ;

		if(it->byCurGuestCount % 10 < 0)
		{
			strcat(tempBuf, RESRCMGR->GetMsg(10)) ;
		}

		// �ִ� ������ ����.
		memset(tempNum, 0, sizeof(tempNum)) ;

		switch(it->byTotalGuestCount)
		{
		case e_GC_20 :	strcat(tempBuf, RESRCMGR->GetMsg(922)) ;	break ;
		case e_GC_15 :	strcat(tempBuf, RESRCMGR->GetMsg(921)) ;	break ;
		case e_GC_10 :	strcat(tempBuf, RESRCMGR->GetMsg(920)) ;	break ;
		case e_GC_5 :		strcat(tempBuf, RESRCMGR->GetMsg(919)) ;	break ;
		default : break ;
		}
		strcat(tempBuf, ")") ;

		// ����Ʈ ���̾�α׿� �߰�.
		m_pRoomList->AddItem( tempBuf, RGBA_MAKE(255, 255, 255, 255), count ) ;

		++it ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Clear_RoomList
//	DESC : ä�ù� ����Ʈ�� ��� ���� �Լ�.
//  DATE : APRIL 11, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Clear_RoomList()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pRoomList) ;

	if(!m_pRoomList)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// ä�ù� ����Ʈ ���̾�α� ����.
	m_pRoomList->RemoveAll() ;

	// ä�ù� ����Ʈ ����.
	m_Room.clear() ;

	// ���� �� �� ���� �����.
	memset(&m_SelectRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;
}

BOOL CChatRoomMainDlg::Add_Room(ST_CR_ROOM_CLT* pInfo)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pInfo) ;

	if(!pInfo)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return FALSE ;
	}

	const ST_CR_ROOM_CLT& addInfo = *pInfo ;
	m_Room.push_back(addInfo) ;

	Refresh_RoomList() ;

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Update_RoomInfo
//	DESC : �� ���� �ϳ��� ������Ʈ �� �� ó���ϴ� �Լ�.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::Update_RoomInfo(ST_CR_ROOM_CLT* pInfo)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pInfo) ;

	if(!pInfo)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// �渮��Ʈ�� ������ �̴������� �ޱ�.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// ����Ʈ ������ �ޱ�.
	if(it == m_Room.end()) return ;

	BYTE bySize = BYTE(m_Room.size());

	// ������Ʈ �� �� ã��.
	for(BYTE count = 0 ; count < bySize ; ++count)
	{
		if(it->byIdx != pInfo->byIdx) continue ;

		// ������ ������Ʈ.
		it->byIdx				= pInfo->byIdx ;
		it->bySecretMode		= pInfo->bySecretMode ;
		it->byRoomType			= pInfo->byRoomType ;
		it->byCurGuestCount		= pInfo->byCurGuestCount ;
		it->byTotalGuestCount	= pInfo->byTotalGuestCount ;

		SafeStrCpy(it->title, pInfo->title, TITLE_SIZE) ;

		break ;
	}

	// ä�ù� ����Ʈ ������Ʈ.
	Refresh_RoomList() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : DeleteRoom
//	DESC : ���ڷ� �Ѿ�� ���� ã�Ƽ�, ����Ʈ���� �����Ѵ�.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::DeleteRoom(BYTE byRoomIdx)
{
	// �渮��Ʈ�� ������ �̴������� �ޱ�.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// ����Ʈ ������ �ޱ�.
	if(it == m_Room.end()) return ;

	BYTE bySize = BYTE(m_Room.size());

	// ������Ʈ �� �� ã��.
	for(BYTE count = 0 ; count < bySize ; ++count)
	{
		if(it->byIdx != byRoomIdx) continue ;

		m_Room.erase(it) ;
		break ;
	}

	// ä�ù� ����Ʈ ������Ʈ.
	Refresh_RoomList() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetPage
//	DESC : ä�ù� ����Ʈ�� �������� �����Ѵ�.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMainDlg::SetPage(BYTE num1, BYTE num2)
{
	// ä�ù� ����Ʈ ������ ����.
	// 080704 LYW --- CharRoomMainDlg : �ӽ� ���� ������ Ȯ��.
	//char tempBuff[10] = {0,} ;
	//char tempNum[10]  = {0,} ;

	char tempBuff[256] = {0,} ;
	char tempNum[256]  = {0,} ;

	sprintf(tempBuff, "%d", num1+1) ;
	strcat(tempBuff, "/") ;
	//strcat(tempBuff, itoa(num2+1, tempNum, 10)) ;
	strcat(tempBuff, itoa(num2+1, tempNum, 256)) ;

	m_pPage->SetStaticText(tempBuff) ;

	m_byCurPage = num1 ;
	m_byMaxPage = num2 ;
}




//-------------------------------------------------------------------------------------------------
//	NAME : GetRoomListItem
//	DESC : ���ڷ� �Ѿ�� ��ȣ�� ���� ã�Ƽ� ��ȯ�Ѵ�.
//  DATE : APRIL 9, 2008 LYW
//-------------------------------------------------------------------------------------------------

ST_CR_ROOM_CLT* CChatRoomMainDlg::GetRoomListItem(BYTE byRoomIdx)
{
	// �渮��Ʈ�� ������ �̴������� �ޱ�.
	LIST_ROOM::iterator it ;
	it = m_Room.begin() ;

	// ����Ʈ ������ �ޱ�.
	if(it == m_Room.end()) return NULL ;

	BYTE bySize = BYTE(m_Room.size());

	// ������Ʈ �� �� ã��.
	for(BYTE count = 0 ; count < bySize ; ++count)
	{
		if(it->byIdx != byRoomIdx)
		{
			++it ;
			continue ;
		}

		ST_CR_ROOM_CLT& room = *it ;

		return &room ;
	}

	return NULL ;
}





void CChatRoomMainDlg::Err_EmptyRoom()
{
	ASSERT(m_pRoomList) ;
	//ASSERT(m_pLoad_Chatroom) ;

	if(!m_pRoomList /*|| !m_pLoad_Chatroom*/)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	if(!IsActive())
	{
		SetActive(TRUE) ;
	}

	WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1490)) ;

	m_pRoomList->RemoveAll() ;

	m_Room.clear() ;
	m_TempRoom.clear() ;

	/*if(m_pLoad_Chatroom->IsActive())
	{
		m_pLoad_Chatroom->SetActive(FALSE) ;
	}*/
}















