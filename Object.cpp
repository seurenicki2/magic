#include "stdafx.h"
#include "Object.h"
#include "MoveManager.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "ObjectStateManager.h"
#include "interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "ObjectBalloon.h"
#include "OBalloonChat.h"
#include "OBalloonName.h"
#include "StreetStallTitleTip.h"
#include "MHFile.h"
#include "ObjectGuagen.h"
#include "GameResourceManager.h"
#include "MOTIONDESC.h"
#include "BattleSystem_Client.h"
#include "MonsterGuageDlg.h"
#include "ChatManager.h"
#include "FacialManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "petmanager.h"
#include "Furniture.h"
#include "MonsterTargetDlg.h"
#include "BattleGuageDlg.h"

CObject::CObject()
{
	m_pObjectBalloon		= NULL;
	m_bObjectBalloonBits	= 0;
	m_StateEffectList.Initialize(3);
	m_pMotionDESC = NULL;
	m_bSelected	= FALSE;
	m_ObjectKind = eObjectKind_None;
	m_dwShockedTime = 0;
}

CObject::~CObject()
{
	Release();
}


BOOL CObject::Init(EObjectKind kind,BASEOBJECT_INFO* pBaseObjectInfo)
{
	m_bMoveSkipCount = 0;
	m_bIsYMoving = FALSE;
	m_BaseObjectInfo = *pBaseObjectInfo;
	m_BaseObjectInfo.ObjectBattleState = eObjectBattleState_Peace;
	m_ObjectKind = kind;
	m_ObjectState.bEndState = FALSE;

	SetFlag(FALSE);
//m_bDieFlag = FALSE;
	m_bInited = TRUE;
	mBattleStateCheckDelay.Init(
		10000);
	m_bHideEffect = FALSE;
	m_dwShockedTime = 0;
	return TRUE;
}


// 071214 LUJ, �ڵ� ����
void CObject::InitObjectBalloon(BYTE bitFlag)
{
	m_pObjectBalloon		= new CObjectBalloon;
	m_bObjectBalloonBits	= bitFlag;

	switch( GetObjectKind() )
	{
	case eObjectKind_BossMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_Monster:
	// 090422 ShinJS --- Ż�� �߰�
	case eObjectKind_Vehicle:
		{
			// 080616 LUJ, �߸��� ������ �ͼ� ���� ƨ��� ����. �̸� ���� ���� �������� �˻� �߰�
			const BASE_MONSTER_LIST* monsterList = ( ( CMonster* )this )->GetSInfo();

			if( monsterList )
			{
				m_pObjectBalloon->InitBalloon( this, monsterList->Tall, bitFlag );
			}

			break;
		}
	case eObjectKind_Player:
		{
			m_pObjectBalloon->InitBalloon(this, 180, bitFlag);
			break;
		}
	case eObjectKind_Npc:
		{
			CNpc* npc = ( CNpc* )this;
			m_pObjectBalloon->InitBalloon( npc, npc->GetSInfo()->Tall, bitFlag );
			break;
		}
	case eObjectKind_FarmObj:
		{
			const CSHFarmRenderObj::stFarmRenderObjInfo* const info = ( ( CSHFarmRenderObj* )this )->GetInfo();

			// 090806 LUJ, �������� �ʴ� ���ҽ��� Ŭ���̾�Ʈ�� ���� �����ϰ� �ϹǷ� �˻��Ѵ�
			if( 0 == info )
			{
				break;
			}

			const CSHFarmManager::stFARMRENDEROBJLIST* const list = g_csFarmManager.CLI_GetFarmRenderObjList( info->nKind );

			// 090806 LUJ, �������� �ʴ� ���ҽ��� Ŭ���̾�Ʈ�� ���� �����ϰ� �ϹǷ� �˻��Ѵ�
			if( 0 == list )
			{
				break;
			}

			m_pObjectBalloon->InitBalloon( this, list->nNameYpos, bitFlag );
			break;
		}
	case eObjectKind_Pet:
		{
			m_pObjectBalloon->InitBalloon(this, PETMGR->GetNameHeight(), bitFlag);
			break;
		}
	//090525 pdy �Ͽ�¡ 
	case eObjectKind_Furniture:
		{
			float fHeightSize = ((CFurniture*)this)->GetMarkingInfo()->fHeightSize;
			m_pObjectBalloon->InitBalloon(this, static_cast<long>(fHeightSize) , bitFlag);
			break;
		}
	default:
		{
			//m_pObjectBalloon->InitBalloon(this, 180, bitFlag);
			m_pObjectBalloon->InitBalloon(this, 180, bitFlag);
			break;
		}
	}

	if(! ( bitFlag & ObjectBalloon_Name ) )
	{
		return;
	}

	COBalloonName* balloonName =  m_pObjectBalloon->GetOBalloonName();

	if( ! balloonName )
	{
		return;
	}

	switch( GetObjectKind() )
	{
	case eObjectKind_Player:
		{	
			balloonName->InitName(m_BaseObjectInfo.ObjectName);

			CPlayer* player = ( CPlayer* )this;

			SetGuildMark();
			SetNickName();
			SetFamilyMark();
			SetAliasMark();

			m_pObjectBalloon->SetFamilyNickName(
				player->GetFamilyNickName());
			break;
		}
	case eObjectKind_Npc:
		{				
			CNpc* npc = ( CNpc* )this;

			if( npc->GetSInfo()->ShowJob )
			{
				m_pObjectBalloon->SetNickName( npc->GetSInfo()->Name );
			}

			const MAPCHANGE_INFO* info  = GAMERESRCMNGR->GetMapChangeInfo( npc->GetNpcUniqueIdx() );

			balloonName->InitName( info ? info->ObjectName : m_BaseObjectInfo.ObjectName );
			break;
		}
	default:
		{
			balloonName->InitName(
				GetObjectName());
			break;
		}
	}
}

void CObject::SetOverInfoOption( DWORD dwOption )
{
	if( m_pObjectBalloon )	
		m_pObjectBalloon->SetOverInfoOption( dwOption );
}

void CObject::SetGuildMark()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetGuildMark( ((CPlayer*)this)->GetGuildMarkName() );
}

void CObject::SetFamilyMark()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetFamilyMark( ((CPlayer*)this)->GetFamilyMarkName() );
}

void CObject::SetNickName()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetNickName( ((CPlayer*)this)->GetNickName() );
}

void CObject::ShowObjectName( BOOL bShow, DWORD dwColor, BOOL isPkMode )		//show/hide
{
	if( !m_pObjectBalloon )								return;
	if( !(m_bObjectBalloonBits & ObjectBalloon_Name) )	return;

	//Alemuri Party War------------------------------------------
	CPlayer* player = (CPlayer*)m_pObjectBalloon;
	DWORD color = GetPartyWarColor(dwColor, player, isPkMode);
	m_pObjectBalloon->ShowObjectName( bShow, color );
	//-----------------------------------------------------------
	//m_pObjectBalloon->ShowObjectName( bShow, dwColor );

}

void CObject::ShowChatBalloon( BOOL bShow, char* chatMsg, DWORD dwColor, DWORD dwAliveTime )
{
	if( !m_pObjectBalloon )								return;
	if( !(m_bObjectBalloonBits & ObjectBalloon_Chat) )	return;

	m_pObjectBalloon->ShowChatBalloon( bShow, chatMsg, dwColor, dwAliveTime );
}

//090116 pdy Ŭ���̾�Ʈ ����ȭ �۾� ( Hide ����Title)
void CObject::HideStreetStallTitle( BOOL bHide )		
{
	if( !m_pObjectBalloon )								return;

	m_pObjectBalloon->HideStreetStallTitle(bHide);
}

//-----

void CObject::InitMove(BASEMOVE_INFO* pMoveInfo)
{
	memset(&m_MoveInfo,0,sizeof(MOVE_INFO));
	memcpy(&m_MoveInfo,pMoveInfo,sizeof(BASEMOVE_INFO));
}

void CObject::Release()
{
	m_bMoveSkipCount = 0;
	m_bInited = FALSE;

	if(m_ShadowObj.IsInited())
	{
		m_ShadowObj.Release();
	}
	RemoveAllObjectEffect();

	m_ObjectKind = eObjectKind_None;
	m_BaseObjectInfo.ObjectState = eObjectState_None;
	m_EngineObject.Release();
	
	SAFE_DELETE( m_pObjectBalloon );
	m_bObjectBalloonBits = 0;
	ReleaseBuffSkillList();
}

void CObject::GetBaseObjectInfo(BASEOBJECT_INFO* pRtInfo)
{
	memcpy(pRtInfo,&m_BaseObjectInfo,sizeof(BASEOBJECT_INFO));
}

void CObject::GetBaseMoveInfo(BASEMOVE_INFO* pRtInfo)
{
	memcpy(pRtInfo,&m_MoveInfo,sizeof(BASEMOVE_INFO));
}

void CObject::Process()
{
	OBJECTSTATEMGR->StateProcess(this);
	MOVEMGR->MoveProcess(this);

	if (m_ShadowObj.IsInited())
		m_ShadowObj.SetEngObjPosition(&GetCurPosition());

	if (m_BaseObjectInfo.ObjectBattleState)
	{
		if (!mBattleStateCheckDelay.Check())
		{
			SetObjectBattleState(eObjectBattleState_Peace);
		}
	}
	//Alemuri Distant player disappear in GTMAP------------------------------------------------------------------------------------------
	//float playerX = 0;
	//float playerY = 0;
	//float myX = 0;
	//float myY = 0;
	//float differenceX;
	//float differenceY;
	//auto object = this;
	//if (object)
	//{
	//	if (HERO)
	//	{
	//		if (object->GetObjectKind() == eObjectKind_Player)
	//		{
	//			if (object->GetObjectName() != HERO->GetObjectName())
	//			{
	//				playerX = object->GetPosition().x;
	//				playerY = object->GetPosition().z;
	//				myX = HERO->GetPosition().x;
	//				myY = HERO->GetPosition().z;

	//				if (playerX > myX)
	//				{
	//					differenceX = playerX - myX;
	//				}
	//				else
	//				{
	//					differenceX = myX - playerX;
	//				}
	//				if (playerY > myY)
	//				{
	//					differenceY = playerY - myY;
	//				}
	//				else
	//				{
	//					differenceY = myY - playerY;
	//				}
	//				if (differenceX > 4507 || differenceY > 4507)
	//				{
	//					CPlayer* pPlayer = (CPlayer*)object;
	//					unsigned short hideLevel = pPlayer->GetCharacterTotalInfo()->HideLevel;
	//						if (hideLevel < 20)
	//							hideLevel += 20;
	//					if (m_hideLevelCheckChanged != hideLevel)
	//					{
	//						OBJECTMGR->SetHide( pPlayer->GetID(), hideLevel );
	//						m_hideLevelCheckChanged = hideLevel;
	//					}
	//				}
	//				else
	//				{
	//					CPlayer* pPlayer = (CPlayer*)object;
	//					unsigned short hideLevel = pPlayer->GetCharacterTotalInfo()->HideLevel;
	//						if (hideLevel > 10)
	//							hideLevel -= 20;
	//					if (m_hideLevelCheckChanged != hideLevel)
	//					{
	//						OBJECTMGR->SetHide( pPlayer->GetID(), hideLevel);
	//						m_hideLevelCheckChanged = hideLevel;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//----------------------------------------------------------------------------------------------------------------------------------------

	// E ������ȭ���� �ּ�ó�� added by hseos 2007.08.02
}

void CObject::SetFlag(BOOL val)
{
	if( val )
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetTargetVersusID() == GetID() )
		{
			GAMEIN->GetMonsterGuageDlg()->TargetVersusDie();
		}
	}
	m_bDieFlag = val; 
}

void CObject::SetDieFlag()
{
	m_DiedTime = gCurTime;

	// �׾������� ó��
	SetFlag(TRUE);
//	m_bDieFlag = TRUE;
	MOVEMGR->MoveStop(this);
	HERO->ClearTarget(this);

	if( HEROPET )
		HEROPET->ClearTarget(this);

	// LBS 03.11.21
	if( OBJECTMGR->GetSelectedObjectID() == GetID() )
	{
		// 070510 LYW --- Object : Modified.
		//OBJECTMGR->SetSelectedObjectID(0);
		GAMEIN->GetMonsterGuageDlg()->StartFadeOut() ;

		//KES �߰�	//���⼭ �ϴ°� �̻��ϴ�. ���Ͱ������� ����α����ؼ� �� ������.
		OnDeselected();
		if( OBJECTMGR->CanOverInfoChange( this ) )
			ShowObjectName( FALSE );
		else
		{
			//Alemuri Party War if Player is in PK mode add the BOOL to the function
			CPlayer* player = (CPlayer*)this;
			BOOL isPk = FALSE;
			if (player)
			{
				isPk = player->IsPKMode();
			}
			ShowObjectName( TRUE, OBJECTMGR->GetObjectNameColor(this), isPk); //Alemuri Party War (adding the PKMode Bool in the function to make the GetPartyWarColor function working)
		}
		//
	}
	
//	OBJECTSTATEMGR->StartObjectState(this, eObjectState_Die);
	
	//CHATMGR->AddMsg( CTC_TOWHOLE, CHATMGR->GetChatMsg(133), );
	// ������ Ŭ�� �ȵǰ� ��
	if( m_ObjectKind == eObjectKind_Player )
		m_EngineObject.EnablePick();
	else
		m_EngineObject.DisablePick();
}

void CObject::Revive(VECTOR3* pRevivePos)
{
	SetFlag(FALSE);
//	m_bDieFlag = FALSE;
	MOVEMGR->SetPosition(this,pRevivePos);
}

void CObject::SetState(EObjectState State)
{
	SetMotionInState(State);
	
	if(m_ObjectState.State_Start_Motion != -1)
	{
		ChangeMotion(m_ObjectState.State_Start_Motion,FALSE);

		if(m_ObjectState.State_Ing_Motion != -1)
			ChangeBaseMotion(m_ObjectState.State_Ing_Motion);
	}
	
	m_BaseObjectInfo.ObjectState		= State;
	m_ObjectState.State_Start_Time = gCurTime;
	m_ObjectState.bEndState		= FALSE;
}

void CObject::SetPosition(VECTOR3* pPos)
{
	GetEngineObject()->SetEngObjPosition(pPos);
}
void CObject::SetAngle(float AngleRad)
{
	GetEngineObject()->SetEngObjAngle(AngleRad);
}
void CObject::GetPosition(VECTOR3* pPos)
{
	GetEngineObject()->GetEngObjPosition(pPos);
}
float CObject::GetAngle()
{
	return GetEngineObject()->GetEngObjAngle();
}
DIRINDEX CObject::GetDirectionIndex()
{
	float angleRad = GetAngle();
	DIRINDEX dir = RADTODIR(angleRad);
	while(dir >= MAX_DIRECTION_INDEX)
		dir -= MAX_DIRECTION_INDEX;
	return dir;
}

void CObject::ChangeMotion(int motion,BOOL bRoop, float rate)
{
	GetEngineObject()->ChangeMotion(motion,bRoop,rate);
}
void CObject::ChangeBaseMotion(int motion)
{
	GetEngineObject()->ChangeBaseMotion(motion);
}

// 070222 LYW --- Object : Add function to change emotion of hero.
void CObject::ChangeEmotion( int emotion ) 
{
	GetEngineObject()->ChangeMotion(emotion,FALSE);
	((CPlayer*)this)->SetRunningEmotion(TRUE) ;
}


void CObject::OnSelected()
{
 	GetEngineObject()->EnableSelfIllumin();
	m_bSelected = TRUE;
}

void CObject::OnDeselected()
{
	// desc_hseos_����Ʈȿ��01
	// S ����Ʈȿ�� �߰� added by hseos 2007.06.13
	//	GetEngineObject()->DisableSelfIllumin();
	// E ����Ʈȿ�� �߰� added by hseos 2007.06.13
	m_bSelected = FALSE;
}

void CObject::SetBattle(DWORD BattleID,BYTE Team)
{
	m_BaseObjectInfo.BattleID = BattleID;
	m_BaseObjectInfo.BattleTeam = Team;
}

void CObject::AddObjectEffect(DWORD KeyValue,OBJECTEFFECTDESC* EffectNumArray,WORD NumOfEffect,CObject* pSkillOperator)
{
	HEFFPROC heff = 0;

	m_StateEffectList.StartGetMultiData(KeyValue);
	// 100111 LUJ, ���� ����Ʈ�� �ִ��� �˻��Ѵ�
	while((heff = (HEFFPROC)m_StateEffectList.GetMultiData()) != NULL)
	{
		EFFECTMGR->IncEffectRefCount(heff);
		break;
	}

	// �������� ���� �����.
	for(int n=0;n<NumOfEffect;++n)
	{
		OBJECTEFFECTDESC& description = EffectNumArray[n];

		if(description.Effect == 0)
			continue;

		heff = EFFECTMGR->StartSkillMainTainEffect(
			description.Effect,
			this,
			description.Flag,
			&description.Position,
			pSkillOperator);

		if(heff == NULL)
			continue;

		m_StateEffectList.Add(
			LPVOID(heff),
			KeyValue);
	}
}

void CObject::RemoveObjectEffect(DWORD KeyValue)
{
	HEFFPROC heff;
	// ������ �ִ� ����Ʈ�� ã�´�.
//	BOOL bFound = FALSE;
	m_StateEffectList.StartGetMultiData(KeyValue);
	
	cPtrList list;

	while((heff = (HEFFPROC)m_StateEffectList.GetMultiData()) != NULL)
	{
		if(EFFECTMGR->ForcedEndEffect(heff) == TRUE)
		{
//			m_StateEffectList.RemoveCurMultiData();
			list.AddTail((void*)KeyValue);
		}
	}

	PTRLISTPOS pos = list.GetHeadPosition();
	while( pos )
	{
		DWORD k = (HEFFPROC)list.GetNext(pos);
		m_StateEffectList.Remove(k);
	}
	list.RemoveAll();

}
void CObject::RemoveAllObjectEffect()
{
	HEFFPROC heff;
	m_StateEffectList.SetPositionHead();
	while((heff = (HEFFPROC)m_StateEffectList.GetData()) != NULL)
	{
		while(EFFECTMGR->ForcedEndEffect(heff) == FALSE)
		{}
	}
	m_StateEffectList.RemoveAll();	//KES ����Ʈ���� ������ �ǳ�? forceEnd�� �ȵȰ���..confirm
}
int	CObject::GetMotionIDX(int mainMotion, int subMotion)
{
	return m_pMotionDESC->GetMotionIDX(mainMotion, subMotion);
}

void CObject::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
}

BOOL CObject::IsNpcMark( DWORD dwValue )
{
	HEFFPROC heff = (HEFFPROC)m_StateEffectList.GetData( dwValue );
	if( heff )
		return TRUE;
	return FALSE;
}

void CObject::SetObjectBattleState(eObjectBattleState state)
{
	if( state )
	{
		mBattleStateCheckDelay.Start();
	}
	else
	{
		mBattleStateCheckDelay.End();
	}

	if( m_BaseObjectInfo.ObjectBattleState != state )
	{
		m_BaseObjectInfo.ObjectBattleState = state;

		
		DWORD	dwTargetID	=	GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID();
		
		if( dwTargetID == GetID() && state == eObjectBattleState_Peace )
		{
			GAMEIN->GetMonsterGuageDlg()->SetTargetVersusInfo( 0 );
		}
		
		if( GetObjectKind() == eObjectKind_Player )
		{
			((CPlayer*)this)->SetBaseMotion();
			if(GetState() == eObjectState_None)
				OBJECTSTATEMGR->EndObjectState(this,eObjectState_None);
		}
	}
}

//090901 pdy ����Ʈ Hideó�� �߰� 
void CObject::HideEffect( BOOL bHide )
{
	m_bHideEffect = bHide ;
}

void CObject::SetShockTime( DWORD dwTime )
{
	m_dwShockedTime = gCurTime + dwTime;
}

BOOL CObject::IsShocked()
{
	if( gCurTime < m_dwShockedTime )
	{
		return TRUE;
	}

	m_dwShockedTime = 0;
	return FALSE;
}

void CObject::InsertBuffSkill( SKILLOBJECT_INFO* pBuffSkill )
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	if( !pBuffSkill )
		return;

	bool bIsCorrect = false;
	
	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		if( pBuffSkill->SkillIdx == pBuffSkillObject->SkillIdx )
			bIsCorrect = true;
	}
	
	if( !bIsCorrect )
	{
		SKILLOBJECT_INFO* pNewBuffSkillInfo = new SKILLOBJECT_INFO;
		memset( pNewBuffSkillInfo, 0, sizeof( SKILLOBJECT_INFO ) );
		memcpy( pNewBuffSkillInfo, pBuffSkill, sizeof( SKILLOBJECT_INFO ) );
		m_BuffSkillList.push_back( pNewBuffSkillInfo );
	}

}

void CObject::RemoveBuffSkill( DWORD dwSkillObjectIdx )
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	CMonsterGuageDlg* pMonsterGuage	=	GAMEIN->GetMonsterGuageDlg();

	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		if( dwSkillObjectIdx == pBuffSkillObject->SkillIdx )
		{
			pMonsterGuage->RemoveSkill( dwSkillObjectIdx );

			m_BuffSkillList.erase( buffSkillListItor );
			buffSkillListItor = m_BuffSkillList.end();
		}
	}
}


void CObject::ReleaseBuffSkillList()
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		SAFE_DELETE( pBuffSkillObject );
	}
	m_BuffSkillList.clear();
}

void CObject::UpdateTargetBuffSkill()
{
	SKILLOBJECT_INFO* pBuffSkillObject	=	NULL;

	CMonsterGuageDlg* pMonsterGuage	=	GAMEIN->GetMonsterGuageDlg();

	pMonsterGuage->RemoveAllSkillIcon();

	for( BuffSkillList::iterator buffSkillListItor	=	m_BuffSkillList.begin();
		 buffSkillListItor != m_BuffSkillList.end(); 
		 ++buffSkillListItor )
	{
		pBuffSkillObject = (*buffSkillListItor);
		pMonsterGuage->InsertSkillIcon( pBuffSkillObject->SkillObjectIdx, 
										pBuffSkillObject->SkillIdx, 
										0, 
										0 );
	}
}

void CObject::SetLife( DWORD life, BYTE type )
{
	CObject* pTargetObject	=	GAMEIN->GetBattleGuageDlg()->GetCurTargetObject();
	if( pTargetObject )
	{
		if( pTargetObject->GetID() == GetID() )
			GAMEIN->GetBattleGuageDlg()->SetBattleTargetInfo( this );
	}

	if( GAMEIN->GetMonsterGuageDlg()->GetCurTarget() )
	{
		if( GAMEIN->GetMonsterGuageDlg()->GetCurTarget()->GetTargetVSID() == GetID() )
		{
			float	fLife		=	(float)GetLife();
			float	fMaxLife	=	(float)GetMaxLife();

			GAMEIN->GetMonsterGuageDlg()->GetCurTarget()->SetTargetVSGuage( fLife / fMaxLife );
		}
	}
}

void CObject::ApplyGravity( BOOL bApplyGravity, float fGravityAcceleration, DWORD dwValidTime )
{
	CEngineObject* pEngineObject = GetEngineObject();
	if( pEngineObject == NULL )
		return;

	pEngineObject->ApplyHeightField( !bApplyGravity );
	
	m_MoveInfo.bApplyGravity = bApplyGravity;
	m_MoveInfo.dwValidGravityTime = gCurTime + dwValidTime;
	m_MoveInfo.fGravityAcceleration = fGravityAcceleration;
	m_MoveInfo.fGravitySpeed = 0.f;

	if( bApplyGravity == FALSE )
	{
		VECTOR3 curPosition={0};
		GetPosition( &curPosition );
		g_pExecutive->GXMGetHFieldHeight( &curPosition.y, curPosition.x, curPosition.z );
		m_MoveInfo.CurPosition.y = curPosition.y;
		SetPosition( &curPosition );
	}
}
void CObject::SetAliasMark()
{
	if( m_pObjectBalloon )
		m_pObjectBalloon->SetAliasMark( (eALIASTYPE)((CPlayer*)this)->GetAliasType() );
}
//
VECTOR3 CObject::GetPosition(void)
{
	VECTOR3 l_v3Pos;
	GetEngineObject()->GetEngObjPosition(&l_v3Pos);

	return l_v3Pos;
}

//Alemuri Party War--------------------------------------------------------
DWORD CObject::GetPartyWarColor(DWORD originalColor, CPlayer* pPlayer, BOOL isPkMode)
{
	if (HERO)
	{
		if (HERO->GetPartyWar())
		{
		//Ally Color
		std::string ally1std = HERO->GetAlly1();
		const char* ally1 = ally1std.c_str();
		if (strcmp(GetObjectName(), ally1) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		std::string ally2std = HERO->GetAlly2();
		const char* ally2 = ally2std.c_str();
		if (strcmp(GetObjectName(), ally2) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		std::string ally3std = HERO->GetAlly3();
		const char* ally3 = ally3std.c_str();
		if (strcmp(GetObjectName(), ally3) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		std::string ally4std = HERO->GetAlly4();
		const char* ally4 = ally4std.c_str();
		if (strcmp(GetObjectName(), ally4) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		std::string ally5std = HERO->GetAlly5();
		const char* ally5 = ally5std.c_str();
		if (strcmp(GetObjectName(), ally5) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		std::string ally6std = HERO->GetAlly6();
		const char* ally6 = ally6std.c_str();
		if (strcmp(GetObjectName(), ally6) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		std::string ally7std = HERO->GetAlly7();
		const char* ally7 = ally7std.c_str();
		if (strcmp(GetObjectName(), ally7) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARALLY;

		//Enemy Color
		std::string enemy1std = HERO->GetEnemy1();
		const char* enemy1 = enemy1std.c_str();
		if (strcmp(GetObjectName(), enemy1) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;
		
		std::string enemy2std = HERO->GetEnemy2();
		const char* enemy2 = enemy2std.c_str();
		if (strcmp(GetObjectName(), enemy2) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;

		std::string enemy3std = HERO->GetEnemy3();
		const char* enemy3 = enemy3std.c_str();
		if (strcmp(GetObjectName(), enemy3) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;

		std::string enemy4std = HERO->GetEnemy4();
		const char* enemy4 = enemy4std.c_str();
		if (strcmp(GetObjectName(), enemy4) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;

		std::string enemy5std = HERO->GetEnemy5();
		const char* enemy5 = enemy5std.c_str();
		if (strcmp(GetObjectName(), enemy5) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;

		std::string enemy6std = HERO->GetEnemy6();
		const char* enemy6 = enemy6std.c_str();
		if (strcmp(GetObjectName(), enemy6) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;

		std::string enemy7std = HERO->GetEnemy7();
		const char* enemy7 = enemy7std.c_str();
		if (strcmp(GetObjectName(), enemy7) == 0)
			if (isPkMode)
				return NAMECOLOR_PARTYWARENEMY;
		}
	}
	return originalColor;
}
//-------------------------------------------------------------------------------