#include "stdafx.h"
#include "ExchangeDialog.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cScriptManager.h"
#include "./Interface/cIconGridDialog.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cStatic.h"

#include "ExchangeManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "ExchangeItem.h"
#include "Item.h"

#include "ProgressDialog.h"
#include "InventoryExDialog.h"
#include "./Input/UserInput.h"

#include "ItemManager.h"
#include "QuickManager.h"

// 071125 LYW --- ExchangeDialog : ∏ﬁΩ√¡ˆ π⁄Ω∫ «Ï¥ı ∆˜«‘.
#include "cMsgBox.h"
#include "MHMap.h"


CExchangeDialog::CExchangeDialog()
{
	m_bInit				= FALSE;
	m_pMyIconGridDlg	= NULL;
	m_pOpIconGridDlg	= NULL;
	m_pMyMoneyEditBox	= NULL;
	m_pOpMoneyEditBox	= NULL;
	m_pLockBtn			= NULL;
	m_pExchangeBtn		= NULL;
	m_pInputMoneyBtn	= NULL;

	m_pMyNameStc		= NULL;
	m_pOpNameStc		= NULL;

	m_bMyLock			= FALSE;
	m_bOpLock			= FALSE;

	m_dwMoney			= 0;
	m_dwOpMoney			= 0;

	m_bExchangable		= TRUE;

	m_nMyItemNum		= 0;
	m_nOpItemNum		= 0;	
}

CExchangeDialog::~CExchangeDialog()
{

}

void CExchangeDialog::Linking()
{
	m_pMyIconGridDlg	= (cIconGridDialog*)GetWindowForID( XCI_ICD_MINE );
	m_pOpIconGridDlg	= (cIconGridDialog*)GetWindowForID( XCI_ICD_OPPOSITE );
	m_pMyMoneyEditBox	= (cEditBox*)GetWindowForID( XCI_EB_MYMONEY );
	m_pMyMoneyEditBox->SetReadOnly( TRUE );				//Ω∫≈©∏≥∆Æ∑Œ
	m_pMyMoneyEditBox->SetAlign( TXT_RIGHT );
//	m_pMyMoneyEditBox->SetValidCheck( VCM_NUMBER );
	m_pOpMoneyEditBox	= (cEditBox*)GetWindowForID( XCI_EB_OPMONEY );
	m_pMyMoneyEditBox->SetReadOnly( TRUE );
	m_pOpMoneyEditBox->SetAlign( TXT_RIGHT );
	m_pLockBtn			= (cPushupButton*)GetWindowForID( XCI_BTN_LOCK );
	m_pExchangeBtn		= (cPushupButton*)GetWindowForID( XCI_BTN_EXCHANGE );
	m_pInputMoneyBtn	= (cButton*)GetWindowForID( XCI_BTN_INPUTMONEY );
	
	m_pMyNameStc		= (cStatic*)GetWindowForID( XCI_INFO2 );
	m_pOpNameStc		= (cStatic*)GetWindowForID( XCI_INFO1 );
	//m_pLockImage		= WINDOWMGR->GetImageOfFile( "./Data/Interface/2DImage/GameIn/28/120210.tif", 28 );
	SCRIPTMGR->GetImage( 77, &m_LockImage, PFT_HARDPATH );

	m_bInit				= TRUE;
}
/*
DWORD CExchangeDialog::ActionEvent( CMouse* mouseInfo, CKeyboard* keyInfo )
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;

//	LONG x = mouseInfo->GetClickDownX();
//	LONG y = mouseInfo->GetClickDownY();

//	if( mouseInfo->LButtonClick() )
//	{
//		we = m_pInputMoneyBtn->ActionEvent( mouseInfo, keyInfo );
//		if( !( we & WE_LBTNCLICK ) )
//		{
//			we = m_pMyMoneyEditBox->ActionEvent( mouseInfo, keyInfo );
//			if( !( we & WE_LBTNCLICK ) )
//				InputMoney( 0, m_dwMoney );		//√ﬂ∞° : ∫Ø∞Ê¿Ã æ»µ«æ˙¿∏∏È «“« ø‰ æ¯¿Ω
//		}
//	}
	
	return cDialog::ActionEvent(mouseInfo, keyInfo);
}
*/
void CExchangeDialog::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		if( lId == XCI_BTN_EXIT || lId == XCI_BTN_CLOSE )
		{
			MSGBASE msg;
			msg.Category	= MP_EXCHANGE;
			msg.Protocol	= MP_EXCHANGE_CANCEL_SYN;
			msg.dwObjectID	= gHeroID;

			NETWORK->Send( &msg, sizeof( MSGBASE ) );
			SetDisable( TRUE );
			//¿ÃªÛ«œ¥Ÿ! ø©±‚º≠ cancel∏ﬁºº¡ˆ∏¶ ∫∏≥ª∞Ì
			//ackπﬁ¿∏∏È æ∆∑°∑Œ. confirm
//			SetActive( FALSE );
		}
		else if( lId == XCI_BTN_INPUTMONEY )
		{
			if( m_bMyLock )
			{
				//"¿·±›ªÛ≈¬ø°º≠¥¬ µ∑ ¿‘∑¬¿Ã æ»µÀ¥œ¥Ÿ."
			}
			else
			{
				//∏∏æ‡ ∂Ù ∞…æ˙¥Ÿ∏È ∏Æ≈œ. //πˆ∆∞ æ»¥≠∑Ø¡ˆ∞‘«œ±‚.
				LONG x = MOUSE->GetMouseEventX();
				LONG y = MOUSE->GetMouseEventY();
				
				WINDOWMGR->MoneyDivideBox( DBOX_EXCHANGE_MONEY,
								x, y, OnInputMoney, OnInputCancel, this, NULL, CHATMGR->GetChatMsg(29) );
				SetDisable( TRUE );
				//pDivideBox->SetMaxValue( MAX_MONEY );?
			}
		}
	}
	if( we & WE_PUSHDOWN )
	{
		if( lId == XCI_BTN_LOCK )
		{
			if( EXCHANGEMGR->IsLockSyncing() )
			{
				m_pLockBtn->SetPush( FALSE );	//DOWNµ«¡ˆ ∏ªæ∆æﬂ «—¥Ÿ.
			}
			else
			{
				MyLock( TRUE );
				EXCHANGEMGR->Lock( TRUE );	//NetworkMsg
			}
		}
		else if( lId == XCI_BTN_EXCHANGE )
		{
			if( m_bMyLock && !EXCHANGEMGR->IsLockSyncing() && m_bOpLock )
			{
				//¿Œ∫•≈‰∏Æ¿« ªÛ≈¬ ∆ƒæ«..
				int nBlankNum = GAMEIN->GetInventoryDialog()->GetBlankNum();
				if( nBlankNum < m_nOpItemNum - m_nMyItemNum )
				{
					//√ﬂ∞° : ±≥»Ø∫“∞°¥… ∏ﬁºº¡ˆ
					//¿Œ∫•≈‰∏Æ ∞¯∞£¿Ã ∫Œ¡∑«’¥œ¥Ÿ.
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 474 ) );

					m_pExchangeBtn->SetPush( FALSE );
				}
				else if( m_dwOpMoney + HERO->GetMoney() > MAX_INVENTORY_MONEY ) //µ∑√º≈©
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(459) );

					m_pExchangeBtn->SetPush( FALSE );
				}
				else
				{
					//√ﬂ∞° : ¿Œ∫•≈‰∏Æ ¿·Ω√ ∏ÿ√„	//quick√¢¿∫.. ¿Œ∫•¿Ã ∏ÿ√Ë¿∏∏È ªÁøÎ ∫“¥…?
					//GAMEIN->GetInventoryDialog()->SetDisable( TRUE );
					//SetDisable( TRUE );
					EXCHANGEMGR->Exchange();
				}
			}
			else
			{
				//æÁ¬  ∏µŒ ∂Ù¿ª «ÿæﬂ ±≥»Ø «œΩ« ºˆ ¿÷Ω¿¥œ¥Ÿ.
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(450) );
				m_pExchangeBtn->SetPush( FALSE );
			} 
		}
	}
	if( we & WE_PUSHUP )
	{
		if( lId == XCI_BTN_LOCK )
		{
			m_pLockBtn->SetPush( TRUE );	//UPµ«¡ˆ ∏ªæ∆æﬂ «—¥Ÿ.
/*
			if( EXCHANGEMGR->IsLockSyncing() || EXCHANGEMGR->IsExchangeSyncing() )
			{
				m_pLockBtn->SetPush( TRUE );	//UPµ«¡ˆ ∏ªæ∆æﬂ «—¥Ÿ.
			}
			else
			{
				EXCHANGEMGR->Lock( FALSE );	//network msg
			}
*/
		}
		else if( lId == XCI_BTN_EXCHANGE )
		{
			m_pExchangeBtn->SetPush( TRUE );
		}
	}
	if( we & WE_LBTNCLICK )
	{
		if( lId == XCI_ICD_MINE )
		{
//			CItem* pItem = GAMEIN->GetInventoryDialog()->GetCurSelectedItem( INV );
//			cIconGridDialog* pIGD = (cIconGridDialog*)GetWindowForID( XCI_ICD_MINE );
//			if( pItem )
//				pIGD->AddIcon( 0, 0, pItem );
		}
	}

}

void CExchangeDialog::SetPlayerName( char* pMyName, char* pOpName )
{
	m_pMyNameStc->SetStaticText( pMyName );
	m_pOpNameStc->SetStaticText( pOpName );
}

void CExchangeDialog::SetActive(BOOL val)
{
	if (MAP->GetMapNum() == StreetTournament && val)
	{
		CHATMGR->AddMsg(CTC_OPERATEITEM, "Trade is disable at this map");
		val = FALSE;
	}

	if (m_bDisable) return;
	if (m_bActive == val) return;

	if (val)
	{
		CProgressDialog* progressDialog = (CProgressDialog*)WINDOWMGR->GetWindowForID(PROGRESS_DIALOG);
		ASSERT(progressDialog);

		if (progressDialog->IsDisable())
		{
			return;
		}

		// Obtener referencias a ventanas que deben cerrarse si est·n activas
		cDialog* dissolveDialog  = WINDOWMGR->GetWindowForID(DIS_DISSOLUTIONDLG);
		cDialog* enchantDialog   = WINDOWMGR->GetWindowForID(ITD_UPGRADEDLG);
		cDialog* reinforceDialog = WINDOWMGR->GetWindowForID(ITR_REINFORCEDLG);
		cDialog* mixDialog       = WINDOWMGR->GetWindowForID(ITMD_MIXDLG);
		cDialog* npcShopDialog   = WINDOWMGR->GetWindowForID(DE_DEALERDLG);     // Zera: bloquear intercambio con tienda NPC
		cDialog* stallShopDialog = WINDOWMGR->GetWindowForID(SSI_STALLDLG);     // Zera: bloquear intercambio con venta de personajes

		ASSERT(dissolveDialog && enchantDialog && reinforceDialog && mixDialog && npcShopDialog && stallShopDialog);

		//Zera Cerrar solo si est·n activas
		if (dissolveDialog->IsActive())     dissolveDialog->SetActive(FALSE);
		if (enchantDialog->IsActive())      enchantDialog->SetActive(FALSE);
		if (reinforceDialog->IsActive())    reinforceDialog->SetActive(FALSE);
		if (mixDialog->IsActive())          mixDialog->SetActive(FALSE);
		if (npcShopDialog->IsActive())      npcShopDialog->SetActive(FALSE);
		if (stallShopDialog->IsActive())    stallShopDialog->SetActive(FALSE);
	}

	// Activar o desactivar el di·logo de intercambio
	cDialog::SetActive(val);

	DWORD color = TTTC_ONDERMILLION;
	color = RGB_HALF(255, 255, 255);
	m_pMyMoneyEditBox->SetNonactiveTextColor(color);
	m_pOpMoneyEditBox->SetNonactiveTextColor(color);

	if (val)
	{
		m_pMyMoneyEditBox->SetEditText("0");
		m_pOpMoneyEditBox->SetEditText("0");

		m_pLockBtn->SetPush(FALSE);
		m_pExchangeBtn->SetPush(FALSE);

		m_pInputMoneyBtn->SetDisable(FALSE);
		m_pInputMoneyBtn->SetActive(TRUE);
	}
	else
	{
		if (!m_bInit) return; // Asegurarse de que estÈ inicializado

		// Eliminar Ìtems del intercambio
		for (int i = 0; i < m_nMyItemNum; ++i)
			DelItem(0, 1);
		for (int i = 0; i < m_nOpItemNum; ++i)
			DelItem(1, 1);

		m_bMyLock = FALSE;
		m_bOpLock = FALSE;

		m_dwMoney = 0;
		m_dwOpMoney = 0;

		m_bExchangable = TRUE;

		m_nMyItemNum = 0;
		m_nOpItemNum = 0;

		EXCHANGEMGR->Init();

		cDialog* pDlg = WINDOWMGR->GetWindowForID(DBOX_EXCHANGE_MONEY);
		if (pDlg)
		{
			WINDOWMGR->AddListDestroyWindow(pDlg);
		}
	}
}


void CExchangeDialog::Render()
{
	cDialog::Render();

	if( !m_LockImage.IsNull() )
	{
		int count = 0 ;

		VECTOR2 vPos;

		if( m_bMyLock )
		{
//			VECTOR2 vPos = { m_pMyIconGridDlg->m_absPos.x + 3, m_pMyIconGridDlg->m_absPos.y + 3 };
//			m_LockImage.RenderSprite( &vScale, NULL, 0, &vPos,
//										RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100) );

			for( count = 0 ; count < 12 ; ++count )
			{
				//if( !m_pMyIconGridDlg->IsAddable( i ) ) continue;
				vPos.x = ( m_pMyIconGridDlg->m_absPos.x ) + ( count % 6 ) * 39 ;
				vPos.y = ( m_pMyIconGridDlg->m_absPos.y ) + ( count / 6 ) * 39 ;

				m_LockImage.RenderSprite( NULL, NULL, 0.0f, &vPos,
										  RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100) );

			}
		}

		if( m_bOpLock )
		{
//			VECTOR2 vPos = { m_pOpIconGridDlg->m_absPos.x + 3, m_pOpIconGridDlg->m_absPos.y + 3 };
//			m_LockImage.RenderSprite( &vScale, NULL, 0, &vPos, 
//										RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100) );

			for( count = 0 ; count < 12 ; ++count )
			{
				//if( !m_pOpIconGridDlg->IsAddable( i ) ) continue;
				
				vPos.x = ( m_pOpIconGridDlg->m_absPos.x ) + ( count % 6 ) * 39 ;
				vPos.y = ( m_pOpIconGridDlg->m_absPos.y ) + ( count / 6 ) * 39 ;

				m_LockImage.RenderSprite( NULL, NULL, 0.0f, &vPos,
										  RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100) );

			}
		}
	}
}

BOOL CExchangeDialog::FakeMoveIcon( LONG x, LONG y, cIcon* icon )
{
	if( m_bMyLock ) return FALSE;

	if( m_bOpLock ) 
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1245 ) );
		return FALSE;
	}

	if( m_bDisable ) return FALSE;	//±≥»Ø√¢ ∏ÿ√„¡ﬂ¿Ã∏È ∏Æ≈œ

	if( icon->GetType() != WT_ITEM )
		return FALSE;

	// 090105 ShinJS --- ITEM_INFO¿« NULL √º≈© √ﬂ∞°
	if( ((CItem*)icon)->GetItemInfo() == NULL )
		return FALSE;

	if( ( GetTableIdxPosition( ((CItem*)icon)->GetPosition() ) != eItemTable_Inventory ))
	{
		if( GetTableIdxPosition( ((CItem*)icon)->GetPosition() ) == eItemTable_Weared )
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(435) );
		
		return FALSE;
	}

	if( ((CItem*)icon)->GetItemInfo()->Category == eItemCategory_Expend )
	{
		if( QUICKMGR->CheckQPosForItemIdx(((CItem*)icon)->GetItemIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(62) );
			return FALSE;
		}
	}
	else
	{
		if( QUICKMGR->CheckQPosForDBIdx(((CItem*)icon)->GetDBIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(62) );
			return FALSE;
		}
	}   //
	//////


	// changeitem ¡ﬂø° ∞≈∑°æ»µ«¥¬∞≈
	if(! ( ((CItem*)icon)->GetItemInfo()->Trade ) )
	{
		return FALSE;
	}
	
	if( ((CItem*)icon)->GetItemInfo()->Category == eItemCategory_Expend )
	{
		if( QUICKMGR->CheckQPosForItemIdx(((CItem*)icon)->GetItemIdx()) == FALSE )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(437) );
			return FALSE;
		}
	}
	else
	{
		if( ((CItem*)icon)->IsQuickLink() != 0 )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(437) );
			return FALSE;
		}
	}

	if( m_pMyIconGridDlg->PtInWindow( x, y ) && m_bMyLock == FALSE )
	{
		// 071125 LYW --- ExchangeDialog : ∫¿¿Œ «ÿ¡¶ µ» æ∆¿Ã≈€ ±≥»Ø ∫“∞° √≥∏Æ √ﬂ∞°.
		const ITEMBASE& pItemBase = ((CItem*)icon)->GetItemBaseInfo();

		if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
		{
			WINDOWMGR->MsgBox(MBI_SEALITEM_EXCHANGE, MBT_OK, CHATMGR->GetChatMsg(1174));

			return FALSE ;
		}

		if( EXCHANGEMGR->LinkItem( (CItem*)icon ) )
			SetDisable( TRUE );
	}
	return FALSE;
}


void CExchangeDialog::AddItem( int nType, POSTYPE pos, cIcon* pItem )
{
	if( nType == 0 )
	{
		m_pMyIconGridDlg->AddIcon( pos-1, pItem );
		++m_nMyItemNum;

		//---√ﬂ∞°

	}
	else
	{
		m_pOpIconGridDlg->AddIcon( pos-1, pItem );
		++m_nOpItemNum;
	}

	//√ﬂ∞° : ±≥»Ø√¢ ∏ÿ√„ «Æ±‚
}


void CExchangeDialog::DelItemOptionInfoAll()
{
	for( int i = 1 ; i <= m_nOpItemNum ; ++i )
	{
		DelItemOptionInfo( (POSTYPE)i );
	}
}

void CExchangeDialog::DelItemOptionInfo( POSTYPE pos )
{
	CExchangeItem* pIcon = (CExchangeItem*)m_pOpIconGridDlg->GetIconForIdx( pos - 1 );
	if( !pIcon ) return;

	ITEMMGR->RemoveOption( pIcon->GetDBId() );
}

//void CExchangeDialog::DelItemRareOptionInfoAll()
//{
//	for( int i = 1; i <= m_nOpItemNum; ++i )
//	{
//		DelItemRareOptionInfo( (POSTYPE)i );
//	}
//}

void CExchangeDialog::DelItem( int nType, POSTYPE pos )
{
	cIcon* pIcon = NULL;

	if( nType == 0 )
	{
		if( m_pMyIconGridDlg->DeleteIcon( pos-1, &pIcon ) )
		{
			if( pIcon == NULL) return;
			
			EXCHANGEMGR->UnLinkItem( 0, pIcon );

			for( WORD i = pos ; i < m_nMyItemNum ; ++i )
			{
				m_pMyIconGridDlg->DeleteIcon( i, &pIcon );
				if( pIcon == NULL ) break;

				((CExchangeItem*)pIcon)->SetPosition( i );
				m_pMyIconGridDlg->AddIcon( i - 1, pIcon );
			}

			//linkitemµµ «œ≥™ ¡Ÿ¿Ã±‚.////////////
			--m_nMyItemNum;
		}
	}
	else
	{
		if( m_pOpIconGridDlg->DeleteIcon( pos-1, &pIcon ) )
		{
			if( pIcon == NULL) return;

			EXCHANGEMGR->UnLinkItem( 1, pIcon );

			for( WORD i = pos ; i < m_nOpItemNum ; ++i )
			{
				m_pOpIconGridDlg->DeleteIcon( i, &pIcon );
				if( pIcon == NULL ) break;

				m_pOpIconGridDlg->AddIcon( i - 1, pIcon );
			}
			--m_nOpItemNum;
		}
	}

	//√ﬂ∞° : ±≥»Ø√¢ ∏ÿ√„ «Æ±‚	
}

MONEYTYPE CExchangeDialog::InputMoney( int nType, MONEYTYPE dwMoney )
{
	DWORDEX color = TTTC_ONDERMILLION;
	DWORDEX money = dwMoney;

	// 091112 ONS ±›æ◊«•Ω√ ªˆªÛ º≥¡§
	color = GetMoneyColor(money);
	color = RGB_HALF(105, 105, 105);

	if( nType == 0 )
	{
		m_dwMoney = dwMoney;
		m_pMyMoneyEditBox->SetNonactiveTextColor( DWORD(color) );
		m_pMyMoneyEditBox->SetEditText( AddComma( dwMoney ) );
	}
	else
	{
		m_dwOpMoney = dwMoney;
		m_pOpMoneyEditBox->SetNonactiveTextColor( DWORD(color) );
		m_pOpMoneyEditBox->SetEditText( AddComma( dwMoney ) ); //»´ƒ·¿∫ Ω«Ω√∞£¿Ã æ∆¥œ∂Û ∂Ù∞…∂ß∏∏ ∫∏ø©¡ÿ¥Ÿ.
	}

	return dwMoney;
}

void CExchangeDialog::OpLock( BOOL bLock )
{
	m_bOpLock = bLock;

	for(WORD i = 0 ; i < m_nOpItemNum ; ++i )
	{
		if( m_pOpIconGridDlg->GetIconForIdx( i ) )
		{
			m_pOpIconGridDlg->GetIconForIdx( i )->SetLock( bLock );
			m_pOpIconGridDlg->GetIconForIdx( i )->SetMovable( FALSE );
		}
	}

	if( bLock == FALSE )	//ªÛ¥Î∞° ∂Ù¿ª «Æ∏È.. ≥™¿« ªÛ≈¬µµ πŸ≤€¥Ÿ.
	{
		//√ﬂ∞° : Syncingø° ¥Î«— πÆ¡¶ ª˝∞¢«œ±‚
		m_pExchangeBtn->SetPush( FALSE );

		//√ﬂ∞° : ¿€µø ∞°¥…
#ifdef _TW_LOCAL_
		m_pOpMoneyEditBox->SetEditText( "0" );
#endif

		/////////MyLock
		m_bMyLock = FALSE;

		for(WORD i = 0 ; i < m_nMyItemNum ; ++i )
		{
			if( m_pMyIconGridDlg->GetIconForIdx( i ) )
			{
				m_pMyIconGridDlg->GetIconForIdx( i )->SetLock( FALSE );
				m_pMyIconGridDlg->GetIconForIdx( i )->SetMovable( FALSE );
			}
		}
		
		m_pInputMoneyBtn->SetDisable( FALSE );
		m_pLockBtn->SetPush( FALSE );
	/////////////
	}
#ifdef _TW_LOCAL_
	else	//lock
	{
		m_pOpMoneyEditBox->SetEditText( AddComma( m_dwOpMoney ) );
	}
#endif
}

void CExchangeDialog::MyLock( BOOL bLock )
{
	m_bMyLock = bLock;

	for(WORD i = 0 ; i < m_nMyItemNum ; ++i )
	{
		if( m_pMyIconGridDlg->GetIconForIdx( i ) )
		{
			m_pMyIconGridDlg->GetIconForIdx( i )->SetLock( bLock );
			m_pMyIconGridDlg->GetIconForIdx( i )->SetMovable( FALSE );
		}
	}
	
	if( bLock )
	{
		m_pInputMoneyBtn->SetDisable( TRUE );

		cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_EXCHANGE_MONEY );
		if( pDlg )
		{
			WINDOWMGR->AddListDestroyWindow( pDlg );
		}
	}
	else
	{
		m_bOpLock = bLock;

		for(WORD i = 0 ; i < m_nOpItemNum ; ++i )
		{
			if( m_pOpIconGridDlg->GetIconForIdx( i ) )
			{
				m_pOpIconGridDlg->GetIconForIdx( i )->SetLock( bLock );
				m_pOpIconGridDlg->GetIconForIdx( i )->SetMovable( FALSE );
			}
		}

		m_pInputMoneyBtn->SetDisable( FALSE );
	}

	m_pLockBtn->SetPush( bLock );
}


void CExchangeDialog::OnInputMoney( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CExchangeDialog* pDlg = (CExchangeDialog*)vData1;
	if( param1 == pDlg->GetMoney() )
	{
		pDlg->SetDisable( FALSE );
		return;
	}
	
	if( !pDlg->IsActive() ) return;
	
	if( pDlg->IsMyLock() || pDlg->IsOpLock() )
	{	//∂Ù∞…∏∞ ªÛ≈¬∂Û∏È
	
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1245 ) );
	
		pDlg->SetDisable( FALSE );
		return;
	}

	if( pDlg->GetMoney() > 0 )
	{
		pDlg->SetDisable( FALSE );
		return;
	}

	MONEYTYPE dwCurMoney = HERO->GetMoney();

	if( dwCurMoney < param1 )
		param1 = dwCurMoney;

	MSG_DWORDEX msg;
	msg.Category	= MP_EXCHANGE;
	msg.Protocol	= MP_EXCHANGE_INPUTMONEY_SYN;
	msg.dwObjectID	= gHeroID;
	msg.dweData		= param1;
	NETWORK->Send( &msg, sizeof( MSG_DWORDEX ) );

	GAMEIN->GetExchangeDialog()->MoneyInputButtonEnable( FALSE );
}


void CExchangeDialog::OnInputCancel( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CExchangeDialog* pDlg = (CExchangeDialog*)vData1;
	pDlg->SetDisable( FALSE );
}

void CExchangeDialog::MoneyInputButtonEnable( BOOL bEnable )
{
	if( bEnable == FALSE )
		m_pInputMoneyBtn->SetDisable( FALSE );	//disable ªÛ≈¬ø°º± setactiveº≥¡§¿Ã æ»µ»¥Ÿ.
	m_pInputMoneyBtn->SetActive( bEnable );
}