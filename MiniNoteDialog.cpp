// MiniNoteDialog.cpp: implementation of the CMiniNoteDialogWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniNoteDialog.h"
#include "WindowIDEnum.h"
#include ".\Interface\cWindowManager.h"
#include ".\interface\cIconDialog.h"
#include "Interface/cScriptManager.h"
#include "cImeEx.h"
#include ".\interface\cTextArea.h"
#include ".\interface\cEditBox.h"
#include ".\interface\cStatic.h"
#include ".\Interface\cSpin.h"

#include "Item.h"
#include "VirtualItem.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include ".\input\Mouse.h"
#include "CommonCalcFunc.h"
#include "cMsgBox.h"
#include ".\Input\UserInput.h"
#include "cDivideBox.h"
#include "cMoneyDivideBox.h"
#include "InventoryExDialog.h"
#include ".\interface\cResourceManager.h"
#include "QuickSlotDlg.h"
#include "GameIn.h"
#include "windows.h"
#include "MHFile.h"
#include "NoteManager.h"
#include "MHMap.h"
#include "ObjectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMiniNoteDialogWrite::CMiniNoteDialogWrite(void)
{
	m_type = WT_MININOTEDLG;
	m_CurMiniNoteMode = -1;
	m_sendMoneyNya = 0;
	m_sendPajakNya = 0;
	itemDbidxnya = 0;
//	mIconDialog( 0, 0 );
	mSourceItem = new CVirtualItem;
}

CMiniNoteDialogWrite::~CMiniNoteDialogWrite(void)
{
	m_MinNoteCtlListArray.RemoveAll();
}

void CMiniNoteDialogWrite::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_MININOTEDLG;
}

void CMiniNoteDialogWrite::Linking()
{
	m_pTitleEdit = (cEditBox*)GetWindowForID(NOTE_MWTITLE);
	m_pTitleEdit->SetEditText("");

	//쓰기
	m_pWNoteText = (cTextArea*)GetWindowForID(NOTE_MWNOTETEXTREA);
	m_pWNoteText->SetEnterAllow(FALSE);
	m_pReceiver = (cStatic*)GetWindowForID(NOTE_MRECEIVER); //받는이 :
	

	m_pMReceiverEdit = (cEditBox*)GetWindowForID(NOTE_MRECEIVEREDIT); //아무개
	m_pMReceiverEdit->SetValidCheck( VCM_CHARNAME );
	m_pMReceiverEdit->SetEditText("");
	
	m_pSendOkBtn	= (cButton*)GetWindowForID(NOTE_MSENDOKBTN);
	mIconDialog		= (cIconDialog*)GetWindowForID(NOTE_ITEM_PACKAGE_WRITE);
	m_pPackMoney	= (cStatic*)GetWindowForID(NOTE_MPACKAGEMONEY_WRITE);
	m_pAddMoney		= (cButton*)GetWindowForID(NOTE_MPACKAGEMONEY_WRITEADD);
	m_pTaxMoney		= (cStatic*)GetWindowForID(NOTE_MMONEYTAX_WRITE);
	m_pPackMoney->SetStaticText("0");
	m_pTaxMoney->SetStaticText("0");

	m_sendPajakNya = 0;
	m_sendMoneyNya = 0;
	mSourceItem = new CVirtualItem;
	
	m_MinNoteCtlListArray.AddTail(m_pTitleEdit);
	m_MinNoteCtlListArray.AddTail(m_pWNoteText);
	m_MinNoteCtlListArray.AddTail(m_pMReceiverEdit);
	m_MinNoteCtlListArray.AddTail(m_pSendOkBtn);
	m_MinNoteCtlListArray.AddTail(m_pReceiver);
	m_MinNoteCtlListArray.AddTail(mIconDialog);
	m_MinNoteCtlListArray.AddTail(m_pPackMoney);
	m_MinNoteCtlListArray.AddTail(m_pAddMoney);
	m_MinNoteCtlListArray.AddTail(m_pTaxMoney);


}

void CMiniNoteDialogWrite::SetMiniNote(char* Sender, char* Note, WORD ItemIdx)
{
	char buf[300] = { 0, };	
	
	if( ItemIdx > 0 )
	{
		ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( ItemIdx );

		if( pItemInfo )
			sprintf( buf, CHATMGR->GetChatMsg(938), pItemInfo->ItemName );
	}

	strcat( buf, Note );
	m_pMReceiverEdit->SetEditText(Sender);
	m_pWNoteText->SetScriptText(buf);
}


int CMiniNoteDialogWrite::GetCurMode()
{
	return m_CurMiniNoteMode;
}

void CMiniNoteDialogWrite::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(MAP->GetMapNum() == StreetTournament && val)
	{
		val = FALSE;
	}

	if(val == TRUE)
	{
		m_pTitleEdit->SetEditText("");
		m_pWNoteText->SetScriptText("");
		m_pMReceiverEdit->SetEditText("");
		m_pPackMoney->SetStaticText("0");
		m_pTaxMoney->SetStaticText("0");
		itemDbidxnya = 0;
		mSourceItem = new CVirtualItem;
		m_sendPajakNya = 0;
		m_sendMoneyNya = 0;
		
	}
	else
	{
		m_pMReceiverEdit->SetFocusEdit(FALSE);
		m_pWNoteText->SetFocusEdit(FALSE);
		m_pPackMoney->SetStaticText("0");
		m_pTaxMoney->SetStaticText("0");
		itemDbidxnya = 0;
		cIcon* pIcon;
		mIconDialog->DeleteIcon( 0, &pIcon );
		CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );
		if( old )
			old->SetLock( FALSE );
		mSourceItem = new CVirtualItem;

		m_sendPajakNya = 0;
		m_sendMoneyNya = 0;
	}

	PTRLISTSEARCHSTART(m_MinNoteCtlListArray, cWindow*,pWin)
	pWin->SetActive(val);
	PTRLISTSEARCHEND

	cDialog::SetActive(val);
}

void CMiniNoteDialogWrite::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( we & WE_BTNCLICK )
	{
		switch( lId )
		{
		case NOTE_MSENDOKBTN :
			{
				char pname[MAX_NAME_LENGTH+1] = {0};
				SafeStrCpy(pname, m_pMReceiverEdit->GetEditText(), MAX_NAME_LENGTH+1);
				char prname[MAX_NAME_LENGTH+1] = {0};
				SafeStrCpy(prname, HERO->GetObjectName(), MAX_NAME_LENGTH+1);
				if(strlen(pname) == 0)
				{
					CHATMGR->AddMsg( CTC_BUYITEM, "Please input Target Name !!!");
					return;
				}
				if(  strcmpi ( pname, prname ) == 0 ) //|| strncmp ( pname, prname, sizeof ( pname ) ) == 0 )  //Block Send to Self //Equal
				{
					CHATMGR->AddMsg( CTC_CHEAT_1, "Error : You Can't Send Note to Yourself !!!");
					return;
				}
				char ptitle[MAX_NOTE_TITLE+1] = {0,};
				SafeStrCpy(ptitle, m_pTitleEdit->GetEditText(), MAX_NOTE_TITLE+1);
				int len = strlen(ptitle);
				if(len <= 2)
				{
					CHATMGR->AddMsg( CTC_BUYITEM, "Please input Title, Minimum 3 Character !!!");
					return;
				}

				if(len >= MAX_NOTE_TITLE)
				{
					len = MAX_NOTE_TITLE - 1;
					ptitle[len] = 0;
				}

				char FullText[512];
				m_pWNoteText->GetScriptText(FullText);
	
				MONEYTYPE kirimduit = m_sendMoneyNya;
				POSTYPE positionnya = 0;
				CItem* kirimanya = ( CItem* )( mSourceItem->GetLinkItem() );
							
				if (kirimanya)
				{
					itemDbidxnya = kirimanya->GetDBIdx();
					positionnya = kirimanya->GetPosition();
					kirimanya->SetLock(FALSE);
				} else {
					itemDbidxnya = 0;
				}

				char Note[MAX_NOTE_LENGTH];
				SafeStrCpy(Note, FullText, MAX_NOTE_LENGTH - 1);
				Note[MAX_NOTE_LENGTH - 1] = 0;
				NOTEMGR->WriteNote(Note, ptitle, pname, itemDbidxnya, positionnya, kirimduit);

				mSourceItem = new CVirtualItem;
				SetActive(FALSE);	
			}
			break ;

		case NOTE_MPACKAGEMONEY_WRITEADD :
			{

					LONG x = MOUSE->GetMouseEventX();
					LONG y = MOUSE->GetMouseEventY();
				
					cMoneyDivideBox* pMoneyDivideBox = WINDOWMGR->MoneyDivideBox(DBOX_INPUT_MONEY, x, y, OnInputMoney, OnInputCancel, this, NULL, CHATMGR->GetChatMsg(29) );
					if( !pMoneyDivideBox )
						pMoneyDivideBox = (cMoneyDivideBox*)WINDOWMGR->GetWindowForID( DBOX_INPUT_MONEY );
					
					pMoneyDivideBox->ChangeKind( 0 );
					pMoneyDivideBox->SetMaxValue( MAXMONEY );
					pMoneyDivideBox->SetValue( 0 );
					
					pMoneyDivideBox->m_pSpin->SetFocusEdit( TRUE );
					pMoneyDivideBox->m_pSpin->SetEditText( AddComma(m_sendMoneyNya) );

					SetDisable(TRUE);
			}
			break ;
		case CMI_CLOSEBTN:
			{
				SetActive(FALSE);
			}
			break;
		}
	}
}

void CMiniNoteDialogWrite::OnInputMoney( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CMiniNoteDialogWrite* pDlg = (CMiniNoteDialogWrite*)vData1;
	
	if( !pDlg->IsActive() ) return;

	pDlg->SetMoney(param1);
	pDlg->SetDisable( FALSE );

}

void CMiniNoteDialogWrite::OnInputCancel( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CMiniNoteDialogWrite* pDlg = (CMiniNoteDialogWrite*)vData1;

	if( !pDlg->IsActive() ) return;

	pDlg->SetMoney(0);
	pDlg->SetDisable( FALSE );
}

void CMiniNoteDialogWrite::SetMoney(MONEYTYPE dweMoney)
{
	if (dweMoney > MAXMONEY2B)//MAXMONEY100B)
	{
		CHATMGR->AddMsg( CTC_BUYITEM, "Max send money is 2,000,000,000" );
		CHATMGR->AddMsg( CTC_BUYITEM, "Note cancelled." );
		m_sendPajakNya = 0;
		m_sendMoneyNya = 0;
		SetActive(FALSE);
		return;
	}

	if ((m_sendPajakNya +  (dweMoney * 3 / 100)) > HERO->GetMoney())
	{
		CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(324) );
		CHATMGR->AddMsg( CTC_BUYITEM, "Note cancelled." );
		m_sendPajakNya = 0;
		m_sendMoneyNya = 0;
		SetActive(FALSE);
		return;
	}

	if (dweMoney == 0)
	{
		m_sendMoneyNya = 0;
		m_pPackMoney->SetStaticText( "0" );
	}


	m_sendMoneyNya = dweMoney;
	m_pPackMoney->SetStaticText( AddComma(dweMoney ) );
	MONEYTYPE pajakbumi = MONEYTYPE((dweMoney * 3) / 100);
	m_sendPajakNya += pajakbumi;
	m_pTaxMoney->SetStaticText( AddComma(m_sendPajakNya) );
	if (dweMoney > 0)
	{
		CHATMGR->AddMsg( CTC_BUYITEM, "Attach %s gold note have special tax 3 percent(%s gold)", AddComma(dweMoney), AddComma(pajakbumi) );
	}
}

BOOL CMiniNoteDialogWrite::FakeMoveIcon( LONG x, LONG y, cIcon* icon )
{
	if( WT_ITEM != icon->GetType() || icon->IsLocked() || m_bDisable )
	{
		return FALSE;
	}
	itemDbidxnya = 0;

	if( ((CItem*)icon)->GetItemInfo() == NULL )
		return FALSE;

	if( ( GetTableIdxPosition( ((CItem*)icon)->GetPosition() ) != eItemTable_Inventory ))
	{
		if( GetTableIdxPosition( ((CItem*)icon)->GetPosition() ) == eItemTable_Weared )
			CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(435) );
		
		return FALSE;
	}

	if( ((CItem*)icon)->GetItemInfo()->Category == eItemCategory_Expend )
	{
		if( QUICKMGR->CheckQPosForItemIdx(((CItem*)icon)->GetItemIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(62) );
			return FALSE;
		}
	}
	else
	{
		if( QUICKMGR->CheckQPosForDBIdx(((CItem*)icon)->GetDBIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(62) );
			return FALSE;
		}
	}

	if( ((CItem*)icon)->GetItemInfo()->Category == eItemCategory_Expend )
	{
		if( QUICKMGR->CheckQPosForItemIdx(((CItem*)icon)->GetItemIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(437) );
			return FALSE;
		}
	}
	else
	{
		if( ((CItem*)icon)->IsQuickLink() != 0 )
		{
			CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(437) );
			return FALSE;
		}
	}
	
	cIcon* pIcon;
	CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );
	if( old )
	{
		mIconDialog->DeleteIcon( 0, &pIcon );
		old->SetLock( FALSE );
		mSourceItem = new CVirtualItem;
	}

	CItem* pItem = (CItem *)icon;
	if (!pItem->GetItemInfo()->Trade && !pItem->GetItemInfo()->Sell && !pItem->GetItemInfo()->Deposit)
	{
		CHATMGR->AddMsg( CTC_BUYITEM, "Only allowed send item mark Sell, Exchange & Storage." );
		return FALSE;
	}

	if(pItem->GetItemInfo()->wSeal == eITEM_TYPE_SEAL && pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_UNSEAL)
	{
		CHATMGR->AddMsg( CTC_BUYITEM, "Can't sent unsealed items." );
		return FALSE;
	}

	if(pItem->GetItemInfo()->Stack > 0)//block Stackable item (will cause duplicate bug)
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, "Error : Unable send Stackable Items." );
		return FALSE;
	}

	if(pItem->GetItemInfo()->SupplyType == ITEM_KIND_PET && pItem->GetItemBaseInfo().nSealed == eITEM_TYPE_UNSEAL)
	{
		CHATMGR->AddMsg( CTC_BUYITEM, "Can't sent unsealed pet." );
		return FALSE;
	}
	
	if (ITEMMGR->IsDupItem( pItem->GetItemIdx() ))
	{
		DWORD Durability = pItem->GetDurability();
		m_sendPajakNya += Durability * 10000;
		CHATMGR->AddMsg( CTC_BUYITEM, "Tax per item 10.000, total tax is %s.", AddComma(m_sendPajakNya) );
	} else {
		const ITEM_OPTION& itemoption = ITEMMGR->GetOption( pItem->GetItemBaseInfo() );
		if (itemoption.mEnchant.mLevel < 8)
		{
			m_sendPajakNya += 200000;
		} else if (itemoption.mEnchant.mLevel < 13 &&  itemoption.mEnchant.mLevel > 7)
		{
			m_sendPajakNya += 500000;
		} else if  (itemoption.mEnchant.mLevel < 16  &&  itemoption.mEnchant.mLevel > 12)
		{
			m_sendPajakNya += 1500000;
		} else {
			m_sendPajakNya += 5000000;
		}

		char bufitem[128] = {0,};
		if (itemoption.mEnchant.mLevel > 0)
		{
			sprintf(bufitem, "+%d %s", itemoption.mEnchant.mLevel, pItem->GetItemInfo()->ItemName) ;
		} else {
			sprintf(bufitem, "%s", pItem->GetItemInfo()->ItemName) ;
		}

		CHATMGR->AddMsg( CTC_BUYITEM, "Tax sent %s is %s.", bufitem, AddComma(m_sendPajakNya) );
	}
	
	if (m_sendPajakNya > HERO->GetMoney())
	{
		CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg(324) );
		m_pTaxMoney->SetStaticText( "" );
		m_pPackMoney->SetStaticText( "" );
		cIcon* pIcon;
		mIconDialog->DeleteIcon( 0, &pIcon );
		CItem* old = ( CItem* )( mSourceItem->GetLinkItem() );
		if( old )
			old->SetLock( FALSE );

		m_sendPajakNya = 0;
		m_sendMoneyNya = 0;
		mSourceItem = new CVirtualItem;
		CHATMGR->AddMsg( CTC_BUYITEM, "Note cancelled." );
		SetActive(FALSE);
		return FALSE;
	}
	
	m_pTaxMoney->SetStaticText( AddComma(m_sendPajakNya) );

	mSourceItem->SetData( pItem->GetItemIdx() );
	mSourceItem->SetLinkItem( pItem );

	ITEMMGR->AddToolTip( mSourceItem );

	mIconDialog->AddIcon( 0, mSourceItem );
	itemDbidxnya = pItem->GetDBIdx();
	
	pItem->SetLock( TRUE );
	mIconDialog->SetDisable(TRUE);


	return FALSE;
}
