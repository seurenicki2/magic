#include "stdafx.h"
#include "SlotDialog.h"
#include "cWindowManager.h"
#include "WindowIDEnum.h"
#include ".\interface\cTextArea.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "FilteringTable.h"
#include "Item.h"
#include "ItemManager.h"
#include "MHTimemanager.h"
#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"
#include "../Interface/cPushupButton.h"
#include "cStatic.h"
#include "cScriptManager.h"
#include "cImage.h"
#include "hero.h"
#include "Player.h"
#include "./Audio/MHAudioManager.h"
#include "MHMap.h"
#include "ObjectStateManager.h"


CSlotDialog::CSlotDialog()
{
	m_type = WT_SLOT_DLG;

	m_pSpinBtn		= NULL ;
	m_pBetminBtn	= NULL ;
	m_pBet2xBtn		= NULL ;
	m_pBet3xBtn		= NULL ;
	m_pBetmaxBtn	= NULL ;
	m_pCancelBtn	= NULL ;
	m_pCloseBtn		= NULL ;
	//m_Depositvalue	= 100000;
	//m_pWait			= FALSE;

}

CSlotDialog::~CSlotDialog()
{
	
}

void CSlotDialog::Linking()
{
	m_pSpinBtn		= (cButton*)GetWindowForID(SLOT_BTN_SPIN);	
	m_pBetminBtn	= (cButton*)GetWindowForID(SLOT_BTN_BETMIN);	
	m_pBet2xBtn		= (cButton*)GetWindowForID(SLOT_BTN_BET2X);	
	m_pBet3xBtn		= (cButton*)GetWindowForID(SLOT_BTN_BET3X);	
	m_pBetmaxBtn	= (cButton*)GetWindowForID(SLOT_BTN_BETMAX);	
	m_pCancelBtn	= (cButton*)GetWindowForID(SLOT_BTN_CANCEL);
	m_pCloseBtn		= (cButton*)GetWindowForID(SLOT_BTN_CLOSE);
	m_pDeposit		= (cStatic*)GetWindowForID(SLOT_DEPOSIT);
	m_pWinmoney		= (cStatic*)GetWindowForID(SLOT_WINMONEY);
	m_pCharmoney	= (cStatic*)GetWindowForID(SLOT_CHARMONEY);
	m_pInfowin		= (cStatic*)GetWindowForID(SLOT_INFOWIN);
	m_pSpin1		= (cStatic*)GetWindowForID(SLOT_IMG1);
	m_pSpin2		= (cStatic*)GetWindowForID(SLOT_IMG2);
	m_pSpin3		= (cStatic*)GetWindowForID(SLOT_IMG3);
	m_pSpin4		= (cStatic*)GetWindowForID(SLOT_IMG4);
	m_pJackpot		= (cStatic*)GetWindowForID(SLOT_JACKPOT);

	//m_pDeposit->SetStaticText(AddComma(m_Depositvalue));
	//m_pWinmoney->SetStaticText("0");
	m_pInfowin->SetStaticText("Spin Slot");
	m_pSpin1->SetStaticText("7");
	m_pSpin2->SetStaticText("7");
	m_pSpin3->SetStaticText("7");
	m_pSpin4->SetStaticText("7");
	m_pWinmoney->SetActive(FALSE);
	//m_pWait	= FALSE;
}

void CSlotDialog::SetActive(BOOL val)
{
	if(MAP->GetMapNum() == StreetTournament || MAP->GetMapNum() == PVP_MAP)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, "Slot Machine is disable at this map" ) ;
		val = FALSE;
	}

	if( m_bDisable ) return;
	cDialog::SetActive(val);

	m_pWait	= FALSE;
	if(val == TRUE)
	{
		m_pCharmoney->SetStaticText(AddComma(HERO->GetMoney()));

		CHero* pemain = OBJECTMGR->GetHero();
		m_pJackpot->SetStaticText(AddComma(pemain->GetMapJackpot()));

		m_pBetminBtn->SetDisable(FALSE);	
		m_pBet2xBtn->SetDisable(FALSE);	
		m_pBet3xBtn->SetDisable(FALSE);	
		m_pBetmaxBtn->SetDisable(FALSE);	
		m_pCancelBtn->SetDisable(FALSE);
		m_pCloseBtn->SetDisable(FALSE);
		m_pSpin1->SetStaticText("7");
		m_pSpin2->SetStaticText("7");
		m_pSpin3->SetStaticText("7");
		m_pSpin4->SetStaticText("7");

		m_Depositvalue	= 200000;
		m_pDeposit->SetStaticText(AddComma(m_Depositvalue));
		m_pWinmoney->SetActive(FALSE);
	}
}

DWORD CSlotDialog::ActionEvent(CMouse * mouseInfo)
{
	if(IsDisable())
	{
		return WE_NULL;
	}

	const DWORD we = cDialog::ActionEvent( mouseInfo );

	return we ;
}

void CSlotDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	if( event != WE_BTNCLICK ) return ;

	switch(id)
	{
	case SLOT_BTN_SPIN:
		{
			if (HERO->GetMoney() < 10000000)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, "Gold minimum for play spin is 10.000.000!");
				return;
			}

			CPlayer* playernya = (CPlayer*)OBJECTMGR->GetObject( HERO->GetID() );
			if(!playernya) return;

			if(playernya->GetVeirfyCaptchaState())
			{
				CHATMGR->AddMsg( CTC_SYSMSG, "Please Veiry Captcha for continue play Slot Machine");
				return;
			}
			
			WORD cheatspincheck = 0;
			if (m_Depositvalue == 200000)
			{
				cheatspincheck += 1;
			} else if (m_Depositvalue == 400000)
			{
				cheatspincheck += 1;
			} else if (m_Depositvalue == 600000)
			{
				cheatspincheck += 1;
			} else if (m_Depositvalue == 1000000)
			{
				cheatspincheck += 1;
			} else {
				cheatspincheck = 0;
			}

			if (cheatspincheck == 0) 
			{
				CHATMGR->AddMsg( CTC_SYSMSG, "Error, please push bet button & try again");
				return;
			}

			m_pInfowin->SetStaticText("Spin...");
			m_pWinmoney->SetStaticText("0");
			m_pWinmoney->SetActive(FALSE);
			m_pWait	= TRUE;

			CHero* pemain = OBJECTMGR->GetHero();
			
			pemain->SetSlotStart(TRUE);
						
			m_pSpinBtn->SetDisable(TRUE);
			m_pBetminBtn->SetDisable(TRUE);	
			m_pBet2xBtn->SetDisable(TRUE);
			m_pBet3xBtn->SetDisable(TRUE);
			m_pBetmaxBtn->SetDisable(TRUE);	
			m_pCancelBtn->SetDisable(TRUE);
			m_pCloseBtn->SetDisable(TRUE);

			MSG_DWORD msgslotsyn;
			memset(&msgslotsyn, 0, sizeof(MSG_DWORD));
			msgslotsyn.Category = MP_NOTE;
			msgslotsyn.Protocol = MP_NOTE_SLOT_SYN;
			msgslotsyn.dwObjectID = pemain->GetID();
			msgslotsyn.dwData	= m_Depositvalue;
	
			NETWORK->Send(&msgslotsyn,sizeof(msgslotsyn)) ;
			
			CObject* pObject = OBJECTMGR->GetObject(HERO->GetID());
			OBJECTSTATEMGR->StartObjectState(pObject, eObjectState_Deal);

		}
		break;
	case SLOT_BTN_BETMIN:
		{
			m_Depositvalue	= 200000;
			m_pDeposit->SetStaticText(AddComma(m_Depositvalue));
		}
		break;
	case SLOT_BTN_BET2X:
		{
			m_Depositvalue	= 400000;
			m_pDeposit->SetStaticText(AddComma(m_Depositvalue));
		}
		break;
	case SLOT_BTN_BET3X:
		{
			m_Depositvalue	= 600000;
			m_pDeposit->SetStaticText(AddComma(m_Depositvalue));
		}
		break;
	case SLOT_BTN_BETMAX:
		{
			m_Depositvalue	= 1000000;
			m_pDeposit->SetStaticText(AddComma(m_Depositvalue));
		}
		break;
	case SLOT_BTN_CANCEL:
		{
			m_Depositvalue	= 0;
			SetActive(FALSE);
		}
		break;
	case SLOT_BTN_CLOSE:
		{
			m_Depositvalue	= 0;
			SetActive(FALSE);
		}
		break;
	}
}


void CSlotDialog::Prosses()
{
	AUDIOMGR->Play(OVER_SOUND_IDX, gHeroID);

	WORD p_rand1	= random(1, 7);
	WORD p_rand2	= random(1, 7);
	WORD p_rand3	= random(1, 7);
	WORD p_rand4	= random(1, 7);
		
	char buff1[2] = {0,};
	char buff2[2] = {0,};
	char buff3[2] = {0,};
	char buff4[2] = {0,};
	sprintf(buff1, "%d", p_rand1);
	sprintf(buff2, "%d", p_rand2);
	sprintf(buff3, "%d", p_rand3);
	sprintf(buff4, "%d", p_rand4);
	m_pSpin1->SetStaticText(buff1);
	m_pSpin2->SetStaticText(buff2);
	m_pSpin3->SetStaticText(buff3);
	m_pSpin4->SetStaticText(buff4);
	
}

void CSlotDialog::SetResultSpin(MONEYTYPE Moneyresult, WORD slot1, WORD slot2, WORD slot3, WORD slot4, MONEYTYPE totjackpot, WORD wincode)
{
	if (!IsActive()) return;

	char buff1[2] = {0,};
	char buff2[2] = {0,};
	char buff3[2] = {0,};
	char buff4[2] = {0,};
	sprintf(buff1, "%d", slot1);
	sprintf(buff2, "%d", slot2);
	sprintf(buff3, "%d", slot3);
	sprintf(buff4, "%d", slot4);
	m_pSpin1->SetStaticText(buff1);
	m_pSpin2->SetStaticText(buff2);
	m_pSpin3->SetStaticText(buff3);
	m_pSpin4->SetStaticText(buff4);

    m_pWinmoney->SetActive(TRUE);	
	m_pWinmoney->SetStaticText(AddComma(Moneyresult));
	m_pCharmoney->SetStaticText(AddComma(HERO->GetMoney()));
	m_pJackpot->SetStaticText(AddComma(totjackpot));
	CObject* pObject = OBJECTMGR->GetObject(HERO->GetID());

	if (Moneyresult > 0 && wincode != 0)
	{
		switch (wincode)
		{
		case 1:
			{
				CHATMGR->AddMsg( CTC_GETMONEY, "Win jackpot 7777 get %s gold!", AddComma(Moneyresult));
				m_pInfowin->SetStaticText("WIN 7777");
			} break;
		case 2:
			{
				CHATMGR->AddMsg( CTC_GETMONEY, "Win jackpot 777 get %s gold!", AddComma(Moneyresult));
				m_pInfowin->SetStaticText("WIN 777");
			} break;
		case 3:
			{
				CHATMGR->AddMsg( CTC_GETMONEY, "Win jackpot 77 get %s gold!", AddComma(Moneyresult));
				m_pInfowin->SetStaticText("WIN 77");
			} break;
		case 4:
			{
				CHATMGR->AddMsg( CTC_GETMONEY, "Win %s gold!", AddComma(Moneyresult));
				m_pInfowin->SetStaticText("WIN xxxx");
			} break;
		case 5:
			{
				CHATMGR->AddMsg( CTC_GETMONEY, "Win %s gold!", AddComma(Moneyresult));
				m_pInfowin->SetStaticText("WIN xxx");
			} break;
		case 6:
			{
				CHATMGR->AddMsg( CTC_GETMONEY, "Win %s gold!", AddComma(Moneyresult));
				m_pInfowin->SetStaticText("WIN xx");
			} break;
		}
		
		OBJECTSTATEMGR->EndObjectState(pObject, eObjectState_Deal);
		OBJECTEFFECTDESC desc( FindEffectNum("char_m_fishing_success_E.beff") );
		pObject->AddObjectEffect( FISHING_SUCCESS_EFFECT, &desc, 1 );

	} else {
		m_pInfowin->SetStaticText("Lose...");

		OBJECTSTATEMGR->EndObjectState(pObject, eObjectState_Deal);
		OBJECTEFFECTDESC desc( FindEffectNum("char_m_fishing_failure_E.beff") );
		pObject->AddObjectEffect( FISHING_FAIL_EFFECT, &desc, 1 );
	}


	m_pWait	= FALSE;
	m_pSpinBtn->SetDisable(FALSE);
	m_pBetminBtn->SetDisable(FALSE);	
	m_pBet2xBtn->SetDisable(FALSE);
	m_pBet3xBtn->SetDisable(FALSE);
	m_pBetmaxBtn->SetDisable(FALSE);	
	m_pCancelBtn->SetDisable(FALSE);
	m_pCloseBtn->SetDisable(FALSE);

	CHero* pemain = OBJECTMGR->GetHero();
	pemain->SetSlotStart(FALSE);
}

void CSlotDialog::UpdateJackpot(MONEYTYPE mJackpot)
{
	m_pJackpot->SetStaticText(AddComma(mJackpot));
}