// DealDialog.cpp: implementation of the CDealDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DealDialog.h"
#include "DealItem.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cIconGridDialog.h"
#include "./Interface/cStatic.h"
#include "./interface/cTabDialog.h"
#include "./interface/cPushupButton.h"
#include "GameIn.h"
#include "ObjectStateManager.h"
#include "cDivideBox.h"
#include "InventoryExDialog.h"
#include "mhFile.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "MHMap.h"
#include "FishingManager.h"
#include "PCRoomManager.h"
//aziz MallShop 24
#include "VipMgr.h"
//aziz Kill Shop 30 Sep
#include "KillMgr.h"
//aziz Reborn Point 13 Oct
#include "UtilityMgr.h"
#include "ChaptchaDlg.h"

//Alemuri Calendar Daily Item----
#include <time.h> 
//-------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void BuyItem(LONG lId, void * p, DWORD we);

CDealDialog::CDealDialog()
{
	m_type = WT_DEALDIALOG;
	m_pMoneyEdit = NULL;
	m_CurSelectedItemIdx = 0;
	m_lCurSelItemPos = -1;
	m_DealerTable.Initialize(32);			// #define MAX_DEALER_NUM 100
	m_fShow = FALSE;

	m_DealerIdx = 0;
}

CDealDialog::~CDealDialog()
{
	Release();
}


void CDealDialog::Add(cWindow * window)
{
	if(window->GetType() == WT_PUSHUPBUTTON)
		AddTabBtn(curIdx1++, (cPushupButton * )window);
	else if(window->GetType() == WT_ICONGRIDDIALOG)
		AddTabSheet(curIdx2++, window);
	else 
		cDialog::Add(window);
}

void CDealDialog::ShowDealer(WORD DealerKey)
{
	if(m_fShow) HideDealer();
	SelectTab(0);
	SetMoney(0, 2);
	ITEMMGR->itDebug();
	DealerData * pDealer = m_DealerTable.GetData(DealerKey);
	if(pDealer == NULL)
	{
		//070126 LYW --- NPCShop : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(197) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(338) );
		if( HERO->GetState() == eObjectState_Deal )
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		return;
	}

	SetActive(TRUE);
	
	for(BYTE i = 0; i < m_bTabNum; ++i )
	{
		cPushupButton* Btn = GetTabBtn(i);
		Btn->SetActive(FALSE);
	}

	CDealItem* pItem = NULL;
	cImage tmpImage;
	m_DealerIdx = DealerKey;
	DWORD DealIdx = 0;
	POS pos = pDealer->m_DealItemList.GetFirstPos();

	for(DealerItem* pDealItem = pDealer->m_DealItemList.GetNextPos(pos);
		0 < pDealItem;
		pDealItem = pDealer->m_DealItemList.GetNextPos(pos))
	{
		pItem = new CDealItem;
		pItem->Init(0,0,DEFAULT_ICONSIZE,DEFAULT_ICONSIZE, ITEMMGR->GetIconImage(pDealItem->ItemIdx, &tmpImage),IG_DEALITEM_START+DealIdx++);
		pItem->SetData(pDealItem->ItemIdx);
		pItem->SetItemIdx(pDealItem->ItemIdx);
		//pItem->SetMovable(FALSE);

		ITEMMGR->AddToolTip( pItem );
		
		char buf[MAX_PATH] = {0};
		pItem->AddToolTipLine("");

		if(pItem->GetBuyPrice())
		{
			wsprintf(
				buf,
				CHATMGR->GetChatMsg(35),
				AddComma(pItem->GetBuyPrice()));
			pItem->AddToolTipLine(
				buf,
				TTTC_BUYPRICE);
		}

		if(0 < pItem->GetBuyFishPoint())
		{
			wsprintf( buf, CHATMGR->GetChatMsg(1540), AddComma(pItem->GetBuyFishPoint()) );
			pItem->AddToolTipLine( buf, TTTC_BUYPRICE );
		}
		//aziz MallShop in Game Method 1
		if(0 < pItem->GetBuyVipPoint())
		{
			wsprintf( buf, CHATMGR->GetChatMsg(2501), AddComma(pItem->GetBuyVipPoint()) );
			pItem->AddToolTipLine( buf, TTTC_BUYPRICE );
		}
		//aziz Kill Shop 30 Sep
		if(0 < pItem->GetBuyKillPoint())
		{
			wsprintf( buf, CHATMGR->GetChatMsg(2504), AddComma(pItem->GetBuyKillPoint()) );
			pItem->AddToolTipLine( buf, TTTC_BUYPRICE );
		}
		//aziz Reborn Point 13 Oct
		if(0 < pItem->GetBuyRebornPoint())
		{
			wsprintf( buf, CHATMGR->GetChatMsg(2509), AddComma(pItem->GetBuyRebornPoint()) );
			pItem->AddToolTipLine( buf, TTTC_BUYPRICE );
		}
		// 100104 ShinJS --- Item Point Type�� ���� ���� �߰�
		pItem->AddPointTypeToolTip();

		// 090114 LYW --- DealDialog : ������ ���� ���� üũ �Լ� ������ ���� ó��.
		if( ITEMMGR->CanEquip(pDealItem->ItemIdx) == eEquip_Disable )
			pItem->SetToolTipImageRGB( TTCLR_ITEM_CANNOTEQUIP ) ;

		WINDOWMGR->AddWindow(pItem);

		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(pDealItem->Tab);
		dlg->AddIcon(pDealItem->Pos, pItem);

		//SW050819 
		cPushupButton* Btn = GetTabBtn(pDealItem->Tab);
		if( !Btn->IsActive() )
		{
			Btn->SetActive(TRUE);
		}
	}
	if (DealerKey != 9991) //Alemuri Calendar Daily Item (only open inventory if it's not daily item Mall)
		GAMEIN->GetInventoryDialog()->SetActive(TRUE);	// taiyo : ������ ������ �� ���� ������?

	m_CurSelectedItemIdx	= 0;
	m_lCurSelItemPos		= -1;
	m_fShow = TRUE;

	ITEMMGR->SetPriceToItem( TRUE );

	//Alemuri Calendar Daily Item----------------------------------------
	if (DealerKey == 9991)
	{
		m_pTitle = (cStatic *)GetWindowForID(STATIC_TITLE);
		if (m_pTitle)
		{
			m_pTitle->SetStaticText("Daily Reward");
		}
		else
		{
			m_pTitle->SetStaticText("Shop");
			//m_pTitle->SetStaticText(CHATMGR->GetChatMsg( 234 ));
		}
	}
	//------------------------------------------------------------------
}

void CDealDialog::HideDealer()
{
	if(!m_fShow) return;

	
	cIcon* pIcon;

	for(BYTE tab=0;tab<m_bTabNum;++tab)
	{
		cIconGridDialog * dlg = (cIconGridDialog *)GetTabSheet(tab);

		for(WORD n=0;n<dlg->m_nCol*dlg->m_nRow;++n)
		{
			pIcon = dlg->GetIconForIdx(n);
			if(pIcon != NULL)
			{
				dlg->DeleteIcon(pIcon);
				//delete pIcon;
				pIcon->SetActive( FALSE );
				WINDOWMGR->AddListDestroyWindow( pIcon );
				pIcon = NULL;
			}
			
		}
	}
	m_DealerIdx = 0;

//KES ���� WINDOMGR->AddListDestroyWindow( pIcon ) �� ��ü.�Ͽ���
//process �߿� �ٷ� delete�����ÿ� ������ ����.
//	for(DWORD n=0;n<m_DealIdx;++n)
//	{
//		WINDOWMGR->DeleteWindowForID(IG_DEALITEM_START+n);
//	}

//���� ���¿��� ���� ���� �ִ�. üũ�ϴ°� �߰� �ʿ�
	if( HERO->GetState() != eObjectState_Die )
	{
		if( HERO->GetState() == eObjectState_Deal  )
		OBJECTSTATEMGR->EndObjectState(HERO,eObjectState_Deal);
	}

	m_fShow = FALSE;

	ITEMMGR->SetPriceToItem( FALSE );
}


void CDealDialog::LoadDealerItem(CMHFile* fp)
{
	char buff[2048];
	char mapname[256];
	char npcname[256];
	DWORD ItemIdx;
	
	DealerItem* pItem;
	DealerData* pCurDealer = NULL;
	char seps[]   = "\t\n";
	char* token;
	int count = 0;

	//Alemuri get map infos---
	DWORD coordX = 0;
	DWORD coordY = 0;
	char coordStrX[4];
	char coordStrY[4];
	std::string coordinates = "";
	//------------------------
	while(1)
	{
		if(fp->IsEOF())
		{
			break;
		}
		count++;
		fp->GetWord(); // map index
		fp->GetString(mapname);
		fp->GetWord(); // npc kind
		fp->GetString(npcname);
		WORD npc_index = fp->GetWord();
		fp->GetWord(); // point x 
		fp->GetWord(); // point y 

		fp->GetWord(); // angle
		BYTE tabnum = fp->GetByte();
		BYTE Pos = 0;
		
		fp->GetLine(buff,2048);

		token = strtok( buff, seps );
		if(token == NULL)
			continue;
		token = strtok( NULL, seps );
		ItemIdx = atoi(token);

		pCurDealer = m_DealerTable.GetData(npc_index);
		if(pCurDealer == NULL)
		{
			pCurDealer = new DealerData;
			m_DealerTable.Add(pCurDealer,npc_index);
		}
		

		if( ItemIdx != 0 )
		{
			pItem = new DealerItem;
			ASSERT(tabnum)
			pItem->Tab = tabnum-1;
			pItem->Pos = Pos++;
			pItem->ItemIdx = ItemIdx;
			pCurDealer->m_DealItemList.AddTail(pItem);

			//Alemuri whatnpc command--------------------
			std::string MapName(mapname);
			ITEMMGR->addNpcIndexDealer(npc_index);
			ITEMMGR->addItemIndexDealer(ItemIdx);
			ITEMMGR->addNpcMapDealer(MapName);
			//-------------------------------------------
		}
		else
		{
			Pos++;
		}
		
		while( 1 )
		{
			token = strtok( NULL, seps );
			if(token == NULL)
				break;
			token = strtok( NULL, seps );
			ItemIdx = atoi(token);
			
			if( ItemIdx != 0 )
			{
				pItem = new DealerItem;
				pItem->Tab = tabnum-1;
				pItem->Pos = Pos++;
				pItem->ItemIdx = ItemIdx;
				pCurDealer->m_DealItemList.AddTail(pItem);

				//Alemuri whatnpc command--------------------
				std::string MapName(mapname);
				ITEMMGR->addNpcIndexDealer(npc_index);
				ITEMMGR->addItemIndexDealer(ItemIdx);
				ITEMMGR->addNpcMapDealer(MapName);
				//-------------------------------------------
			}
			else
			{
				Pos++;
			}
		}		
	}
}
void CDealDialog::Linking()
{
	m_pMoneyEdit = (cStatic *)GetWindowForID(DE_MONEYEDIT);
	m_pMoneyEdit->SetTextXY( 4, 4 );
	m_pMoneyEdit->SetAlign( TXT_RIGHT );

	// 100104 ShinJS --- PC�� ����Ʈ ��� Static �߰�
	m_pPointImage = (cStatic *)GetWindowForID(DE_POINTIMG);
	m_pPointImage->SetActive( FALSE );

	m_pPointEdit = (cStatic *)GetWindowForID(DE_POINTEDIT);
	m_pPointEdit->SetActive( FALSE );
}
void CDealDialog::Release()
{
	m_DealerTable.SetPositionHead();

	for(DealerData* pDealer = m_DealerTable.GetData();
		0 < pDealer;
		pDealer = m_DealerTable.GetData())
	{
		POS pos = pDealer->m_DealItemList.GetFirstPos();

		for(DealerItem* pDealerItem = pDealer->m_DealItemList.GetNextPos( pos );
			0 < pDealerItem;
			pDealerItem = pDealerItem = pDealer->m_DealItemList.GetNextPos( pos ))
		{
			delete pDealerItem;
		}

		pDealer->m_DealItemList.DeleteAll();
		delete pDealer;
	}
	m_DealerTable.RemoveAll();
}

void CDealDialog::OnSelectedItem()
{	
	cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
	CDealItem* pItem = (CDealItem*)gridDlg->GetIconForIdx((WORD)gridDlg->GetCurSelCellPos());
	if(!pItem)
	{
		SetMoney(0, 2);
		m_CurSelectedItemIdx	= 0;
		m_lCurSelItemPos		= -1;
		return;
	}

	m_CurSelectedItemIdx	= pItem->GetItemIdx();
	m_lCurSelItemPos		= gridDlg->GetCurSelCellPos();
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo(m_CurSelectedItemIdx);
	
	SetMoney(pItemInfo->BuyPrice, 1);
}
void CDealDialog::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

void CDealDialog::SetMoney(MONEYTYPE value, BYTE colorType)
{
	if(colorType == 0)		//�Ĵ°���ǥ��
		m_pMoneyEdit->SetFGColor(RGB_HALF(255,0,0));
	else if(colorType == 1)	//��°���ǥ��
		// 080401 LYW --- DealDialog : Change text color.
		//m_pMoneyEdit->SetFGColor(TTTC_BUYPRICE);
		m_pMoneyEdit->SetFGColor(RGB_HALF(10, 10, 10));
	else
		// 080401 LYW --- DealDialog : change text color.
		//m_pMoneyEdit->SetFGColor(RGB_HALF(255,255,255));
		m_pMoneyEdit->SetFGColor(RGB_HALF(255, 0, 0));

	m_pMoneyEdit->SetStaticText( AddComma( value ) );
}

void CDealDialog::SellItem(CItem* pItem)
{
	if(0 == pItem)
	{
		return;
	}
	else if(pItem->IsLocked())
	{
		return;
	}

	const ITEM_INFO* const pItemInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );

	if(0 == pItemInfo)
	{
		return;
	}
	else if(FALSE == pItemInfo->Sell)
	{
		return;
	}

	SetDisable( TRUE );
	GAMEIN->GetInventoryDialog()->SetDisable( TRUE );

	if(pItemInfo->Stack && pItem->GetDurability() > 1)
	{
		m_sellMsg.Category = MP_ITEM;
		m_sellMsg.Protocol = MP_ITEM_SELL_SYN;
		m_sellMsg.dwObjectID = HEROID;
		m_sellMsg.TargetPos	= pItem->GetPosition();
		m_sellMsg.SellItemNum = WORD(pItem->GetDurability());
		m_sellMsg.wSellItemIdx = pItem->GetItemIdx();
		m_sellMsg.wDealerIdx = m_DealerIdx;

		cDivideBox * pDivideBox = WINDOWMGR->DivideBox(
			DBOX_SELL, 
			LONG(pItem->GetAbsX()),
			LONG(pItem->GetAbsY()),
			OnFakeSellItem,
			OnCancelSellItem,
			this,
			pItem,
			CHATMGR->GetChatMsg(26));

		pDivideBox->SetValue(
			pItem->GetDurability());
		pDivideBox->SetMaxValue(
			pItemInfo->Stack);
	}
	else
	{
		m_sellMsg.Category = MP_ITEM;
		m_sellMsg.Protocol = MP_ITEM_SELL_SYN;
		m_sellMsg.dwObjectID = HEROID;
		m_sellMsg.TargetPos	= pItem->GetPosition();
		m_sellMsg.SellItemNum = WORD(pItem->GetDurability());
		m_sellMsg.wSellItemIdx = pItem->GetItemIdx();
		m_sellMsg.wDealerIdx = m_DealerIdx;

		if(pItemInfo->EquipType == eEquipType_Weapon || pItemInfo->EquipType == eEquipType_Armor || pItemInfo->EquipType == eEquipType_Accessary)
		{
			const ITEM_OPTION& option	= ITEMMGR->GetOption(pItem->GetItemBaseInfo());
			char msgfull[256];
			sprintf(msgfull, CHATMGR->GetChatMsg(339), pItemInfo->ItemName, AddComma(pItemInfo->SellPrice));
				
			if(option.mEnchant.mLevel >= 12)
			{
				CChaptchaDlg* ChaptchaDlg = (CChaptchaDlg*)GAMEIN->GetChaptchaDlg();
				if(!ChaptchaDlg)
				{
					ChaptchaDlg->SetTypeCaptcha(4);
					//ChaptchaDlg->SetItemnameTodelete(itemnya);
					ChaptchaDlg->SetFullmsg(msgfull);
					ChaptchaDlg->SetTypeGame(FALSE);
					ChaptchaDlg->SetActive(TRUE);
					CHATMGR->AddMsg( CTC_OPERATEITEM, "Are you sure to sell this item? we are detect this item enchant level 12+. Please fill captcha!" );
				} else {
					ChaptchaDlg->SetTypeCaptcha(4);
					//ChaptchaDlg->SetItemnameTodelete(itemnya);
					ChaptchaDlg->SetFullmsg(msgfull);
					ChaptchaDlg->SetTypeGame(FALSE);
					ChaptchaDlg->SetActive(TRUE);
					CHATMGR->AddMsg( CTC_OPERATEITEM, "Are you sure to sell this item? we are detect this item enchant level 12+. Please fill captcha!" );
				}
			} else {
				WINDOWMGR->MsgBox(
				MBI_SELLITEM,
				MBT_YESNO, 
				CHATMGR->GetChatMsg(339),
				pItemInfo->ItemName,
				AddComma(pItemInfo->SellPrice));
			}
		} else {
			WINDOWMGR->MsgBox(
				MBI_SELLITEM,
				MBT_YESNO, 
				CHATMGR->GetChatMsg(339),
				pItemInfo->ItemName,
				AddComma(pItemInfo->SellPrice));
		}
	}
}

BOOL CDealDialog::FakeMoveIcon(LONG x, LONG y, cIcon * icon)
{
	// 090106 LYW --- DealDialog : â�� ��ȯ �������� ����Ͽ�, 
	// â���� �������� �ٷ� �Ǹ� �Ҽ� �ִ� ���׸� �����Ѵ�.


	// �Լ����� Ȯ��.
	if( !icon ) return FALSE ;


	// ������ Ÿ������ Ȯ��.
	if( icon->GetType() != WT_ITEM ) return FALSE ;


	// ������ ������ �޴´�.
	CItem * pItem = ( CItem * )icon ;
	if( !pItem ) return FALSE ;

    ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( ((CItem*)icon)->GetItemIdx() ) ;	
	if( !pItemInfo ) return FALSE ;


	// �Ǹ� ���� ���������� Ȯ���Ѵ�.
	if( !pItemInfo->Sell ) return FALSE ;
	
	const eITEMTABLE tableIdx = ITEMMGR->GetTableIdxForAbsPos(
		pItem->GetPosition());

	if( tableIdx != eItemTable_Inventory )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1809 ) ) ;
		return FALSE ;
	}


	// �Ǹ� �޽����� �����Ѵ�.
	m_sellMsg.Category			= MP_ITEM ;
	m_sellMsg.Protocol			= MP_ITEM_SELL_SYN ;
	m_sellMsg.dwObjectID		= HEROID ;

	m_sellMsg.TargetPos			= pItem->GetPosition() ;
	m_sellMsg.SellItemNum		= (WORD)pItem->GetDurability() ;
	m_sellMsg.wSellItemIdx		= pItem->GetItemIdx() ;
	m_sellMsg.wDealerIdx		= m_DealerIdx ;


	// ���þ������̰� �ϳ� �̻��� �������� �Ǹ� �Ϸ��� ���,
	if( ITEMMGR->IsDupItem( pItem->GetItemIdx() ) && pItem->GetDurability() > 1 )
	{
		// �Ǹ� ���� �Է� â�� Ȱ��ȭ �Ѵ�.
		cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_SELL, (LONG)pItem->GetAbsX(), (LONG)pItem->GetAbsY(),
			OnFakeSellItem, OnCancelSellItem, this, pItem, CHATMGR->GetChatMsg(26) ) ;
		pDivideBox->SetValue( m_sellMsg.SellItemNum ) ;

		// 091215 ONS ������Stack�ִ���� �����ۺ��� ������ ������ ó���Ѵ�.
		pDivideBox->SetMaxValue( pItemInfo->Stack ) ;
	}
	// ���� �������̶��, �Ǹ� �ϰڴ��� Ȯ���ϴ� �޼��� â�� Ȱ��ȭ �Ѵ�.
	else
	{	
		if(pItemInfo->EquipType == eEquipType_Weapon || pItemInfo->EquipType == eEquipType_Armor || pItemInfo->EquipType == eEquipType_Accessary )
		{
			const ITEM_OPTION& option	= ITEMMGR->GetOption(pItem->GetItemBaseInfo());
			char msgfull[256];
			sprintf(msgfull, CHATMGR->GetChatMsg(339), pItemInfo->ItemName, AddComma(pItemInfo->SellPrice));
				
			if(option.mEnchant.mLevel >= 12)
			{
				CChaptchaDlg* ChaptchaDlg = (CChaptchaDlg*)GAMEIN->GetChaptchaDlg();
				if(!ChaptchaDlg)
				{
					ChaptchaDlg->SetTypeCaptcha(4);
					//ChaptchaDlg->SetItemnameTodelete(itemnya);
					ChaptchaDlg->SetFullmsg(msgfull);
					ChaptchaDlg->SetTypeGame(FALSE);
					ChaptchaDlg->SetActive(TRUE);
					CHATMGR->AddMsg( CTC_OPERATEITEM, "Are you sure to sell this item? we are detect this item enchant level 12+. Please fill captcha!" );
				} else {
					ChaptchaDlg->SetTypeCaptcha(4);
					//ChaptchaDlg->SetItemnameTodelete(itemnya);
					ChaptchaDlg->SetFullmsg(msgfull);
					ChaptchaDlg->SetTypeGame(FALSE);
					ChaptchaDlg->SetActive(TRUE);
					CHATMGR->AddMsg( CTC_OPERATEITEM, "Are you sure to sell this item? we are detect this item enchant level 12+. Please fill captcha!" );
				}
			} else {
				WINDOWMGR->MsgBox(
				MBI_SELLITEM,
				MBT_YESNO, 
				CHATMGR->GetChatMsg(339),
				pItemInfo->ItemName,
				AddComma(pItemInfo->SellPrice));
			}
		} else {
			WINDOWMGR->MsgBox(
				MBI_SELLITEM,
				MBT_YESNO, 
				CHATMGR->GetChatMsg(339),
				pItemInfo->ItemName,
				AddComma(pItemInfo->SellPrice));
		}
		/*
		WINDOWMGR->MsgBox(
			MBI_SELLITEM,
			MBT_YESNO,
			CHATMGR->GetChatMsg(339),
			pItemInfo->ItemName,
			AddComma( pItemInfo->SellPrice ));
			*/
	}


	// ����â�� Disable ��Ų��.
	SetDisable( TRUE ) ;


	// �κ��丮�� Disable ��Ų��.
	CInventoryExDialog* pInvenDlg = GAMEIN->GetInventoryDialog() ;
	if( pInvenDlg )
	{
		pInvenDlg->SetDisable( TRUE ) ;
	}

	return FALSE ;
}

void CDealDialog::OnFakeSellItem( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CDealDialog * tDlg = ( CDealDialog * )vData1;
	CItem* pItem = (CItem*)vData2;
	ASSERT( pItem );

	if( param1 == 0 ) 
	{
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Deal);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
		return;
	}

	tDlg->m_sellMsg.SellItemNum = (WORD)((tDlg->m_sellMsg.SellItemNum > param1 ? param1 : tDlg->m_sellMsg.SellItemNum));

	WORD SellNum = tDlg->m_sellMsg.SellItemNum;
	if(! ITEMMGR->IsDupItem( tDlg->m_sellMsg.wSellItemIdx ))
		SellNum = 1;

	char temp[32] = {0};
	SafeStrCpy(temp, AddComma(pItem->GetItemInfo()->SellPrice), 32);
	WINDOWMGR->MsgBox(
		MBI_SELLITEM,
		MBT_YESNO,
		CHATMGR->GetChatMsg(340),
		pItem->GetItemInfo()->ItemName,
		SellNum,
		temp,
		AddComma(pItem->GetItemInfo()->SellPrice * SellNum));
}

void CDealDialog::OnCancelSellItem( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CDealDialog * tDlg = ( CDealDialog * )vData1;

	tDlg->SetDisable( FALSE );
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
}


void CDealDialog::SendSellItemMsg()
{
	const eITEMTABLE TargetTableIdx = ITEMMGR->GetTableIdxForAbsPos(m_sellMsg.TargetPos);
	CItem* pTargetItem = ITEMMGR->GetItemofTable(TargetTableIdx, m_sellMsg.TargetPos);

	if(!pTargetItem)
	{
		OnCancelSellItem(0, NULL, 0, this, NULL);
		return ;
	}

	if(m_sellMsg.wSellItemIdx != pTargetItem->GetItemIdx())
	{
		OnCancelSellItem(0, NULL, 0, this, NULL);
		return ;
	}
	if(m_sellMsg.SellItemNum > pTargetItem->GetDurability())
	{
		OnCancelSellItem(0, NULL, 0, this, NULL);
		return ;
	}

	NETWORK->Send( &m_sellMsg, sizeof(m_sellMsg) );
}

void CDealDialog::OnSellPushed()
{
	if(CItem* const pItem = GAMEIN->GetInventoryDialog()->GetCurSelectedItem(eItemTable_Inventory))
	{
		FakeMoveIcon(
			LONG(pItem->GetAbsX() + 20),
			LONG(pItem->GetAbsY() + 20),
			pItem);
	}
}

void CDealDialog::OnFakeBuyItem( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
/*	// ��� �ִ��� üũ ��
	tDlg->m_buyMsg.BuyItemNum	= param1;
	NETWORK->Send( &tDlg->m_buyMsg, sizeof(tDlg->m_buyMsg) );
*/
	if( param1 == 0 )
	{
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Deal);
		ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
		return;
	}

	CDealDialog * tDlg = ( CDealDialog * )vData1;
	CDealItem* pItem = (CDealItem*)vData2;
	ASSERT( pItem );

	tDlg->m_buyMsg.BuyItemNum	= (WORD)param1;

	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );
	if( !pItemInfo )
		return;

	switch( pItemInfo->wPointType )
	{
		// PC�� ����Ʈ ����
	case eItemPointType_PCRoomPoint:
		{
			DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1 * tDlg->m_buyMsg.BuyItemNum;

			char totalPrice[32], totalPoint[32];
			SafeStrCpy( totalPrice, AddComma( pItem->GetBuyPrice() * tDlg->m_buyMsg.BuyItemNum ), 32 );
			SafeStrCpy( totalPoint, AddComma( dwNeedPoint ), 32 );

			WINDOWMGR->MsgBox(
				MBI_BUYITEM,
				MBT_YESNO,
				CHATMGR->GetChatMsg( 2015 ),
				pItem->GetItemName(),
				tDlg->m_buyMsg.BuyItemNum,
				AddComma(pItem->GetBuyPrice()),
				totalPrice,
				totalPoint );
		}
		break;
	default:
		{
			// [07-02-2012] Death Check Item buy more than 4000 M ??
			char temp[32] = {0};
			SafeStrCpy(temp, AddComma(pItem->GetBuyPrice()), 32);
			DWORDEX totalprice = (DWORDEX)pItem->GetBuyPrice() * (DWORDEX)tDlg->m_buyMsg.BuyItemNum;
			if (totalprice <= MAX_INVENTORY_MONEY)
			{
				WINDOWMGR->MsgBox(
				MBI_BUYITEM,
				MBT_YESNO,
				CHATMGR->GetChatMsg(341),
				pItem->GetItemName(),
				tDlg->m_buyMsg.BuyItemNum,
				temp,
				AddComma(pItem->GetBuyPrice() * tDlg->m_buyMsg.BuyItemNum));
			}
			// Added by Death
			else
			{
				CHATMGR->AddMsg(CTC_BUYITEM, CHATMGR->GetChatMsg(506));
				ITEMMGR->SetDisableDialog(FALSE, eItemTable_Deal);
				ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
				return;
			}
		}
		break;
	}

	
	
}

void CDealDialog::OnCancelBuyItem( LONG iId, void* p, DWORDEX param1, void * vData1, void * vData2 )
{
	CDealDialog * tDlg = ( CDealDialog * )vData1;
	tDlg->SetDisable( FALSE );
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
}

//Alemuri Calendar Daily Item----------------------------------------------------------------------------------
void CDealDialog::OnDailyPushed()
{
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);

	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
	int year = aTime->tm_year + 1900; // Year is # years since 1900

	char buffday[3];
	char buffmonth[3];
	char buffyear[5];	
	sprintf(buffday, "%d", day);
	sprintf(buffmonth, "%d", month);
	sprintf(buffyear, "%d", year);

	std::string dayStr(buffday);
	std::string monthStr(buffmonth);
	std::string yearStr(buffyear);
	std::string newDaily = dayStr + "," + monthStr + "," + buffyear;

	if (m_DealerIdx == 9991)
	{
		cIconGridDialog* m_pMyIconGridDlg	= (cIconGridDialog*)GetWindowForID( DE_TABDLG1 );
		m_pMyIconGridDlg->GetIconForIdx( day - 1 )->SetLock( TRUE ); //-1 because icons starts from 0
	}

	MSGBASE msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category = MP_PCROOM;
	msg.Protocol = MP_PCROOM1_DAILYITEM_SYN;
	msg.dwObjectID = HEROID;
	NETWORK->Send( &msg, sizeof(msg) );
}
//--------------------------------------------------------------------------------------------------------------

void CDealDialog::OnBuyPushed()
{
	if(m_lCurSelItemPos == -1)
		return;
	
	//Alemuri Calendar Daily Item-------------------------------------
	if (m_DealerIdx == 9991) //in case the dealer is the calendar dealer don't let buy anything
		return;
	//----------------------------------------------------------------

	cIconGridDialog * gridDlg = (cIconGridDialog *)GetTabSheet(GetCurTabNum());
	CDealItem* pItem = (CDealItem*)gridDlg->GetIconForIdx((WORD)m_lCurSelItemPos);

	if( pItem )
	{
		FakeBuyItem( (LONG)(pItem->GetAbsX()+20), (LONG)(pItem->GetAbsY()+20), pItem);
	}
}


void CDealDialog::SendBuyItemMsg()
{
	// 100104 ShinJS --- Item Point Type ����
	ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( m_buyMsg.wBuyItemIdx );
	if( !pItemInfo )
		return;
    
	m_buyMsg.wDealerIdxDbg = 0;
	m_buyMsg.wDealerIdxDbg = ITEMMGR->m_itdbg;
	switch( pItemInfo->wPointType )
	{
		// Coin ���� Item
	case eItemPointType_Item:
		{
			// Coin �������� ���ŷ����� ������ ���
			DWORD dwCoinItemIdx = pItemInfo->dwPointTypeValue1;
			DWORD dwNeedCoinCnt = pItemInfo->dwPointTypeValue2;
			if( !CanBuyCoinItem( m_buyMsg.wBuyItemIdx, dwCoinItemIdx, dwNeedCoinCnt * m_buyMsg.BuyItemNum ) )
			{
				SetDisable( FALSE );
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				return;
			}
		}
		break;

		// PC�� ����Ʈ ����
	case eItemPointType_PCRoomPoint:
		{
			DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1 * m_buyMsg.BuyItemNum;

			// ����Ʈ ����
			if( !PCROOMMGR->CanBuyItem( m_buyMsg.wBuyItemIdx, dwNeedPoint ) )
			{
				SetDisable( FALSE );
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				return;
			}
		}
		break;
	}
	
	NETWORK->Send( &m_buyMsg, sizeof(m_buyMsg) );
}

void CDealDialog::CancelBuyItem()
{
	SetDisable( FALSE );
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
}

void CDealDialog::FakeBuyItem(LONG x, LONG y, CDealItem* pItem)
{
	DWORD itemIdx = pItem->GetItemIdx();
	if( itemIdx == 0 ) return;

	//Alemuri Calendar Daily Item----------------------
	if (m_DealerIdx == 9991) //don't let buy if it's the NPC Calendar Daily Item
		return;
	//-------------------------------------------------

	if( ITEMMGR->IsDupItem( itemIdx ) )
	{
		// ������ â ����
		m_buyMsg.Category		= MP_ITEM;
		m_buyMsg.Protocol		= MP_ITEM_BUY_SYN;
		m_buyMsg.dwObjectID		= HEROID;
		m_buyMsg.wBuyItemIdx	= itemIdx;
		m_buyMsg.wDealerIdx		= m_DealerIdx;

		//m_buyMsg.BuyItemNum		= 0;

		cDivideBox * pDivideBox = WINDOWMGR->DivideBox( DBOX_BUY, x, y, OnFakeBuyItem, OnCancelBuyItem,
			//070126 LYW --- NPCShop : Modified message number.
														//this, pItem, CHATMGR->GetChatMsg(187) );
														this, pItem, CHATMGR->GetChatMsg(27) );
		if( !pDivideBox )
			return;

		pDivideBox->SetMaxValue( MAX_ITEMBUY_NUM );
//		pDivideBox->SetValue(1);

		SetDisable( TRUE );
		GAMEIN->GetInventoryDialog()->SetDisable( TRUE );

	}
	else
	{
		if( HERO->GetMoney() < pItem->GetBuyPrice() )
		{
			//���� �����մϴ�.
			//070126 LYW --- NPCShop : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(117) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(324) );
			return;			
		}

		if( 0<pItem->GetBuyFishPoint() && FISHINGMGR->GetFishPoint() < pItem->GetBuyFishPoint())
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1533 ) );
			return;			
		}
		//aziz MallShop in Game Method 1
		//aziz MallShop 24
		if( 0 < pItem->GetBuyVipPoint() && VIPMGR->GetVipPoint() < pItem->GetBuyVipPoint())
		{
			CHATMGR->AddMsg( CTC_OPERATOR, CHATMGR->GetChatMsg( 2502 ) );
			return;			
		}
		//aziz Kill Shop 30 Sep
		if( 0 < pItem->GetBuyKillPoint() && KILLMGR->GetKillPoint() < pItem->GetBuyKillPoint())
		{
			CHATMGR->AddMsg( CTC_OPERATOR, CHATMGR->GetChatMsg( 2505 ) );
			return;			
		}
		//aziz Reborn Point 13 Oct
		if( 0 < pItem->GetBuyRebornPoint() && UTILITYMGR->GetRebornPoint() < pItem->GetBuyRebornPoint())
		{
			CHATMGR->AddMsg( CTC_OPERATOR, CHATMGR->GetChatMsg( 2510 ) );
			return;			
		}

		// 100104 ShinJS --- Item Point Type ����
		ITEM_INFO * pItemInfo = ITEMMGR->GetItemInfo( itemIdx );
		if( !pItemInfo )
			return;

		switch( pItemInfo->wPointType )
		{
			// Coin ����
		case eItemPointType_Item:
			{
				// Coin �������� ���ŷ����� ������ ���
				DWORD dwCoinItemIdx = pItemInfo->dwPointTypeValue1;
				DWORD dwNeedCoinCnt = pItemInfo->dwPointTypeValue2;
				if( !CanBuyCoinItem( itemIdx, dwCoinItemIdx, dwNeedCoinCnt ) )
				{
					return;
				}
			}
			break;

			// PC�� ����Ʈ ����
		case eItemPointType_PCRoomPoint:
			{
				DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1;

				// ����Ʈ ����
				if( !PCROOMMGR->CanBuyItem( itemIdx, dwNeedPoint ) )
				{
					return;
				}
			}
			break;
		}
		
		SetDisable( TRUE );
		GAMEIN->GetInventoryDialog()->SetDisable( TRUE );

		m_buyMsg.Category		= MP_ITEM;
		m_buyMsg.Protocol		= MP_ITEM_BUY_SYN;
		m_buyMsg.dwObjectID		= HEROID;
		m_buyMsg.wBuyItemIdx	= itemIdx;
		m_buyMsg.wDealerIdx		= m_DealerIdx;
		m_buyMsg.BuyItemNum		= 1;

		const MONEYTYPE BuyPrice = pItem->GetBuyPrice();
		char buf[256] = {0,};
		DWORD dwBuyFishPoint = pItem->GetBuyFishPoint();
		//aziz MallShop in Game Method 1
		DWORD dwBuyVipPoint = pItem->GetBuyVipPoint();
		//aziz Kill Shop 30 Sep
		DWORD dwBuyKillPoint = pItem->GetBuyKillPoint();
		//aziz Reborn Point 13 Oct
		DWORD dwBuyRebornPoint = pItem->GetBuyRebornPoint();
		if(0 < dwBuyFishPoint)
		{
			sprintf(buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 1534 ));

			char temp[32];
			SafeStrCpy(temp, AddComma(BuyPrice), 32 );

			WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
				pItem->GetItemName(), temp, AddComma( dwBuyFishPoint ));
		}
		//aziz MallShop in Game Method 1
		if(0 < dwBuyVipPoint)
		{
			sprintf(buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 2503 ));

			char temp[32];
			SafeStrCpy(temp, AddComma(BuyPrice), 32 );

			WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
				pItem->GetItemName(), temp, AddComma( dwBuyVipPoint ));
		}
		//aziz Kill Shop 30 Sep
		if(0 < dwBuyKillPoint)
		{
			sprintf(buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 2506 ));

			char temp[32];
			SafeStrCpy(temp, AddComma(BuyPrice), 32 );

			WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
				pItem->GetItemName(), temp, AddComma( dwBuyKillPoint ));
		}
		//aziz Reborn Point 13 Oct
		if(0 < dwBuyRebornPoint)
		{
			sprintf(buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 2511 ));

			char temp[32];
			SafeStrCpy(temp, AddComma(BuyPrice), 32 );

			WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
				pItem->GetItemName(), temp, AddComma( dwBuyRebornPoint ));
		}
		else
		{
			// 100105 ShinJS --- PointType �� ���� Msg ����
			switch( pItemInfo->wPointType )
			{
				// PC�� ����Ʈ ����
			case eItemPointType_PCRoomPoint:
				{
					DWORD dwNeedPoint = pItemInfo->dwPointTypeValue1;

					char buf[MAX_PATH] = {0,};
					char temp[32];
					sprintf( buf, "%s %s", CHATMGR->GetChatMsg( 342 ), CHATMGR->GetChatMsg( 2014 ) );	// "[%s]^n%s���� ��ðڽ��ϱ�? (PC�� ����Ʈ %s �Ҹ�)"
					SafeStrCpy( temp, AddComma( dwNeedPoint ), 32 );

					WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, buf, 
						pItem->GetItemName(), AddComma( BuyPrice ), temp );
				}
				break;
			default:
				{
					WINDOWMGR->MsgBox( MBI_BUYITEM, MBT_YESNO, CHATMGR->GetChatMsg( 342 ), 
						pItem->GetItemName(), AddComma( BuyPrice ) );
				}
				break;
			}
			
		}

	}
}

void CDealDialog::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	if(val == FALSE)
	{
		HideDealer();
	}
	cTabDialog::SetActive(val);

	// 070326 LYW --- StorageDialog : Close inventory.
	if( !val )
	{
		CInventoryExDialog* pWindow = GAMEIN->GetInventoryDialog() ;

		if( pWindow->IsActive() )
		{
			VECTOR2* pPos = pWindow->GetPrevPos() ;
			pWindow->SetAbsXY( (LONG)pPos->x, (LONG)pPos->y ) ;

			pWindow->SetActive( FALSE ) ;
		}
	}

	SetActivePointInfo( FALSE, 0 );
}
void CDealDialog::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if(we == WE_RBTNCLICK)// ||*/ we == WE_LBTNDBLCLICK)//right click//beyond
	{
		OnBuyPushed();
	}
}

// 070329 LYW --- DealDialog : Add function to setting positio.
void CDealDialog::ShowDealDialog( BOOL val )
{
	SetAbsXY( (LONG)m_relPos.x, (LONG)m_relPos.y ) ;

	SetActive( val ) ;

	CInventoryExDialog* pWindow = GAMEIN->GetInventoryDialog() ;

	VECTOR2 vPos = {0, } ;
	vPos.x = pWindow->GetAbsX() ;
	vPos.y = pWindow->GetAbsY() ;

	memcpy( pWindow->GetPrevPos(), &vPos, sizeof(VECTOR2) ) ;

	if( val )
	{
		pWindow->SetAbsXY((LONG)(m_relPos.x + 10) + GetWidth(), (LONG)m_relPos.y ) ;
		if (m_DealerIdx != 9991) //Alemuri Calendar Daily Item
			if( !pWindow->IsActive() )pWindow->SetActive( TRUE ) ;
	}
	
	//Alemuri Calendar Daily Item--------------------------------

	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);

	int day = aTime->tm_mday;
	int month = aTime->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
	int year = aTime->tm_year + 1900; // Year is # years since 1900

	char buffday[3];
	char buffmonth[3];
	char buffyear[5];	
	sprintf(buffday, "%d", day);
	sprintf(buffmonth, "%d", month);
	sprintf(buffyear, "%d", year);

	std::string dayStr(buffday);
	std::string monthStr(buffmonth);
	std::string yearStr(buffyear);
	std::string newDaily = dayStr + "," + monthStr + "," + buffyear;

	if (m_DealerIdx == 9991)
	{
		cIconGridDialog* m_pMyIconGridDlg	= (cIconGridDialog*)GetWindowForID( DE_TABDLG1 );
		m_pMyIconGridDlg->GetIconForIdx( day - 1 )->SetLock( TRUE ); //-1 because icons starts from 0
	
		m_pTitle = (cStatic *)GetWindowForID(STATIC_TITLE);
		if (m_pTitle)
			m_pTitle->SetStaticText("Daily Reward");

		cStatic* rewardTextImg = (cStatic *)GetWindowForID(STATICREWARDTEXT);
		if (rewardTextImg)
			rewardTextImg->SetActive(TRUE);

		cWindow* dailyButton = this->GetWindowForID(DE_GETREWARD);
		if (dailyButton)
			dailyButton->SetActive(TRUE);

		cWindow* sellButton = this->GetWindowForID(DE_SELLITEM);
		if (sellButton)
			sellButton->SetActive(FALSE);
		cWindow* buyButton = this->GetWindowForID(DE_BUYITEM);
		if (buyButton)
			buyButton->SetActive(FALSE);
		cWindow* cancelButton = this->GetWindowForID(DE_CANCEL);
		if (cancelButton)
			cancelButton->SetActive(FALSE);
		cWindow* tab7Button = this->GetWindowForID(DE_TABBTN7);
		if (tab7Button)
			tab7Button->SetActive(FALSE);
		cWindow* tab6Button = this->GetWindowForID(DE_TABBTN6);
		if (tab6Button)
			tab6Button->SetActive(FALSE);
		cWindow* tab5Button = this->GetWindowForID(DE_TABBTN5);
		if (tab5Button)
			tab5Button->SetActive(FALSE);
		cWindow* tab4Button = this->GetWindowForID(DE_TABBTN4);
		if (tab4Button)
			tab4Button->SetActive(FALSE);
		cWindow* tab3Button = this->GetWindowForID(DE_TABBTN3);
		if (tab3Button)
			tab3Button->SetActive(FALSE);
		cWindow* tab2Button = this->GetWindowForID(DE_TABBTN2);
		if (tab2Button)
			tab2Button->SetActive(FALSE);
		cWindow* staticMoneyEdit = this->GetWindowForID(STATICMONEYEDIT);
		if (staticMoneyEdit)
		{
			staticMoneyEdit->SetAlpha(0);
			staticMoneyEdit->SetActive(FALSE);
		}
		cWindow* moneyEdit = this->GetWindowForID(DE_MONEYEDIT);
		if (moneyEdit)
		{
			moneyEdit->SetAlpha(0);
			moneyEdit->SetActive(FALSE);
		}
		cWindow* staticPointEdit = this->GetWindowForID(STATICPOINTEDIT);
		if (staticPointEdit)
		{
			staticPointEdit->SetAlpha(0);
			staticPointEdit->SetActive(FALSE);
		}
		cWindow* pointEdit = this->GetWindowForID(DE_POINTEDIT);
		if (pointEdit)
		{
			pointEdit->SetAlpha(0);
			pointEdit->SetActive(FALSE);
		}
		cWindow* staticPointImage = this->GetWindowForID(DE_POINTIMG);
		if (staticPointImage)
		{
			staticPointImage->SetAlpha(0);
			staticPointImage->SetActive(FALSE);
		}
	}
	else
	{
		//cIconGridDialog* m_pMyIconGridDlg	= (cIconGridDialog*)GetWindowForID( DE_TABDLG1 );
		//if (m_pMyIconGridDlg)
			//m_pMyIconGridDlg->GetIconForIdx( day - 1 )->SetLock( FALSE ); //-1 because icons starts from 0

		m_pTitle = (cStatic *)GetWindowForID(STATIC_TITLE);
		if (m_pTitle)
		{
			m_pTitle->SetStaticText("Shop");
			//m_pTitle->SetStaticText(CHATMGR->GetChatMsg( 234 ));
		}

		cStatic* rewardTextImg = (cStatic *)GetWindowForID(STATICREWARDTEXT);
		if (rewardTextImg)
			rewardTextImg->SetActive(FALSE);

		cWindow* dailyButton = this->GetWindowForID(DE_GETREWARD);
		if (dailyButton)
			dailyButton->SetActive(FALSE);

		cWindow* sellButton = this->GetWindowForID(DE_SELLITEM);
		if (sellButton)
			sellButton->SetActive(TRUE);
		cWindow* buyButton = this->GetWindowForID(DE_BUYITEM);
		if (buyButton)
			buyButton->SetActive(TRUE);
		cWindow* cancelButton = this->GetWindowForID(DE_CANCEL);
		if (cancelButton)
			cancelButton->SetActive(TRUE);
		cWindow* tab7Button = this->GetWindowForID(DE_TABBTN7);
		if (tab7Button)
			tab7Button->SetActive(TRUE);
		cWindow* tab6Button = this->GetWindowForID(DE_TABBTN6);
		if (tab6Button)
			tab6Button->SetActive(TRUE);
		cWindow* tab5Button = this->GetWindowForID(DE_TABBTN5);
		if (tab5Button)
			tab5Button->SetActive(TRUE);
		cWindow* tab4Button = this->GetWindowForID(DE_TABBTN4);
		if (tab4Button)
			tab4Button->SetActive(TRUE);
		cWindow* tab3Button = this->GetWindowForID(DE_TABBTN3);
		if (tab3Button)
			tab3Button->SetActive(TRUE);
		cWindow* tab2Button = this->GetWindowForID(DE_TABBTN2);
		if (tab2Button)
			tab2Button->SetActive(TRUE);
		cWindow* staticMoneyEdit = this->GetWindowForID(STATICMONEYEDIT);
		if (staticMoneyEdit)
		{
			staticMoneyEdit->SetAlpha(255);
			staticMoneyEdit->SetActive(TRUE);
		}
		cWindow* moneyEdit = this->GetWindowForID(DE_MONEYEDIT);
		if (moneyEdit)
		{
			moneyEdit->SetAlpha(255);
			moneyEdit->SetActive(TRUE);
		}
		cWindow* staticPointEdit = this->GetWindowForID(STATICPOINTEDIT);
		if (staticPointEdit)
		{
			staticPointEdit->SetAlpha(255);
			staticPointEdit->SetActive(TRUE);
		}
		cWindow* pointEdit = this->GetWindowForID(DE_POINTEDIT);
		if (pointEdit)
		{
			pointEdit->SetAlpha(255);
			pointEdit->SetActive(TRUE);
		}
		cWindow* staticPointImage = this->GetWindowForID(DE_POINTIMG);
		if (staticPointImage)
		{
			staticPointImage->SetAlpha(255);
			staticPointImage->SetActive(TRUE);
		}	
	}
	//-----------------------------------------------------------
}

// 090227 ShinJS --- Coin Item�� ���԰��ɿ��� �Ǵ��Լ�
BOOL CDealDialog::CanBuyCoinItem( DWORD dwBuyItemIdx, DWORD dwCoinType, DWORD dwCoinCnt ) const
{
	ITEM_INFO * pBuyItemInfo = ITEMMGR->GetItemInfo( dwBuyItemIdx );
	ITEM_INFO * pCoinItemInfo = ITEMMGR->GetItemInfo( dwCoinType );
	if( !pBuyItemInfo || !pCoinItemInfo )
		return FALSE;

	const POSTYPE inventoryStartPosition = TP_INVENTORY_START;
	const POSTYPE inventoryEndPosition = POSTYPE(TP_INVENTORY_END + TABCELL_INVENTORY_NUM * HERO->Get_HeroExtendedInvenCount());
	CInventoryExDialog* inventory = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
	if( !inventory )		return FALSE;

	DWORD dwCoinCntOfInventory = 0;

	// �κ��丮�� �˻��Ͽ� �ش� Coin�� ���������� ���Ѵ�
	for( POSTYPE position = inventoryStartPosition ; inventoryEndPosition > position ; ++position )
	{
		CItem* item = inventory->GetItemForPos( position );
		if( !item )		continue;

		const DWORD	itemIndex	= item->GetItemIdx();

		// �ش� Coin�� ���
		if( itemIndex == dwCoinType )
		{
			const BOOL	isDuplicate = ITEMMGR->IsDupItem( itemIndex );
			const DWORD quantity	= ( isDuplicate ? item->GetDurability() : 1 );

			dwCoinCntOfInventory += quantity;
		}
	}

	// ������ ������ ���
	if( dwCoinCntOfInventory < dwCoinCnt )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1908), pBuyItemInfo->ItemName, pCoinItemInfo->ItemName, dwCoinCnt - dwCoinCntOfInventory );
		return FALSE;
	}

	return TRUE;
}

void CDealDialog::SetActivePointInfo( BOOL bShow, DWORD dwPoint )
{
	m_pPointEdit->SetActive( bShow );
	m_pPointImage->SetActive( bShow );

	m_pPointEdit->SetStaticValue( LONG( dwPoint ) );

	//Alemuri Calendar Daily Item----
	if (m_DealerIdx == 9991)
	{
		m_pPointEdit->SetAlpha(0);
		m_pPointEdit->SetActive(FALSE);
	}
	else
	{
		m_pPointEdit->SetAlpha(255);
		m_pPointEdit->SetActive(TRUE);	
	}
	//-------------------------------
}

BOOL CDealDialog::IsActivePointInfo()
{
	return m_pPointEdit->IsActive();
}