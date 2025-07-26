#include "Stdafx.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "Interface/cButton.h"
#include "Interface/cComboBox.h"
#include "Interface/cEditBox.h"
#include "Interface/cPushupButton.h"
#include "Interface/cResourceManager.h"
#include "Interface/cWindowManager.h"
#include "ChatManager.h"
#include "ChatRoomMgr.h"
#include "../[CC]Header/FilteringTable.h"
#include "ChatRoomOptionDlg.h"
#include "cIMEex.h"





//-------------------------------------------------------------------------------------------------
//	NAME : CChatRoomOptionDlg
//	DESC : ������ �Լ�.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomOptionDlg::CChatRoomOptionDlg(void)
{
	// �ʱ�ȭ ó��.
	m_pOpen				= NULL ;
	m_pClose			= NULL ;

	m_pCodeBox			= NULL ;

	m_pGuestCount		= NULL ;
	m_pRoomType			= NULL ;

	m_pTitleBox			= NULL ;

	m_pOKBtn			= NULL ;
	m_pCancelBtn		= NULL ;

	m_bySecretMode		= e_RM_Open ;
	//m_wSecretCode		= 0 ;

	m_byGeustCount		= e_GC_20 ;
	m_byRoomType		= e_RT_Party ;

	//memset(m_TitleBuff, 0, 128) ;


	//m_byStoredMode			= e_RM_Open ;										
	////m_wStoredCode			= 0 ;
	//memset(m_code, 0, sizeof(SECRET_CODE_SIZE+1)) ;

	//m_byStoredGuestCount	= e_GC_20 ;
	//m_byStoredRoomType		= e_RT_Party ;

	//memset(m_StoredTitleBuff, 0, 128) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomOptionDlg
//	DESC : �Ҹ��� �Լ�.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomOptionDlg::~CChatRoomOptionDlg(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : Linking
//		 : 
//	DESC : �ɼ�â�� ���� ��Ʈ�ѵ��� ��ũ�Ѵ�.
//		 : 
//		 : 1. ���� ��Ʈ�� ������ ��ũ.
//		 : 2. ��ũ �� ��Ʈ�� ��ȿ�� üũ.
//		 : 3. ,�� ������� �ʴ� EditBox�� ����.
//		 : 4. ������ ������ �ɼ� â ����.
//		 : 5. ä�ù� �Ŵ����� �ɼ�â ���.
//		 : 
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Linking()
{
	// ���� ��Ʈ�� ������ ��ũ.
	m_pOpen				= (cPushupButton*)GetWindowForID(CROD_CHK_ON) ;			
	m_pClose			= (cPushupButton*)GetWindowForID(CROD_CHK_OFF) ;

	m_pCodeBox			= (cEditBox*)GetWindowForID(CROD_EDB_CODE) ;

	m_pGuestCount		= (cComboBox*)GetWindowForID(CROD_CMB_GUEST) ;
	m_pRoomType			= (cComboBox*)GetWindowForID(CROD_CMB_ROOMTYPE) ;

	m_pTitleBox			= (cEditBox*)GetWindowForID(CROD_EDB_TITLE) ;

	m_pOKBtn			= (cButton*)GetWindowForID(CROD_BTN_OK) ;
	m_pCancelBtn		= (cButton*)GetWindowForID(CROD_BTN_CANCEL) ;



	// ��ũ �� ��Ʈ�� ��ȿ�� üũ.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	ASSERT(m_pCodeBox) ;

	ASSERT(m_pGuestCount) ;
	ASSERT(m_pRoomType) ;

	ASSERT(m_pTitleBox) ;

	ASSERT(m_pOKBtn) ;
	ASSERT(m_pCancelBtn) ;

	if( !m_pOpen	 || !m_pClose	 || !m_pCodeBox || !m_pGuestCount || 
		!m_pRoomType || !m_pTitleBox || !m_pOKBtn	|| !m_pCancelBtn )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// ,�� ������� �ʴ� EditBox�� ����.
	//m_pCodeBox->SetValidCheck( VCM_NORMAL_NUMBER ) ;							

	// ������ ������ �ɼ� â ����.
	SetStoredDlg() ;



	// ä�ù� �Ŵ����� �ɼ�â ���.
	CHATROOMMGR->SetOptionDlg(this) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetStoredDlg
//		 : 
//	DESC : The function to setting this dialog to default.
//		 : 
//		 : 1. ����/����� ����.
//		 : 2. ��ȣ �Է� ����.
//		 : 3. ���� �ο� ����.
//		 : 4. ä�ù� �з� ����.
//		 : 5. ä�ù� ���� ����.
//		 : 
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
BOOL CChatRoomOptionDlg::SetStoredDlg() 
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	ASSERT(m_pCodeBox) ;

	ASSERT(m_pGuestCount) ;
	ASSERT(m_pRoomType) ;

	ASSERT(m_pTitleBox) ;

	ASSERT(m_pOKBtn) ;
	ASSERT(m_pCancelBtn) ;

	if( !m_pOpen || !m_pClose || !m_pCodeBox || !m_pGuestCount || 
		!m_pRoomType || !m_pTitleBox || !m_pOKBtn	|| !m_pCancelBtn )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return FALSE ;
	}														



	// ä�ù� ��� ����.
	//if(m_byStoredMode == e_RM_Open)
	if(CHATROOMMGR->GetOptSavedMode() == e_RM_Open)
	{
		m_pOpen->SetPush(TRUE) ;												
		m_pClose->SetPush(FALSE) ;
	}
	else
	{
		m_pOpen->SetPush(FALSE) ;												
		m_pClose->SetPush(TRUE) ;
	}

	//if(strlen(m_code) == 0)
	if(CHATROOMMGR->GetOptSavedMode() == e_RM_Close)
	{
		if(strlen(CHATROOMMGR->GetOptSecretCode()) == 0)
		{
			m_pCodeBox->SetEditText("") ;
		}
		else
		{
			//m_pCodeBox->SetEditText(m_code) ;
			m_pCodeBox->SetEditText(CHATROOMMGR->GetOptSecretCode()) ;
		}

		m_pCodeBox->SetDisable(FALSE) ;
		m_pCodeBox->SetFocusEdit(FALSE) ;
	}
	else
	{
		m_pCodeBox->SetEditText("") ;
		m_pCodeBox->SetDisable(TRUE) ;
		m_pCodeBox->SetFocusEdit(FALSE) ;
	}

	m_pGuestCount->SetMaxLine(e_GC_Max);
	m_pGuestCount->SetCurSelectedIdx(CHATROOMMGR->GetOptTotalGuest());
	m_pGuestCount->SelectComboText(CHATROOMMGR->GetOptTotalGuest());
	m_pRoomType->SetMaxLine(e_RTM_Max);
	m_pRoomType->SetCurSelectedIdx(CHATROOMMGR->GetOptRoomType());
	m_pRoomType->SelectComboText(CHATROOMMGR->GetOptRoomType());
	m_pTitleBox->SetEditText(CHATROOMMGR->GetOptRoomTitle());
	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetDlgToDefault
//		 : 
//	DESC : The function to setting this dialog to default.
//		 : 
//		 : 1. ����/����� �ʱ�ȭ.
//		 : 2. ��ȣ �Է� �ʱ�ȭ.
//		 : 3. ���� �ο� �ʱ�ȭ.
//		 : 4. ä�ù� �з� �ʱ�ȭ.
//		 : 5. ä�ù� ���� �ʱ�ȭ.
//		 : 
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::SetDlgToDefault()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	ASSERT(m_pCodeBox) ;

	ASSERT(m_pGuestCount) ;
	ASSERT(m_pRoomType) ;

	ASSERT(m_pTitleBox) ;

	ASSERT(m_pOKBtn) ;
	ASSERT(m_pCancelBtn) ;

	if( !m_pOpen || !m_pClose || !m_pCodeBox || !m_pGuestCount || 
		!m_pRoomType || !m_pTitleBox || !m_pOKBtn	|| !m_pCancelBtn )
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ����/����� �ʱ�ȭ.
	m_pOpen->SetPush(TRUE) ;												
	m_pClose->SetPush(FALSE) ;

	m_bySecretMode		= e_RM_Open ;
	//m_wSecretCode		= 0 ;



	// ��ȣ �Է� �ʱ�ȭ.
	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(TRUE) ;
	m_pCodeBox->SetFocusEdit(FALSE) ;



	// ���� �ο� �ʱ�ȭ.
	m_pGuestCount->SetCurSelectedIdx(0) ;
	m_pGuestCount->SelectComboText(0) ;



	// ä�ù� �з� �ʱ�ȭ.
	m_pRoomType->SetCurSelectedIdx(0) ;
	m_pRoomType->SelectComboText(0) ;

	m_byGeustCount		= e_GC_20 ;
	m_byRoomType		= e_RT_Party ;



	// ä�ù� ���� �ʱ�ȭ.
	m_pTitleBox->SetEditText("") ;	
}





//-------------------------------------------------------------------------------------------------
//	NAME : OnActionEvent
//	DESC : ������Ʈ�ѿ� �̺�Ʈ�� �߻����� �� ó���ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::OnActionEvent(LONG id, void* p, DWORD event)
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
	case CROD_CHK_ON :			Chk_Open() ;		break ;
	case CROD_CHK_OFF :			Chk_Close() ;		break ;

	case CROD_CMB_GUEST :		Cmb_Guest() ;		break ;
	case CROD_CMB_ROOMTYPE :	Cmb_Roomtype() ;	break ;

	case CROD_BTN_OK :			Btn_Ok() ;			break ;
	case CROD_BTN_CANCEL :		SetActive(FALSE);	break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Chk_Open
//	DESC : ä�ù��� �������� �����ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Chk_Open() 
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	if(!m_pOpen || !m_pClose)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// �������� ����.
	BOOL bChecked = FALSE ;

	bChecked = m_pOpen->IsPushed() ;
	if(!bChecked) m_pOpen->SetPush(TRUE) ;

	m_bySecretMode = e_RM_Open ;
	m_pClose->SetPush(FALSE) ;

	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(TRUE) ;
	m_pCodeBox->SetFocusEdit(FALSE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Chk_Close
//	DESC : ä�ù��� ��������� �����ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Chk_Close()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pOpen) ;
	ASSERT(m_pClose) ;

	if(!m_pOpen || !m_pClose)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ����� ���� ����.
	BOOL bChecked = FALSE ;

	bChecked = m_pClose->IsPushed() ;											
	if(!bChecked) m_pClose->SetPush(TRUE) ;

	m_bySecretMode = e_RM_Close ;
	m_pOpen->SetPush(FALSE) ;

	m_pCodeBox->SetEditText("") ;
	m_pCodeBox->SetDisable(FALSE) ;
	m_pCodeBox->SetFocusEdit(TRUE) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Cmb_Guest
//	DESC : ä�ù��� ������ �� �ִ� �����ο��� �����ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Cmb_Guest()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pGuestCount) ;

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
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Cmb_Roomtype()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pRoomType) ;

	if(!m_pRoomType)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	//m_byRoomType = m_pRoomType->GetCurSelectedIdx() ;							
	// ä�ù� �з� ����.
	int nIndex = 0 ;
	nIndex = m_pRoomType->GetCurSelectedIdx() ;

	switch(nIndex)
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
//	DESC : ������ �ɼ������� ������ ��û�ϴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Btn_Ok()
{
	// ���� ��Ʈ�� üũ.
	ASSERT(m_pCodeBox) ;
	ASSERT(m_pTitleBox) ;

	if(!m_pCodeBox || !m_pTitleBox)
	{
		CHATROOMMGR->Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// ��й�ȣ üũ.
	// 080704 LYW --- ChatRoomOptionDlg : �ӽ� ���� ������ Ȯ��.
	//char codeBuf[SECRET_CODE_SIZE] = {0,} ;

	char codeBuf[256] = {0,} ;
	strcpy(codeBuf, m_pCodeBox->GetEditText()) ;								

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

	// ��й�ȣ ���͸� üũ.
	if( FILTERTABLE->FilterChat(codeBuf) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		m_pCodeBox->SetEditText("") ;
		SetActive(FALSE) ;
		return;
	}



	// ä�ù� ���� üũ.
	// 080704 LYW --- ChatRoomOptionDlg : �ӽ� ���� ������ Ȯ��.
	//char titleBuf[TITLE_SIZE] = {0,} ;

	char titleBuf[256] = {0,} ;
	strcpy(titleBuf, m_pTitleBox->GetEditText()) ;

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

	// ���� ���͸� üũ.
	if( FILTERTABLE->FilterChat(titleBuf) )
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1225)) ;
		m_pTitleBox->SetEditText("") ;
		SetActive(FALSE) ;
		return;
	}

	// �������� üũ.
	if(HEROID != CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATROOMMGR->AddChatRoomMsg(CHATMGR->GetChatMsg(1487), e_ChatMsg_System) ;
		return ;
	}

	// ���� �ð� üũ.
	if(!CHATROOMMGR->IsEnableExecuteCom())
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
		return ;
	}



	// �ɼ� ���� ��û.
	MSG_CR_ELEMENT msg ;
	memset(&msg, 0, sizeof(MSG_CR_ELEMENT)) ;

	msg.Category			= MP_CHATROOM ;
	msg.Protocol			= MP_CHATROOM_CHANGE_OPTION_SYN ;

	msg.dwObjectID			= HEROID ;

	msg.bySecretMode		= m_bySecretMode ;
	SafeStrCpy(msg.code, m_pCodeBox->GetEditText(), SECRET_CODE_SIZE) ;			

	msg.byTotalGuestCount	= m_byGeustCount ;
	msg.byRoomType			= m_byRoomType ;

	SafeStrCpy(msg.title, titleBuf, 64) ;

	NETWORK->Send( &msg, sizeof(msg) ) ;										

	SetActive(FALSE) ;

	// �ð� ������Ʈ.
	CHATROOMMGR->UpdateMyLastComTime() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Btn_Cancel
//	DESC : ���� ���̴� �ɼ��� ����ϰ�, �ɼ�â�� �ݴ� �Լ�.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomOptionDlg::Btn_Cancel()
{
	SetStoredDlg();
	SetActive(FALSE);
}