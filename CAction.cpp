#include "stdafx.h"
#include "CAction.h"
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
#include "StreetStallManager.h"

#include "ObjectManager.h"
#include "NpcScriptManager.h"
#include "cMsgBox.h"
#include "interface/cWindowManager.h"
#include "WindowIDEnum.h"
#include "ChatManager.h"
#include "MoveManager.h"
#include "objectstatemanager.h"
#include "ItemManager.h"
#include "Gamein.h"
#include "InventoryExDialog.h"
#include "../[cc]skill/client/manager/skillmanager.h"

// 080822 LYW --- CAction : npc ��ȯ �Ŵ��� ����� ȣ���Ѵ�.
#include "./NpcRecallMgr.h"

// 080904 LWY --- SkillManager : ���������� ����� �� ���� ��ų ��� �߰��ϸ鼭 �ʿ��� ��� ���� �߰�.
#include "WindowIDEnum.h"
#include "cmsgbox.h"
#include "../interface/cWindowManager.h"
#include "./SiegeWarfareMgr.h"

// 081125 LYW --- CAction : ����Ʈ ���� �߰� ó��.
#include "./cMapMoveDialog.h"

#include "VehicleManager.h"
#include "RidePopupMenuDlg.h"
#include "DungeonMgr.h"

CAction::CAction()
{
	Clear();
}

CAction::~CAction()
{

}

BOOL CAction::HasAction()
{
	return m_ActionKind==eActionKind_None ? FALSE : TRUE;
}

void CAction::CopyFrom(CAction* pSrc)
{
	if(pSrc == NULL)
		Clear();
	else
		memcpy(this,pSrc,sizeof(CAction));
}

void CAction::InitSkillAction(cActiveSkillInfo* pSkillInfo,CActionTarget* pTarget)
{
	float dis = ( float )( pSkillInfo->GetSkillInfo()->Range - 50 );
	InitAction(eActionKind_Skill,pSkillInfo,dis,pTarget);	//
}
void CAction::InitSkillActionRealExecute(cActiveSkillInfo* pSkillInfo,CActionTarget* pTarget)
{
	float dis = ( float )( pSkillInfo->GetSkillInfo()->Range - 50 );
	InitAction(eActionKind_Skill_RealExecute,pSkillInfo,dis,pTarget);	//
}

void CAction::InitStallAction( float Distance, CActionTarget* pTarget )
{
	InitAction(eActionKind_TalkToOwner, NULL, Distance, pTarget);	//
}

void CAction::InitMoveAction(VECTOR3* pToPos)
{
	m_ActionTarget.InitActionTarget(pToPos,0);
	InitAction(eActionKind_Move, NULL, 0, &m_ActionTarget);
}



void CAction::InitAction(int ActionKind,void* pActionInfo,float Distance,CActionTarget* pTarget)
{
	m_ActionKind = ActionKind;
	m_Distance = Distance;
	m_pActionInfo = pActionInfo;
	m_ActionTarget.CopyFrom(pTarget);
}
void CAction::Clear()
{
	m_ActionKind = eActionKind_None;
}

BOOL CAction::CheckTargetDistance(VECTOR3* pCurPos)
{
	VECTOR3* pTargetPos = m_ActionTarget.GetTargetPosition();
	if(pTargetPos == NULL)
		return FALSE;

	float dist = CalcDistanceXZ(pCurPos,pTargetPos);
	if(dist <= m_Distance)
		return TRUE;
	else
		 return FALSE;
}

BOOL CAction::ExcuteAction(CHero* pOperator)
{
	switch(m_ActionKind) {
	case eActionKind_Skill:
		{
			// 100617 ShinJS �������� ��ų�� ���ŵ� �� �ߵ���Ų��.
			if( HERO->GetCurrentSkill() > 0 )
				return FALSE;

			cActiveSkillInfo* pNextSkill = (cActiveSkillInfo*)m_pActionInfo;

  			// 080904 LYW --- SkillManager : ������ �� ����� �� ���� ��ų üũ.
  			if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  				SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  				SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
  			{
  				if( SIEGEWARFAREMGR->IsFobiddenSkill(pNextSkill->GetIndex()) )
  				{
  					WINDOWMGR->MsgBox( MBI_SKILL_SIEGEWAR_FORBIDDENSKILL, MBT_OK, CHATMGR->GetChatMsg(358) ) ;
  					return TRUE;
  				}
  			}

			if(pNextSkill->IsInRange(*pOperator, m_ActionTarget))
			{
				SKILLMGR->ExcuteSkillSYN(pOperator,&m_ActionTarget,pNextSkill);
			}
			else
			{	// ����
				SKILLMGR->ExcuteSkillSYN(pOperator,&m_ActionTarget,TRUE);
			}			
		}
		break;
		
	case eActionKind_Skill_RealExecute:
		{
	  		// 080904 LYW --- SkillManager : ������ �� ����� �� ���� ��ų üũ.
	  		if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
	  			SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
	  			SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
	  		{
	  			if( SIEGEWARFAREMGR->IsFobiddenSkill( ((cSkillInfo*)m_pActionInfo)->GetIndex()) )
	  			{
	  				WINDOWMGR->MsgBox( MBI_SKILL_SIEGEWAR_FORBIDDENSKILL, MBT_OK, CHATMGR->GetChatMsg(358) ) ;
	  				return TRUE;
	  			}
	  		}

			SKILLMGR->RealExcuteSkillSYN(pOperator,&m_ActionTarget,(cSkillInfo*)m_pActionInfo);
		}
		break;

	case eActionKind_TalkToNpc:
		{
			// ����/��æƮ/��ȭ/���� ���� ��� �Ұ�
			{
				cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
				cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
				cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
				cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

				ASSERT( mixDialog && enchantDialog && reinforceDialog && dissoloveDialog );

				const BOOL isOpen = mixDialog->IsActive()		||
									enchantDialog->IsActive()	||
									reinforceDialog->IsActive()	||
									dissoloveDialog->IsActive();

				if( isOpen )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1104 ) );
					break;
				}
			}

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( m_ActionTarget.GetTargetID() );
			if( pNpc == NULL )	break;
			if((pNpc->GetNpcJob() == BOMUL_ROLE) && (pNpc->GetState() == eObjectState_Die))
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(918));
				break;
			}

			// 080922 LYW --- CAction : �罨 ���.
  			else if( pNpc->GetNpcJob() == SIEGEFLAGROLE_RUSHEN || pNpc->GetNpcJob() == SIEGEFLAGROLE_ZEVYN )
  			{
				MSG_WORD msg ;

				msg.Category		= MP_SIEGEWARFARE ;
				msg.Protocol		= MP_SIEGEWARFARE_CHECK_FLAGUSE_SYN ;

				msg.dwObjectID		= HEROID ;

				msg.wData			= pNpc->GetNpcKind() ;

				NETWORK->Send( &msg, sizeof(MSG_WORD) ) ;

  				//WINDOWMGR->MsgBox(MBI_SIEGEWARFLAG, MBT_YESNO, CHATMGR->GetChatMsg(1686)) ;
  				break;
  			}

			// 081125 LYW --- CAction : ����Ʈ ���� ���� ����Ʈ üũ.
			else if( pNpc->GetNpcJob() == LIMITDUNGEON_ROLE )
			{
				cMapMoveDialog* const pDlg = GAMEIN->GetMapMoveDlg() ;
	   
   				if( pDlg )
				{
					pDlg->SetUseCase(eLimitDungeon) ;
   					pDlg->SetActive(TRUE) ;
				}

				break;
			}
			// 091014 ONS ����ġ��� NPC.
			else if( pNpc->GetNpcJob() == DUNGEON_SWITCH_ROLE )
			{
				MSG_DWORD2 msg;
  				msg.Category = MP_NPC;
  				msg.Protocol = MP_NPC_SWITCHNPC_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData1 = m_ActionTarget.GetTargetID();	//NPC Idx
				msg.dwData2 = pNpc->GetNpcUniqueIdx();
				NETWORK->Send( &msg, sizeof( msg ) );
				break;
			}
			// 100329 ShinJS --- ���� ����
			else if( DungeonMGR->IsDungeonEntranceNpc( pNpc->GetNpcJob() ) )
			{
				cMapMoveDialog* const pDlg = GAMEIN->GetMapMoveDlg() ;
   				if( pDlg )
				{
					pDlg->SetUseCase(eDungeon, pNpc->GetID()) ;
					if( pDlg->GetMapListCount() > 0 )
					{
   						pDlg->SetActive(TRUE) ;
					}
					else
					{
						pDlg->SetActive(FALSE) ;
						WINDOWMGR->MsgBox( MBI_NOTICE, MBT_OK, CHATMGR->GetChatMsg(2297) );
					}
				}
				break;
			}
  
  			// 080822 LYW --- CAction : ��ȯ �� npc���� Ȯ���Ѵ�.
  			NPC_TOTALINFO* pTotalInfo = NULL ;
  			pTotalInfo = pNpc->GetNpcTotalInfo() ;
  
  			if( pTotalInfo )
  			{
  				if( NPCRECALLMGR->Check_RecallNpc(pTotalInfo->dwRecallNpcIdx) )
  				{
  					pNpc->OnClicked() ;
  					break;
  				}
  			}

            // 090227 ShinJS --- �̵�NPC�� ��� �̵��� �غ������� Ȯ���Ѵ�
			if( pNpc->IsReadyToMove() )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1907 ), pNpc->GetObjectName() );
				break;
			}

			MSG_DWORD2 msg;
			msg.Category = MP_NPC;
			msg.Protocol = MP_NPC_SPEECH_SYN;
			msg.dwObjectID = HERO->GetID();
			msg.dwData1 = m_ActionTarget.GetTargetID();	//NPC Idx
			msg.dwData2 = pNpc->GetNpcUniqueIdx();

			OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);

			NETWORK->Send(&msg, sizeof(msg));
		
			pNpc->OnClicked();
		}
		break;
	case eActionKind_TalkToOwner:
//������� ����ȭ �ɶ����� ��ġ�ø���.
//		CHATMGR->AddMsg( CTC_SYSMSG, "������ ����� �Ͻ������� �ߴܵǾ����ϴ�. �̿뿡 ������ ��� �˼��մϴ�." );
		STREETSTALLMGR->EnterStreetStall( m_ActionTarget.GetTargetID() );
		break;
	case eActionKind_Move:
		MOVEMGR->SetHeroTarget( m_ActionTarget.GetTargetPosition(), TRUE);
		break;
	case eActionKind_EquipItem:
		{
			CItem* pItem = ITEMMGR->GetItem((DWORD)m_pActionInfo);		
			if(pItem == NULL)
				break;
			GAMEIN->GetInventoryDialog()->UseItem(pItem);
		}
		break;
	case eActionKind_VehicleGetOn:
		{
			CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetSelectedObject();
			if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )
			{
				// �߸��� �����Ͱ� ���õȰ�� �˾�â�� �ݴ´�
				CRidePopupMenuDlg* pDlg = GAMEIN->GetRidePopupMenuDlg();
				if( pDlg )
					pDlg->SetActive( FALSE );
				break;
			}
			else if(0 == pVehicle->CanGetOn(HERO))
			{
				break;
			}

			MSG_DWORD2 message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_VEHICLE;
			message.Protocol = MP_VEHICLE_MOUNT_ASK_SYN;
			message.dwObjectID = HEROID;
			message.dwData1 = pVehicle->GetID();
			NETWORK->Send( &message, sizeof( message ) );
		}
	default:
		ASSERT(0);
		break;
	}

	return TRUE;
}


void CAction::ExcutePetAction(CHeroPet* pOperator)
{
	switch(m_ActionKind) {
	case eActionKind_Skill:
		{
			cActiveSkillInfo* pNextSkill = (cActiveSkillInfo*)m_pActionInfo;

			if(pNextSkill->IsInRange(*pOperator, m_ActionTarget))
			{
				SKILLMGR->ExcutePetSkillSYN(pOperator,&m_ActionTarget,pNextSkill);
			}
			else
			{	// ����
				SKILLMGR->ExcutePetSkillSYN(pOperator,&m_ActionTarget,TRUE);
			}			
		}
		break;
		
	case eActionKind_Skill_RealExecute:
		SKILLMGR->RealExcutePetSkillSYN(pOperator,&m_ActionTarget,(cSkillInfo*)m_pActionInfo);
		break;

	case eActionKind_Move:
		MOVEMGR->SetHeroPetTarget( m_ActionTarget.GetTargetPosition() );
		break;

	default:
		ASSERT(0);
		break;
	}
}
