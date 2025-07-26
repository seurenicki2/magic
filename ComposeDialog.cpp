#include "stdafx.h"
#include "ComposeDialog.h"
#include "ItemManager.h"
#include "WindowIDEnum.h"
#include "cWindowManager.h"
#include "interface/cIconGridDialog.h"
#include "interface/cIconDialog.h"
#include "interface/cListDialog.h"
#include "interface/cButton.h"
#include "interface/cStatic.h"
#include "cMsgBox.h"
#include "cScriptManager.h"
#include "ChatManager.h"
#include "ProgressDialog.h"
#include "../[CC]Header/GameResourceManager.h"
#include "MHMap.h"

const WORD firstSourcePosition	= 0;
const WORD secondSourcePosition	= 1;
const WORD resultPosition		= 2;

CComposeDialog::CComposeDialog() :
mOptionListDialog( 0 ),
mSourceIconDialog( 0 ),
mResultIconGridDialog( 0 )
{
	ZeroMemory( &mKeyIconBase,		sizeof( mKeyIconBase ) );
	ZeroMemory( &mComposeMessage,	sizeof( mComposeMessage ) );
}

CComposeDialog::~CComposeDialog()
{
	Release();
}

void CComposeDialog::Linking()
{
	Release();

	mPageStatic				= ( cStatic*			)GetWindowForID( COMPOSE_PAGE_STATIC );
	mPagePreviousButton		= ( cButton*			)GetWindowForID( COMPOSE_PAGE_PREV_BUTTON );
	mPageNextButton			= ( cButton*			)GetWindowForID( COMPOSE_PAGE_NEXT_BUTTON );
	mSubmitButton			= ( cButton*			)GetWindowForID( COMPOSE_SUBMIT_BUTTON );
	mOptionListDialog		= ( cListDialog*		)GetWindowForID( COMPOSE_OPTION_LISTDLG );
	mSourceIconDialog		= ( cIconDialog*		)GetWindowForID( COMPOSE_SOURCE_ICONDLG );
	mResultIconGridDialog	= ( cIconGridDialog*	)GetWindowForID( COMPOSE_RESULT_ICONGRIDDLG );

	if( ! mPageStatic			||
		! mPagePreviousButton	||
		! mPageNextButton		||
		! mSubmitButton			||
		! mOptionListDialog		||
		! mSourceIconDialog		||
		! mResultIconGridDialog )
	{
		cprintf( "linking is failed in CComposeDialog\n" );
		return;
	}

	if( mOptionListDialog )
	{
		mOptionListDialog->SetShowSelect( TRUE );
	}

	// 080916 LUJ, �������� �Ҵ��ϰ� â�� �����Ų��.
	{
		ITEMBASE emptyItemBase;
		ZeroMemory( &emptyItemBase, sizeof( emptyItemBase ) );

		cImage image;
		SCRIPTMGR->GetImage( 0, &image, PFT_HARDPATH );		

		for(WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
		{
			CItem* item = new CItem( &emptyItemBase );
			item->SetToolTip( "", RGB_HALF( 255, 255, 255 ), &image );
			item->SetMovable( FALSE );
			
			mSourceIconDialog->AddIcon( i, item );
			mItemList.push_back( item );
		}

		for(WORD i = 0; i < mResultIconGridDialog->GetCellNum(); ++i )
		{
			CItem* item = new CItem( &emptyItemBase );
			item->SetToolTip( "", RGB_HALF( 255, 255, 255 ), &image );
			item->SetMovable( FALSE );
			
			mResultIconGridDialog->AddIcon( i, item );
			mItemList.push_back( item );
		}
	}
}

void CComposeDialog::Release()
{
	{
		for(	ItemList::iterator it = mItemList.begin();
				mItemList.end() != it;
				++it )
		{
			SAFE_DELETE( *it );
		}

		mItemList.clear();
	}
}

BOOL CComposeDialog::FakeMoveIcon( LONG x, LONG y, cIcon* fakeMoveIcon )
{
	CItem* fakeMoveItem = ( CItem* )fakeMoveIcon;

	if( !	fakeMoveItem				||
			fakeMoveItem->IsLocked()	||
			WT_ITEM != fakeMoveItem->GetType() )
	{
		return FALSE;
	}
	// 080916 LUJ, �κ��丮�̿ܿ� ������ FALSE
	else if( ! ITEMMGR->IsEqualTableIdxForPos( eItemTable_Inventory, fakeMoveItem->GetPosition() ) )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1678 ) );
		return FALSE;
	}
	else if( ITEMMGR->IsDupItem( fakeMoveItem->GetItemIdx() ) )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1679 ) );
		return FALSE;
	}

	const ComposeScript* script = GAMERESRCMNGR->GetComposeScript( fakeMoveItem->GetItemIdx() );

	if( ! script )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1680 ) );
		return FALSE;
	}
	else if( script->mKeyItemIndex != mKeyIconBase.wIconIdx )
	{
		WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_OK,
			CHATMGR->GetChatMsg( 1681 ) );
		return FALSE;
	}

	// 080916 LUJ, ��� �������� �ϳ� ��ϵ� ���¿���, �巡���� ��Ḧ �ٸ� ��ġ�� �������� ��쿡 ��ũ��Ʈ ��ġ �˻縦 �Ѵ�
	{
		CItem* firstSourceItem	= ( CItem* )mSourceIconDialog->GetIconForIdx( firstSourcePosition );
		CItem* secondSourceItem	= ( CItem* )mSourceIconDialog->GetIconForIdx( secondSourcePosition );

		if( !	firstSourceItem							||
				firstSourceItem->GetType()	!= WT_ITEM	||
			!	secondSourceItem						||
				secondSourceItem->GetType()	!= WT_ITEM )
		{
			return FALSE;
		}
		
		WORD position = WORD(-1);
		mSourceIconDialog->GetPositionForXYRef( x, y, position );

		// 080916 LUJ, ���� ����ϴ� ������ �� ���� ���� ��ũ��Ʈ�� ����ؾ��Ѵ�. ���� ��쿡�� üũ�ϸ� �ȴ�
		//			1. �� ���Կ� �̹� ��ᰡ �ְ�, �� ���Կ� �������� �õ��� ���
		//			2. �� ���� ��� ��ᰡ �ִ� ���¿���, ���� ���Կ� �������� �õ��� ���	
		const BOOL isNeedCheck = (
			( firstSourceItem->GetItemIdx()		&& secondSourceItem->GetItemIdx() )			||
			( firstSourceItem->GetItemIdx()		&& ( firstSourcePosition != position ) )	||
			( secondSourceItem->GetItemIdx()	&& ( secondSourcePosition != position ) ) );

		if( isNeedCheck )
		{
			for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
			{
				CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

				if( !	item ||
						item->GetType() != WT_ITEM	||
					!	item->GetDBIdx() )
				{
					continue;
				}
				else if( script != GAMERESRCMNGR->GetComposeScript( item->GetItemIdx() ) )
				{
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg( 1681 ) );
					return FALSE;
				}
			}
		}
	}

	// 080916 LUJ, ù��° Ȥ�� �ι�° �����ܿ� �ҽ� �������� �����Ų��
	{
		WORD position = 0;

		if( !	mSourceIconDialog->GetPositionForXYRef( x, y, position )	||
				position == resultPosition )
		{
			return FALSE;
		}

		// 080916 LUJ, ����� ���� �������� �ʱ�ȭ����
		Clear( mSourceIconDialog->GetIconForIdx( resultPosition ) );

		CItem* updateItem = ( CItem* )mSourceIconDialog->GetIconForIdx( position );

		if( !	updateItem ||
				updateItem->GetType() != WT_ITEM )
		{
			return FALSE;
		}
		else
		{
			CItem* item = ITEMMGR->GetItem( updateItem->GetDBIdx() );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		UpdateItem( *updateItem, fakeMoveItem->GetItemBaseInfo() );

		// 080916 LUJ, ��ġ��Ų �������� ����� �� ������ ��ٴ�
		fakeMoveItem->SetLock( TRUE );
	}

	DWORD sourceItemSize = 0;

	for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
	{
		CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

		if( !	item						||
				item->GetType() != WT_ITEM	||
			!	item->GetItemIdx() )
		{
			continue;
		}

		++sourceItemSize;
	}

	// 080916 LUJ, �ռ� ������ �����̸� ��ư�� Ȱ��ȭ�Ѵ�.
	if( sourceItemSize == script->mSourceSize )
	{
		mSubmitButton->SetActive( TRUE );
	}
	// 080916 LUJ, �������� �ϳ��� ��ϵ� ��쿡�� ����� �����Ѵ�
	else if( 1 == sourceItemSize )
	{
		// 080916 LUJ, �ɼ� ����Ʈ�� ������ �ɼ��� ���
		{
			mOptionListDialog->RemoveAll();

			const ComposeScript::OptionMap& optionMap = script->mOptionMap;

			for(	ComposeScript::OptionMap::const_iterator it = optionMap.begin();
					optionMap.end() != it;
					++it )
			{
				const ITEM_OPTION::Drop::Key&	key		= it->first;
				const ComposeScript::Option&	option	= it->second;

				char text[ MAX_PATH ] = { 0 };

				switch( key )
				{
				case ITEM_OPTION::Drop::KeyPlusStrength:
				case ITEM_OPTION::Drop::KeyPlusIntelligence:
				case ITEM_OPTION::Drop::KeyPlusDexterity:
				case ITEM_OPTION::Drop::KeyPlusWisdom:
				case ITEM_OPTION::Drop::KeyPlusVitality:
				case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
				case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
				case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
				case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
				case ITEM_OPTION::Drop::KeyPlusCriticalRate:
				case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
				case ITEM_OPTION::Drop::KeyPlusAccuracy:
				case ITEM_OPTION::Drop::KeyPlusEvade:
				case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
				case ITEM_OPTION::Drop::KeyPlusLife:
				case ITEM_OPTION::Drop::KeyPlusMana:
				case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
				case ITEM_OPTION::Drop::KeyPlusManaRecovery:
					{
						sprintf(
							text,
							"%s %+0.1f",
							ITEMMGR->GetName( key ),
							option.mValue );
						break;
					}
				case ITEM_OPTION::Drop::KeyPercentStrength:
				case ITEM_OPTION::Drop::KeyPercentIntelligence:
				case ITEM_OPTION::Drop::KeyPercentDexterity:
				case ITEM_OPTION::Drop::KeyPercentWisdom:
				case ITEM_OPTION::Drop::KeyPercentVitality:
				case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
				case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
				case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
				case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
				case ITEM_OPTION::Drop::KeyPercentCriticalRate:
				case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
				case ITEM_OPTION::Drop::KeyPercentAccuracy:
				case ITEM_OPTION::Drop::KeyPercentEvade:
				case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
				case ITEM_OPTION::Drop::KeyPercentLife:
				case ITEM_OPTION::Drop::KeyPercentMana:
				case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
				case ITEM_OPTION::Drop::KeyPercentManaRecovery:
					{
						sprintf(
							text,
							"%s %+0.1f%%",
							ITEMMGR->GetName( key ),
							option.mValue * 100.0f );
						break;
					}
				}

				mOptionListDialog->AddItemAutoLine( text, CTC_OPERATEITEM );
			}
		}

		UpdateResult( *script, 0 );
	}
	
	return FALSE;
}

void CComposeDialog::OnActionEvent( LONG id, void * p, DWORD event )
{
	switch( id )
	{
	case COMPOSE_SUBMIT_BUTTON:
		{
			CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( firstSourcePosition );

			if( !	item	||
					item->GetType() != WT_ITEM )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, "Source item is not found" );
				break;
			}

			const ComposeScript* script = GAMERESRCMNGR->GetComposeScript( item->GetItemIdx() );

			if( ! script )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, "Script is not found" );
				break;
			}

			const int selectedRow = mOptionListDialog->GetClickedRowIdx();

			if( 0 > selectedRow )
			{
				WINDOWMGR->MsgBox(
					MBI_NOTICE,
					MBT_OK,
					CHATMGR->GetChatMsg( 1682 ) );
				break;
			}
			else if( int( script->mOptionMap.size() ) <= selectedRow )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, "It's invalid option" );
				break;
			}

			ZeroMemory( &mComposeMessage, sizeof( mComposeMessage ) );
			mComposeMessage.Category	= MP_ITEM;
			mComposeMessage.Protocol	= MP_ITEM_COMPOSE_SYN;
			mComposeMessage.dwObjectID	= HEROID;

			// 080916 LUJ, ������ �ɼ��� �־��ش�
			{
				ComposeScript::OptionMap::const_iterator it = script->mOptionMap.begin();			
				std::advance( it, selectedRow );

				mComposeMessage.mOptionKey = it->first;
			}

			// 080916 LUJ, ������ ����� �־��ش�
			{
				if( script->mResultSet.empty() )
				{
					break;
				}

				const char* textPage = mPageStatic->GetStaticText();

				if( ! textPage )
				{
					break;
				}

				const LONG		resultPosition	= ( mResultIconGridDialog->GetCurSelCellPos() + max( 0, atoi( textPage ) - 1 ) * mResultIconGridDialog->GetCellNum() );
				const BOOL		isHidden		= ( script->mResultSet.size() != script->mResultMap.size() );
				const DWORD		resultSize		= script->mResultSet.size() + ( isHidden ? 1 : 0 );

				CItem* item = GetResultItem( POSTYPE( mResultIconGridDialog->GetCurSelCellPos() ) );

				if( !	item	||
						resultSize <= DWORD( resultPosition ) )
				{
					WINDOWMGR->MsgBox(
						MBI_NOTICE,
						MBT_OK,
						CHATMGR->GetChatMsg( 1683 ) );
					break;
				}
				// 080916 LUJ, ������ ��ȣ�� ������ ���� ����� �����ߴٴ� ��
				else if( ! item->GetItemIdx() )
				{
					mComposeMessage.mIsRandomResult = TRUE;					
				}
				else
				{
					ICONBASE& icon = mComposeMessage.mResultItem;
					icon.wIconIdx	= item->GetItemIdx();
					icon.dwDBIdx	= item->GetDBIdx();
					icon.Position	= item->GetPosition();
				}
			}

			// 080917 LUJ, Ű �������� �����Ѵ�
			mComposeMessage.mKeyItem = mKeyIconBase;

			// 080916 LUJ, ��Ḧ �־�����
			for( WORD i = 0; i < script->mSourceSize; ++i )
			{
				CItem* item = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

				if( !	item	||
						item->GetType() != WT_ITEM )
				{
					continue;
				}

				ICONBASE& icon = mComposeMessage.mUsedItem[ ( mComposeMessage.mUsedItemSize )++ ];
				icon.wIconIdx	= item->GetItemIdx();
				icon.dwDBIdx	= item->GetDBIdx();
				icon.Position	= item->GetPosition();
			}

			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );

				if( dialog )
				{
					dialog->Wait( CProgressDialog::eActionCompose );
				}
			}

			SetDisable( TRUE );
			break;
		}
	case COMPOSE_CANCEL_BUTTON:
	case CMI_CLOSEBTN:
		{
			SetActive( FALSE );
			break;
		}
	case COMPOSE_PAGE_PREV_BUTTON:
	case COMPOSE_PAGE_NEXT_BUTTON:
		{
			for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
			{
				CItem* sourceItem = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

				if( !	sourceItem	||
						sourceItem->GetType() != WT_ITEM )
				{
					continue;
				}

				const ComposeScript* script = GAMERESRCMNGR->GetComposeScript( sourceItem->GetItemIdx() );

				if( ! script )
				{
					continue;
				}

				const char* page = mPageStatic->GetStaticText();

				if( ! page )
				{
					break;
				}

				// 080916 LUJ, ������ ������ 1���� ǥ�õǳ� ������ 0�����̴�. �׷��� 1�� ���ش�
				UpdateResult(
					*script,
					-1 + atoi( page ) + ( id == COMPOSE_PAGE_PREV_BUTTON ? -1 : 1 ) );
				break;
			}

			break;
		}
	}
}

void CComposeDialog::SetActive( BOOL isActive )
{
	if(MAP->GetMapNum() == StreetTournament && isActive)
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, "Compose is disable at this map" );
		return;
	}

	if( isActive )
	{
		Clear( *mResultIconGridDialog );
		Clear( *mSourceIconDialog );

		mOptionListDialog->RemoveAll();
		mSubmitButton->SetActive( FALSE );

		mPageStatic->SetStaticText( "0" );
		mPageNextButton->SetActive( FALSE );
		mPagePreviousButton->SetActive( FALSE );

		// 080916 LUJ, �κ��丮�� �Բ� ���� ��ġ�� ��ġ�ȴ�
		{
			cDialog* inventory = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( inventory );

			const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
			const DWORD			space	= 150;
			const DWORD			x		= ( screen.dwWidth - m_width - inventory->GetWidth() - space ) / 2;
			const DWORD			y		= ( screen.dwHeight - max( inventory->GetHeight(), m_height ) ) / 2;

			SetAbsXY( x + space + inventory->GetWidth(), y );

			inventory->SetAbsXY( x, y );
			inventory->SetActive( TRUE );
		}
	}
	// 080916 LUJ, Ű �����۰� ��ϵ� ������ ����� �����Ѵ�
	else
	{
		CItem* keyItem = ITEMMGR->GetItem( mKeyIconBase.dwDBIdx );

		if( keyItem )
		{
			keyItem->SetLock( FALSE );
		}

		// 080916 LUJ, ��ϵ� �������� ����� �����Ѵ�
		for( WORD i = 0; i < mSourceIconDialog->GetCellNum(); ++i )
		{
			CItem* sourceItem = ( CItem* )mSourceIconDialog->GetIconForIdx( i );

			if( !	sourceItem	||
					sourceItem->GetType() != WT_ITEM )
			{
				continue;
			}

			CItem* inventoryItem = ITEMMGR->GetItem( sourceItem->GetDBIdx() );

			if( ! inventoryItem )
			{
				continue;
			}

			inventoryItem->SetLock( FALSE );
		}
	}

	cDialog::SetActive( isActive );
}

void CComposeDialog::SetKeyItem( const ITEMBASE& keyItem )
{
	mKeyIconBase.wIconIdx	= keyItem.wIconIdx;
	mKeyIconBase.dwDBIdx	= keyItem.dwDBIdx;
	mKeyIconBase.Position	= keyItem.Position;
}

// 080916 LUJ, ��Ʈ���� �ʱ�ȭ�Ѵ�
void CComposeDialog::Clear( cIconDialog& dialog ) const
{
	for( WORD i = 0; i < dialog.GetCellNum(); ++i )
	{
		Clear( dialog.GetIconForIdx( i ) );
	}

	dialog.SetCurSelCellPos( -1 );
}

// 080916 LUJ, ��Ʈ���� �ʱ�ȭ�Ѵ�
void CComposeDialog::Clear( cIconGridDialog& dialog ) const
{
	for( WORD i = 0; i < dialog.GetCellNum(); ++i )
	{
		Clear( dialog.GetIconForIdx( i ) );
	}

	dialog.SetCurSelCellPos( -1 );
}

// 080916 LUJ, ��Ʈ���� �ʱ�ȭ�Ѵ�
void CComposeDialog::Clear( cIcon* icon ) const
{
	if( !	icon	||
			icon->GetType() != WT_ITEM )
	{
		return;
	}

	CItem& item = *( ( CItem* )icon );

	cImage image;
	item.SetBasicImage( &image );

	ITEMBASE emptyItemBase;
	ZeroMemory( &emptyItemBase, sizeof( emptyItemBase ) );	
	item.SetItemBaseInfo( emptyItemBase);
	item.SetToolTip( "" );
}

CItem* CComposeDialog::GetResultItem( POSTYPE position )
{
	CItem* item = ( CItem* )mResultIconGridDialog->GetIconForIdx( WORD( position ) );

	if( !	item	||
			item->GetType() != WT_ITEM )
	{
		return 0;
	}

	return item;
}

// 080916 LUJ, �ռ� ���� �޽��� ó��
void CComposeDialog::Proceed( const MSG_ITEM_COMPOSE_ACK& message )
{
	// 080916 LUJ, ��� �������� ��������(�������� ��Ḧ ����� �ٲپ���)
	{
		// 090122 LUJ, ITEMBASE Ÿ������ ����
		const ITEMBASE& resultItem = message.mResultItem;

		CItem* item = ITEMMGR->GetItem( resultItem.dwDBIdx );

		if( ! item )
		{
			cprintf( "error: result item is removed by unknown reason" );
			return;
		}

		item->SetItemBaseInfo( resultItem );
		cImage iconImage;
		item->SetBasicImage( ITEMMGR->GetIconImage( resultItem.wIconIdx, &iconImage ) );
		item->SetLock( FALSE );
		ITEMMGR->RefreshItem( item );
	}

	// 080916 LUJ, �Ҹ�� ��Ḧ ��������
	for( DWORD i = 0; i < message.mUsedItemSize; ++i )
	{
		const ICONBASE& iconBase	= message.mUsedItem[ i ];
		CItem*			item		= 0;

		ITEMMGR->DeleteItem( iconBase.Position, &item );
	}

	// 080916 LUJ, �Ҹ�� Ű �������� ��������
	{
		CItem*				keyItem		= ITEMMGR->GetItem( message.mUsedKeyItem.dwDBIdx );
		const ITEM_INFO*	keyItemInfo	= ITEMMGR->GetItemInfo( message.mUsedKeyItem.wIconIdx );

		if( ! keyItem ||
			! keyItemInfo )
		{
			return;
		}

		// 080916 LUJ, �ߺ��ǰ� ������ 2�� �̻� ���� ���, ������ �����Ѵ�
		if( keyItemInfo->Stack &&
			keyItem->GetDurability() > 1 )
		{
			ITEMBASE itemBase = keyItem->GetItemBaseInfo();
			--( itemBase.Durability );

			keyItem->SetItemBaseInfo( itemBase );
			ITEMMGR->RefreshItem( keyItem );
		}
		else
		{
			CItem* deletedItem = 0;
			ITEMMGR->DeleteItem( keyItem->GetPosition(), &deletedItem );
		}
	}

	// 080916 LUJ, ����� �ռ�â�� ǥ������
	{
		// 080916 LUJ, �ʱ�ȭ
		{
			Clear( *mResultIconGridDialog );
			Clear( *mSourceIconDialog );

			mOptionListDialog->RemoveAll();
			mSubmitButton->SetActive( FALSE );

			mPageStatic->SetStaticText( "" );
			mPageNextButton->SetActive( FALSE );
			mPagePreviousButton->SetActive( FALSE );
		}

		CItem* resultItem = ( CItem* )mSourceIconDialog->GetIconForIdx( resultPosition );

		if( !	resultItem	||
				resultItem->GetType() != WT_ITEM )
		{
			return;
		}

		// 090122 LUJ, Ÿ���� ����Ǿ� ���� ���ڷ� ���
		UpdateItem( *resultItem, message.mResultItem );
	}

	SetDisable( FALSE );
	// 090122 LUJ, ó�� �Ŀ��� ��ᰡ �����Ƿ� �ռ� ��ư�� ǥ������ �ʵ��� ����
	mSubmitButton->SetActive( FALSE );

	// 080916 LUJ, Ű �������� ���� ��� â�� ����. ����: disable���¿����� SetActive()�� �۵����� �ʴ´�
	{
		CItem* item = ITEMMGR->GetItem( mKeyIconBase.dwDBIdx );

		if( ! item )
		{
			SetActive( FALSE );
		}
	}
}

// 080916 LUJ, �ռ� ���� �޽��� ó��
void CComposeDialog::Proceed( const MSG_ITEM_COMPOSE_NACK& message )
{
	switch( message.mType )
	{
	case MSG_ITEM_COMPOSE_NACK::TypeInvalidUsedItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by invalid used item" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeNotMatchedScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by not matched script" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeNotExistedScript:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by not existed script" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeNotExistedOption:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by not existed option" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeWrongUsedItemSize:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by wrong item size" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeInvalidResult:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by invalid result" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeEmptyResult:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by empty result" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeInvalidKeyItem:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by invalid key item" );
			break;
		}
	case MSG_ITEM_COMPOSE_NACK::TypeFailedUpdateMemory:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by memory update failure on server" );
			break;
		}
	default:
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, "composition is failed by unknown reason" );
			break;
		}
	}

	SetDisable( FALSE );
}

// 080916 LUJ, �־��� ITEMBASE ������ �������� �����Ѵ�
void CComposeDialog::UpdateItem( CItem& item, const ITEMBASE& itemBase ) const
{
	cImage image;
	item.SetBasicImage( ITEMMGR->GetIconImage( itemBase.wIconIdx, &image ) );	
	item.Init(
		LONG( item.GetAbsX() ),
		LONG( item.GetAbsY() ),
		DEFAULT_ICONSIZE,
		DEFAULT_ICONSIZE,
		ITEMMGR->GetIconImage( itemBase.wIconIdx, &image ) );
	item.SetItemBaseInfo( itemBase );

	ITEMMGR->AddToolTip( &item	);
}

// 080916 LUJ, ����� ������ ������ �����Ѵ�
void CComposeDialog::UpdateResult( const ComposeScript& script, int pageIndex )
{
	Clear( *mResultIconGridDialog );

	WORD iconIndex = 0;

	// 080916 LUJ, �Լ� ������ ��� ����� mResultMap��, ǥ�õǴ� ����� resultSet�� �ִ�.
	//				������ �ٸ��� ������ �������� �ִٴ� ��
	const DWORD isHiddenResult = ( script.mResultMap.size() != script.mResultSet.size() );

	// 080916 LUJ, ù��° �������̰�, ������ �������� ���� ��� ����ǥ�� ǥ���ؾ��Ѵ�.	
	if( !	pageIndex &&
			isHiddenResult )
	{
		CItem* item = GetResultItem( iconIndex++ );

		if( ! item )
		{
			return;
		}

		ITEMBASE itemBase;
		ZeroMemory( &itemBase, sizeof( itemBase ) );
		UpdateItem( *item, itemBase );		
		
		cImage backgroundImage;
		SCRIPTMGR->GetImage( 0, &backgroundImage, PFT_HARDPATH );
		item->SetToolTip(
			"",
			RGBA_MAKE( 255, 255, 255, 255 ),
			&backgroundImage,
			TTCLR_ITEM_CANEQUIP );
		item->AddToolTipLine( CHATMGR->GetChatMsg( 1684 ) );
	}

	// 080916 LUJ, �ݺ��ڰ� �̵��� ��ġ�� ���Ѵ�
	DWORD position = mResultIconGridDialog->GetCellNum() * pageIndex;
	
	if( script.mResultSet.size() <= position )
	{
		return;
	}
	else if( 0 < pageIndex && isHiddenResult )
	{
		// 100517 ONS ������ �������� �������, 
		// �ι�° ���������� ���� �������� -1�ؾ� ���������� �������� ��µȴ�. ( pos : 30 -> 29 )
		position -= 1; 
	}

	ComposeScript::ResultSet::const_iterator it = script.mResultSet.begin();
	std::advance( it, position );

	// 080916 LUJ, ��� �������� �����Ѵ�
	for(	;
			it != script.mResultSet.end();
			++it )
	{
		CItem* item = GetResultItem( iconIndex );

		if( ! item )
		{
			continue;
		}

		ITEMBASE itemBase;
		ZeroMemory( &itemBase, sizeof( itemBase ) );
		itemBase.wIconIdx = *it;
		UpdateItem( *item, itemBase );

		if( mResultIconGridDialog->GetCellNum() <= ++iconIndex )
		{
			break;
		}
	}

	// 080916 LUJ, ������ ��ȣ ǥ��
	{
		char text[ MAX_PATH ] = { 0 };
		sprintf( text, "%d", pageIndex + 1 );
		mPageStatic->SetStaticText( text );
	}	

	// 080916 LUJ, ������ �̵� ��ư ǥ��
	{
		const DWORD resultSize	= script.mResultSet.size() + ( isHiddenResult ? 1 : 0 );
		const int pageSize	= ( resultSize / mResultIconGridDialog->GetCellNum() ) + ( 0 < ( resultSize % mResultIconGridDialog->GetCellNum() ) ? 1 : 0 );

		// 080916 LUJ, �� ������ ���ϸ� ������ ��ư�� ǥ���� �ʿ䰡 ����
		if( pageSize == 1 )
		{
			mPageNextButton->SetActive( FALSE );
			mPagePreviousButton->SetActive( FALSE );
			return;
		}
		else if( pageSize == pageIndex + 1 )
		{
			mPagePreviousButton->SetActive( TRUE );
			mPageNextButton->SetActive( FALSE );
		}
		else if( ! pageIndex )
		{
			mPagePreviousButton->SetActive( FALSE );
			mPageNextButton->SetActive( TRUE );
		}
		else
		{
			mPageNextButton->SetActive( TRUE );
			mPagePreviousButton->SetActive( TRUE );
		}
	}
}

void CComposeDialog::Send()
{
	NETWORK->Send( &mComposeMessage, mComposeMessage.GetSize() );
	// 080916 LUJ, ���� �Ŀ��� �޸𸮰� ������
	ZeroMemory( &mComposeMessage, sizeof( mComposeMessage ) );

	SetDisable( TRUE );
}