/*************************************************************************************************************
 *
 *	File Name	::	cAntiBotDialog.cpp
 *
 *	Owner		::	Death
 *
 *	Purpose Application	::	AntiBot , Auto Joy By has dialog display if player atk more one hour
 *
 *	Change Log	::
 *
 *	Death , Sep 05		Initial Version
 *
 *
 *************************************************************************************************************/

//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include "stdafx.h"
#include "cAntiBotDialog.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cMsgBox.h"
#include "cResourceManager.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"

#include "./interface/cWindowManager.h"
#include "./interface/cButton.h"
#include "./interface/cStatic.h"
#include "./interface/cEditBox.h"
#include "./interface/cTextArea.h"

#include "ExitManager.h"


//=================================================================================================
// NAME			: CAntiBotDialog()
// PURPOSE		: The Function Constructor.
// ATTENTION	:
//=================================================================================================
CAntiBotDialog::CAntiBotDialog(void)
{
	m_pBtnOk		= NULL ;
	m_pBtnCancel	= NULL ;
	m_pRanNum		= NULL ;
	m_pEditBox		= NULL ;
	m_pText			= NULL ;

	m_nRanNum = 0 ;
}


//=================================================================================================
// NAME			: ~CAntiBotDialog()
// PURPOSE		: The Function Destructor.
// ATTENTION	:
//=================================================================================================
CAntiBotDialog::~CAntiBotDialog(void)
{

}

//=================================================================================================
// NAME			: ~Linking()
// PURPOSE		: The function for linking components to window manager.
// ATTENTION	:
//=================================================================================================
void CAntiBotDialog::Linking()
{
	m_pBtnOk		= ( cButton* )GetWindowForID(ANTIBOT_BTN_OK) ;	

	m_pRanNum		= ( cStatic* )WINDOWMGR->GetWindowForIDEx(ANTIBOT_ST_RANNUM) ;
	m_pRanNum->SetStaticText("") ;

	m_pEditBox		= ( cEditBox* )WINDOWMGR->GetWindowForIDEx(ANTIBOT_EB_EDITBOX) ;
	m_pEditBox->SetValidCheck( VCM_CHARNAME ) ;
	m_pEditBox->SetEditText("") ;
	m_pEditBox->SetTextOffset(5,5,3) ;

	m_pText			= ( cTextArea* )WINDOWMGR->GetWindowForIDEx(ANTIBOT_TA_TEXT) ;
	// 070118 LYW --- Delete this line.
	//m_pText->SetScriptText("정말 캐릭터를 삭제 하시려면 화면에 보이는 숫자를 입력하십시오.") ;

	
	// 070117 LYW --- Add static control for title.
	// 070118 LYW --- Delete this line.
	//m_pTitle		= ( cStatic* )WINDOWMGR->GetWindowForIDEx(CTF_ST_TEXT) ;
	//m_pTitle->SetStaticText("◈알림") ;
}


//=================================================================================================
// NAME			: OnActionEvent()
// PURPOSE		: The function for process event.
// ATTENTION	:
//=================================================================================================
void CAntiBotDialog::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( we & WE_BTNCLICK )
	{
		switch( lId )
		{
		case ANTIBOT_BTN_OK :
			{
				if( strcmp(m_pRanNum->GetStaticText(), m_pEditBox->GetEditText() ) != 0 )
				{
					// Go To Select character Screen
					EXITMGR->SendExitMsg( eEK_CHARSEL );
				}				
				m_pEditBox->SetEditText("") ;
				m_pRanNum->SetStaticText("") ;
				SetActive( FALSE ) ;
						
				WINDOWMGR->UnHideAllActivedWindow();										
				WINDOWMGR->SetOpendAllWindows( TRUE ) ;										
				WINDOWMGR->ShowBaseWindow() ;
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
				//cWindow::SetActive(0);
				
			}
			break ;
		}
	}
}


//=================================================================================================
// NAME			: OnActionEvent()
// PURPOSE		: Make random number for certificate some condition.
// ATTENTION	:
//=================================================================================================
void CAntiBotDialog::MakeRandomNumber()
{
	char szStr[8] ;
	m_nRanNum = random(0, 99) ;

	if( m_pRanNum )
	{
		m_pRanNum->SetStaticText("") ;
		sprintf(szStr, "%d", m_nRanNum ) ;
		m_pRanNum->SetStaticText(szStr) ;
	}
}


