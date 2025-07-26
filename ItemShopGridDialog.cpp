// ItemShopGridDialog.cpp: implementation of the CItemShopGridDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemShopGridDialog.h"

#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "ItemManager.h"
#include "./Interface/cScriptManager.h"
#include "./Interface/cWindowManager.h"
#include "ChatManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemShopGridDialog::CItemShopGridDialog()
{
	m_type		= WT_ICONGRIDDIALOG;
	m_nIconType = WT_ITEM;	//�� �׸�����̾�α׿��� �ٷ� ������ ����
//	m_TabNumber = 0;
}

CItemShopGridDialog::~CItemShopGridDialog()
{}


void CItemShopGridDialog::Init()
{
	m_type = WT_ICONGRIDDIALOG;
}


BOOL CItemShopGridDialog::DeleteItem(POSTYPE Pos,CItem** ppItem)
{
	if( !IsAddable( ( WORD )( Pos - TP_SHOPITEM_START ) ) )
		return DeleteIcon( ( WORD )( Pos - ( TP_SHOPITEM_START ) ), ( cIcon ** )ppItem );
//	if(!IsAddable((WORD)(Pos - (TP_SHOPITEM_START+(TABCELL_SHOPITEM_NUM*m_TabNumber)))))
//		return DeleteIcon((WORD)(Pos-(TP_SHOPITEM_START+(TABCELL_SHOPITEM_NUM*m_TabNumber))), (cIcon **)ppItem);
	else
		return FALSE;
}


BOOL CItemShopGridDialog::AddItem( const ITEMBASE* pItemBase)
{
	if( pItemBase->dwDBIdx == 0 )
	{
		DEBUGMSG( 0, "Item DB idx == 0" );
		return FALSE;
	}

	CItem* newItem = ITEMMGR->GetItem(
		pItemBase->dwDBIdx);

	if(newItem)
	{
		ITEMBASE pIb = newItem->GetItemBaseInfo();
		pIb.Position = pItemBase->Position;

		newItem->SetItemBaseInfo( pIb );

		POSTYPE pos = GetRelativePosition(newItem->GetPosition());
		return AddIcon(pos, newItem);
	}

	// ItemManager�� ������ �����߰�
	newItem = ITEMMGR->MakeNewItem( pItemBase, "CItemShopGridDialog::AddItem" );
	if( newItem == NULL)
		return FALSE;

	newItem->SetMovable(TRUE);

	//�� ��ȯ
	ITEMMGR->RefreshItem( newItem );


	POSTYPE relPos = GetRelativePosition(newItem->GetPosition());
	return AddIcon(relPos, newItem);
}


POSTYPE CItemShopGridDialog::GetRelativePosition( POSTYPE absPos)
{
	return POSTYPE(absPos - TP_SHOPITEM_START);
}

CItem* CItemShopGridDialog::GetItemForPos(POSTYPE absPos)
{
	if(TP_SHOPITEM_START <= absPos && absPos < TP_SHOPITEM_END)
	{
		return ( CItem* )GetIconForIdx( ( WORD )( absPos - TP_SHOPITEM_START ) );
		//return (CItem*)GetIconForIdx((WORD)(absPos-(TP_SHOPITEM_START+(TABCELL_SHOPITEM_NUM*m_TabNumber))));
	}

	return NULL;
}