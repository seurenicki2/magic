// HeroMove.cpp: implementation of the CHeroMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _CHEATENABLE_
#include "CheatMsgParser.h"
#endif	//_CHEATENABLE_
#include "HeroMove.h"
#include "MoveManager.h"
#include "ObjectManager.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "MousePointer.h"
#include "Interface/cEditBox.h"
#include "GameResourceManager.h"
#include "ExchangeManager.h"
#include "chatmanager.h"
#include "NpcScriptManager.h"
#include "BattleSystem_Client.h"
#include "Input/UserInput.h"
#include "MouseCursor.h"
#include "Interface/cWindowManager.h"
#include "PartyManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/object/skillobject.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "NpcScriptDialog.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "InventoryExDialog.h"
#include "VehicleManager.h"
#include "WindowIdEnum.h"
#include "cMsgBox.h"
#include "cHousingMgr.h"
#include "AntiBotManager.h"

#ifdef _TESTCLIENT_
#include "MapChange.h"
#include "MainGame.h"
#endif

CHeroMove::CHeroMove()
{

}

CHeroMove::~CHeroMove()
{

}

void CHeroMove::OnMouseOver( CObject* pObject )
{
	if( !pObject )	return;

	OBJECTMGR->OnMouseOver( pObject );
	
	if( HERO )
	{
		// 091028 ShinJS --- Ż�� ���콺Ŀ�� �߰�
		eCURSOR cursor = eCURSOR_DEFAULT;

		switch( pObject->GetObjectKind() )
		{
		case eObjectKind_Npc:
			{
				if( ((CNpc*)pObject)->GetNpcKind() != 30 && 
					((CNpc*)pObject)->GetNpcKind() != 31 )
					cursor = eCURSOR_DEALER;
			}
			break;
		case eObjectKind_Vehicle:
			{
				cursor = eCURSOR_VEHICLE;
			}
			break;
		case eObjectKind_Furniture:
			{
				CFurniture* pFurniture = (CFurniture*)pObject;
				stFurniture* pstFurniture = pFurniture->GetFurnitureInfo();
				stFunitureInfo* pstFurnitureInfo = GAMERESRCMNGR->GetFunitureInfo( pstFurniture->dwFurnitureIndex );

				if( pstFurnitureInfo && pstFurnitureInfo->dwActionIndex )
				{
					cursor = eCURSOR_HOUSING_ACTION_FURNITURE;
				}
			}
			break;
		default:
			{
				CBattle* pBattle = BATTLESYSTEM->GetBattle();
				if( pBattle->IsEnemy( HERO, pObject ) )
					cursor = eCURSOR_ATTACK;
			}
			break;
		}

		CURSOR->SetCursor( cursor );
	}
}

void CHeroMove::OnMouseLeave( CObject* pObject )
{
	if( !pObject )	return;
	OBJECTMGR->OnMouseLeave( pObject );
}


void CHeroMove::OnClickPlayer(DWORD MouseEvent, CPlayer* pPlayer)
{
	// 090613 ONS ĳ���� ��Ŭ�� �˾��޴� ���� ó�� ���� 
	// ĳ���Ͱ� ���ִ� �������� ������ ���콺��ư �巡�׽� ������ �ʴ� �˾��޴� ���� ����.
	// ���ư Down���� Up���� ĳ����ID�� �������� �˾��޴� ����.
	if( pPlayer == NULL ) return;

	DWORD dwPlayerID = pPlayer->GetID();
	if(HEROID == dwPlayerID) return;
	
	static DWORD dwDownPlayerID = 0;

	if( MouseEvent == MOUSEEVENT_RCLICK )
	{
		dwDownPlayerID = dwPlayerID;
	}
	else if( MouseEvent == MOUSEEVENT_RBUTTONUP)
	{
		if(dwDownPlayerID == dwPlayerID)
		{
			OBJECTMGR->SetSelectedObject(
				pPlayer);

			if(cDialog* const dialog = WINDOWMGR->GetWindowForID(ID_POPUPMENU_DIALOG))
			{
				SHORT_RECT screenRect = {0};
				WORD value = 0;

				g_pExecutive->GetRenderer()->GetClientRect(
					&screenRect,
					&value,
					&value);

				dialog->SetAbsXY(
					screenRect.right < MOUSE->GetMouseX() + dialog->GetWidth() ? screenRect.right - dialog->GetWidth() : MOUSE->GetMouseX(),
					screenRect.bottom < MOUSE->GetMouseY() + dialog->GetHeight() ? screenRect.bottom - dialog->GetHeight() : MOUSE->GetMouseY());
				dialog->SetActive(
					TRUE);
			}
		}
	}
	else
	{
		OnAttack( pPlayer, MouseEvent );
	}
}

void CHeroMove::OnClickMonster(DWORD MouseEvent,CMonster* pMonster)
{
#ifdef _TESTCLIENT_
	if(KEYBOARD->GetKeyPressed(KEY_CONTROL))
	{
		if(MouseEvent & MOUSEEVENT_LCLICK)
		{
			TARGETSET Target;
			Target.pTarget = pMonster;
			memset(&Target.ResultInfo,0,sizeof(RESULTINFO));
			Target.ResultInfo.bCritical = rand()%2 ? TRUE : FALSE;
			Target.ResultInfo.RealDamage = rand() % 50;
			Target.ResultInfo.CounterDamage = 0;
			//Target.TargetData = rand() % 100;
			EFFECTMGR->StartEffectProcess(GAMERESRCMNGR->m_TestClientInfo.Effect,HERO,&Target,1,pMonster->GetID(),EFFECT_FLAG_HEROATTACK);
		}
		else if(MouseEvent == MOUSEEVENT_RCLICK)
		{
			TARGETSET Target;
			Target.pTarget = HERO;
			memset(&Target.ResultInfo,0,sizeof(RESULTINFO));
			Target.ResultInfo.bCritical = rand()%2 ? TRUE : FALSE;
			Target.ResultInfo.RealDamage = rand() % 50;
			Target.ResultInfo.CounterDamage = 0;
			EFFECTMGR->StartEffectProcess(GAMERESRCMNGR->m_TestClientInfo.MonsterEffect,pMonster,&Target,1,gHeroID,EFFECT_FLAG_HEROATTACK);
		}
		return;
	}
	else
	{
		CActionTarget Target;
		Target.InitActionTarget(pMonster,NULL);
		BOOL bSkill = FALSE;
		if(MouseEvent & MOUSEEVENT_RCLICK)
		{
			bSkill = TRUE;
			SKILLMGR->OnSkillCommand(HERO, &Target, bSkill);
		}
	}
#endif

	OnAttack( pMonster, MouseEvent );
}

void CHeroMove::OnClickNpc(DWORD MouseEvent, CNpc* pNpc)
{
	if( HERO->GetState() != eObjectState_None &&
		HERO->GetState() != eObjectState_Move &&
		HERO->GetState() != eObjectState_Immortal )
		return;
	
	if( MouseEvent & MOUSEEVENT_DBLCLICK )
	{
		CActionTarget Target;
		Target.InitActionTarget( pNpc, NULL );

		CAction MoveAction;
		float fDistance = NPC_TALKING_DISTANCE;

		if(	pNpc->GetNpcJob() == HOUSING_RANK1_ROLE ||			//090604 pdy �Ͽ�¡ ���� 
			pNpc->GetNpcJob() == HOUSING_RANK2_ROLE || 
			pNpc->GetNpcJob() == HOUSING_RANK3_ROLE	)
		{
			stDynamicHouseNpcInfo* pInfo = 
				HOUSINGMGR->FindDynimiHouseNpcInfoByNpcKind( pNpc->GetNpcTotalInfo()->MapNum , pNpc->GetNpcKind() );	
			if( ! pInfo )
				return;

			fDistance = pInfo->fDistance;	//RANK NPC�� �ܰ��� ���� ũ�Ⱑ �ٸ����� ��ũ��Ʈ�� ����� �Ÿ��� �����´� 
		}

		MoveAction.InitAction( eActionKind_TalkToNpc, NULL, fDistance, &Target );
		MOVEMGR->SetHeroActionMove( &Target, &MoveAction );		
	}	
	else if( MouseEvent & MOUSEEVENT_LCLICK )
	{
		if( OBJECTMGR->GetSelectedObjectID() == pNpc->GetID() )
		{
				CActionTarget Target;
 				Target.InitActionTarget( pNpc, NULL );

				CAction MoveAction;
				float fDistance = NPC_TALKING_DISTANCE;
				
				if( pNpc->GetNpcJob() == HOUSING_RANK1_ROLE ||		//090604 pdy �Ͽ�¡ ���� 
					pNpc->GetNpcJob() == HOUSING_RANK2_ROLE ||
					pNpc->GetNpcJob() == HOUSING_RANK3_ROLE	)
				{
					stDynamicHouseNpcInfo* pInfo = 
						HOUSINGMGR->FindDynimiHouseNpcInfoByNpcKind( pNpc->GetNpcTotalInfo()->MapNum , pNpc->GetNpcKind() );
					if( ! pInfo )
						return;

					fDistance = pInfo->fDistance;	//RANK NPC�� �ܰ��� ���� ũ�Ⱑ �ٸ����� ��ũ��Ʈ�� ����� �Ÿ��� �����´� 
				}

				MoveAction.InitAction( eActionKind_TalkToNpc, NULL, fDistance, &Target );
				MOVEMGR->SetHeroActionMove( &Target, &MoveAction );
		}
	}
}

void CHeroMove::OnClickSkillObject(DWORD MouseEvent,cSkillObject* pObj)
{
	OnAttack( pObj, MouseEvent );
}

void CHeroMove::OnClickGround(DWORD MouseEvent,VECTOR3* pPos)
{
	// 091022 ShinJS --- Ż�� ��ġ���� ���
	CObject* pObject = OBJECTMGR->GetObject( VEHICLE_INSTALL_ID );
	if( pObject != NULL )
	{
		VEHICLEMGR->InstallVehicle( pObject );
		return;
	}


#ifdef _TESTCLIENT_

	if(KEYBOARD->GetKeyPressed(KEY_MENU) && KEYBOARD->GetKeyPressed(KEY_SPACE))
	{
		VECTOR3 pos;
		pos.x = pPos->x;
		pos.y = 0;
		pos.z = pPos->z;
		MOVEMGR->SetPosition(HERO,&pos);
	}
	else
	{
		if( MouseEvent & MOUSEEVENT_LCLICK )
			MOVEMGR->SetHeroTarget(pPos);
		else
			MOVEMGR->SetHeroTarget(pPos,FALSE);

		HERO->DisableAutoAttack();
	}
#else

#ifdef _CHEATENABLE_
	if(KEYBOARD->GetKeyPressed(KEY_MENU) && KEYBOARD->GetKeyPressed(KEY_SPACE))
	{
#ifndef _GMTOOL_
		if( CHEATMGR->IsCheatEnable() )	
#endif
		{
			MOVE_POS msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_MOVE_SYN;
			msg.dwObjectID = HEROID;
			msg.dwMoverID = HEROID;
			msg.cpos.Compress(pPos);
			NETWORK->Send(&msg,sizeof(msg));
		}
	}
	else
#endif	//_CHEATENABLE_
	{
		if(HERO)
		{
			if( MouseEvent & MOUSEEVENT_LCLICK )
				MOVEMGR->SetHeroTarget(pPos);
			else
				MOVEMGR->SetHeroTarget(pPos,FALSE);

			HERO->DisableAutoAttack();
			HERO->SetCurComboNum(0);
			HERO->SetMovingAction(NULL);
		}
	}

#endif	//_TESTCLIENT_
}

void CHeroMove::OnClickMapObject(DWORD MouseEvent, CObject* pObj)
{

}

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.05.07
void CHeroMove::OnClickFarmRenderObj(DWORD MouseEvent, CSHFarmRenderObj* pObj)
{
	if( HERO->GetState() != eObjectState_None &&
		HERO->GetState() != eObjectState_Move &&
		HERO->GetState() != eObjectState_Immortal )
		return;

	if( BATTLESYSTEM->GetBattle()->GetBattleKind() != eBATTLE_KIND_NONE )
		return;

	// ���� ��ȭâ�� �� ������ ����~
   	if (g_csFarmManager.CLI_GetTargetObj() && 
 		GAMEIN->GetNpcScriptDialog()->IsActive())
	{
		return;
	}

	// 080515 KTH -- ���� â�� ���� ������ ����.
	if ( GAMEIN->GetAnimalDialog()->IsActive() )
		return;

	if( MouseEvent & MOUSEEVENT_DBLCLICK )
	{
  		CSHFarmRenderObj* const renderObject = g_csFarmManager.CLI_GetTargetObj();

		if(NULL == renderObject)
		{
			return;
		}
		else if(CSHFarmManager::RENDER_ANIMAL_KIND > renderObject->GetInfo()->nKind)
		{
			return;
		}
		else if(CSHFarmManager::RENDER_ANIMAL_KIND + CSHFarmManager::RENDER_ANIMAL_KIND_NUM <= renderObject->GetInfo()->nKind)
		{
			return;
		}

		VECTOR3 stHeroPos;
		HERO->GetPosition(&stHeroPos);
		VECTOR3 stTargetPos;
		pObj->GetPosition(&stTargetPos);

		CAnimal* const pAnimal = g_csFarmManager.CLI_GetAnimalInfo(
			CSHFarmZone::FARM_ZONE(g_csFarmManager.CLI_GetTargetObjID()->nFarmZone),
			WORD(g_csFarmManager.CLI_GetTargetObjID()->nFarmID),
			WORD(g_csFarmManager.CLI_GetTargetObjID()->nAnimalCageID),
			WORD(g_csFarmManager.CLI_GetTargetObjID()->nAnimalID));

		if(pAnimal->GetOwner() != gHeroID)
		{
			return;
		}
		else if (CalcDistanceXZ(&stTargetPos, &stHeroPos) <= CSHFarmManager::PEN_OBJECT_CONTROL_DISTANCE)
		{
			MOVEMGR->SetLookatPos(g_csFarmManager.CLI_GetTargetObj(), &stHeroPos, 0, gCurTime);
			MOVEMGR->SetLookatPos(HERO, &stTargetPos, 0, gCurTime);

			GAMEIN->GetAnimalDialog()->SetActive(TRUE);
			GAMEIN->GetAnimalDialog()->SetAnimal(pAnimal);
		}
		else
		{
			CActionTarget Target;
			Target.InitActionTarget(
				pObj,
				NULL);

			CAction MoveAction;
			MoveAction.InitAction( eActionKind_Farm, NULL, (float)CSHFarmManager::PEN_OBJECT_CONTROL_DISTANCE, &Target );
			MOVEMGR->SetHeroActionMove( &Target, &MoveAction );
		}
	}	
	else if( MouseEvent & MOUSEEVENT_LCLICK )
	{
		if( OBJECTMGR->GetSelectedObjectID() == pObj->GetID() )
		{
			CActionTarget Target;
			Target.InitActionTarget( pObj, NULL );

			CAction MoveAction;
			MoveAction.InitAction( eActionKind_Farm, NULL, (float)CSHFarmManager::OBJECT_CONTROL_DISTANCE, &Target );
			MOVEMGR->SetHeroActionMove( &Target, &MoveAction );		
		}
	}
}

void CHeroMove::OnClickVehicle(DWORD MouseEvent,CVehicle* pVehicle)
{
	ASSERT( pVehicle );
	if( !pVehicle )		return;

	if( MouseEvent == MOUSEEVENT_RCLICK )
	{
		// ��ġ�� �ȵ� ���
		if( !pVehicle->IsInstalled() )
			return;

		OBJECTMGR->SetSelectedObject( pVehicle );
		
		if(cDialog* const dialog = WINDOWMGR->GetWindowForID(ID_RIDEPOPUPMENU_DIALOG))
		{
			SHORT_RECT screenRect = {0};
			WORD value = 0;

			g_pExecutive->GetRenderer()->GetClientRect(
				&screenRect,
				&value,
				&value);

			dialog->SetAbsXY(
				screenRect.right < MOUSE->GetMouseX() + dialog->GetWidth() ? screenRect.right - dialog->GetWidth() : MOUSE->GetMouseX(),
				screenRect.bottom < MOUSE->GetMouseY() + dialog->GetHeight() ? screenRect.bottom - dialog->GetHeight() : MOUSE->GetMouseY());
			dialog->SetActive(
				TRUE);
		}
	}
	else if( MouseEvent & MOUSEEVENT_LCLICK )
	{
		// 091022 ShinJS --- Manager���� ��ġ�۾��ϵ��� ����
		VEHICLEMGR->InstallVehicle( pVehicle );
	}
}

//090508 pdy �Ͽ�¡ �߰� 
void CHeroMove::OnClickFurniture(DWORD MouseEvent, CFurniture* pFurniture)
{
	if( MouseEvent == MOUSEEVENT_RCLICK )
	{
		OBJECTMGR->SetSelectedObject( pFurniture );
		pFurniture->ShowObjectName( TRUE, NAMECOLOR_SELECTED );

		if(cDialog* const dialog = WINDOWMGR->GetWindowForID(HOUSING_ACTIONPOPUPMENU_DLG))
		{
			SHORT_RECT screenRect = {0};
			WORD value = 0;

			g_pExecutive->GetRenderer()->GetClientRect(
				&screenRect,
				&value,
				&value);

			dialog->SetAbsXY(
				screenRect.right < MOUSE->GetMouseX() + dialog->GetWidth() ? screenRect.right - dialog->GetWidth() : MOUSE->GetMouseX(),
				screenRect.bottom < MOUSE->GetMouseY() + dialog->GetHeight() ? screenRect.bottom - dialog->GetHeight() : MOUSE->GetMouseY());
			dialog->SetActive(
				TRUE);
		}
	}
}

#define ATTACK_TICK	400

void CHeroMove::OnAttack(CObject* pTargetObj,DWORD MouseEvent)
{	
	static CObject* pLastObj		= NULL;
	static DWORD	dwAttackTime	= 0;
	
	if( HERO && pTargetObj )
	{
		if( HERO->GetCharacterTotalInfo()->bVisible )	//�Ⱥ��϶��� ������ �ȵǵ��� ���� //KES
		{
			if( pLastObj != pTargetObj || gCurTime - dwAttackTime > ATTACK_TICK )
			{
				// RaMa - 2005.01.31   -> �����ִ� ������ �� ����.
				if( pTargetObj->GetState() == eObjectState_Die )
					return;

				if( MouseEvent & MOUSEEVENT_LCLICK )
				{
					if( OBJECTMGR->GetSelectedObjectID() == pTargetObj->GetID() )
					{
						if( HERO->IsPKMode() )
						if( PARTYMGR->IsPartyMember( pTargetObj->GetID() ) )						
						{ //������������ ��Ƽ���� �����Ҷ��� �޼��� ���� �ϱ� ����
							//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(630) );
							return;
						}
						
						CActionTarget Target;
						ANTIBOTMGR->IncreseAttackMonsterCount();
						Target.InitActionTarget(pTargetObj,NULL);
						
						HERO->EnableAutoAttack(&Target);		
						SKILLMGR->OnSkillCommand(HERO, &Target, FALSE);	
						pLastObj		= pTargetObj;
						dwAttackTime	= gCurTime;
					}
				}
				else if( MouseEvent & MOUSEEVENT_DBLCLICK )
				{
					if( HERO->IsPKMode() )
					if( PARTYMGR->IsPartyMember( pTargetObj->GetID() ) )						
					{ //������������ ��Ƽ���� �����Ҷ��� �޼��� ���� �ϱ� ����
						//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(630) ); //Alemuri Party War Deactivate this Message
						return;
					}
					CActionTarget Target;
					Target.InitActionTarget(pTargetObj,NULL);
					
					HERO->EnableAutoAttack(&Target);		
					SKILLMGR->OnSkillCommand(HERO, &Target, FALSE);	
					pLastObj		= pTargetObj;
					dwAttackTime	= gCurTime;
				}
			}
		}
	}
}

// LBS ������ ���� 03.09.30
void CHeroMove::OnDbClickPlayer(DWORD MouseEvent,CPlayer* pPlayer)
{
	if( HERO->GetState() != eObjectState_None &&
		HERO->GetState() != eObjectState_Move &&
		HERO->GetState() != eObjectState_Immortal )
		return;

	if( OBJECTMGR->GetSelectedObjectID() )
	{
		if( OBJECTMGR->GetSelectedObjectID() == pPlayer->GetID() )
		{
			//�������� ��쿡�� �޼����� ������.
			if( pPlayer->GetState() == eObjectState_StreetStall_Owner )
			{
				CActionTarget Target;
				Target.InitActionTarget( pPlayer, NULL );

				CAction MoveAction;
				MoveAction.InitStallAction( STREETSTALL_ENTER_DISTANCE, &Target );
				MOVEMGR->SetHeroActionMove( &Target, &MoveAction );
			}
			else
			{
				OnAttack( pPlayer, MOUSEEVENT_DBLCLICK );
			}
		}
	}
}

void CHeroMove::OnDbClickMonster(DWORD MouseEvent,CMonster* pMonster)
{
	OnAttack( pMonster, MouseEvent );
}

void CHeroMove::OnDbClickPet(DWORD MouseEvent,CPet* pPet)
{
	OnAttack( pPet, MouseEvent );
}

void CHeroMove::OnDbClickNpc(DWORD MouseEvent, CNpc* pNpc)
{
	if( HERO->GetState() != eObjectState_None &&
		HERO->GetState() != eObjectState_Move &&
		HERO->GetState() != eObjectState_Immortal )
		return;

	CActionTarget Target;
	Target.InitActionTarget( pNpc, NULL );
				
	CAction MoveAction;
	float fDistance = NPC_TALKING_DISTANCE;

	MoveAction.InitAction( eActionKind_TalkToNpc, NULL, fDistance, &Target );
	MOVEMGR->SetHeroActionMove( &Target, &MoveAction );
}

void CHeroMove::OnDbClickVehicle(DWORD MouseEvent, CVehicle* pVehicle)
{
	// ž������ ��û
	if( pVehicle->IsGetOn( HERO ) )
	{
		// 090423 ShinJS --- ������ �� �ִ� �������� �˻�
		if( !pVehicle->CanGetOff( HERO ) )
		{
			return;
		}

		// 090316 LUJ, ������ ��û�Ѵ�
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_DISMOUNT_SYN;
		message.dwObjectID = HEROID;
		NETWORK->Send( &message, sizeof( message ) );
	}
	// ž�� ��û
	else
	{
		CActionTarget Target;
		Target.InitActionTarget( pVehicle, NULL );

		CAction MoveAction;
		float fDistance = NPC_TALKING_DISTANCE;

		MoveAction.InitAction( eActionKind_VehicleGetOn, NULL, fDistance, &Target );
		MOVEMGR->SetHeroActionMove( &Target, &MoveAction );
	}
}

void CHeroMove::AttackSelectedObject()
{
	CObject* pTargetObj = OBJECTMGR->GetSelectedObject();
	if( !pTargetObj ) return;

	OnAttack(pTargetObj,MOUSEEVENT_LCLICK);
}

// LBS 03.10.09
void CHeroMove::SelectLoginPoint( WORD LoginPointIndex )
{
	MSG_DWORD msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_SAVEPOINT_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = LoginPointIndex;
				
	NETWORK->Send(&msg, sizeof(msg));

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(207) );

	HERO->GetCharacterTotalInfo()->LoginPoint_Index = LoginPointIndex;
}


void CHeroMove::MapChange( WORD MapIndex, DWORD ItemIdx, DWORD ItemPos )
{

	MSG_DWORD3 msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHANGEMAP_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = MapIndex;
	msg.dwData2 = ItemIdx;
	msg.dwData3 = ItemPos;
	
	NETWORK->Send(&msg, sizeof(msg));	
	
	g_UserInput.SetInputFocus(FALSE);
}

// 070917 LYW --- HeroMove : Add function to process change map through the npc.
void CHeroMove::MapChangeToNpc(DWORD dwMapNum, float xPos, float zPos)
{
#ifdef _TESTCLIENT_
	GAMERESRCMNGR->m_TestClientInfo.Map = static_cast<MAPTYPE>(dwMapNum);
	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&dwMapNum,4);
#else
	MSG_DWORD3 msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_NPC_CHANGEMAP_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = dwMapNum;
	msg.dwData2 = (DWORD)xPos;
	msg.dwData3 = (DWORD)zPos;
	
	NETWORK->Send(&msg, sizeof(msg));	
	
	g_UserInput.SetInputFocus(FALSE);
#endif
}

// 070121 LYW --- Add fucntion to change map.
void CHeroMove::ChangeMap( WORD MapIndex )
{
#ifdef _TESTCLIENT_
	MAPCHANGE_INFO* info = GAMERESRCMNGR->GetMapChangeInfo(MapIndex);

	if (info)
	{
		GAMERESRCMNGR->m_TestClientInfo.x = info->MovePoint.x;
		GAMERESRCMNGR->m_TestClientInfo.z = info->MovePoint.z;

		GAMERESRCMNGR->m_TestClientInfo.Map = static_cast<MAPTYPE>(info->MoveMapNum);
		MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
		MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&(info->MoveMapNum),4);
	}
#else
	//---KES 071007 �޼��� �������� �������� �� ����
	static DWORD dwLastChangeTime = 0;

	if( gCurTime - dwLastChangeTime < 3000 )	return;	//3�� �̳��� ������ �ʴ´�.
	else dwLastChangeTime = gCurTime;
	//-----------------------------------------------

	MSG_DWORD msg;
	msg.Category	= MP_USERCONN;
	msg.Protocol	= MP_USERCONN_CHANGEMAP_SYN;
	msg.dwObjectID	= HEROID;
	msg.dwData		= MapIndex ;
	
	NETWORK->Send(&msg, sizeof(msg));	
	
	g_UserInput.SetInputFocus(FALSE);
#endif
}
