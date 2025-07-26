#include "stdafx.h"
#include "QuickManager.h"
#include "ObjectManager.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "cQuickItem.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cScriptManager.h"
#include "ItemManager.h"
#include "cSkillTreeManager.h"
#include "./Interface/cIcon.h"
#include "./Input/UserInput.h"
#include "ChatManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/skillinfo.h"
#include "QuickDlg.h"
#include "ConductManager.h"
#include "ConductInfo.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "cSkillBase.h"
#include "StorageDialog.h"

GLOBALTON(CQuickManager);

CQuickManager::CQuickManager()
{
	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_QUICKITEM_STARTINDEX);
}

CQuickManager::~CQuickManager()
{
	m_IconIndexCreator.Release();
}

// 080702 LYW --- QuickManager : È®Àå½½·Ô Ãß°¡ Ã³¸®.
cQuickItem* CQuickManager::NewQuickItem( WORD tab, WORD pos )
{
	cQuickItem* pQuickItem;
	pQuickItem = new cQuickItem( tab, pos );

	pQuickItem->Init(0,0,DEFAULT_ICONSIZE,DEFAULT_ICONSIZE, NULL, m_IconIndexCreator.GenerateIndex());
	WINDOWMGR->AddWindow(pQuickItem);	

	mQuickItemTable[ tab ][ pos ] = pQuickItem;

	return pQuickItem;
}

// 080702 LYW --- QuickManager : È®Àå ½½·Ô Ãß°¡ Ã³¸®.
void CQuickManager::AddQuickItem( WORD tab, WORD pos, SLOT_INFO* pSlot )
{
	cQuickItem* pQuickItem = NULL;
	pQuickItem = mQuickItemTable[ tab ][ pos ];

	if (!pQuickItem) return;

	CItem* pItem = ITEMMGR->GetItem(pSlot->dbIdx);

	if (CheckItemFromStorage(pItem))
		return;

	//Zera Restricción por ID: no permitir el ítem 21000493
	if (pItem && pItem->GetItemIdx() == 21000493)
		return;

	pQuickItem->ClearLink();
	pQuickItem->SetSlotInfo(*pSlot);

	switch (pSlot->kind)
	{
	case QuickKind_None:
		{
		}
		break;
	case QuickKind_SingleItem:
		{
			CItem* pItem = ITEMMGR->GetItem(pSlot->dbIdx);

			// 071126 LYW --- QuickManager : ºÀÀÎ ¾ÆÀÌÅÛ Ã³¸®.
			if (!pItem) return;

			if (pItem)
			{
				pQuickItem->SetCoolTime(pItem->GetRemainedCoolTime());
			}

			const ITEMBASE& pItemBase = pItem->GetItemBaseInfo();

			if (pItemBase.nSealed == eITEM_TYPE_SEAL)
			{
				pQuickItem->SetSeal(TRUE);
			}
		}
		break;
	case QuickKind_MultiItem:
		{
			CItem* item = ITEMMGR->GetItem(pSlot->dbIdx);

			if (item)
			{
				pQuickItem->SetCoolTime(item->GetRemainedCoolTime());
			}

			WORD dur = (WORD)(GAMEIN->GetInventoryDialog()->GetTotalItemDurability(pSlot->idx));
			pQuickItem->SetSlotData(dur);
		}
		break;
	case QuickKind_Skill:
		{
			const LEVELTYPE level = pSlot->data;
			const DWORD index = pSlot->idx;
			cSkillBase* const pSkillBase = SKILLTREEMGR->GetSkill(index);

			if (0 == pSkillBase)
			{
				break;
			}

			// 080303 ggomgrak --- SlotData Update
			pQuickItem->SetSlotData(level);
			pQuickItem->SetData(pSkillBase->GetSkillIdx());

			SKILLTREEMGR->SetToolTipIcon(pQuickItem, index);
		}
		break;
	}

	GetImage(pQuickItem);
	ITEMMGR->AddToolTip(pQuickItem);
}

void CQuickManager::RemoveQuickItem( WORD tab, WORD pos )
{
	cQuickItem* pQuickItem = mQuickItemTable[ tab ][ pos ];

	if( !pQuickItem ) return ;

	pQuickItem->SetSeal(FALSE) ;

	pQuickItem->ClearLink();
	pQuickItem->SetCoolTime( 0 );
}

void CQuickManager::ChangeQuickItem( WORD tab1, WORD pos1, WORD tab2, WORD pos2 )
{
	cQuickItem* pQuickItem1 = mQuickItemTable[ tab1 ][ pos1 ];

	if(!pQuickItem1) return ;

	cQuickItem* pQuickItem2 = mQuickItemTable[ tab2 ][ pos2 ];

	SLOT_INFO info1 = pQuickItem1->GetSlotInfo();
	SLOT_INFO info2 = pQuickItem2->GetSlotInfo();

	RemoveQuickItem( tab1, pos1 );
	RemoveQuickItem( tab2, pos2 );

	AddQuickItem( tab1, pos1, &info2 );
	AddQuickItem( tab2, pos2, &info1 );
}

void CQuickManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_QUICK_INFO:
		{
			Quick_Info( pMsg ) ;
		}
		break;
	case MP_QUICK_ADD_ACK:
		{
			Quick_Add_Ack( pMsg ) ;
		}
		break;
	case MP_QUICK_REMOVE_ACK:
		{
			Quick_Remove_Ack( pMsg ) ;
		}
		break;
	case MP_QUICK_CHANGE_ACK:
		{
			Quick_Change_Ack( pMsg ) ;
		}
		break;
	}
}


void CQuickManager::UseQuickItem( WORD tab, WORD pos )
{
	cQuickItem* pQuickItem = mQuickItemTable[ tab ][ pos ];
	//Alemuri quickitem need a fix if pos == 65535?
	CItem* pItem	=	ITEMMGR->GetItem( pQuickItem->GetSlotInfo().dbIdx );

	if( CheckItemFromStorage( pItem ) )
	{
		DeleteLinkdedQuickItem( pItem->GetItemBaseInfo().dwDBIdx );
		return;
	}	
	
	if(!pQuickItem) return;

	switch( pQuickItem->GetSlotInfo().kind )
	{
	case QuickKind_SingleItem:
		{
			CItem* pItem = ITEMMGR->GetItem( pQuickItem->GetSlotInfo().dbIdx );

			if( pItem )
			{
				GAMEIN->GetInventoryDialog()->UseItem( pItem );
			}
		}
		break;
	case QuickKind_MultiItem:
		{
			CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForIdx( pQuickItem->GetSlotInfo().idx );

			if( pItem )
			{
				GAMEIN->GetInventoryDialog()->UseItem( pItem );
			}

		}
		break;
	case QuickKind_Skill:
		{
#ifdef _TESTCLIENT_
			cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( pQuickItem->GetSlotInfo().idx );

			if( pSkillInfo )
			{
				SKILLMGR->OnSkillCommand( pSkillInfo->GetIndex() + pSkillInfo->GetLevel() );
			}
#else
			const cSkillBase* const pSkillBase = SKILLTREEMGR->GetSkill(
				pQuickItem->GetSlotInfo().idx);

			if( pSkillBase )
			{
				SKILLMGR->OnSkillCommand(
					pSkillBase->GetSkillIdx());
			}
#endif
		}
		break;

		// 070220 LYW --- QuickManager : Add case part to use conduct item.
	case QuickKind_Conduct :
		{
			CONDUCTMGR->OnConductCommand( pQuickItem->GetSlotInfo().idx ) ;
		}
		break ;
	case QuickKind_PetSkill:
		{
			if( !HEROPET )
				return;
			
			CItem* pItem = ITEMMGR->GetItem( pQuickItem->GetSlotInfo().dbIdx );
			if( !pItem )
				return;

			ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pQuickItem->GetSlotInfo().idx );
			if( !pInfo )
				return;

			DWORD target = OBJECTMGR->GetSelectedObjectID();

			DWORD skillindex = HEROPET->GetRealSkillIndex( pInfo->SupplyValue );

			if( target )
			{
				SKILLMGR->OnPetSkillCommand( skillindex );
			}
		}
		break;

	}
}

void CQuickManager::Release()
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];
			ReleaseQuickItem( pQuick );
			mQuickItemTable[ sheet ][ pos ] = NULL;
		}
	}
}

void CQuickManager::RefreshQickItem()
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];

			if(!pQuick) continue ;

			if(QuickKind_MultiItem == pQuick->GetSlotInfo().kind)
			{
				const WORD quantity = WORD(GAMEIN->GetInventoryDialog()->GetTotalItemDurability(
					pQuick->GetSlotInfo().idx));

				pQuick->SetZeroCount(
					quantity <= 0);
				pQuick->SetSlotData(
					quantity);
			}

			ITEMMGR->AddToolTip(
				pQuick);
		}
	}
}

// 071215 LYW --- QuickManager : Äü½½·Ô¿¡ µî·Ï µÈ ¾ÆÀÌÅÛÀÌ ÀÎº¥¿¡¼­ »èÁ¦µÉ ¶§, Äü ¾ÆÀÌÅÛÀ» »èÁ¦ÇÏ´Â ÇÔ¼ö Ãß°¡.
//
// Äü ½½·Ô¿¡ µî·Ï µÈ ¾ÆÀÌÅÛÀÌ ÀÎº¥Åä¸®¿¡¼­ »èÁ¦µÉ ¶§, Äü½½·Ô¿¡ µî·ÏµÈ Äü ¾ÆÀÌÅÛµµ »èÁ¦ÇØ¾ß ÇÑ´Ù. 
// ºÀÀÎ°ü·Ã ¾ÆÀÌÅÛÀÌ³ª, ÀÎÃ¦Æ®, °­È­¿¡ ÀÇÇØ ±âº» ¾ÆÀÌÅÛÁ¤º¸¿¡¼­ º¯°æÀÌ µÈ ¾ÆÀÌÅÛµéÀÇ ÅøÆÁÀÌ »Ñ·ÁÁö°í ÀÖ´Â»óÈ²¿¡¼­, 
// ÀÎº¥¿¡¼­¸¸ ¾ÆÀÌÅÛÀÌ »èÁ¦µÉ °æ¿ì, ÀÌÀü¿¡ ÁøÇàÇÏ°í ÀÖ´ø Ã³¸®°úÁ¤ÀÇ Á¤º¸µéÀ» ¾òÁö ¸øÇØ, ¿¡·¯°¡ ¹ß»ıÇÏ±â ¶§¹®ÀÌ´Ù.
//
void CQuickManager::DeleteLinkdedQuickItem(DWORD dwDBIdx)
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ] ;

			if( !pQuick ) continue ;

			if( pQuick->GetSlotInfo().kind != QuickKind_SingleItem ) continue ; 			

			if(pQuick->GetSlotInfo().dbIdx != dwDBIdx) continue ;

			RemoveQuickItem(sheet, pos) ;
		}
	}
}

// 071215 LYW --- QuickManager : ºÀÀÎµÈ ¾ÆÀÌÅÛÀÌ ¸µÅ©°¡ °É·ÁÀÖÀ» °æ¿ì,
// ºÀÀÎÀÌ Ç®·ÈÀ» °æ¿ì ºÀÀÎÀ» Ç®¾îÁÖ±â À§ÇÑ ÇÔ¼ö Ãß°¡.
void CQuickManager::UnsealQuickItem(DWORD dwDBIdx)
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		// Ã¹¹øÂ° ½½·Ô ºÀÀÎ ÇØÁ¦ Ã³¸®.
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ] ;

			if( !pQuick ) continue ;

			if( pQuick->GetSlotInfo().kind != QuickKind_SingleItem ) continue ; 			

			if(pQuick->GetSlotInfo().dbIdx != dwDBIdx) continue ;

			pQuick->SetSeal(FALSE) ;
		}
	}
}

void CQuickManager::ReleaseQuickItem(cQuickItem * quick)
{
	if( quick == NULL )
	{
		ASSERT(0);
		return;
	}
	m_IconIndexCreator.ReleaseIndex(quick->GetID());
	WINDOWMGR->AddListDestroyWindow( quick );
}

void CQuickManager::GetImage( cQuickItem * pQuickItem )
{
	cImage lowImage;
	cImage highImage;

	switch(pQuickItem->GetSlotInfo().kind)
	{
	case QuickKind_None:
		{
			pQuickItem->SetImage2( NULL, NULL );
		}
		break;
	case QuickKind_SingleItem:
	case QuickKind_MultiItem:
	case QuickKind_PetItem:
	case QuickKind_PetSkill:
		{
			SCRIPTMGR->GetImage(
				ITEMMGR->GetItemInfo(pQuickItem->GetSlotInfo().idx)->Image2DNum,
				&lowImage,
				PFT_ITEMPATH);
			pQuickItem->SetImage2(
				&lowImage,
				0);
		}
		break;
	case QuickKind_Skill:
		{
			const cSkillBase* const skillBase = SKILLTREEMGR->GetSkill(
				pQuickItem->GetSlotInfo().idx);

			if(0 == skillBase)
			{
				break;
			}

			const cSkillInfo* const skillInfo = SKILLMGR->GetSkillInfo(
				skillBase->GetSkillIdx());

			if(0 == skillInfo)
			{
				break;
			}

			SCRIPTMGR->GetImage(
				skillInfo->GetImage(),
				&highImage,
				PFT_SKILLPATH);
			pQuickItem->SetImage2(
				&lowImage,
				&highImage );
		}
		break;
		// 070216 LYW --- QuickManager : Add case part for conduct to GetImage function.
	case QuickKind_Conduct :
		{
			cConductInfo* pConductInfo = CONDUCTMGR->GetConductInfo(
				pQuickItem->GetSlotInfo().idx ) ;

			if( pConductInfo == NULL )
			{
				ASSERT(0) ;
				return ;
			}

			if( pConductInfo->GetHighImage() != -1 )
				SCRIPTMGR->GetImage( pConductInfo->GetHighImage(), &highImage, PFT_CONDUCTPATH ) ;

			pQuickItem->SetImage2( &lowImage, &highImage ) ;
		}
		break ;
	}
}

BOOL CQuickManager::CheckQPosForItemIdx( DWORD ItemIdx )
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];

			if( pQuick->GetSlotInfo().idx == ItemIdx )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


BOOL CQuickManager::CheckQPosForDBIdx( DWORD DBIdx )
{
	for( WORD sheet = 0; sheet < TOTAL_SLOTPAGE; sheet++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			cQuickItem* pQuick = mQuickItemTable[ sheet ][ pos ];

			if( pQuick->GetSlotInfo().dbIdx == DBIdx )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void CQuickManager::Quick_Info( void* pMsg )
{
	MSG_QUICK_INFO* pmsg = ( MSG_QUICK_INFO* )pMsg;

	for( WORD tab = 0; tab < TOTAL_SLOTPAGE; tab++ )
	{
		for( WORD pos = 0; pos < MAX_SLOTNUM; pos++ )
		{
			if(tab < MAX_SLOTPAGE)
			{
				AddQuickItem( tab, pos, &( pmsg->tab[ tab ].slot[ pos ] ) );
			}
			else
			{
				AddQuickItem( tab, pos, &( pmsg->tab[ tab ].slot[ pos ] ) );
			}
		}
	}
}

void CQuickManager::Quick_Add_Ack( void* pMsg )
{
	MSG_QUICK_ADD_SYN* pmsg = ( MSG_QUICK_ADD_SYN* )pMsg;

	if(pmsg->tabNum < MAX_SLOTPAGE)
	{
		AddQuickItem( pmsg->tabNum, pmsg->pos, &( pmsg->slot ) );
	}
	else
	{
		AddQuickItem( pmsg->tabNum, pmsg->pos, &( pmsg->slot ) );
	}
}

void CQuickManager::Quick_Remove_Ack( void* pMsg )
{
	MSG_QUICK_REMOVE_SYN* pmsg = ( MSG_QUICK_REMOVE_SYN* )pMsg;

	RemoveQuickItem( pmsg->tabNum, pmsg->pos );
}

void CQuickManager::Quick_Change_Ack( void* pMsg )
{
	MSG_QUICK_CHANGE_SYN* pmsg = ( MSG_QUICK_CHANGE_SYN* )pMsg;

	ChangeQuickItem( pmsg->tabNum1, pmsg->pos1, pmsg->tabNum2, pmsg->pos2 );
}

void CQuickManager::AddToolTip( cQuickItem* pQuickItem )
{
	switch(pQuickItem->GetSlotInfo().kind)
	{
	case QuickKind_Skill:
		{
			const cSkillBase* const skillBase = SKILLTREEMGR->GetSkill(
				pQuickItem->GetSlotInfo().idx);

			if(0 == skillBase)
			{
				break;
			}

			SKILLTREEMGR->SetToolTipIcon(
				pQuickItem,
				skillBase->GetSkillIdx());
			break;
		}
	default:
		{
			ITEMMGR->AddToolTip(
				pQuickItem);
			break;
		}
	}
}

void CQuickManager::RefreshSkillItem( DWORD skillIndex )
{
	for( WORD i = 0; i < TOTAL_SLOTPAGE; ++i )
	{
		for( WORD j = 0; j < MAX_SLOTNUM; ++j )
		{
			cQuickItem* item = mQuickItemTable[ i ][ j ];

			if(0 == item)
			{
				continue;
			}
			else if( QuickKind_Skill != item->GetSlotInfo().kind )
			{
				continue;
			}

			const cSkillBase* const quickSkill = SKILLTREEMGR->GetSkill(
				item->GetData());
			const cSkillBase* const refreshSkill = SKILLTREEMGR->GetSkill(
				skillIndex);

			if(0 == quickSkill)
			{
				RemoveQuickItem(
					i,
					j);
			}
			else if(quickSkill == refreshSkill)
			{
				item->SetData(
					refreshSkill->GetSkillIdx());
				GetImage(
					item);
			}
		}
	}
}

BOOL CQuickManager::CheckItemFromStorage( CItem* pItem )
{
	if( !pItem )
		return FALSE;
	
	CStorageDialog* pStorageDlg	=	GAMEIN->GetStorageDialog();

	int iStorageNum	=	pStorageDlg->GetStorageNum( pItem->GetItemBaseInfo().Position );

	if( iStorageNum < 0 )
		return FALSE;

	return	TRUE;
}