#include "stdafx.h"
#include ".\ChaptchaDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "ChatManager.h"
#include "cMsgBox.h"
#include "cResourceManager.h"
#include "./interface/cWindowManager.h"
#include "./interface/cButton.h"
#include "./interface/cStatic.h"
#include "./interface/cEditBox.h"
#include "./interface/cTextArea.h"
#include "exitmanager.h"
#include "ItemManager.h"
#include "GameIn.h"
#include "MHAudioManager.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "DealDialog.h"



//=================================================================================================
// NAME			: CCertificateDlg()
// PURPOSE		: The Function Constructor.
// ATTENTION	:
//=================================================================================================
CChaptchaDlg::CChaptchaDlg(void)
{
	m_type = WT_CHAPTCHA_DLG;

	m_pBtnOk		= NULL ;
	m_pBtnCancel	= NULL ;
	m_pRanNum		= NULL ;
	m_pEditBox		= NULL ;
	m_pText			= NULL ;
	m_pTimer		= NULL ;

	m_nRanNum = 0 ;
	isGameCalled = FALSE;
	m_secnumber = 0;
	m_captchatype = 0;
	//itemname =	"";

}


//=================================================================================================
// NAME			: ~CCertificateDlg()
// PURPOSE		: The Function Destructor.
// ATTENTION	:
//=================================================================================================
CChaptchaDlg::~CChaptchaDlg(void)
{
}


//=================================================================================================
// NAME			: ~Linking()
// PURPOSE		: The function for linking components to window manager.
// ATTENTION	:
//=================================================================================================
void CChaptchaDlg::Linking()
{
	m_pBtnOk		= ( cButton* )GetWindowForID(CAPTCHA_BTN_OK) ;
	m_pBtnCancel	= ( cButton* )WINDOWMGR->GetWindowForIDEx(CAPTCHA_BTN_CANCEL) ;
	m_pRanNum		= ( cStatic* )WINDOWMGR->GetWindowForIDEx(CAPTCHA_RANNUM) ;
	m_pTimer		= ( cStatic* )WINDOWMGR->GetWindowForIDEx(CAPTCHA_TIMER) ;
	m_pRanNum->SetStaticText("") ;
	m_pEditBox		= ( cEditBox* )WINDOWMGR->GetWindowForIDEx(CAPTCHA_EDITBOX) ;
	m_pEditBox->SetEditText("") ;

	m_pText			= ( cTextArea* )WINDOWMGR->GetWindowForIDEx(CAPTCHA_TEXT) ;

	m_pText->SetScriptText("Input this number");

	m_pTimer->SetStaticText("");
	
	 //Alemuri----------
	attempts = 0;
	//------------------

}

void CChaptchaDlg::SetActive(BOOL val)
{
	if(val == TRUE)
	{
		cDialog::SetActive(val);
		char bufftext[256]= { 0, };
		
		if(m_captchatype == 3)
		{
			sprintf(bufftext, "Input this number to delete          %s", itemname );
		} else if(m_captchatype == 4) {
			sprintf(bufftext, fullmsg);
		} else {
			sprintf(bufftext, "Input this number");
		}

		m_pText->SetScriptText(bufftext);

		MakeRandomNumber();
	} else {
		m_pText->SetScriptText("");
		cDialog::SetAlwaysTop(FALSE);
		cDialog::SetFocus(FALSE);
		cDialog::SetFocusEdit(FALSE);
		cDialog::SetActive(val);
	}
}

void CChaptchaDlg::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( we & WE_BTNCLICK )
	{
		switch( lId )
		{
		case CAPTCHA_BTN_OK :
			{
				if(!isGameCalled)
				{
					if( strcmp(m_pRanNum->GetStaticText(), m_pEditBox->GetEditText() ) == 0 )
					{
						isGameCalled = FALSE;
						//CHARSELECT->DeleteCharacter();
						m_pEditBox->SetEditText("") ;
						m_pRanNum->SetStaticText("") ;
//						itemname =	"";
						SetActive( FALSE ) ;
						if(m_captchatype == 3)
						{
							ITEMMGR->SendDeleteItem();
							AUDIOMGR->Play(69, gHeroID);
						} else if(m_captchatype == 4)
						{
							GAMEIN->GetDealDialog()->SendSellItemMsg();
						}
					}
					else
					{
						isGameCalled = FALSE;
						m_pEditBox->SetEditText("") ;
						m_pRanNum->SetStaticText("") ;
						//itemname =	"";
						SetActive( FALSE ) ;

						if(m_captchatype == 3)
						{
							GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
							GAMEIN->GetStorageDialog()->SetDisable( FALSE );
							{
								cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
								ASSERT( dialog );

								dialog->SetDisable( FALSE );
							}

							CHATMGR->AddMsg(CTC_SYSMSG, "Wrong captcha, cancel delete item!");
						} else if(m_captchatype == 4)
						{
							CDealDialog::OnCancelSellItem(0,NULL,0,GAMEIN->GetDealDialog(),NULL);
							CHATMGR->AddMsg(CTC_SYSMSG, "Wrong captcha, cancel sell item!");
						}

						//WINDOWMGR->MsgBox( MBI_CANCEL, MBT_OK, RESRCMGR->GetMsg(289) );
					}
				} else {
						if( strcmp("", m_pEditBox->GetEditText() ) == 0 )
						{
							m_pEditBox->SetEditText("0");
							EXITMGR->SendExitMsg( eEK_CHARSEL ); //Alemuri go back to the character selection if push Ok with the Empty EditBox
						}
						
						DWORD veirynumb = DWORD(atoi(m_pEditBox->GetEditText()));
						MSG_DWORD2 msg;
						ZeroMemory( &msg, sizeof(msg) );
						msg.Category = MP_USERCONN;
						msg.Protocol = MP_USERCONN_CAPTCHA_VERIFY;
						msg.dwObjectID = gHeroID;
						msg.dwData1 = veirynumb;
						msg.dwData2 = m_captchatype;
						NETWORK->Send( &msg, sizeof(msg) );

						isGameCalled = FALSE;

						DWORD correctNumber = atoi(m_pRanNum->GetStaticText());

						m_pEditBox->SetEditText("") ;
						m_pRanNum->SetStaticText("") ;
						m_secnumber = 0;
						m_captchatype = 0;
						SetActive( FALSE ) ;
						 //Alemuri go back to the character selection if too many attempts are wrong
						attempts += 1;

						if (correctNumber == veirynumb) //Alemuri reset the counts if the entry is correct
							attempts = 0;

						if (attempts == 3)
						{
							EXITMGR->SendExitMsg( eEK_CHARSEL );
							attempts = 0;
						}
						//---------------------------------------------------------------------------

				}
			}
			break ;

		case CAPTCHA_BTN_CANCEL :
			{
				if(!isGameCalled)
				{
					if(m_captchatype == 3)
					{
						GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
						GAMEIN->GetStorageDialog()->SetDisable( FALSE );
						{
							cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
							ASSERT( dialog );

							dialog->SetDisable( FALSE );
						}
					} else if(m_captchatype == 4)
					{
						CDealDialog::OnCancelSellItem(0,NULL,0,GAMEIN->GetDealDialog(),NULL);
					}

					isGameCalled = FALSE;
					m_pEditBox->SetEditText("") ;
					m_pRanNum->SetStaticText("") ;
//					itemname =	"";
					SetActive( FALSE ) ;
					//CHARSELECT->SetDisablePick( FALSE );

				} else {
					CHATMGR->AddMsg(CTC_SYSMSG, "Please verify that you are not a bot!");
				}
			}
			break ;
		}
	}
}


void CChaptchaDlg::MakeRandomNumber()
{
	char szStr[8] ;
	m_nRanNum = random(10, 99) ;

	if(!isGameCalled)
	{
		if( m_pRanNum )
		{
			m_pRanNum->SetStaticText("") ;
			sprintf(szStr, "%d", m_nRanNum ) ;
			m_pRanNum->SetStaticText(szStr) ;
		}
	} else {
		m_pBtnCancel->SetDisable(TRUE);
		m_pRanNum->SetStaticText("") ;
		sprintf(szStr, "%d", m_secnumber ) ;
		m_pRanNum->SetStaticText(szStr) ;
	}
}

void CChaptchaDlg::SetSecNumber(DWORD number)
{
	m_secnumber = number ;
	
	if(cDialog::IsActive())
	{
		m_pBtnCancel->SetDisable(TRUE);
		
		if(m_captchatype == 1)
		{
			m_pText->SetScriptText("Input this number to continue playing Slot Machine.");
		} else {
			m_pText->SetScriptText("Input this number to continue play.");
		}
		char szStr[8] ;
		m_pRanNum->SetStaticText("") ;
		sprintf(szStr, "%d", m_secnumber ) ;
		m_pRanNum->SetStaticText(szStr) ;
		m_pRanNum->SetFontIdx(6);
		m_pRanNum->SetFGColor(RGB_HALF(255,255,0));
	} else {

		cDialog::SetActive(TRUE);

		m_pBtnCancel->SetDisable(TRUE);

		if(m_captchatype == 1)
		{
			m_pText->SetScriptText("Input this number to continue playing Slot Machine.");
		} else {
			m_pText->SetScriptText("Input this number to continue play.");
		}
		char szStr[8] ;
		isGameCalled = TRUE;
		m_pRanNum->SetStaticText("");
		sprintf(szStr, "%d", m_secnumber );
		m_pRanNum->SetStaticText(szStr) ;
		m_pRanNum->SetFontIdx(6);
		m_pRanNum->SetFGColor(RGB_HALF(255,255,0));
	}

	cDialog::SetAlwaysTop(TRUE);
	cDialog::SetFocus(TRUE);
	cDialog::SetFocusEdit(TRUE);
}

void CChaptchaDlg::SetTimer(WORD timex)
{
	if(timex == 0 && isGameCalled)
	{
		EXITMGR->SendExitMsg( eEK_CHARSEL ); //Alemuri go back to the character selection if the captcha is not answered in 1 minute

		if( strcmp("", m_pEditBox->GetEditText() ) == 0 )
		{
			m_pEditBox->SetEditText("0");
		}
						
		DWORD veirynumb = DWORD(atoi(m_pEditBox->GetEditText()));
		MSG_DWORD2 msg;
		ZeroMemory( &msg, sizeof(msg) );
		msg.Category = MP_USERCONN;
		msg.Protocol = MP_USERCONN_CAPTCHA_VERIFY;
		msg.dwObjectID = gHeroID;
		msg.dwData1 = veirynumb;
		msg.dwData2 = m_captchatype;
		NETWORK->Send( &msg, sizeof(msg) );

		isGameCalled = FALSE;
		m_pEditBox->SetEditText("") ;
		m_pRanNum->SetStaticText("") ;
		m_secnumber = 0;
		m_captchatype = 0;
		SetActive( FALSE ) ;
	}

	char bufftime[6];
	sprintf(bufftime, "(%d)", timex);
	m_pTimer->SetStaticText(bufftime);
}

void CChaptchaDlg::SetItemnameTodelete( char* itemnamex )
{
	sprintf(itemname, "%s", itemnamex);
}

void CChaptchaDlg::SetFullmsg( char* msg )
{
	sprintf(fullmsg, "%s", msg);
}
