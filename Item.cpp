#include "stdafx.h"
#include "Item.h"
#include "ItemManager.h"
#include "ObjectManager.h"
#include "cQuickItem.h"
#include "GameIn.h"
#include "Interface/cFont.h"

#include "ChatManager.h"


CItem::CItem( const ITEMBASE* pInfo )
{
	m_type					= WT_ITEM;
	m_bAdded				= FALSE;
	m_ItemBaseInfo			= *pInfo;
	m_pLinkItem				= NULL;
	m_LinkPos				= 0;
	m_Param					= 0;
	
	m_bNoDeffenceTooltip = FALSE;

	slotEnchant = 0;
}


CItem::~CItem()
{}


/*
eITEM_KINDBIT CItem::GetItemKind()
{
	ITEM_INFO * pInfo = ITEMMGR->GetItemInfo(m_ItemBaseInfo.wIconIdx);
	ASSERT(pInfo);	//KES 040512
	if(pInfo)
		return (eITEM_KINDBIT)pInfo->ItemKind;

	return (eITEM_KINDBIT)0;
}
*/

const ITEM_INFO* CItem::GetItemInfo()
{
	ITEM_INFO * pInfo = ITEMMGR->GetItemInfo(m_ItemBaseInfo.wIconIdx);
	ASSERT(pInfo);
	if(pInfo)
	ASSERT(pInfo->ItemIdx == m_ItemBaseInfo.wIconIdx);
	return pInfo;
}


void CItem::SetItemBaseInfo( const ITEMBASE& itemInfo )
{
	m_ItemBaseInfo = itemInfo;
}


void CItem::Render()
{
	if( !m_bActive )
	{
		return;
	}
	if( m_ItemBaseInfo.nSealed == eITEM_TYPE_SEAL )
	{
		cDialog::Render();

		m_SealImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
									RGBA_MERGE(0xffffff, 255));
		
		/*RECT rect={(LONG)m_absPos.x, (LONG)m_absPos.y+12, 1,1};
		CFONT_OBJ->RenderFont(0,"[����]", strlen("[����]"),&rect,RGBA_MAKE(250,250,128,255));*/
	}
	// 071124 LYW --- Item : ���� ���� ������ �ð� ���� ó��.
	else if( m_ItemBaseInfo.nSealed == eITEM_TYPE_UNSEAL )
	{
		cIcon::Render();

		DWORD dwElapsedMill	= gCurTime -m_ItemBaseInfo.LastCheckTime ;
		DWORD dwElapsedSecond = dwElapsedMill / 1000 ;

		SetLastCheckTime(gCurTime - ( dwElapsedMill - dwElapsedSecond * 1000 )) ;

		if( GetItemBaseInfo().nRemainSecond > (int)dwElapsedSecond )
		{
			int nRemainTime = GetItemBaseInfo().nRemainSecond - (int)dwElapsedSecond ;
			SetRemainTime(nRemainTime) ;
		}
		else
		{
			SetRemainTime(0) ;
		}
	}
	else
	{
		cIcon::Render();
	}

	if( m_Param == 1 )
	{
		m_LockImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
			RGBA_MERGE(0xffffff, m_alpha * m_dwOptionAlpha / 100));
	}

	if( m_bZeroCount )
	{
		m_ZeroImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
									RGBA_MERGE(0xffffff, 200 * m_dwOptionAlpha / 100));
	}

	if( mIsVisibleDurability &&
		ITEMMGR->IsDupItem(m_ItemBaseInfo.wIconIdx) )
	{
		static char nums[3];
		wsprintf(nums,"%2d", m_ItemBaseInfo.Durability);
		//RECT rect={(LONG)m_absPos.x+23, (LONG)m_absPos.y+25, 1,1};
		//CFONT_OBJ->RenderFont(0,nums,strlen(nums),&rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));		// color hard coding : taiyo 
		RECT rect ;

		rect.top = (LONG)m_absPos.y+24 ;
		rect.right = 1 ;
		rect.bottom = 1 ;

		if( m_ItemBaseInfo.Durability > 99 )
		{
			rect.left = (LONG)m_absPos.x+16 ;
			
		}
		else
		{
			rect.left = (LONG)m_absPos.x+22 ;
		}
		m_dwImageRGB = RGB(200, 200, 200);//RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 )
		CFONT_OBJ->RenderFontShadow(13, 1, nums,strlen(nums),&rect,RGBA_MAKE(0,255,255,255));		// color hard coding : taiyo RGBA_MAKE(100,225,255,255))
	}

	if (slotEnchant > 0) //alemuri
	{
		static char nums[3];
		wsprintf(nums, "%2d", slotEnchant);
		//RECT rect={(LONG)m_absPos.x+23, (LONG)m_absPos.y+25, 1,1};
		//CFONT_OBJ->RenderFont(0,nums,strlen(nums),&rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));		// color hard coding : taiyo 
		RECT rect;

		rect.top = (LONG)m_absPos.y + 0;
		rect.right = 1;
		rect.bottom = 1;

		char plus[2];
		plus[0] = nums[0];
		plus[1] = nums[1];
		nums[0] = '+';
		nums[1] = plus[0];
		nums[2] = plus[1];
		rect.left = (LONG)m_absPos.x + 14;

		CFONT_OBJ->RenderFontShadow(0, 1, nums, strlen(nums), &rect, RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));		// color hard coding : taiyo 
	}
}

void CItem::SetLock( BOOL bLock )
{
	cIcon::SetLock( bLock );
	
	SetMovable( !bLock );
	//������ ��뵵 ����
}

BOOL CItem::CanMoveToDialog( cDialog* ptargetdlg )
{
	return IsMovable();
}

BOOL CItem::CanDelete()
{
	return IsMovable();
}

// 071124 LYW --- Item : �̺�Ʈ �Լ� �߰�.
DWORD CItem::ActionEvent(CMouse* mouseInfo)
{
	const DWORD windowEvent = CBaseItem::ActionEvent(
		mouseInfo);

	switch(m_ItemBaseInfo.nSealed)
	{
	case eITEM_TYPE_UNSEAL:
		{
			if(WE_MOUSEOVER != windowEvent)
			{
				break;
			}

			ITEMMGR->AddTimeToolTip(
				this,
				ITEMMGR->GetOption(GetItemBaseInfo()),
				FALSE);
			break;
		}
	}

	return windowEvent;
}