#include "StdAfx.h"
#include "ProgressDialog.h"
#include "cStatic.h"
#include "ObjectGuagen.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "GameResourceManager.h"
#include "cWindowManager.h"
#include "ObjectManager.h"
#include "ComposeDialog.h"
#include "ReinforceDlg.h"
#include "EnchantDialog.h"
#include "MixDialog.h"
#include "InventoryExDialog.h"
#include "ApplyOptionDialog.h"
#include "PetResurrectionDialog.h"
#include "CookDlg.h"
#include "ItemManager.h"
#include "MoveManager.h"
#include "./SiegeWarfareMgr.h"
#include "./Interface/cTextArea.h"

const DWORD unableColor = RGB( 190, 190, 190 );
const DWORD enableColor = RGB( 255, 255, 255 );

CProgressDialog::CProgressDialog() :
mAction(Action()),
mTextImage(0),
mTextArea(0),
mCancelBtn(0),
m_bHideDlg(FALSE)
{
	m_bAlwaysTop = TRUE;
}

CProgressDialog::~CProgressDialog()
{}


void CProgressDialog::Linking()
{
	mTextImage = GetWindowForID(
		PROGRESS_TEXT_IMAGE);
   	mTextArea  = (cTextArea*)GetWindowForID(
		PROGRESS_TEXT_AREA);
   	mTextArea->SetReadOnly(
		TRUE);

	mCancelBtn = GetWindowForID(
		PROGRESS_TEXT_CANCELBTN);
	mText = (cStatic*)GetWindowForID(
		PROGRESS_DIALOG_TEXT);
	mGauge = (CObjectGuagen*)GetWindowForID(
		PROGRESS_DIALOG_GAUGE);
}


void CProgressDialog::SetActive( BOOL isActive )
{
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
		ASSERT( dialog );

		dialog->SetDisable( isActive );
	}
	
	if( ! isActive )
	{
		Restore();
	}

	cDialog::SetActive( isActive );
}

void CProgressDialog::Wait(DWORD second, LPCTSTR text, cbFUNC functionPointer)
{
	mAction = Action();
	mAction.mBeginTick = GetTickCount();
	mAction.mEndTick = GetTickCount() + second * 1000;
	mAction.mType = eActionNone;
	mAction.mIsWaiting = TRUE;
	mAction.mFunctionPointer = functionPointer;

	mCancelBtn->SetDisable(
		FALSE);
	mText->SetStaticText(
		text);
	mGauge->SetValue(
		0,
		0);
	mGauge->SetValue(
		1,
		second * 1000);

	MSG_DWORD message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_ITEM;
	message.Protocol = MP_ITEM_WORKING_START;
	message.dwObjectID = gHeroID;
	message.dwData = gHeroID;
	NETWORK->Send(
		&message,
		sizeof(message));
}

void CProgressDialog::Wait( eAction action )
{
	mAction = Action();
	m_bHideDlg = FALSE;
	m_dwExtra = 0;

	if( mAction.mIsWaiting )
	{
		CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );

		return;
	}

	// 080827 LYW --- ProgressDialog : ���ͽõ� ������� üũ�Ѵ�.
	{
		mAction.mBeginTick	= GetTickCount();

		if( action == eActionUseWaterSeed )
		{
			mAction.mEndTick	= mAction.mBeginTick + SIEGEWARFAREMGR->Get_WaterSeedUsingSecond();
		}
		else if( action == eActionExCooking )
		{
			mAction.mEndTick	= mAction.mBeginTick + WAITMILISECOND_FOR_COOK;
		}
		else
		{
			mAction.mEndTick	= mAction.mBeginTick + WAITMILISECOND;
		}

		mAction.mType		= action;
		mAction.mIsWaiting	= TRUE;

		mGauge->SetValue( 0, 0 );

		if( action == eActionUseWaterSeed )
		{
			mGauge->SetValue( GUAGEVAL( 1.0f ), SIEGEWARFAREMGR->Get_WaterSeedUsingSecond() );
		}
		else if( action == eActionExCooking)
		{
			mGauge->SetValue( GUAGEVAL( 1.0f ), WAITMILISECOND_FOR_COOK );
		}
		else
		{
			mGauge->SetValue( GUAGEVAL( 1.0f ), WAITMILISECOND );
		}
	}

	switch( action )
	{
	case eActionMix:
		{
			mText->SetStaticText( CHATMGR->GetChatMsg( 1073 ) );
			break;
		}
	case eActionReinforce:
		{
			mText->SetStaticText( CHATMGR->GetChatMsg( 1074 ) );
			break;
		}
	case eActionEnchant:
		{
			mText->SetStaticText( CHATMGR->GetChatMsg( 1076 ) );
			break;
		}
	case eActionApplyOption:
	case eActionCompose:
		{
			mText->SetStaticText( CHATMGR->GetChatMsg( 1424 ) );
			break;
		}
	case eActionUseWaterSeed :
		{
			mTextArea->SetScriptText(  CHATMGR->GetChatMsg( 1694 ) ) ;
			//mText->SetStaticText( "���ͽõ带 �칰�� �ֱ� ���� �غ����Դϴ�." ) ;
			break ;
		}
	case eActionChangeSize:
		{
			mText->SetStaticText(  CHATMGR->GetChatMsg( 1729 ) ) ;
			break;
		}
	case eActionVehicleSummon:
	case eActionSummonMonster:
		{
			mText->SetStaticText(  CHATMGR->GetChatMsg( 1738 ) ) ;
			break;
		}
	case eActionExCooking:
		{
			mText->SetStaticText(  CHATMGR->GetChatMsg( 1810 ) ) ;
			break;
		}
	case eActionVehicleSeal:
		{
			mText->SetStaticText( CHATMGR->GetChatMsg( 1833 ) ) ;
			break;
		}
	}

	mTextArea->SetActive(FALSE) ;
	mCancelBtn->SetActive(FALSE) ;

	mTextImage->SetActive(TRUE) ;
	mText->SetActive(TRUE) ;

	SetActive( TRUE );
	SetDisable( TRUE );

	// 091106 ShinJS --- Action�� ���� Component, Message ó��
	switch( action )
	{
	case eActionUseWaterSeed:
		{
			mTextArea->SetActive(TRUE) ;
			mCancelBtn->SetActive(TRUE) ;

			mTextImage->SetActive(FALSE) ;
			mText->SetActive(FALSE) ;

			SetDisable( FALSE );
		}
		break;
	case eActionExSummonEffect:
		{
			SetDisable( FALSE );

			m_bHideDlg = TRUE;

			MSG_DWORD2 message;
			message.Category	= MP_ITEM;
			message.Protocol	= MP_ITEM_WORKING_STARTEX;
			message.dwObjectID	= gHeroID;
			message.dwData1		= gHeroID;
			message.dwData2		= (DWORD) action;

			NETWORK->Send( &message, sizeof( message ) );

		}
		break;
	case eActionExCooking:
	case eActionVehicleSummon:
	case eActionVehicleSeal:
		{
			MSG_DWORD2 message;
			message.Category	= MP_ITEM;
			message.Protocol	= MP_ITEM_WORKING_STARTEX;
			message.dwObjectID	= gHeroID;
			message.dwData1		= gHeroID;
			message.dwData2		= (DWORD)action;

			NETWORK->Send( &message, sizeof( message ) );
		}
		break;
	default:
		// ������ �����Ͽ� �ش� �÷��̾ ������ �۾� ������ �ϰ� ������ �˸���.
		// ����: �������� ���¸� �����ؾ��� �� �ִ�. �׷��� ������ ���� �׸���� ���� �÷��̾�Դ� ���������� ������ ���� ���̴�.
		//		 ĳ���� �ɱ� ����(rest)�� ���������� ���̵��� ������ߴ� ���� ����ϵ���
		{
			MSG_DWORD message;
			message.Category	= MP_ITEM;
			message.Protocol	= MP_ITEM_WORKING_START;
			message.dwObjectID	= gHeroID;
			message.dwData		= gHeroID;

			NETWORK->Send( &message, sizeof( message ) );
		}
		break;
	}
}


void CProgressDialog::Render()
{
	if( mAction.mIsWaiting )
	{
		const DWORD currentTick = GetTickCount();

		// ƽ ���� �����÷ε� ���¿� �ƴ� ���� ��� üũ�ؾ� �Ѵ�.
		if( ( mAction.mBeginTick < mAction.mEndTick && mAction.mEndTick < currentTick ) ||
			( mAction.mBeginTick > mAction.mEndTick && mAction.mEndTick < currentTick && mAction.mBeginTick > currentTick ) )
		{
			SetActive( FALSE );

			switch( mAction.mType )
			{
			case eActionNone:
				{
					if(0 == mAction.mFunctionPointer)
					{
						break;
					}

					(*mAction.mFunctionPointer)(
						GetID(),
						0,
						0);
					break;
				}
			case eActionReinforce:
				{
					CReinforceDlg* dialog = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
					ASSERT( dialog );

					dialog->Send();
					break;
				}
			case eActionMix:
				{
					CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
					ASSERT( dialog );

					dialog->Send();
					break;
				}
			case eActionEnchant:
				{
					CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
					ASSERT( dialog );

					dialog->Send();
					break;
				}
			case eActionPetSummon:
			case eActionPetGradeUp:
			case eActionChangeSize:
			case eActionSummonMonster:
			case eActionExSummonEffect:
				{
					CInventoryExDialog* dialog = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
					ASSERT( dialog );

					dialog->Send();
					break;
				}
			case eActionApplyOption:
				{
					CApplyOptionDialog* dialog = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );
					ASSERT( dialog );

					dialog->Send();
					break;
				}
				// 080916 LUJ, ������ �ռ� �߰�
			case eActionCompose:
				{
					CComposeDialog* dialog = ( CComposeDialog* )WINDOWMGR->GetWindowForID( COMPOSE_DIALOG );
					ASSERT( dialog );

					dialog->Send();
					break;
				}
			case eActionPetResurrection:
				{
					CPetResurrectionDialog* dialog = ( CPetResurrectionDialog* )WINDOWMGR->GetWindowForID( PET_RES_DLG );
					ASSERT( dialog );

					dialog->Send();
				}
				break;

				// 080827 LYW --- ProgressDialog : ���� �õ� ����� �߰��Ѵ�.
   			case eActionUseWaterSeed :
   				{
   					SIEGEWARFAREMGR->IsEndUsing_WaterSeed() ;
   				}
   				break ;

			case eActionExCooking:
				{
					CCookDlg* dialog = (CCookDlg*)WINDOWMGR->GetWindowForID( COOKDLG );
					ASSERT( dialog );

					dialog->Send();
				}
				break;

			// 090422 ShinJS --- Ż�� ��ȯ
			case eActionVehicleSummon:
				{
					CInventoryExDialog* pDlg = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
					CItem* pItem = pDlg->Get_QuickSelectedItem();

					if( pItem )
					{
						// ��ȯ ��Ŷ ����
						MSG_VEHICLE_SUMMON msg;
						msg.Category = MP_VEHICLE;
						msg.Protocol = MP_VEHICLE_SUMMON_SYN;
						msg.dwObjectID = gHeroID;
						msg.SummonPos.Compress( &m_vecVehicleSummonPos );
						msg.IconBase.wIconIdx = pItem->GetItemIdx();
						msg.IconBase.Position = pItem->GetPosition();
                        msg.IconBase.dwDBIdx = pItem->GetDBIdx();
						NETWORK->Send( &msg, sizeof( msg ) );
					}
				}
				break;

			// 090422 ShinJS --- Ż�� ����
			case eActionVehicleSeal:
				{
					MSGBASE msg;
					ZeroMemory( &msg, sizeof( msg ) );
					msg.Category = MP_VEHICLE;
					msg.Protocol = MP_VEHICLE_UNSUMMON_SYN;
					msg.dwObjectID = gHeroID;
					NETWORK->Send( &msg, sizeof( msg ) );
				}
				break;

			default:
				{
					ASSERT( 0 );
					break;
				}
			}

			mAction.mIsWaiting	= FALSE;
		}
	}

	if(!m_bHideDlg)
		cDialog::Render();
}


void CProgressDialog::SetText( const char* text )
{
	mText->SetStaticText( text );
}


void CProgressDialog::Cancel()
{
	Restore();
	SetActive( FALSE );

	switch( mAction.mType )
	{
	case eActionReinforce:
		{
			CReinforceDlg* dialog = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			ASSERT( dialog );

			dialog->Restore();
			dialog->SetActive( FALSE );

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1084 ) );
			break;
		}
	case eActionMix:
		{
			CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			ASSERT( dialog );

			dialog->Restore();
			dialog->SetActive( FALSE );

			CHATMGR->AddMsg( CTC_SYSMSG,CHATMGR->GetChatMsg( 785 ) );
			break;
		}
	case eActionEnchant:
		{
			CEnchantDialog* dialog = ( CEnchantDialog* )WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			ASSERT( dialog );

			dialog->Restore();
			dialog->SetActive( FALSE );

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1084 ) );
			break;
		}
	case eActionChangeSize:
	case eActionSummonMonster:
	case eActionExSummonEffect:
		{
			CInventoryExDialog* dialog = ( CInventoryExDialog* )WINDOWMGR->GetWindowForID( IN_INVENTORYDLG );
			ASSERT( dialog );

			dialog->Restore();

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1084 ) );
			break;
		}
	case eActionApplyOption:
		{
			CApplyOptionDialog* dialog = ( CApplyOptionDialog* )WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );
			ASSERT( dialog );

			dialog->Restore();

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1084 ) );
			break;
		}

		// 080827 LYW --- ProgressDialog : ���ͽõ� ����� �߰��Ѵ�.
   	case eActionUseWaterSeed :
   		{
   			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1084 ) ) ;
   			break ;
   		}

	default:
		{
			ASSERT( 0 );
			break;
		}
	}	
}


void CProgressDialog::Restore()
{
	SetDisable( FALSE );

	mAction.mIsWaiting	= FALSE;

	// 080901 LUJ, ��� â���� Ȱ��ȭ��Ų��
	{
		class
		{
		public:
			void operator()( cDialog* dialog )
			{
				if( dialog )
				{
					dialog->SetDisable( FALSE );
				}
			}
		}
		SetEnable;

		SetEnable( WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG ) );
		SetEnable( WINDOWMGR->GetWindowForID( ITMD_MIXDLG ) );
		SetEnable( WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG ) );
		SetEnable( WINDOWMGR->GetWindowForID( IN_INVENTORYDLG ) );
		SetEnable( WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG ) );
	}
}


void CProgressDialog::OnActionEvent( LONG id, void* p, DWORD event )
{
	switch(id)
	{
	case PROGRESS_TEXT_CANCELBTN:
		{
			MSGBASE message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_SIEGEWARFARE ;
			message.Protocol = MP_SIEGEWARFARE_USE_WATERSEED_CANCEL ;
			message.dwObjectID = gHeroID;
			NETWORK->Send(
				&message,
				sizeof(message));

			if(mAction.mFunctionPointer)
			{
				(*mAction.mFunctionPointer)(
					PROGRESS_TEXT_CANCELBTN,
					0,
					0);
			}

			break;
		}
	}
}
