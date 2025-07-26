#include "stdafx.h"
#include "../WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "../Interface/cResourceManager.h"
#include "./GameIn.h"
#include "./ChatManager.h"
#include "./ChatRoomMgr.h"
#include "./ChatRoomDlg.h"
#include "./ChatRoomGuestListDlg.h"
#include "./ChatRoomOptionDlg.h"
#include "./Input/Mouse.h"
#include "./Input/Keyboard.h"
#include "./Interface/cButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cListDialog.h"
#include "./Interface/cStatic.h"
#include "./cMsgBox.h"
#include "Interface/cScriptManager.h"
#include "./FilteringTable.h"
#include "./ObjectManager.h"

CChatRoomDlg::CChatRoomDlg(void)
{
	// �ʱ�ȭ ó��.
	m_pRoomTitle		= NULL ;

	m_pChatList			= NULL ;

	m_pInputBox			= NULL ;

	m_pBtn_GuestList	= NULL ;
	m_pBtn_Option		= NULL ;

	m_pBtn_Close		= NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomDlg
//	DESC : �Ҹ��� �Լ�.
//-------------------------------------------------------------------------------------------------
CChatRoomDlg::~CChatRoomDlg(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : ���� ��Ʈ�� ��ũ �Լ�.
//-------------------------------------------------------------------------------------------------
void CChatRoomDlg::Linking()
{
	// ���� ��Ʈ�� ��ũ.
	m_pRoomTitle		= (cStatic*)GetWindowForID(CRD_STC_ROOMTITLE) ;

	m_pChatList			= (cListDialog*)GetWindowForID(CRD_LST_CHATLIST) ;

	m_pInputBox			= (cEditBox*)GetWindowForID(CRD_EDB_INPUTBOX) ;
	//m_pInputBox			= (cTextArea*)GetWindowForID(CRD_EDB_INPUTBOX) ;

	m_pBtn_GuestList	= (cButton*)GetWindowForID(CRD_BTN_GUESTLIST) ;
	m_pBtn_Option		= (cButton*)GetWindowForID(CRD_BTN_OPTION) ;

	m_pBtn_Close		= (cButton*)GetWindowForID(CRD_BTN_CLOSE_DLG) ;

	if(!m_pRoomTitle || !m_pChatList || !m_pInputBox || !m_pBtn_GuestList || !m_pBtn_Option || !m_pBtn_Close)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ä�ù� �Ŵ����� ���.
	CHATROOMMGR->SetChatRoomDlg(this) ;
}

void CChatRoomDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	if(!m_pBtn_GuestList || !m_pBtn_Option)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ��Ʈ�� ���̵� üũ.
	switch(id)
	{
	// ������ ��ư.
	case CRD_BTN_GUESTLIST : 
		{
			// ������ ���̾�α� �ޱ�.
			CChatRoomGuestListDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetChatRoomGuestListDlg() ;

			ASSERT(pDlg) ;

			if(!pDlg)
			{
				CHATROOMMGR->Throw_Error("Failed to receive gld!!", __FUNCTION__) ;
				return ;
			}

			// ������ ���̾�α� ����/�ݱ� ó��.
			pDlg->SetActive(!pDlg->IsActive()) ;
		}
		break ;

	// ä�� �ɼ� ��ư.
	case CRD_BTN_OPTION :
		{
			// �������� üũ.
			if(CHATROOMMGR->GetMyRoomOwnerIdx() != HEROID)
			{
				AddMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
				return ;
			}

			// �ɼ� ���̾�α� �ޱ�.
			CChatRoomOptionDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetChatRoomOptionDlg() ;

			ASSERT(pDlg) ;

			if(!pDlg)
			{
				CHATROOMMGR->Throw_Error("Failed to receive od!!", __FUNCTION__) ;
				return ;
			}

			// �ɼ�â ����/�ݱ� ó��.
			pDlg->SetActive(!pDlg->IsActive()) ;
		}
		break ;

	// ä�ù� �ݱ� ��ư.
	case CRD_BTN_CLOSE_DLG :
		{
			// ���� Ȯ�� �˸�â Ȱ��ȭ.
			WINDOWMGR->MsgBox( MBI_CR_ENDCHAT, MBT_YESNO, CHATMGR->GetChatMsg(1480) ) ;
		}
		break ;

	default : break ;
	}
}

void CChatRoomDlg::AddMsg(char* pMsg, BYTE byType)
{
	if(!pMsg) 
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	char buf[MAX_PATH] = {0};
	int nLen = strlen( pMsg );
	int nCpyNum = 0;
	int ntextLen = 54 ;

	while( nLen > ntextLen )
	{
		if( (pMsg + ntextLen ) != CharNext( CharPrev( pMsg, pMsg + ntextLen ) ) )
		{
			nCpyNum = ntextLen - 1 ;
		}
		else
		{
			nCpyNum = ntextLen ;
		}

		strncpy( buf, pMsg, nCpyNum ) ;
		buf[ nCpyNum ] = 0 ;

		m_pChatList->AddItem( buf, GetMsgColor(byType) ) ;

		nLen -= nCpyNum ;
		pMsg  += nCpyNum ;

		if( *pMsg == ' ' ) ++pMsg ;
	}

	if( nLen > 0 )
	{
		m_pChatList->AddItem( pMsg, GetMsgColor(byType) ) ;
	}

	//m_pChatList->AddItem(pMsg, GetMsgColor(byType)) ;
}

void CChatRoomDlg::AddCommonMsg(BYTE byType, char* pMsg)
{
	// �Լ� �Ķ���� üũ.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// �޽��� Ÿ�� Ȯ��.
	// 080704 LYW --- ChatRoomDlg : �ӽ� ���� ������ Ȯ��.
	//char szStr[128] = {0,} ;
	char szStr[256] = {0,} ;

	strcat(szStr, "[") ;
	strcat(szStr, "<") ;
	switch(byType)
	{
	// ä�ù� ����.
	case e_ChatMsg_StepIn :
		{
			strcat(szStr, pMsg) ;
			strcat(szStr, ">") ;
			strcat(szStr, CHATMGR->GetChatMsg(1481)) ;
		}
		break ;

	// ä�ù� ����.
	case e_ChatMsg_StepOut :
		{
			strcat(szStr, pMsg) ;
			strcat(szStr, ">") ;
			strcat(szStr, CHATMGR->GetChatMsg(1482)) ;
		}
		break ;

	default : break ;
	}
	strcat(szStr, "]") ;

	// �޽��� ���.
	m_pChatList->AddItem(szStr, GetMsgColor(byType)) ;
}

void CChatRoomDlg::SetRoomTitle(ST_CR_ROOM_CLT* pInfo)
{
	if(!pInfo)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	char tempMsg[256] = {0, } ;
	char tempRoomNum[256] = {0, } ;

	strcat(tempMsg, "[") ;
	sprintf(tempRoomNum, CHATMGR->GetChatMsg(1483), pInfo->byIdx) ;
	strcat(tempMsg, tempRoomNum) ;
	strcat(tempMsg, "]") ;
	strcat(tempMsg, "-") ;



	// ����/����� ����.
	strcat(tempMsg, "[") ;
	switch(pInfo->bySecretMode)
	{
	case e_RM_Open :	strcat(tempMsg, RESRCMGR->GetMsg(892)) ; break ;
	case e_RM_Close :	strcat(tempMsg, RESRCMGR->GetMsg(893)) ; break ;
	default : break ;
	}
	strcat(tempMsg, "]") ;



	// ä�ù� �з� ����.
	strcat(tempMsg, "[") ;
	switch(pInfo->byRoomType)
	{
	case e_RT_Party :			strcat(tempMsg, RESRCMGR->GetMsg(880)) ;	break ;
	case e_RT_Deal :			strcat(tempMsg, RESRCMGR->GetMsg(881)) ;	break ;
	case e_RT_Normal_Chat :		strcat(tempMsg, RESRCMGR->GetMsg(882)) ;	break ;
	case e_RT_Local_Chat :		strcat(tempMsg, RESRCMGR->GetMsg(883)) ;	break ;
	case e_RT_Make_Friend :		strcat(tempMsg, RESRCMGR->GetMsg(884)) ;	break ;
	default : break ;
	}
	strcat(tempMsg, "]") ;



	// ä�ù� ���� ����.
	m_pRoomTitle->SetStaticText(tempMsg) ;



	// ���� ����.
	m_pRoomTitle->SetToolTip(pInfo->title, RGBA_MAKE(255, 255, 255, 255)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetMsgColor
//	DESC : �޽��� Ÿ�Կ� ���� ������ ��ȯ�ϴ� �Լ�.
//-------------------------------------------------------------------------------------------------
DWORD CChatRoomDlg::GetMsgColor(BYTE byColorType)
{
	// ���� ���� ����.
	DWORD dwColor = RGBA_MAKE(255, 255, 255, 255) ;



	// �޽��� Ÿ�� Ȯ��.
	switch(byColorType)
	{
	case e_ChatMsg_StepIn :
	case e_ChatMsg_StepOut :
	case e_ChatMsg_System :				dwColor = RGBA_MAKE(255, 255, 0, 255) ; break ;

	case e_ChatMsg_MyMsg :				dwColor = RGBA_MAKE(0, 255, 0, 255) ; break ;
	case e_ChatMsg_OtherMsg :			dwColor = RGBA_MAKE(255, 255, 255, 255) ; break ;

	case e_ChatMsg_Whisper_Sener :		dwColor = RGBA_MAKE(255, 0, 255, 255) ; break ;
	case e_ChatMsg_Whisper_Receiver :	dwColor = RGBA_MAKE(0, 255, 255, 255) ; break ;
	default : break ;
	}



	// ���� �� ���� ��ȯ.
	return dwColor ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : CloseChatRoom
//	DESC : ä�ù��� �ݴ� ó���� �ϴ� �Լ�.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomDlg::CloseChatRoom()
{
	SetActive(FALSE) ;
	SetDlgToDefault() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDlgToDefault
//	DESC : ä��â�� ó��(�⺻) ���·� �ʱ�ȭ �ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomDlg::SetDlgToDefault()
{
	// ä�ù� ���� �ʱ�ȭ.
	m_pRoomTitle->SetStaticText("") ;

	// ä�� �Է� ���� �ʱ�ȭ.
	m_pInputBox->SetEditText("") ;
	//m_pInputBox->SetScriptText("") ;

	// ä�� ���� �ʱ�ȭ.
	m_pChatList->RemoveAll() ;
}