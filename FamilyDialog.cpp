// FamilyDialog.cpp: implementation of the CFamilyDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../Input/Mouse.h"															// ���콺 ��� ������ �ҷ��´�.

#include "WindowIDEnum.h"															// ������ ���̵� ��������� �ҷ��´�.

#include "./Interface/cWindowManager.h"												// ������ �Ŵ��� ��������� �ҷ��´�.
#include "../Interface/cResourceManager.h"											// ���ҽ� �Ŵ��� ��������� �ҷ��´�.

#include "ObjectManager.h"															// ������Ʈ �Ŵ��� ��������� �ҷ��´�.
//#include "FamilyManager.h"															// �йи� �Ŵ��� ��������� �ҷ��´�.
#include "ChatManager.h"															// ä�� �Ŵ��� ��������� �ҷ��´�.

#include "../hseos/family/shfamilymanager.h"										// �йи� �Ŵ��� �ش������� �ҷ��´�.

#include "./Interface/cScriptManager.h"												// ��ũ��Ʈ �Ŵ��� ��������� �ҷ��´�.

#include "cFont.h"	// �׽�Ʈ�� ���� ��Ʈ�� �ҷ��´�.


#include "./interface/cStatic.h"													// ����ƽ �������̽� ��������� �ҷ��´�.
#include "./interface/cComboBox.h"													// �޺��ڽ� �������̽� ��������� �ҷ��´�.
#include "./interface/cListDialog.h"												// ����Ʈ ���̾�α� �������̽� ��������� �ҷ��´�.
#include "./interface/cPushupButton.h"												// Ǫ���� ��ư �������̽� ��������� �ҷ��´�.
#include "cMsgBox.h"																// �޽��� �ڽ� �������̽� ��������� �ҷ��´�.

#include "GameIn.h"																	// ������ ��������� �ҷ��´�.
//#include "Family.h"																	// �йи� ��������� �ҷ��´�.

#include "FamilyDialog.h"															// �йи� ���̾�α� ��������� �ҷ��´�.
#include "FamilyRankDialog.h"														// �йи� ��ŷ ���̾�α� ��������� �ҷ��´�.
#include "FamilyNickNameDialog.h"													// �йи� ȣĪ ���� ���̾�α� ��������� �ҷ��´�.
//#include "FamilyUnion.h"	

#include "cImageSelf.h"
#include "FamilyMarkDialog.h"
// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.10.11
#include "../hseos/Farm/SHFarmManageDlg.h"
// E ����ý��� �߰� added by hseos 2007.10.11

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFamilyDialog::CFamilyDialog()														// ������ �Լ�.
{
	m_pFamilyName = NULL;
	m_pMasterName = NULL;
	m_pHonorPoint = NULL;
	m_pLvListDlg = NULL;
	m_pIDListDlg = NULL;
	m_pMemberID	= NULL;
	m_pMemberLevel = NULL;
	m_pMemberClass = NULL;
	m_pMemberLogin = NULL;
	m_pMemberNickName = NULL;
	m_pMemberGuildName = NULL;
	m_pChangeNickName = NULL;
	m_pInvite = NULL;
	m_pRegistMark = NULL;
	m_pLeaveBtn	= NULL;
	m_pExpelBtn	= NULL;
	m_pFarmManageBtn = NULL;
	m_pPayTaxBtn = NULL;
	m_pTransferBtn = NULL;
	m_nCurSelectIdx = 0;
	ZeroMemory(
		m_pClassImg,
		sizeof(m_pClassImg));
	ZeroMemory(
		m_pLongInImg,
		sizeof(m_pLongInImg));
	ZeroMemory(
		m_pLogOutImg,
		sizeof(m_pLogOutImg));
}

CFamilyDialog::~CFamilyDialog()														// �Ҹ��� �Լ�.
{
	//ReleaseIconList() ;																// Ŭ���� ������ ����Ʈ�� �����Ѵ�.
}

void CFamilyDialog::Linking()														// ��Ʈ�ѵ��� ��ũ�ϴ� �Լ�.
{
	m_pFamilyName		= (cStatic*)GetWindowForID(FMY_NAME) ;						// �йи� �̸��� ��ũ�Ѵ�.
	m_pMasterName		= (cStatic*)GetWindowForID(FMY_MASTERNAME) ;				// ������ �̸��� ��ũ�Ѵ�.
	m_pHonorPoint		= (cStatic*)GetWindowForID(FMY_HONORPOINT) ;				// �� ����Ʈ�� ��ũ�Ѵ�.

	m_pLvListDlg		= (cListDialog*)GetWindowForID(FMY_MEMBER_LV_INFO_LIST) ;	// ��� Lv ���� ����Ʈ ���̾�α׸� ��ũ�Ѵ�.
	m_pIDListDlg		= (cListDialog*)GetWindowForID(FMY_MEMBER_ID_INFO_LIST) ;	// ��� ID ���� ����Ʈ ���̾�α׸� ��ũ�Ѵ�.

	m_pMemberID			= (cStatic*)GetWindowForID(FMY_MEMBERID) ;					// ��� ���̵� ��ũ�Ѵ�.
	m_pMemberLevel		= (cStatic*)GetWindowForID(FMY_MEMBERLEVEL) ;				// ��� ������ ��ũ�Ѵ�.
	m_pMemberClass		= (cStatic*)GetWindowForID(FMY_MEMBERCLASS) ;				// ��� Ŭ������ ��ũ�Ѵ�.
	m_pMemberLogin		= (cStatic*)GetWindowForID(FMY_MEMBERLOGIN) ;				// ��� �α����� ��ũ�Ѵ�.
	m_pMemberNickName	= (cStatic*)GetWindowForID(FMY_MEMBERNICKNAME) ;			// ��� ȣĪ�� ��ũ�Ѵ�.
	m_pMemberGuildName  = (cStatic*)GetWindowForID(FMY_MEMBERGUILDNAME) ;			// ��� ������ ��ũ�Ѵ�.

	m_pChangeNickName	= (cButton*)GetWindowForID(FMY_CHANGENICKNAME) ;			// ȣĪ���� ��ư�� ��ũ�Ѵ�.
	m_pInvite			= (cButton*)GetWindowForID(FMY_INVITE) ;					// �ʴ��û ��ư�� ��ũ�Ѵ�.
	m_pRegistMark		= (cButton*)GetWindowForID(FMY_REGISTMARK) ;				// ������ ��ư�� ��ũ�Ѵ�.

	// desc_hseos_�йи�01
	// S �йи� �߰� added by hseos 2007.10.22
	m_pLeaveBtn			= (cButton*)GetWindowForID(FMY_LEAVE);
	m_pExpelBtn			= (cButton*)GetWindowForID(FMY_EXPEL);
	m_pFarmManageBtn	= (cButton*)GetWindowForID(FMY_FARM_MANAGE);
	// 091204 ONS �йи� ���̾�α׿� �йи��� �̾� �� ���弼�� ���� ��� ��ư �߰�
	m_pPayTaxBtn		= (cButton*)GetWindowForID(FMY_PAYTAX);
	m_pTransferBtn		= (cButton*)GetWindowForID(FMY_TRANSFER);

	// E �йи� �߰� added by hseos 2007.10.22
}

void CFamilyDialog::UpdateAllInfo() 												// �йи� â�� ���̴� ��� ������ ������Ʈ �Ѵ�.
{
	CSHFamily* pFamily = HERO->GetFamily() ;										// �йи� ������ �޾ƿ´�.

	if( pFamily )																	// �йи� ������ ��ȿ���� üũ�Ѵ�.
	{
		m_pFamilyName->SetStaticText(pFamily->Get()->szName) ;						// �йи����� �����Ѵ�.

		m_pMasterName->SetStaticText(pFamily->Get()->szMasterName) ;				// �������� �̸��� �����Ѵ�.

		SetMemberList(pFamily) ;													// ��� ����Ʈ�� �����Ѵ�.

		SetHonorPoint(0) ;															// �� ����Ʈ�� �����Ѵ�.

		CSHFamilyMember* pMember = pFamily->GetMember(0) ;							// ��� ������ �޾ƿ´�.

		if( pMember )																// ��� ������ ��ȿ���� üũ�Ѵ�.
		{
			SetMemberInfo( pMember) ;												// ��� ������ �����Ѵ�.
		}

		// ù��° ����� �����Ѵ�.
		SelectMemberOfList( 0 );

  		if (pFamily->Get()->nMasterID == gHeroID)
		{
			m_pLeaveBtn->SetDisable(TRUE);
			m_pLeaveBtn->SetImageRGB(RGB( 190, 190, 190 ));
			m_pInvite->SetDisable(FALSE);	
			m_pInvite->SetImageRGB(RGB( 255, 255, 255 ));
			m_pExpelBtn->SetDisable(FALSE);
			m_pExpelBtn->SetImageRGB(RGB( 255, 255, 255 ));
			m_pTransferBtn->SetDisable(FALSE);
			m_pTransferBtn->SetImageRGB(RGB( 255, 255, 255 ));
			m_pRegistMark->SetDisable(FALSE);
			m_pRegistMark->SetImageRGB(RGB( 255, 255, 255 ));
		}
		else
		{
			m_pExpelBtn->SetDisable(TRUE);
			m_pExpelBtn->SetImageRGB(RGB( 190, 190, 190 ));
			m_pInvite->SetDisable(TRUE);	
			m_pInvite->SetImageRGB(RGB( 190, 190, 190 ));
			m_pRegistMark->SetDisable(TRUE);
			m_pRegistMark->SetImageRGB(RGB( 190, 190, 190 ));
			m_pTransferBtn->SetDisable(TRUE);
			m_pTransferBtn->SetImageRGB(RGB( 190, 190, 190 ));
			m_pLeaveBtn->SetDisable(FALSE);
			m_pLeaveBtn->SetImageRGB(RGB( 255, 255, 255 ));


		}
	}
}

void CFamilyDialog::SetHonorPoint( DWORD dwPoint )									// �� ����Ʈ�� �����ϴ� �Լ�.
{
	CSHFamily* pFamily = HERO->GetFamily() ;										// �йи� ������ �޾ƿ´�.

	if( pFamily )																	// �йи� ������ ��ȿ���� üũ�Ѵ�.
	{
 		char tempBuf[10+5] = {0, } ;												// �йи� ����Ʈ�� ���ڿ��� �ٲپ� �� �ӽ� ���� ����.

		itoa(pFamily->GetEx()->nHonorPoint, tempBuf, 10) ;							// �йи� ����Ʈ�� ���ڷ� ��ȯ�Ѵ�.

		m_pHonorPoint->SetStaticText(tempBuf) ;										// �йи� ����Ʈ�� �����Ѵ�.
	}

	// �� ����Ʈ�� �����Ǵ� ��Ȳ���� dwPoint �� - int ���� �ֱ� ������  
	// �ݵ�� int �� ĳ���� �ؼ� ������� Ȯ���ؾ� ��. by hseos
	//if( (int)dwPoint > 0 )															// �Ѿ�� �� ����Ʈ�� 0���� ũ��,
	//{
	//	char tempBuf[128] = {0, } ;													// �� ����Ʈ�� ���ڿ��� �ٲپ� �� �ӽ� ���� ����.								

	//	sprintf(tempBuf, "�� ����Ʈ�� %d ����Ʈ ����Ͽ����ϴ�.", dwPoint) ;		// �� ����Ʈ�� ���ڷ� ��ȯ�Ѵ�.

	//	CHATMGR->AddMsg(CTC_SYSMSG, tempBuf) ;										// ä��â�� ����Ʈ �� ������ �Ѵ�.
	//}
}

void CFamilyDialog::SetMemberInfo( CSHFamilyMember* pMember )						// �йи� â �ϴܺ�, ����� �� ������ ��Ÿ���� �Լ�.
{
	m_pMemberID->SetStaticText( pMember->Get()->szName ) ;							// ��� �̸��� �����Ѵ�.

	char tempBuf[10] = {0, } ;														// ��� ������ ���ڷ� ��ȯ �� �ӽ� ���۸� �����Ѵ�.
	itoa(pMember->Get()->nLV, tempBuf, 10) ;										// ��� ������ ���ڷ� ��ȯ�Ѵ�.
	m_pMemberLevel->SetStaticText( tempBuf ) ;										// ��� ������ �����Ѵ�.

	int nClass		= pMember->Get()->nJobFirst ;									// ����� Ŭ���� �迭 ���� �޴´�.
	int nRace		= pMember->Get()->nRace ;										// ����� ���� ��ȣ�� �޴´�.
	int nJobGrade	= pMember->Get()->nJobGrade ;									// ����� Ŭ���� �ܰ踦 �޴´�.
	int nJobNum		= pMember->Get()->nJobCur ;										// ����� Ŭ���� ��ȣ�� �޴´�.

	if( nJobGrade == 1 )													// ���� �ܰ谡 1�϶���, �迭���� �Ѱ��� �����ۿ� ���� ������
	{
		nJobNum = 1 ;														// ���� ��ȣ�� 1�� �����Ѵ�.
	}

	int nClassIdx = ((nClass)*1000) + ((nRace+1)*100) + (nJobGrade*10) + nJobNum ;	// ����� Ŭ���� ��ȣ�� �޴´�.

	m_pMemberClass->SetStaticText(RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(nClassIdx))) ;	// ��� Ŭ������ �����Ѵ�.


	ZeroMemory(tempBuf, sizeof(tempBuf)) ;											// �ӽ� ���۸� �ʱ�ȭ �Ѵ�.

	int nIsLogIn = pMember->Get()->eConState ;										// ����� �α��� ���θ� �޴´�.
	char* pChatMsg = NULL;

	switch(nIsLogIn)																// �α��� ���θ� Ȯ���Ѵ�.
	{
	//case MEMBER_CONSTATE_LOGOFF :	strcpy(tempBuf, "���Ӿ���") ;					// ���� ���� ���·� �����Ѵ�.
	//case MEMBER_CONSTATE_LOGIN :	strcpy(tempBuf, "������") ;						// ���� ���� ���·� �����Ѵ�.

	case CSHGroupMember::MEMBER_CONSTATE_LOGOFF:
		{
			// ���� ���� ���·� �����Ѵ�.
			//strcpy(tempBuf, CHATMGR->GetChatMsg( 1443 )) ;
			pChatMsg = CHATMGR->GetChatMsg( 1443 );// 080306 NYJ --- Buffer Overflow�� �����ϱ� ���� �����ͷ� �޾ƿ��°� �����ҵ�.
		}
		break ;
	case CSHGroupMember::MEMBER_CONSTATE_LOGIN:
		{
			// ���� ���� ���·� �����Ѵ�.
			//strcpy(tempBuf, CHATMGR->GetChatMsg( 1442 )) ;
			pChatMsg = CHATMGR->GetChatMsg( 1442 );// 080306 NYJ --- Buffer Overflow�� �����ϱ� ���� �����ͷ� �޾ƿ��°� �����ҵ�.
		}
		break ;
	}
	
	m_pMemberLogin->SetStaticText(/*tempBuf*/pChatMsg) ;										// ����� �α��� ���θ� �����Ѵ�.
	m_pMemberNickName->SetStaticText( pMember->Get()->szNickname ) ;				// ����� ȣĪ�� �����Ѵ�.
	m_pMemberGuildName->SetStaticText( pMember->GetEx()->szGuild ) ;				// ����� ������ �����Ѵ�.
}

void CFamilyDialog::SetMemberList(CSHFamily* pFamily)
{
	DWORD dwColor = 0xffffffff;
	int nLevelBlank = 0;
	int nClass		= 0;
	int nRace		= 0;
	int nJobGrade	= 0;
	int nJobNum		= 0;
	int nLogIn		= 0;
	char levelBuf[8] = {0};
	CSHFamilyMember* pMember = 0;

	m_pLvListDlg->RemoveAll();
	m_pIDListDlg->RemoveAll();

	int nMemberCount = pFamily->Get()->nMemberNum ;									// ��� ���� ���Ѵ�.

	for( int bigCount = 0 ; bigCount < nMemberCount ; ++bigCount )					// ��� ����ŭ ������ ������.
	{
		pMember = pFamily->GetMember(bigCount) ;									// ī��Ʈ�� �ش��ϴ� ��� ������ �޴´�.

		if( pMember && pMember->Get()->nID != 0 )									// ��� ������ ��ȿ���� üũ�Ѵ�.
		{

			nClass		= pMember->Get()->nJobFirst ;								// ����� Ŭ���� �迭 ���� �޴´�.
			nRace		= pMember->Get()->nRace ;									// ����� ���� ��ȣ�� �޴´�.
			nJobGrade	= pMember->Get()->nJobGrade ;								// ����� Ŭ���� �ܰ踦 �޴´�.
			nJobNum		= pMember->Get()->nJobCur ;

			if( nJobGrade == 1 )													// ���� �ܰ谡 1�϶���, �迭���� �Ѱ��� �����ۿ� ���� ������
			{
				nJobNum = 1 ;														// ���� ��ȣ�� 1�� �����Ѵ�.
			}

			int nClassIdx = ((nClass)*1000) + ((nRace+1)*100) + (nJobGrade*10) + nJobNum ;	// ����� Ŭ���� ��ȣ�� �޴´�.

			//int nIconNum = GetClassIconNum(nClassIdx) ;								// Ŭ������ �ش��ϴ� �̹��� ��ȣ�� �޴´�.
			//if( nIconNum >= 0 && m_ClassIconList.GetCount() >= nIconNum )			// ������ �̹��� ��ȣ�� ������ Ȯ���Ѵ�.
			// 080320 LYW --- FamilyDialog : Ŭ���� ������ ����� ���ҽ� �Ŵ����� �ű�
 			int nIconNum = RESRCMGR->GetClassIconNum(nClassIdx) ;
 			if( nIconNum >= 0 && RESRCMGR->GetClassIconCount() >= nIconNum )
			{
				SCRIPTMGR->GetImage( nIconNum, &m_pClassImg[bigCount], PFT_CLASS_ICON_PATH ) ;	// Ŭ���� ������ �̹����� �Ҵ�޴´�.
			}

			if(pMember->Get()->eConState==CSHGroupMember::MEMBER_CONSTATE_LOGOFF)	// ����� �α��� ���̶��,
			{
				dwColor = RGB_HALF(200, 200, 200) ;									// ȸ������ ������ �����Ѵ�.
			}
			else																	// ����� �α׾ƿ� ���¶��,
			{
				dwColor = RGB_HALF(10, 10, 10) ;									// ���������� ������ �����Ѵ�.				
			}

			int nLevel = pMember->Get()->nLV ;

			itoa(nLevel, levelBuf, 10) ;											// ��� ������ ���� ���ۿ� ���ڷ� ��ȯ�Ͽ� ��´�.

			//itoa(pMember->Get()->nLV, levelBuf, 12) ;								// ��� ������ ���� ���ۿ� ���ڷ� ��ȯ�Ͽ� ��´�.

			nLevelBlank = strlen(levelBuf) ;										// ���� ������ ���̸� ���Ѵ�.

			m_pLvListDlg->AddItem( levelBuf, dwColor );
			
			m_pIDListDlg->AddItem( pMember->Get()->szName, dwColor ) ;				// ��� ����Ʈ�� ������ �����Ѵ�.

			nLogIn = pMember->Get()->eConState ;									// ����� �α��� ���θ� �޴´�.

			if( nLogIn )															// ����� �α��� ���̶��,
			{
				SCRIPTMGR->GetImage( 102, &m_pLongInImg[bigCount], PFT_HARDPATH ) ;	// �α��� �̹����� �����Ѵ�.
			}
			else																	// ����� �α׾ƿ� ���̶��,
			{
				SCRIPTMGR->GetImage( 103, &m_pLongInImg[bigCount], PFT_HARDPATH ) ;	// �α׾ƿ� �̹����� �����Ѵ�.
			}
		}
	}
}

void CFamilyDialog::Render()														// Ŭ���� �̹����� �α��� �̹����� ����� ���� �Լ�.
{
	if( !IsActive() ) return ;														// �йи� â�� Ȱ��ȭ ���� �ƴ϶��, �����Ѵ�.

	cDialog::RenderWindow();														// �����츦 �����Ѵ�.

	cDialog::RenderComponent();														// ������ ���۳�Ʈ���� �����Ѵ�.


	CSHFamily* pFamily = HERO->GetFamily() ;										// �йи� ������ �޾ƿ´�.

	if( pFamily && pFamily->Get()->nMasterID != 0 )									// �йи� ������ ��ȿ�ϰ�, ������ ���̵� 0�� �ƴϸ�,
	{
		VECTOR2 scale = {1.0f, 1.0f};
		VECTOR2 vPos = {0};
		DWORD dwColor = 0xffffffff;

		for( UINT count = 0 ; count < pFamily->Get()->nMemberNum ; ++count )		// ��� ����ŭ ������ ������.
		{
			vPos.y = (float)(GetAbsY() + 156) + (16*count) ;						// �̹��� �����ġ Y�� �����Ѵ�.

			vPos.x = (float)(GetAbsX() + 30) ;										// �̹��� �����ġ X�� �����Ѵ�.

			if( !m_pClassImg[count].IsNull() )										// ��� Ŭ���� �̹����� ���� �ƴ϶��,
			{
				m_pClassImg[count].RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;	// ��� Ŭ���� �̹����� ����Ѵ�.
			}

			vPos.x += 190  ;														// �α��� ��ġ�� ����� �����Ѵ�.

			if( !m_pLongInImg[count].IsNull() )										// ��� �α��� ���� �̹����� ���� �ƴ϶��,
			{
				m_pLongInImg[count].RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;	// ��� �α��� ���� �̹����� ����Ѵ�.
			}
		}
	}

	cImageSelf* pcsFamilyEmblem = HERO->GetFamilyEmblem();
	if (pcsFamilyEmblem)
	{
		VECTOR2 scale, pos2;
		scale.x = 1; scale.y = 1;
		pos2.x = GetAbsX()+210;
		pos2.y = GetAbsY()+66;
		pcsFamilyEmblem->RenderSprite(&scale, NULL, 0 , &pos2, 0xffffffff);	
	}
}

DWORD CFamilyDialog::ActionEvent( CMouse* mouseInfo )								// �йи� â�� �̺�Ʈ�� ó���ϴ� �Լ�.
{
	DWORD we = WE_NULL;																// ������ ������ �̺�Ʈ ������ �����Ѵ�.

	if( !m_bActive )		return we;												// �����찡 Ȱ��ȭ ���°� �ƴϸ�, �̺�Ʈ�� �����Ѵ�.

	we = cDialog::ActionEvent(mouseInfo);											// �⺻ ���̾�α��� �̺�Ʈ�� �޾ƿ´�.

	return we;																		// ������ �̺�Ʈ�� ���� ó�� �Ѵ�.
}

void CFamilyDialog::OnActionEvent( LONG lId, void* p, DWORD we )																		// �йи� â�� ��ư�� �̺�Ʈ ó���� �ϴ� �Լ�.
{
	CSHFamily* pFamily = HERO->GetFamily() ;																							// �йи� ������ �޾ƿ´�.

	if( we & (WE_BTNCLICK | WE_ROWCLICK) )
	{
		switch( lId )																													// �Ѿ�� ��ư�� ���̵� Ȯ���Ѵ�.
		{
		case FMY_CHANGENICKNAME :																										// ȣĪ������ Ŭ���Ͽ��ٸ�, (ȣĪ ������ ��� ����� ����� �� �ִ�.)
			{
				if( pFamily )																											// �йи� ������ ��ȿ���� üũ�Ѵ�.
				{
					CSHFamilyMember* pMember = pFamily->GetMember(m_nCurSelectIdx) ;													// ������ ��� ������ �޾ƿ´�.

					if( pMember )																										// ��� ������ ��ȿ���� üũ�Ѵ�.
					{
						CFamilyNickNameDialog* pDlg = GAMEIN->GetFamilyNickNameDlg() ;													// ȣĪ ����â ������ �޴´�.

						if( pDlg )																										// ȣĪ ����â ������ ��ȿ���� üũ�Ѵ�.
						{
							pDlg->SetActive(TRUE) ;																						// ȣĪ ����â�� Ȱ��ȭ ��Ų��.
						}
					}
				}
			}
			break ;

		case FMY_INVITE :																												// �ʴ��û�� Ŭ���Ͽ��ٸ�,
			{
				if( pFamily && pFamily->Get()->nMasterID == gHeroID )																// �йи� ������ ��ȿ�ϰ�, HERO�� ������ �̸�,
				{
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetSelectedObject() ;														// ������ �÷��̾� ������ �޴´�.

					if( pPlayer )																										// �÷��̾� ������ ��ȿ���� üũ�Ѵ�.
					{
						if( pPlayer->GetObjectKind() == eObjectKind_Player )															// �÷��̾� Ÿ���� �������� üũ�Ѵ�.
						{
							if( pPlayer->GetID() != gHeroID )																		// ������ ������ �������� üũ�Ѵ�.
							{
								CSHFamilyMember* pMember = NULL ;																		// �йи� ��� ������ ���� �����͸� �����Ѵ�.			

								for( int count = 0 ; count < MAX_MEMBER_COUNT ; ++count )												// ����� ���̵�� ���ϱ� ���� for���� ������.
								{
									pMember = pFamily->GetMember(count) ;																// ����� ������ �޴´�.

									if( pMember )																						// ��� ������ ��ȿ���� üũ�Ѵ�.
									{
										if( pMember->Get()->nID != pPlayer->GetID() )													// ����� ���̵�� ������ ����� ���̵� ���� ������,
										{
											g_csFamilyManager.CLI_RequestInvite(pPlayer) ;												// �ʴ� ��û�� �Ѵ�.
											return ;																					// ���� ó���� �Ѵ�.
										}
										else																							// ������ ����� ���̵�� ����� ���̵� ������,
										{
											WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1316) );
										}
									}

									pMember = NULL ;																					// ��� ������ ���� �����͸� NULL ó���� �Ѵ�.
								}
							}
							else
							{
								WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1317 ) );
							}							
						}
						else
						{
							WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1318 ) );
						}
					}
					else																												// �÷��̾� ������ ��ȿ���� �ʴٸ�,
					{
						WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1319 ) );
					}
				}
				else
				{
					WINDOWMGR->MsgBox( MBI_FAMILY_ABLE_MASTER, MBT_OK, CHATMGR->GetChatMsg( 732 ) );									// ���� ó���� �Ѵ�.
				}
			}
			break ;

		case FMY_REGISTMARK :																											// ���� ����� Ŭ���Ͽ��ٸ�,
			{
 				GAMEIN->GetFamilyMarkDlg()->SetActive(TRUE);			
//				WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg(528) );										// ���� ó���� �Ѵ�.
			}
			break ;

		// desc_hseos_�йи�01
		// S �йи� �߰� added by hseos 2007.10.22
		case FMY_LEAVE:
			{
		 		WINDOWMGR->MsgBox( MBI_FAMILY_LEAVE, MBT_YESNO, CHATMGR->GetChatMsg( 1142 ), HERO->GetFamily()->Get()->szName, int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_LEAVE]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_LEAVE] );
			}
			break;
		case FMY_EXPEL:
			{
				if (gHeroID == HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID)
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1163 ));
				}
				else
				{
	 		   		WINDOWMGR->MsgBox( MBI_FAMILY_EXPEL, MBT_YESNO, CHATMGR->GetChatMsg( 1143 ), HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->szName, int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_EXPEL]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_EXPEL] );
				}
			}
			break;
		case FMY_FARM_MANAGE:
			{
				GAMEIN->GetFarmManageDlg()->SetActive( TRUE );
			}
			break;
		// 091125 ONS �йи� ���̾�α׿� �йи��� �̾� ��� �߰�
		case FMY_TRANSFER:
			{
				if (gHeroID == HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID)
				{
					// ������ �ڽſ��Դ� �̾��� �� ����.
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1996 ));
				}
				else
				{
	 		   		WINDOWMGR->MsgBox( MBI_FAMILY_TRANSFER, MBT_YESNO, CHATMGR->GetChatMsg( 1990 ), HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->szName);
				}
			}
			break;
		// 091204 ONS �йи� ���̾�α׿� ���弼�� ���� ��� ��ư �߰�
		case FMY_PAYTAX:
			{
				g_csFamilyManager.CLI_RequestPayTax(2);
			}
			break;
		// E �йи� �߰� added by hseos 2007.10.22

		case FMY_MEMBER_LV_INFO_LIST:
			{
				int nClickedIndex = m_pLvListDlg->GetClickedRowIdx();
				if( nClickedIndex < 0 )
					break;

				CSHFamily* pFamily = HERO->GetFamily();
				if( !pFamily )
					break;

				CSHFamilyMember* pMember = pFamily->GetMember( nClickedIndex );
				if( !pMember )
					break;
				
				SetMemberList( pFamily );
				SetMemberInfo( pMember);
				SelectMemberOfList( nClickedIndex );
			}
			break;
		}
	}
}

void CFamilyDialog::SelectMemberOfList( int nIndex )
{
	// �� �ʱ�ȭ
	DWORD dwBaseColor = RGB_HALF(100, 100, 100);

	int nListCnt = m_pLvListDlg->GetListItem()->GetCount();
	for( int idx = 0 ; idx < nListCnt ; ++idx )
	{
		PTRLISTPOS pos = m_pLvListDlg->GetListItem()->FindIndex( idx );
		if( !pos )			continue;

		ITEM* pItem = (ITEM*)m_pLvListDlg->GetListItem()->GetAt( pos );
		if( !pItem )			continue;

		pItem->rgb = dwBaseColor;
	}

	nListCnt = m_pIDListDlg->GetListItem()->GetCount();
	for( int idx = 0 ; idx < nListCnt ; ++idx )
	{
		PTRLISTPOS pos = m_pIDListDlg->GetListItem()->FindIndex( idx );
		if( !pos )			continue;

		ITEM* pItem = (ITEM*)m_pIDListDlg->GetListItem()->GetAt( pos );
		if( !pItem )			continue;

		pItem->rgb = dwBaseColor;
	}

	ITEM* pItem = m_pLvListDlg->GetItem( nIndex );
	if( !pItem )		return;
	pItem->rgb = RGB_HALF(255, 255, 0); 									// �������� ������ �ٽ� �����Ѵ�.

	pItem = m_pIDListDlg->GetItem( nIndex );
	if( !pItem )		return;
	pItem->rgb = RGB_HALF(255, 255, 0) ; 									// �������� ������ �ٽ� �����Ѵ�.

	m_nCurSelectIdx = nIndex;

	if( m_pIDListDlg->GetClickedRowIdx() != nIndex )
		m_pIDListDlg->SetCurSelectedRowIdxWithoutRowMove( nIndex );

	if( m_pLvListDlg->GetClickedRowIdx() != nIndex )
		m_pLvListDlg->SetCurSelectedRowIdxWithoutRowMove( nIndex );
}