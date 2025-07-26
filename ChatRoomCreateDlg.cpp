#include "stdafx.h"
#include "../WindowIDEnum.h"
#include "../cMsgBox.h"
#include "./Interface/cResourceManager.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cButton.h"
#include "./Interface/cComboBox.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cPushupButton.h"
#include "./ChatManager.h"
#include "./FilteringTable.h"
#include "./ChatRoomMgr.h"
#include "./ChatRoomCreateDlg.h"
#include "cIMEex.h"

CChatRoomCreateDlg::CChatRoomCreateDlg(void)
{
	// �ʱ�ȭ ó��.
	m_pOpen				= NULL ;																		// Initialize all member variables.
	m_pClose			= NULL ;

	m_pCodeBox			= NULL ;

	m_pGuestCount		= NULL ;
	m_pRoomType			= NULL ;

	m_pTitleBox			= NULL ;

	m_pOKBtn			= NULL ;
	m_pCancelBtn		= NULL ;

	m_bySecretMode		= e_RM_Open ;

	m_byGeustCount		= e_GC_20 ;
	m_byRoomType		= e_RT_Party ;

	m_bIsWritedTitle	= FALSE;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomCreateDlg
//	DESC : �Ҹ��� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
CChatRoomCreateDlg::~CChatRoomCreateDlg(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//	DESC : ���� ��Ʈ�ѵ��� ��ũ�ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Linking()
{
	// ���� ��Ʈ�ѵ��� ��ũ.
	m_pOpen				= (cPushupButton*)GetWindowForID(CRCD_CHK_ON) ;									// Link child controls.
	m_pClose			= (cPushupButton*)GetWindowForID(CRCD_CHK_OFF) ;

	m_pCodeBox			= (cEditBox*)GetWindowForID(CRCD_EDB_CODE) ;

	m_pGuestCount		= (cComboBox*)GetWindowForID(CRCD_CMB_GUEST) ;
	m_pRoomType			= (cComboBox*)GetWindowForID(CRCD_CMB_ROOMTYPE) ;

	m_pTitleBox			= (cEditBox*)GetWindowForID(CRCD_EDB_TITLE) ;

	m_pOKBtn			= (cButton*)GetWindowForID(CRCD_BTN_OK) ;
	m_pCancelBtn		= (cButton*)GetWindowForID(CRCD_BTN_CANCEL) ;

	if( !m_pOpen	 || !m_pClose	 || !m_pCodeBox || !m_pGuestCount || 
		!m_pRoomType || !m_pTitleBox || !m_pOKBtn	|| !m_pCancelBtn )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	//m_pCodeBox->SetValidCheck( VCM_NORMAL_NUMBER ) ;													// Setting valid to number.



	// �⺻ ������ â���� ����.
	SetDefaultDlg() ;																					// Setting dialog state to default.



	// ä�ù� �Ŵ����� ����â ���.
	CHATROOMMGR->SetCreateDlg(this) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetActive
//	DESC : ä�ù� ���� ���̾�α׸� �⺻ ���·� �����ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::SetActive(BOOL val)
{
	if(val) SetDefaultDlg() ;																			// Setting dialog state to default.

	cDialog::SetActive(val) ;																			// Active this dialog or not.
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDefaultDlg
//	DESC : ä�ù� ���� â�� �⺻���·� �����ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::SetDefaultDlg() 
{
	if( !m_pOpen || !m_pClose || !m_pCodeBox || !m_pGuestCount || !m_pTitleBox )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ä�ù� ������� ����.
	m_pOpen->SetPush(TRUE) ;
	m_pClose->SetPush(FALSE) ;

	m_bySecretMode		= e_RM_Open ;

	// ä�ù� ���� ����.
	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(TRUE) ;
	m_pCodeBox->SetFocusEdit(FALSE) ;
	m_pGuestCount->SetMaxLine(e_GC_Max);
	m_pGuestCount->SetCurSelectedIdx(0) ;
	m_pGuestCount->SelectComboText(0) ;

	m_pRoomType->SetMaxLine(e_RT_Max);
	m_pRoomType->SetCurSelectedIdx(0);
	m_pRoomType->SelectComboText(0);

	m_byGeustCount = e_GC_20;
	m_byRoomType = e_RT_Party;

	m_pTitleBox->SetEditText(CHATMGR->GetChatMsg(1479)) ;												// Setting editbox for title of chatroom.
	m_bIsWritedTitle = FALSE;
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : EventFunc�� ���� �Ѿ�� Event�� ó���ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::OnActionEvent(LONG id, void* p, DWORD event)
{
	if(!p)
	{
		CHATROOMMGR->Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// Check id & Handling message.
	switch(id)
	{
	case CRCD_CHK_ON :			Chk_Open() ;		break ;
	case CRCD_CHK_OFF :			Chk_Close() ;		break ;

	case CRCD_CMB_GUEST :		Cmb_Guest() ;		break ;
	case CRCD_CMB_ROOMTYPE :	Cmb_Roomtype() ;	break ;

	case CRCD_BTN_OK :			Btn_Ok() ;			break ;
	case CRCD_BTN_CANCEL :		SetActive(FALSE);	break ;
	// 100205 ONS ���� ���񿡵�Ʈ Ŭ���� �ڽ� �ʱ�ȭ
	case CRCD_EDB_TITLE:
		{
			if( !m_bIsWritedTitle )
			{
				m_pTitleBox->SetEditText(""); 
				m_bIsWritedTitle = TRUE;
			}
		}
		break;
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Chk_Open
//	DESC : �������� �����ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Chk_Open() 
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	if(!m_pOpen || !m_pClose)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ���� ���� ����.
	BOOL bChecked = FALSE ;

	bChecked = m_pOpen->IsPushed() ;																	// Receive current state.
	if(!bChecked) m_pOpen->SetPush(TRUE) ;

	m_bySecretMode = e_RM_Open ;
	m_pClose->SetPush(FALSE) ;																			// Switching secret mode.

	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(TRUE) ;
	m_pCodeBox->SetFocusEdit(FALSE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Chk_Close
//	DESC : ����� ���� �����ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Chk_Close()
{
	if(!m_pOpen || !m_pClose)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ����� ���� ����.
	BOOL bChecked = FALSE ;

	bChecked = m_pClose->IsPushed() ;																	// Receive current state.
	if(!bChecked) m_pClose->SetPush(TRUE) ;

	m_bySecretMode = e_RM_Close ;
	m_pOpen->SetPush(FALSE) ;																			// Switching secret mode.

	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(FALSE) ;
	m_pCodeBox->SetFocusEdit(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Cmb_Guest
//	DESC : �ִ� ������ ���� �����ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Cmb_Guest()
{
	if(!m_pGuestCount)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	m_byGeustCount = BYTE(m_pGuestCount->GetCurSelectedIdx());
}





//-------------------------------------------------------------------------------------------------
//	NAME : Cmb_Roomtype
//	DESC : ä�ù� �з��� �����ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Cmb_Roomtype()
{
	if(!m_pRoomType)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	switch(m_pRoomType->GetCurSelectedIdx())
	{
	case 0 : m_byRoomType = e_RT_Party ;		break ;
	case 1 : m_byRoomType = e_RT_Deal ;			break ;
	case 2 : m_byRoomType = e_RT_Normal_Chat ;	break ;
	case 3 : m_byRoomType = e_RT_Local_Chat ;	break ;
	case 4 : m_byRoomType = e_RT_Make_Friend ;	break ;
	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Ok
//	DESC : Ȯ�� ��ư �̺�Ʈ�� ó���ϴ� �Լ�.
//	DATE : APRIL 3, 2008
//-------------------------------------------------------------------------------------------------
void CChatRoomCreateDlg::Btn_Ok()
{
	if(!m_pCodeBox || !m_pTitleBox)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ��й�ȣ �ޱ�.
	// 080704 LYW --- ChatRoomCreateDlg : �ӽ� ���� ������ Ȯ��.
	//char codeBuf[SECRET_CODE_SIZE] = {0,} ;
	char codeBuf[256] = {0,} ;
	strcpy(codeBuf, m_pCodeBox->GetEditText()) ;

	// ��й�ȣ üũ.
	if(m_bySecretMode == e_RM_Close)
	{
		if( strcmp("", codeBuf) == 0 )
		{
			WINDOWMGR->MsgBox(	MBI_CR_SECRETCODE_ERR, 
								MBT_OK, 
								CHATMGR->GetChatMsg(1478) ) ;
			return ;
		}
	}

	// ��й�ȣ ���͸�.
	if( FILTERTABLE->FilterChat(codeBuf) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		m_pCodeBox->SetEditText("") ;
		SetActive(FALSE) ;
		return;
	}



	// ���� �ޱ�.
	// 080704 LYW --- ChatRoomCreateDlg : �ӽ� ���� ������ Ȯ��.
	//char titleBuf[TITLE_SIZE] = {0,} ;
	char titleBuf[256] = {0,} ;
	strcpy(titleBuf, m_pTitleBox->GetEditText()) ;

	// ���� üũ.
	if( strcmp("", titleBuf) == 0 )
	{
		if( strcmp("", codeBuf) == 0 )
		{
			WINDOWMGR->MsgBox(	MBI_CR_ROOMTITLE_ERR, 
								MBT_OK, 
								CHATMGR->GetChatMsg(1479) ) ;
			return ;
		}
	}

	// ���� ���͸�.
	if( FILTERTABLE->FilterChat(titleBuf) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		m_pTitleBox->SetEditText("") ;
		SetActive(FALSE) ;
		return;
	}



	// ���� �ð� üũ.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// ä�ù� ���� ���� ���� �� �޽��� ����.
	MSG_CR_ELEMENT msg ;
	memset(&msg, 0, sizeof(MSG_CR_ELEMENT)) ;

	msg.Category			= MP_CHATROOM ;
	msg.Protocol			= MP_CHATROOM_CREATE_ROOM_SYN ;

	msg.dwObjectID			= HEROID ;

	msg.bySecretMode		= m_bySecretMode ;
	SafeStrCpy(msg.code, m_pCodeBox->GetEditText(), SECRET_CODE_SIZE) ;

	msg.byTotalGuestCount	= m_byGeustCount ;
	msg.byRoomType			= m_byRoomType ;

	SafeStrCpy(msg.title, titleBuf, 64) ;

	NETWORK->Send( &msg, sizeof(msg) ) ;																// Send message to server.

	SetActive(FALSE) ;

	// �ð� ������Ʈ.
	CHATROOMMGR->UpdateMyLastComTime() ;
}