#include "stdafx.h"
#include "ObjectManager.h"
#include "AppearanceManager.h"
#include "MoveManager.h"
#include "ItemManager.h"
#include "cSkillTreeManager.h"
#include "CharacterCalcManager.h"
#include "ObjectStateManager.h"
#include "PartyManager.h"
#include "Effect\EffectManager.h"
#include "ChatManager.h"
#include "GameIn.h"
#include "ObjectBalloon.h"
#include "MonsterGuageDlg.h"
#include "GuildFieldWar.h"
#include "BattleSystem_Client.h"
#include "./interface/cButton.h"
#include "CheatMsgParser.h"
#include "ObjectGuagen.h"
#include "interface/cStatic.h"
#include "interface/cWindowManager.h"
#include "MainGame.h"
#include "MHCamera.h"
#include "StatusIconDlg.h"
#include "cSkillTreeManager.h"
#include "../[cc]skill/client/object/skillobject.h"
#include "UserInfoManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "NpcScriptDialog.h"
#include "../InventoryExDialog.h"
#include "./ChatRoomMgr.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "Furniture.h"
#include "./NpcRecallMgr.h"
#include "VehicleManager.h"
#include "StreetTournamentMgr.h"
#include "MHMap.h"

DWORD gHeroID = 0;
DWORD gUserID = 0;


GLOBALTON(CObjectManager);


CObjectManager::CObjectManager()
{
	m_pHero			= NULL;
	mpHeroPet		= NULL;
	mpHeroPetBackup = NULL;
	m_pCharCalcMgr	= CHARCALCMGR;
	m_ObjectTable.Initialize(1024);
	m_HideObjectTable.Initialize(10);
	
	m_PlayerPool.Init(MAX_PLAYEROBJECT_NUM, 50, "CPlayer");
	m_MonsterPool.Init(MAX_MONSTEROBJECT_NUM, 50, "CMonster");
	m_NpcPool.Init(MAX_NPCOBJECT_NUM, 50, "CNpc");
//	m_BossMonsterPool.Init(MAX_MONSTEROBJECT_NUM, 50, "CBossMonster");	//KES ������ MAX_MONSTEROBJECT_NUM�� �ʿ��Ѱ�?
	m_BossMonsterPool.Init(10, 5, "CBossMonster");	//KES
	m_MapObjectPool.Init(MAX_MAPOBJECT_NUM, 10, "CMapObject");
	m_PetPool.Init( MAX_PETOBJECT_NUM, 10, "CPet" );
	// 090422 ShinJS --- Ż�� �߰��۾�
	m_VehiclePool.Init( MAX_VEHICLEOBJECT_NUM, 50, "CVehicle" );
	m_FurniturePool.Init(MAX_NPCOBJECT_NUM, 50, "CFurniture");
	m_csFarmRenderObjPool.Init(MAX_FARM_RENDER_OBJ_NUM, 50, "CSHFarmRenderObj");

	m_SelectedObjectID = 0;
	m_dwOverInfoOption = eOO_NAME_CLICKEDONLY;
	m_bAllNameShow = FALSE;
	m_dwEffectOption = eOO_EFFECT_DEFAULT;
	m_dwLodMode = 0;

	m_SelectEffectHandle = 0;
	///
}

CObjectManager::~CObjectManager()
{
}


void CObjectManager::AddShadow(CObject* pObject)
{
	if(pObject->GetObjectKind() == eObjectKind_Player)
	{
		if(((CPlayer*)pObject)->m_CharacterInfo.bVisible == FALSE)
		{
			RemoveShadow(pObject);
			return;
		}
	}

	BYTE bShadow = GAMERESRCMNGR->m_GameDesc.bShadow;
	if(bShadow == MHSHADOW_CIRCLE || bShadow == MHSHADOW_DETAIL)
	{		
		if(pObject == HERO && bShadow == MHSHADOW_DETAIL)
			pObject->GetEngineObject()->EnableShadow(TRUE);
		else
		{
			pObject->GetEngineObject()->EnableShadow(FALSE);
			pObject->m_ShadowObj.Init(
				"sh.mod",
				0,
				eEngineObjectType_Shadow,
				0,
				USHRT_MAX);
			pObject->m_ShadowObj.ApplyHeightField(TRUE);
		}
	}
	else
	{
		pObject->GetEngineObject()->EnableShadow(FALSE);
	}
}

void CObjectManager::RemoveShadow(CObject* pObject)
{
	if(pObject->m_ShadowObj.IsInited())
	{
		pObject->m_ShadowObj.Release();
	}
	pObject->GetEngineObject()->EnableShadow(FALSE);
}
//sort test
void CObjectManager::SortItem(SEND_HERO_TOTALINFO * sendHeroTotalInfo)
{
	gHeroID = m_pHero->GetID();
	CInventoryExDialog* pInvenDlg = NULL;
	pInvenDlg = GAMEIN->GetInventoryDialog();
	pInvenDlg->InitExtended_Inventory(&sendHeroTotalInfo->ChrTotalInfo);

	ITEMMGR->NetworkMsgParse(MP_ITEM_SORT, &sendHeroTotalInfo->ItemTotalInfo);
}

void CObjectManager::RegisterHero(SEND_HERO_TOTALINFO * sendHeroTotalInfo)
{
	m_pHero = new CHero;
	m_pHero->Init(eObjectKind_Player, &sendHeroTotalInfo->BaseObjectInfo);
	m_pHero->InitPlayer(&sendHeroTotalInfo->ChrTotalInfo);
	m_pHero->InitHero(&sendHeroTotalInfo->HeroTotalInfo);
	gHeroID = m_pHero->GetID();
	m_pHero->InitObjectBalloon(ObjectBalloon_Name | ObjectBalloon_Chat | ObjectBalloon_Title | ObjectBalloon_MunpaMark );

	// 071210 LYW --- ObjectManager : Ȯ�� �κ��丮�� Ȱ��ȭ ���θ� �����Ѵ�.
	CInventoryExDialog* pInvenDlg = NULL ;
	pInvenDlg = GAMEIN->GetInventoryDialog() ;

	if( !pInvenDlg ) return ;

	pInvenDlg->InitExtended_Inventory( &sendHeroTotalInfo->ChrTotalInfo ) ;
	BASEMOVE_INFO moveInfo;
	moveInfo.SetFrom(&sendHeroTotalInfo->SendMoveInfo);	
	
	MOVEMGR->InitMove(m_pHero,&moveInfo);

	for(CAddableInfoIterator iter(&sendHeroTotalInfo->AddableInfo);
		CAddableInfoList::None != iter.GetInfoKind();
		iter.ShiftToNextData())
	{
		switch(iter.GetInfoKind())
		{
		case (CAddableInfoList::ItemOption):
			{
				ITEM_OPTION OptionInfo[MAX_ITEM_OPTION_NUM+SLOT_STORAGE_NUM];	//iter.GetInfoData( ����Ҹ� �䱸�Ѵ�.
				iter.GetInfoData(&OptionInfo);						//iter.GetInfoDataptr( �� �ϳ� �������..

				for( WORD i = 0; i < sendHeroTotalInfo->OptionNum; ++i )
				{
					ITEM_OPTION& option = OptionInfo[ i ];

					ITEMMGR->AddOption( option );
				}
			}
			break;
		case (CAddableInfoList::SkillTree):
			{
				SKILL_BASE SkillTreeInfo[MAX_SKILL_TREE] = {0};
				iter.GetInfoData(
					SkillTreeInfo);

				for(WORD i = 0; i < sendHeroTotalInfo->SkillNum; ++i)
				{
					SKILL_BASE skillBase = SkillTreeInfo[i];
					
					// LUJ, �н��� ��ų�� ���� ������ �����ؾ�, ��Ʈ ������ ���ʽ��� �߸� ������� �ʴ´�.
					if(0 < skillBase.mLearnedLevel)
					{
						skillBase.Level = skillBase.mLearnedLevel;
					}
					// LUJ, �н� ��ų�� �ƴϰ� ���� ��ų�� �ƴ� ���� �н��Ѵ�
					else if(FALSE == CHARCALCMGR->IsJobSkill(skillBase.wSkillIdx))
					{
						continue;
					}

					SKILLTREEMGR->Update(
						skillBase);
				}
			}
		}
	}

	ITEMMGR->NetworkMsgParse(MP_ITEM_TOTALINFO_LOCAL, &sendHeroTotalInfo->ItemTotalInfo);
//	SKILLTREEMGR->NetworkMsgParse(MP_SKILLTREE_INFO_LOCAL, &sendHeroTotalInfo->SkillTreeInfo);
	

	m_ObjectTable.Add(m_pHero,m_pHero->GetID());


	// �������̽��� ����
	m_pHero->ApplyInterface();

	ApplyOverInfoOption( m_pHero );
	ApplyShadowOption( m_pHero );

//KES 040117
	m_pHero->GetEngineObject()->DisablePick();

	CHARCALCMGR->CalcCharStats(HERO);
}

CPlayer* CObjectManager::AddPlayerThroughCache(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,CHARACTER_TOTALINFO* pTotalInfo, SEND_CHARACTER_TOTALINFO* pMsg)
{
	if(CObject* pPreObj = GetObject(pBaseObjectInfo->dwObjectID))
	{
		RemoveObject(&pPreObj);
	}

	CPlayer* pPlayer = m_PlayerPool.Alloc();
	
	pPlayer->Init(eObjectKind_Player,pBaseObjectInfo);
	pPlayer->InitPlayer(pTotalInfo);
	pPlayer->InitObjectBalloon(ObjectBalloon_Name | ObjectBalloon_Chat | ObjectBalloon_Title | ObjectBalloon_MunpaMark );	

	APPEARANCEMGR->ReserveToAppearObject(pPlayer, (void*)pMsg);

	MOVEMGR->InitMove(pPlayer,pMoveInfo);	

	m_ObjectTable.Add(pPlayer,pPlayer->GetID());

	// 070523 ����. �ٸ� �÷��̾� ���ӽ� ��Ʈ üũ�ϱ� ���� ó��. �̵� ���� ����� �� ȣ��Ǿ�� ��
	pPlayer->OnStartObjectState( eObjectState_Connect );
	pPlayer->SetState( pBaseObjectInfo->ObjectState );
	return pPlayer;
}



CPlayer* CObjectManager::AddPlayer(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,CHARACTER_TOTALINFO* pTotalInfo)
{
	if(CObject* pPreObj = GetObject(pBaseObjectInfo->dwObjectID))
	{
		RemoveObject(&pPreObj);
	}

	CPlayer* pPlayer = m_PlayerPool.Alloc();

	pPlayer->Init(eObjectKind_Player,pBaseObjectInfo);
	pPlayer->InitPlayer(pTotalInfo);
	pPlayer->InitObjectBalloon(ObjectBalloon_Name | ObjectBalloon_Chat | ObjectBalloon_Title | ObjectBalloon_MunpaMark );

	APPEARANCEMGR->InitAppearance(pPlayer);

	MOVEMGR->InitMove(pPlayer,pMoveInfo);	
	OBJECTSTATEMGR->InitObjectState(pPlayer);
	
	m_ObjectTable.Add(pPlayer,pPlayer->GetID());
	
	if( pTotalInfo->HideLevel )
	{
		SetHide( pPlayer->GetID(), pPlayer->GetCharacterTotalInfo()->HideLevel );
	}
	return pPlayer;
}

CMonster* CObjectManager::AddMonster(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,MONSTER_TOTALINFO* pTotalInfo)
{
//	LOGFILE("MP_USERCONN_MONSTER_ADD\t%d\t%s",pBaseObjectInfo->dwObjectID,pBaseObjectInfo->ObjectName);

	CObject* pPreObj = GetObject(pBaseObjectInfo->dwObjectID);
	ASSERT(pPreObj == NULL);
	if(pPreObj)
	{
//		LOGFILE("Overlapped\t%s",OBJECTLOG(pPreObj));
		RemoveObject(&pPreObj);
	}

	const BASE_MONSTER_LIST* const monsterList = GAMERESRCMNGR->GetMonsterListInfo(
		pTotalInfo->MonsterKind);

	if(NULL == monsterList)
	{
		OutputDebug(
			"%s: data is invalid. monsterKind: %d",
			__FUNCTION__,
			pTotalInfo->MonsterKind);
		return NULL;
	}

	CMonster* const pMonster = m_MonsterPool.Alloc();

	if(NULL == pMonster)
	{
		return NULL;
	}

	BOOL IsFieldBoss = false;
	BOOL IsSpecialMonster = false;
	BOOL IsToghterPlayMonster = false;

	switch(monsterList->ObjectKind)
	{
	case eObjectKind_FieldBossMonster: IsFieldBoss = true;           break;
	case eObjectKind_SpecialMonster:   IsSpecialMonster = true;      break;
	case eObjectKind_ToghterPlayMonster:IsToghterPlayMonster = true; break;
	default: break;
	}

	pMonster->Init(
		eObjectKind_Monster,
		pBaseObjectInfo);
	pMonster->InitMonster(pTotalInfo);
	pMonster->InitObjectBalloon(ObjectBalloon_Name | ObjectBalloon_Chat);
#ifdef _GMTOOL_
	pMonster->GetObjectBalloon()->SetNickName(
		pTotalInfo->mScriptName);
#endif

	APPEARANCEMGR->InitAppearance(pMonster);

	MOVEMGR->InitMove(pMonster,pMoveInfo);

	OBJECTSTATEMGR->InitObjectState(pMonster);

	m_ObjectTable.Add(pMonster,pMonster->GetID());

	ApplyOverInfoOption( pMonster );
	ApplyShadowOption( pMonster );

	if(IsFieldBoss) pMonster->SetObjectKind(eObjectKind_FieldBossMonster);
	else if(IsSpecialMonster) pMonster->SetObjectKind(eObjectKind_SpecialMonster);
	else if(IsToghterPlayMonster) pMonster->SetObjectKind(eObjectKind_ToghterPlayMonster);

	return pMonster;
}

CBossMonster* CObjectManager::AddBossMonster(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,MONSTER_TOTALINFO* pTotalInfo)
{
	if(CObject* pPreObj = GetObject(pBaseObjectInfo->dwObjectID))
	{
		RemoveObject(
			&pPreObj);
	}

	CBossMonster* const pBoss = m_BossMonsterPool.Alloc();
	pBoss->Init(eObjectKind_BossMonster, pBaseObjectInfo);
	pBoss->InitMonster(pTotalInfo);
	pBoss->InitObjectBalloon(ObjectBalloon_Name | ObjectBalloon_Chat);

	APPEARANCEMGR->InitAppearance(pBoss);
	MOVEMGR->InitMove(pBoss,pMoveInfo);
	
	OBJECTSTATEMGR->InitObjectState(pBoss);

	m_ObjectTable.Add(pBoss,pBoss->GetID());

	ApplyOverInfoOption( pBoss );
	ApplyShadowOption( pBoss );	
	
	return pBoss;
}


CNpc* CObjectManager::AddNpc(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,NPC_TOTALINFO* pTotalInfo)
{
	CNpc* pNpc = m_NpcPool.Alloc();
	pNpc->Init(eObjectKind_Npc,pBaseObjectInfo);
	pNpc->InitNpc(pTotalInfo);
	pNpc->InitObjectBalloon(ObjectBalloon_Name);
	pNpc->m_MoveInfo.CurPosition.x = 0;
	pNpc->m_MoveInfo.CurPosition.y = 0;
	pNpc->m_MoveInfo.CurPosition.z = 0;

	APPEARANCEMGR->InitAppearance(pNpc);
	MOVEMGR->InitMove(pNpc,pMoveInfo);

	OBJECTSTATEMGR->InitObjectState(pNpc);
	
	m_ObjectTable.Add(pNpc,pNpc->GetID());

	ApplyOverInfoOption( pNpc );
	ApplyShadowOption( pNpc );

	return pNpc;
}

CFurniture* CObjectManager::AddFuniture(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,stFurniture* pFurnitureInfo)
{
	CFurniture* pFurniture = m_FurniturePool.Alloc();
	pFurniture->Init(eObjectKind_Furniture,pBaseObjectInfo);
	pFurniture->InitFurniture(pFurnitureInfo);

	MOVEMGR->InitMove(pFurniture,pMoveInfo);
	APPEARANCEMGR->InitAppearance(pFurniture);
	OBJECTSTATEMGR->InitObjectState(pFurniture);
	m_ObjectTable.Add(pFurniture,pFurniture->GetID());
	
	pFurniture->InitMarkingInfo();
	pFurniture->InitObjectBalloon(ObjectBalloon_Name);

	ApplyOverInfoOption( pFurniture );
	ApplyShadowOption( pFurniture );
	
	return pFurniture;
}

void CObjectManager::AddSkillObject(cSkillObject* pSObj)
{
	CObject* pPreObj = GetObject(pSObj->GetID());
	ASSERT(pPreObj == NULL);
	if(pPreObj)
	{
		RemoveObject(&pPreObj);
	}

	m_ObjectTable.Add(pSObj,pSObj->GetID());
}

void CObjectManager::RemoveAllObject()
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		RemoveObject(&pObject);
	}
	m_ObjectTable.RemoveAll();
	
	SetSelectedObjectID(0);
	m_bAllNameShow		= FALSE;
	
	if( GAMEIN->GetMonsterGuageDlg() )
		GAMEIN->GetMonsterGuageDlg()->SetActive(FALSE);
}

void CObjectManager::RemoveObject(CObject** ppObject)
{
	if(*ppObject)
	{
//		LOGFILE("RemoveObject\t%s",OBJECTLOG(*ppObject));

		m_ObjectTable.Remove((*ppObject)->GetID());

//		if(*ppObject == GetDPObject())
//			SetDPObject(NULL);

		if(m_SelectedObjectID == (*ppObject)->GetID() )
		{
			SetSelectedObjectID(0);
		}

		int kind = (*ppObject)->GetObjectKind();
		(*ppObject)->Release();
		if((*ppObject) == HERO)
		{
			delete (*ppObject);
			m_pHero = NULL;
		}
		else if( (*ppObject) == mpHeroPet )
		{
			delete (*ppObject);
			mpHeroPet = NULL;
		}
		else
		{
			switch(kind)
			{
			case eObjectKind_Player:
				m_PlayerPool.Free((CPlayer*)*ppObject);
				break;
			case eObjectKind_Monster:
			case eObjectKind_FieldBossMonster:
			case eObjectKind_FieldSubMonster:
				m_MonsterPool.Free((CMonster*)*ppObject);
				break;
			case eObjectKind_SpecialMonster:
				m_MonsterPool.Free((CMonster*)*ppObject);
				break;
			case eObjectKind_ToghterPlayMonster:
				m_MonsterPool.Free((CMonster*)*ppObject);
				break;
			case eObjectKind_Npc:
				m_NpcPool.Free((CNpc*)*ppObject);
				break;
			case eObjectKind_SkillObject:
				// DO Nothing
				break;
				
			case eObjectKind_BossMonster:
				m_BossMonsterPool.Free((CBossMonster*)*ppObject);
				break;
			case eObjectKind_MapObject:
				m_MapObjectPool.Free((CMapObject*)*ppObject);
				break;
			// desc_hseos_����ý���_01
			// S ����ý��� �߰� added by hseos 2007.05.07
			case eObjectKind_FarmObj:
				m_csFarmRenderObjPool.Free((CSHFarmRenderObj*)*ppObject);
				break;
			// E ����ý��� �߰� added by hseos 2007.05.07
			case eObjectKind_Pet:
				m_PetPool.Free((CPet*)*ppObject);
				break;
			// 090422 ShinJS --- Ż�� �߰��۾�
			case eObjectKind_Vehicle:
				m_VehiclePool.Free((CVehicle*)*ppObject);
				break;
			//090325 pdy �Ͽ�¡ 
			case eObjectKind_Furniture:
				m_FurniturePool.Free((CFurniture*)*ppObject);
				break;
			default:
				{
				ASSERT(0);
				}
			}
		}
		*ppObject = NULL;		
	}
}

void CObjectManager::AddGarbageObject(CObject* pObject)
{
	RemoveObject(&pObject);
}

CObject* CObjectManager::GetObject(DWORD dwObjectID)
{
	return m_ObjectTable.GetData(dwObjectID);
}

void CObjectManager::GetTargetInRange(VECTOR3* pPivotPos,float Radius,ObjectArray<CObject*>* pObjectArray,BOOL bPK,WORD PARAM)
{
	pObjectArray->Init();
	
	VECTOR3 ObjectPos;
	float dist;
	float dx,dz;

	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		if( pObject->GetObjectKind() == eObjectKind_Npc )
			continue;

		if(pObject->IsDied() == TRUE)
			continue;

		if(pObject->GetObjectKind() == eObjectKind_Monster )
		{
			if( ( ( CMonster* )pObject )->GetSInfo()->SpecialType == 2 )
			{
				continue;
			}
		}

		switch( PARAM )
		{
		case 1:
			{
				if( pObject == HERO )
					continue;

				if( pObject == HEROPET )
					continue;

				if( pObject->GetObjectKind() == eObjectKind_Player )
				{
					CBattle* pBattle = BATTLESYSTEM->GetBattle();
					if( !pBattle->IsEnemy( HERO, pObject ) )
						continue;
				}

				if( pObject->GetObjectKind() == eObjectKind_Pet )
				{
					CPet* pPet = ( CPet* )pObject;
					CPlayer* pPlayer = pPet->GetMaster();

					if( pPlayer )
					{
						CBattle* pBattle = BATTLESYSTEM->GetBattle();
						if( !pBattle->IsEnemy( HERO, pPlayer ) )
							continue;
					}
					else
					{
						continue;
					}
				}
			}
			break;
		case 2:
			{
				if( pObject != HERO && !PARTYMGR->IsPartyMember(pObject->GetID()) )
					continue;
			}
			break;
		case 3:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;

				if( HERO->GetFamilyIdx() != ((CPlayer*)pObject)->GetFamilyIdx() )
					continue;
			}
			break;
		case 4:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;

				if( HERO->GetGuildIdx() != ((CPlayer*)pObject)->GetGuildIdx() )
					continue;
			}
			break;
		case 5:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;
			}
			break;
		case 6:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;

				if( HERO == pObject )
					continue;
			}
			break;


		}

		
		pObject->GetPosition(&ObjectPos); 
		dx = (pPivotPos->x - ObjectPos.x);
		dz = (pPivotPos->z - ObjectPos.z);
		dist = sqrtf(dx*dx + dz*dz) - pObject->GetRadius();
		if(dist <= Radius)
			pObjectArray->AddObject(pObject);
	}
}

DWORD CObjectManager::GetSingleTargetInRange(VECTOR3* pPivotPos,float Radius,BOOL bPK,WORD PARAM)
{
	VECTOR3 ObjectPos;
	float dist;
	float dx,dz;

	m_ObjectTable.SetPositionHead();

	while(CObject* const pObject = m_ObjectTable.GetData())
	{
		if( pObject->GetObjectKind() == eObjectKind_Npc )
			continue;

		if(pObject->IsDied() == TRUE)
			continue;
		
		if(pObject->GetObjectKind() == eObjectKind_Monster )
		{
			if( ( ( CMonster* )pObject )->GetSInfo()->SpecialType == 2 )
			{
				continue;
			}
			else if(CObject* const ownerObject = OBJECTMGR->GetObject(pObject->GetOwnerIndex()))
			{
				if(ownerObject->GetObjectKind() == eObjectKind_Player)
				{
					continue;
				}
			}
		}

		switch( PARAM )
		{
		case 1:
			{
				if( pObject == HERO )
					continue;

				if( pObject == HEROPET )
					continue;

				if( pObject->GetObjectKind() == eObjectKind_Player )
				{
					CBattle* pBattle = BATTLESYSTEM->GetBattle();
					if( !pBattle->IsEnemy( HERO, pObject ) )
						continue;
				}
			}
			break;
		case 2:
			{
				if( pObject != HERO && !PARTYMGR->IsPartyMember(pObject->GetID()) )
					continue;
			}
			break;
		case 3:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;

				if( HERO->GetFamilyIdx() != ((CPlayer*)pObject)->GetFamilyIdx() )
					continue;
			}
			break;
		case 4:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;

				if( HERO->GetGuildIdx() != ((CPlayer*)pObject)->GetGuildIdx() )
					continue;
			}
			break;
		case 5:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;
			}
			break;
		case 6:
			{
				if( pObject->GetObjectKind() != eObjectKind_Player )
					continue;

				if( HERO == pObject )
					continue;
			}
			break;
		}
		
		pObject->GetPosition(&ObjectPos); 
		dx = (pPivotPos->x - ObjectPos.x);
		dz = (pPivotPos->z - ObjectPos.z);
		dist = sqrtf(dx*dx + dz*dz) - pObject->GetRadius();
		if(dist <= Radius)
			return pObject->GetID();
	}

	return 0;
}

CObject* CObjectManager::GetSelectedObject()
{
	return GetObject(
		m_SelectedObjectID);
}

void CObjectManager::SetSelectedEffect(CObject* pObject)
{
	if( pObject )
	{
		if( m_SelectEffectHandle )
		{
			EFFECTMGR->ForcedEndEffect( m_SelectEffectHandle );
		}
		//m_SelectEffectHandle = EFFECTMGR->StartEffectProcess(364,pObject,NULL,0,0);

		TARGETSET set;
		set.pTarget = pObject;
		m_SelectEffectHandle = EFFECTMGR->StartEffectProcess(364,HERO,&set,1,pObject->GetID());
	}
	else
	{
		if( m_SelectEffectHandle )
		{
			EFFECTMGR->ForcedEndEffect( m_SelectEffectHandle );
		}
	}
}

void CObjectManager::SetSelectedObject(CObject* pObject)
{
	CMonsterGuageDlg* pGuageDlg = GAMEIN->GetMonsterGuageDlg() ;

	BOOL rt = SeletedLastObject(pObject, pGuageDlg) ;

	if( rt == FALSE ) return ;
	
	SetSelectedEffect( pObject );

	if( !pObject || !pGuageDlg )
	{
		SetSelectedObjectID(0);
		GAMEIN->GetPartyDialog()->SetClickedMemberID(0);
		return ;
	}

#ifdef _CHEATENABLE_
	CHEATMGR->SetCheatTargetObject(pObject);
#endif

	m_SelectedObjectID = pObject->GetID();
	pObject->OnSelected();

	pGuageDlg->SetName( pObject, pObject->GetObjectName() ) ; 

	switch(pObject->GetObjectKind())
	{
	case eObjectKind_Player :	SelectedPlayer(pObject, pGuageDlg) ;	break ;

	case eObjectKind_Npc :		SelectedNpc(pObject, pGuageDlg) ;		break ;
	
	case eObjectKind_Monster:
	case eObjectKind_BossMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_FieldSubMonster:
		SelectedMonster(pObject, pGuageDlg) ;	break ;
	// desc_hseos_����ý���_01
	// S ����ý��� �߰� added by hseos 2007.08.22
	case eObjectKind_FarmObj :	SelectedFarmObj(pObject, pGuageDlg);	break ;
	// E ����ý��� �߰� added by hseos 2007.08.22
	case eObjectKind_Pet:	SelectedPet(pObject, pGuageDlg) ;	break ;
	// 090422 ShinJS --- Ż�� �߰�
	case eObjectKind_Vehicle:	SelectedVehicle( pObject, pGuageDlg );	break;
	//090508 pdy �Ͽ�¡ �߰� 
	case eObjectKind_Furniture : SelectedFurniture(pObject, pGuageDlg);	break ;
	}
}

BOOL CObjectManager::SeletedLastObject(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	CObject* pLastSelectedObject = GetSelectedObject();
	//Alemuri Party War if Player is in PK mode add the BOOL to the function
	CPlayer* player = (CPlayer*)pObject;
	BOOL isPk = FALSE;
	if (player)
	{
		isPk = player->IsPKMode();
	}
	//-----------------------------------------------------------------------

	if( pLastSelectedObject == pObject ) return FALSE ;

	if( pLastSelectedObject && pLastSelectedObject->IsInited() )
	{
		if(( CanOverInfoChange( pLastSelectedObject ) && !m_bAllNameShow ) ||
			( pLastSelectedObject->GetObjectKind() & eObjectKind_Monster ))
		{
			pLastSelectedObject->ShowObjectName( FALSE );
		}
		else
		{
			pLastSelectedObject->ShowObjectName( TRUE, GetObjectNameColor(pLastSelectedObject), isPk );	//Alemuri Party War the Bool isPk is added to the function
		}
		
		pLastSelectedObject->OnDeselected();
	}

	return TRUE ;
}

void CObjectManager::SelectedPlayer(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	CPlayer* pPlayer = ((CPlayer*)pObject) ;

	pGuageDlg->SetAlpha(255) ;
	pGuageDlg->SetActive( TRUE ) ;
	pGuageDlg->GetMonsterGuage()->SetActive( TRUE ) ;
	
	if( pPlayer->GetGuildIdx() != 0 )
	{
		pGuageDlg->GetGuildStatic()->SetActive( TRUE ) ;
		pGuageDlg->GetUnionStatic()->SetActive( TRUE ) ;

		pGuageDlg->SetGuildName( pPlayer->GetGuildName(), pPlayer->GetGuildUnionName() ) ;
	}
	else
	{
		pGuageDlg->GetGuildStatic()->SetActive( FALSE ) ;
		pGuageDlg->GetUnionStatic()->SetActive( FALSE ) ;
	}
	pGuageDlg->SetGuage( pObject );
}

void CObjectManager::SelectedNpc(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	CNpc* pNpc = ((CNpc*)pObject) ;

	DWORD dwSelectedID = m_SelectedObjectID ;

	pGuageDlg->ResetDlg() ;
//	SetSelectedObjectID(0) ;

	m_SelectedObjectID = dwSelectedID ;

	
	pGuageDlg->GetMonsterGuage()->SetActive( FALSE ) ;

	WORD npcKind = pNpc->GetNpcKind();

	if( npcKind == 30 || npcKind == 31 || npcKind == 0 || pNpc->GetNpcJob() == DUNGEON_WARP_ROLE)
	{
		pGuageDlg->SetActive( FALSE ) ;
	}
	else
	{
		pGuageDlg->SetActive( TRUE ) ;
	}

	pGuageDlg->GetGuildStatic()->SetActive(FALSE) ;
	pGuageDlg->SetGuildName("", "") ;
}

void CObjectManager::SelectedPet(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	DWORD dwSelectedID = m_SelectedObjectID ;

	pGuageDlg->SetActive( TRUE ) ;
	pGuageDlg->ResetDlg() ;
//	SetSelectedObjectID(0) ;

	m_SelectedObjectID = dwSelectedID ;

	pGuageDlg->GetMasterStatic()->SetActive( TRUE );
	pGuageDlg->GetMonsterGuage()->SetActive( FALSE ) ;

	pGuageDlg->GetGuildStatic()->SetActive(FALSE) ;
	pGuageDlg->SetGuildName("", "") ;
}

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.08.22
void CObjectManager::SelectedFarmObj(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
 	// ���� ��ȭâ�� �� ������ ����~
   	if(g_csFarmManager.CLI_GetTargetObj())
	{
		if(TRUE == GAMEIN->GetNpcScriptDialog()->IsActive())
		{
			return;
		}
		else if(TRUE == GAMEIN->GetAnimalDialog()->IsActive())
		{
			return;
		}
	}

	DWORD dwSelectedID = m_SelectedObjectID ;

	pGuageDlg->ResetDlg() ;
//	SetSelectedObjectID(0) ;

	m_SelectedObjectID = dwSelectedID ;
	
//	pGuageDlg->GetMonsterGuage()->SetActive( FALSE ) ;
	pGuageDlg->SetActive( TRUE ) ;

	pGuageDlg->GetGuildStatic()->SetActive(FALSE) ;
	pGuageDlg->SetGuildName("", "") ;

	g_csFarmManager.CLI_SetTargetObj((CSHFarmRenderObj*)pObject);
}
// E ����ý��� �߰� added by hseos 2007.08.22

void CObjectManager::SelectedMonster(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	pGuageDlg->ResetDlg() ;
	pGuageDlg->SetActive( TRUE ) ;

	pGuageDlg->GetHPStatic()->SetActive( TRUE ) ;
	pGuageDlg->GetMonsterGuage()->SetActive( TRUE ) ;
	pGuageDlg->SetGuage( pObject ) ;
	pGuageDlg->GetGuildStatic()->SetActive(FALSE) ;
}

void CObjectManager::SelectedVehicle(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	pGuageDlg->ResetDlg() ;
	pGuageDlg->SetActive( TRUE ) ;
	pGuageDlg->GetHPStatic()->SetActive( FALSE ) ;
	pGuageDlg->GetMonsterGuage()->SetActive( FALSE ) ;
	pGuageDlg->GetGuildStatic()->SetActive(FALSE) ;

	// ShinJS 091105 --- �������� �̸� ���
	pGuageDlg->SetGuildName( (char*)((CVehicle*)pObject)->GetMasterName(), "" );
}

void CObjectManager::SelectedFurniture(CObject* pObject, CMonsterGuageDlg* pGuageDlg)
{
	DWORD dwSelectedID = m_SelectedObjectID ;
	pGuageDlg->ResetDlg() ;
	m_SelectedObjectID = dwSelectedID ;
	pGuageDlg->GetMonsterGuage()->SetActive( FALSE ) ;
	pGuageDlg->SetActive( TRUE ) ;

	pGuageDlg->GetGuildStatic()->SetActive(FALSE) ;
	pGuageDlg->SetGuildName("", "") ;
}

void CObjectManager::SetOverInfoOption( DWORD dwOption )
{
	m_dwOverInfoOption = dwOption;
	ApplyOverInfoOptionToAll();
}

void CObjectManager::ApplyOverInfoOptionToAll()
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		ApplyOverInfoOption( pObject );
		//Alemuri Stealth Visible for Party Members---------------------------------------------
		CPlayer* playerObject = (CPlayer*)pObject;
		if (playerObject)
			SetHide(playerObject->GetID(), playerObject->GetCharacterTotalInfo()->HideLevel);
		//--------------------------------------------------------------------------------------
	}
}

void CObjectManager::ApplyOverInfoOption( CObject* pObject )
{
	if( pObject == NULL ) return;																	// ���� ������Ʈ ������ ��ȿ���� üũ�Ѵ�.

	pObject->SetOverInfoOption( 1 );																// �̸�, ä��, ���� ������ Ȱ��ȭ ��Ų��.

	CBattle* pBattle = BATTLESYSTEM->GetBattle();

	if( !pBattle )		return;

	if( pObject->GetObjectKind() == eObjectKind_Player )											// ������Ʈ Ÿ���� �÷��̾� �̸�,
	{
		// 090210 LYW --- ObjectManager : ���� ��ƾ ó������ �ʿ��� ���� ������Ʈ �����͸� �޴´�.
		CEngineObject* pEngineObj = pObject->GetEngineObject() ;
		const CHARACTER_TOTALINFO* pInfo = ((CPlayer*)pObject)->GetCharacterTotalInfo();
		if( !pEngineObj )	return;
		if( !pInfo )		return;

		// 090116 ShinJS --- ĳ���� �Ⱥ��̱� �ɼ� ����
		if( pObject != HERO && MAINGAME->GetCurStateNum() == eGAMESTATE_GAMEIN )
		{
			// ĳ���� �Ⱥ��̱�
			if( m_dwOverInfoOption & eOO_HIDE_PLAYER )
			{
				// ���� �ƴ� ��� �Ⱥ��̱�
				if( !pBattle->IsEnemy( HERO, pObject ) )
				{
					// ĳ���͸� �����
					pEngineObj->HideWithScheduling() ;

					// �׸��� ���̱� �ɼ� üũ�Ͽ� �׸��� �����ֱ�
					if( m_dwShadowOption & eOO_SHADOW_OTHERCHARACTER )
						AddShadow( pObject );
				}
				// ���� ��� ���̱�
				else
				{
					// 090213 ShinJS --- ĳ������ HIDE ������ ���� ���̱� ���θ� �����Ѵ�
					// ĳ���� ������ HIDE�� �ƴ� ���
					if( pInfo->bVisible == TRUE )
					{
						// ĳ���͸� �����ش�
						pEngineObj->Show() ;
					}
					// ĳ���� ������ HIDE�� ���
					else
					{
						const CHARACTER_TOTALINFO* pHeroInfo = HERO->GetCharacterTotalInfo();

						// ���ڽ��� �����Ҽ� �ִ°�� ���ĸ� �����Ͽ� �����ش�
						if( pHeroInfo )
						if( pInfo->HideLevel <= pHeroInfo->DetectLevel )
						{
							pEngineObj->Show();
							pEngineObj->SetAlpha( 0.3f );
						}
					}
				}

				// 090210 LYW --- ObjectManager : ���� ���� ����Ʈ(105�� �̻� ����� ����)�� Ȱ��ȭ ���̸� �Ⱥ��̰� �Ѵ�.
				((CPlayer*)pObject)->EndJobEffect() ;
				HERO->EndMarryEffect() ;
				
			}
			// ĳ���� ���̱�
			else
			{
				// ���� �ƴ� ��쿡 �����ֱ�(���� ���� �Ⱥ��̱� �ɼ� �߿��� �������� ����)
				if( !pBattle->IsEnemy( HERO, pObject ) )
				{
					// 090213 ShinJS --- ĳ������ HIDE ������ ���� ���̱� ���θ� �����Ѵ�
					// ĳ���� ������ HIDE�� �ƴ� ���
					if( pInfo->bVisible == TRUE )
					{
						pEngineObj->Show() ;
					}
					// ĳ���� ������ HIDE�� ���
					else
					{
                        const CHARACTER_TOTALINFO* pHeroInfo = HERO->GetCharacterTotalInfo();

						// ���ڽ��� �����Ҽ� �ִ°�� ���ĸ� �����Ͽ� �����ش�
						if( pHeroInfo )
						if( pInfo->HideLevel <= pHeroInfo->DetectLevel )
						{
							pEngineObj->Show();
							pEngineObj->SetAlpha( 0.3f );
						}
					}
				}

				// 090210 LYW --- ObjectManager : ���� ���� ����Ʈ(105�� �̻� ����� ����)�� ������ ���̰� �Ѵ�.
				((CPlayer*)pObject)->StartJobEffect() ;
			}

			//090116 pdy Ŭ���̾�Ʈ ����ȭ �۾� (Hide ����Title )
			if( !IsOverInfoOption(eOO_HIDE_STREETSTALLTITLE) || m_bPress_ShowNameKey )
			{
				pObject->HideStreetStallTitle( FALSE );						// �������� Ÿ��Ʋ ����⸦ ��Ȱ��ȭ �Ѵ�.
			}

			else 	
			{
				pObject->HideStreetStallTitle( TRUE );	
			}
			// 090116 pdy END
		}

#ifndef _GMTOOL_																					// GM ������ üũ�Ѵ�.

		if( pInfo->bVisible == FALSE )																// �÷��̾��� ĳ���� ������ HIDE �̸�,
		{
			pObject->ShowObjectName( FALSE );														// �̸��� ��Ȱ��ȭ ��Ų��.

			return;																					// ���� ó���� �Ѵ�.
		}
#endif																								// GM �� ó�� ����.

		if( GUILDWARMGR->IsEnemy( (CPlayer*)pObject ) )											// ��� �ʵ� ������ ���̸�,
		{
			pObject->ShowObjectName( TRUE, NAMECOLOR_PKMODE, ((CPlayer*)pObject)->IsPKMode() );										// PK���� �̸��� Ȱ��ȭ �Ѵ�.

			return;																					// ���� ó���� �Ѵ�.
		}
		if(STREETTOURNAMENTMGR->isStreetTournament())
		{
			if(STREETTOURNAMENTMGR->GetState() == 3 && pObject->GetID() != HEROID)
			//if(STREETTOURNAMENTMGR->GetState() == 3)
			{
				pObject->GetObjectBalloon()->SetName("Enemy");
				pObject->ShowObjectName( TRUE, NAMECOLOR_PKMODE, ((CPlayer*)pObject)->IsPKMode() );
				return;
			}

			if(STREETTOURNAMENTMGR->GetState() == 4)
			{
				char nickplayer[MAX_NAME_LENGTH+1] = "";
				SafeStrCpy(nickplayer, pObject->GetObjectName(), MAX_NAME_LENGTH+1);
				pObject->GetObjectBalloon()->SetName(nickplayer);
				return;
			}
		}

	}
	// 090116 ShinJS --- �� �Ⱥ��̱� �ɼ� ����
	else if( pObject->GetObjectKind() == eObjectKind_Pet )
	{
		// 090210 LYW --- ObjectManager : ���� ��ƾ ó������ �ʿ��� ���� ������Ʈ �����͸� �޴´�.
		CEngineObject* pEngineObj = pObject->GetEngineObject() ;

		// �� �Ⱥ��̱�
		if( pObject != HEROPET && m_dwOverInfoOption & eOO_HIDE_PET )
		{
			// ���� �ƴ� ��� �� �Ⱥ��̱�(���� �׸��ڰ� ����?)
			if( !pBattle->IsEnemy( HERO, pObject ) )
			{
				pEngineObj->HideWithScheduling() ;
			}
			// ���� ���
			else
			{
				pEngineObj->Show() ;
			}
		}
		// �� ���̱�
		else if( pObject != HEROPET )
		{
			// ���� �ƴ� ��쿡�� �����ش�(���� ��� �������� �������� �ִ�)
			if( !pBattle->IsEnemy( HERO, pObject ) )
			{
				pEngineObj->Show() ;
			}
		}
	}


	if( GetSelectedObject() != pObject )															// ���õ� ������Ʈ ������ ���� ������Ʈ ������ ���� ������,
	{
		if( pObject->GetObjectKind() == eObjectKind_Player )										// ������Ʈ Ÿ���� �÷��̾�� ������,
		{
			if(STREETTOURNAMENTMGR->isStreetTournament())
			{
				//if(STREETTOURNAMENTMGR->GetState() == 3)
				if(STREETTOURNAMENTMGR->GetState() == 3 && pObject->GetID() != HEROID)
				{
					pObject->GetObjectBalloon()->SetName("Enemy");
					pObject->ShowObjectName( TRUE, NAMECOLOR_PKMODE, ((CPlayer*)pObject)->IsPKMode() );
					return;
				}
				if(STREETTOURNAMENTMGR->GetState() == 4)
				{
					char nickplayer[MAX_NAME_LENGTH+1] = "";
					SafeStrCpy(nickplayer, pObject->GetObjectName(), MAX_NAME_LENGTH+1);
					pObject->GetObjectBalloon()->SetName(nickplayer);
					return;
				}
			}

			if( ((CPlayer*)pObject)->IsPKMode() && MAP->GetMapNum()==PVP)							//PVP Color
			{
				pObject->ShowObjectName( TRUE, NAMECOLOR_PVPMODE, ((CPlayer*)pObject)->IsPKMode() );									

				return;																				
			}
			if( ((CPlayer*)pObject)->IsPKMode() )
			{
				pObject->ShowObjectName( TRUE, NAMECOLOR_PKMODE, ((CPlayer*)pObject)->IsPKMode() );									// �������� �̸��� Ȱ��ȭ �Ѵ�.

				return;																			// ���� ó���� �Ѵ�.
			}

			if( m_dwOverInfoOption & eOO_NAME_PARTY || m_bAllNameShow )								// ��Ƽ�� �̸�ǥ�� �ɼ��̰ų�, �ٸ� ĳ���� �̸� ǥ�� ����̸�,
			{
				if( PARTYMGR->IsPartyMember( pObject->GetID() ) )									// �÷��̾ ��Ƽ���̶��,
				{
					pObject->ShowObjectName( TRUE, NAMECOLOR_PARTY, ((CPlayer*)pObject)->IsPKMode() );								// ��Ƽ �̸����� Ȱ��ȭ �Ѵ�.
					
					return;																			// ���� ó���� �Ѵ�.
				}
			}
			if( m_dwOverInfoOption & eOO_NAME_MUNPA || m_bAllNameShow )								// ���� �̸�ǥ�� �ɼ��̰ų�, �ٸ� ĳ���� �̸� ǥ�� ����̸�,
			{
				if( HERO )																			// ����� ������ ��ȿ���� üũ�Ѵ�.
				{
					if( HERO->GetGuildIdx() != 0 )													// ������� ��� �ε����� ��ȿ���� üũ�Ѵ�.
					{
						if( HERO->GetGuildIdx() == ((CPlayer*)pObject)->GetGuildIdx())				// �÷��̾��� ����ε����� ������� ��� �ε����� ���ٸ�,
						{
							pObject->ShowObjectName( TRUE, NAMECOLOR_MUNPA, ((CPlayer*)pObject)->IsPKMode() );						// ��� �̸����� Ȱ��ȭ �Ѵ�.

							if(((CPlayer*)pObject)->GetGuildMarkName() > 0)							// ��� ��ũ���� �ִٸ�,
							{
								pObject->SetGuildMark();											// ��� ��ũŬ �����Ѵ�.
							}

							return;																	// ���� ó���� �Ѵ�.
						}
					}
				}
			}

			if( ( m_dwOverInfoOption & eOO_NAME_WHOLE ) || m_bAllNameShow )							// �ٸ� ĳ���� �̸�ǥ�� �ɼ��̰ų�, �̸��� ��� ǥ�� �����̸�,
			{
				pObject->ShowObjectName( TRUE );													// �÷��̾��� �̸��� Ȱ��ȭ �Ѵ�.

				return;																				// ���� ó���� �Ѵ�.
			}

			//090116 pdy ������Ÿ��Ʋ ����� �۾� || m_bPress_ShowNameKey �߰�  
			if( ( m_dwOverInfoOption & eOO_NAME_WHOLE ) || m_bAllNameShow || m_bPress_ShowNameKey )	// �ٸ� ĳ���� �̸�ǥ�� �ɼ��̰ų�, �̸��� ��� ǥ�� �����̸�,
			{
				pObject->ShowObjectName( TRUE );													// �÷��̾��� �̸��� Ȱ��ȭ �Ѵ�.

				return;																				// ���� ó���� �Ѵ�.
			}
			
			pObject->ShowObjectName( FALSE );														// ������Ʈ�� �̸� ���̱� ���θ� FALSE�� �����Ѵ�.
		}
		else if( pObject->GetObjectKind() == eObjectKind_Npc )										// ������Ʈ�� NPC�� ������,
		{
			if( m_bAllNameShow )																	// �̸��� �����ִ� ����̸�,
			{
				pObject->ShowObjectName( TRUE );													// ������Ʈ �̸��� Ȱ��ȭ �Ѵ�.
			}
			else																					// �̸��� �����ִ� ��尡 �ƴϸ�,
			{
				pObject->ShowObjectName( FALSE );													// ������Ʈ �̸��� ��Ȱ��ȭ �Ѵ�.
			}
		}
	}
	else																							// ���õ� ������Ʈ�� ���� ������Ʈ ������ ������,
	{
		pObject->ShowObjectName( TRUE, NAMECOLOR_SELECTED, ((CPlayer*)pObject)->IsPKMode() );										// ���õ� ���� �̸��� �����Ѵ�.
	}

	//091020 pdy ApplyOverInfoOption�� �߸��� ĳ�������� �̸��� Ȱ��ȭ�Ǵ� ���� ����
	if( pObject->GetObjectKind() == eObjectKind_Npc )
	{
		CNpc* const pNpc = ( CNpc* )pObject;

		if( pNpc->GetNpcKind() == 31 || pNpc->GetNpcJob() == MAP_MOVE_ROLE )
		{
			pObject->ShowObjectName( TRUE );
		}
	}
}

void CObjectManager::SetAllNameShow( BOOL bShow )
{ 
	if( m_bAllNameShow == bShow )
		return;

	m_bAllNameShow = bShow;
	
//	if( !( m_dwOverInfoOption & eOO_NAME_WHOLE ) )
	ApplyOverInfoOptionToAll();
}

//090116 pdy Ŭ���̾�Ʈ ����ȭ �۾� (ShowName On Press Key ) 
void CObjectManager::SetPress_ShowNameKey( BOOL bPressKey )
{ 
	if( m_bPress_ShowNameKey == bPressKey )
		return;

	m_bPress_ShowNameKey = bPressKey;
	
	ApplyOverInfoOptionToAll();
}

void CObjectManager::OnMouseOver( CObject* pObject )
{
	if( pObject->GetID() != m_SelectedObjectID )
	if( !m_bAllNameShow || pObject->GetObjectKind() & eObjectKind_Monster )
	if( CanOverInfoChange( pObject ) )
	{
		//if(	!(pObject->GetObjectKind() & eObjectKind_Npc) )
			pObject->ShowObjectName( TRUE );
	}
}

void CObjectManager::OnMouseLeave( CObject* pObject )
{
	if( pObject->GetID() != m_SelectedObjectID )
	if( !m_bAllNameShow || pObject->GetObjectKind() & eObjectKind_Monster )
	if( CanOverInfoChange( pObject ) )
	{
		//if(	!(pObject->GetObjectKind() & eObjectKind_Npc) )
			pObject->ShowObjectName( FALSE );
	}
}

BOOL CObjectManager::CanOverInfoChange( CObject* pObject )
{
	if( pObject->GetObjectKind() == eObjectKind_Player )
	{
		if( ((CPlayer*)pObject)->IsPKMode() )		return FALSE;	
		if( m_dwOverInfoOption & eOO_NAME_WHOLE ||
			m_dwOverInfoOption & m_bPress_ShowNameKey)						//090116 pdy Ŭ���̾�Ʈ ����ȭ �۾� ( Ű���������� �ٲ��� �ʴ´�)
			return FALSE;


		if( m_dwOverInfoOption & eOO_NAME_PARTY )
		if( PARTYMGR->IsPartyMember( pObject->GetID() ) )
			return FALSE;

		if( m_dwOverInfoOption & eOO_NAME_MUNPA )
		if( HERO )
		if( HERO->GetGuildIdx() == ((CPlayer*)pObject)->GetGuildIdx() && ((CPlayer*)pObject)->GetGuildMemberRank())
			return FALSE;

		// guildfieldwar
		if( GUILDWARMGR->IsEnemy( (CPlayer*)pObject ) )
			return FALSE;
	}

	return TRUE;
}

DWORD CObjectManager::GetObjectNameColor( CObject* pObject )
{
	if( pObject->GetObjectKind() == eObjectKind_Player )
	{
		if( strncmp( pObject->GetObjectName(), "GM", 2 ) == 0 )
			return NAMECOLOR_GM;
		//aziz PVP name Color //battle arena
		if( ((CPlayer*)pObject)->IsPKMode() && MAP->GetMapNum()==PVP) //PVP Name Color
			return NAMECOLOR_PVPMODE;
		if(STREETTOURNAMENTMGR->isStreetTournament())
		{
			if(STREETTOURNAMENTMGR->GetState() == 3 && pObject->GetID() != HEROID)
			//if(STREETTOURNAMENTMGR->GetState() == 3)
			{
				pObject->GetObjectBalloon()->SetName("Enemy");
				pObject->ShowObjectName( TRUE, NAMECOLOR_PKMODE );
				return NAMECOLOR_PKMODE;
			}

			if(STREETTOURNAMENTMGR->GetState() == 4)
			{
				char nickplayer[MAX_NAME_LENGTH+1] = "";
				SafeStrCpy(nickplayer, pObject->GetObjectName(), MAX_NAME_LENGTH+1);
				pObject->GetObjectBalloon()->SetName(nickplayer);
			}
		}
		if( ((CPlayer*)pObject)->IsPKMode() )
			return NAMECOLOR_PKMODE;

		if( m_dwOverInfoOption & eOO_NAME_PARTY )
		if( PARTYMGR->IsPartyMember( pObject->GetID() ) )
			return NAMECOLOR_PARTY;

		if( m_dwOverInfoOption & eOO_NAME_MUNPA )
		if( HERO )
		if( HERO->GetGuildIdx() )
		if( HERO->GetGuildIdx() == ((CPlayer*)pObject)->GetGuildIdx())
			return NAMECOLOR_MUNPA;	

		if( GUILDWARMGR->IsEnemy( (CPlayer*)pObject ) )
			return NAMECOLOR_PKMODE;

		return NAMECOLOR_PLAYER;
	}
	/*
	else if( pObject->GetObjectKind() == eObjectKind_Npc )
	{
		return NAMECOLOR_NPC;
	}
	*/

	return NAMECOLOR_DEFAULT;
}

void CObjectManager::SetLodMode( DWORD dwOption )
{
	if (m_dwLodMode == dwOption)
		return;

	m_dwLodMode = dwOption;
	ApplyPlayerLodModeToAll();
}

void CObjectManager::ApplyPlayerLodModeToAll()
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		if ( pObject->GetObjectKind() == eObjectKind_Player )
			 //pObject->GetObjectKind() == eObjectKind_Npc
			 //pObject->GetObjectKind() == eObjectKind_Monster
		{
			APPEARANCEMGR->InitAppearance(pObject);
		}
	}
}

void CObjectManager::SetShadowOption( DWORD dwOption )
{
	m_dwShadowOption = dwOption;
	ApplyShadowOptionToAll();
}

void CObjectManager::ApplyShadowOptionToAll()
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		ApplyShadowOption( pObject );
	}
}

void CObjectManager::ApplyShadowOption( CObject* pObject )
{
	// 070430 LYW --- ObjectManager : Deactive shadow to gate npc.
	if( ((CNpc*)pObject)->GetNpcKind() == 31 ||
		((CNpc*)pObject)->GetNpcJob() == DUNGEON_WARP_ROLE)
	{
		RemoveShadow( pObject );
		return ;
	}

	if( pObject == NULL ) return;

//Shadow on/off
	//�ӽ÷� ���� �ֵ���(��������)

	//
	if( pObject->GetObjectKind() == eObjectKind_Player )
	{
		//090603 pdy �Ͽ�¡ ž�¹��� ���� (ž�½ÿ� �׸��ڸ� ���ش�)
		if( ((CPlayer*)pObject)->IsRideFurniture()  )
		{
			RemoveShadow( pObject );
			return ;
		}
	}

//Shadow show/hide
	if( m_dwShadowOption & eOO_SHADOW_OTHERCHARACTER )
	{
		if( pObject->GetObjectKind() == eObjectKind_Player )
		if( pObject != HERO )
		{
			if(((CPlayer*)pObject)->m_CharacterInfo.bVisible == FALSE)
			{
				RemoveShadow(pObject);
				return;

			}
			AddShadow( pObject );
			return;
		}
	}

	if( m_dwShadowOption & eOO_SHADOW_ONLYHERO  )
	{
		if( pObject == HERO )
		{
			AddShadow( pObject );
			return;
		}
	}

	if( m_dwShadowOption & eOO_SHADOW_MONSTER )
	{
		if( ( pObject->GetObjectKind() & eObjectKind_Monster ) || ( pObject->GetObjectKind() == eObjectKind_Npc ) )
		{
#ifdef _GMTOOL_
			if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM  && CAMERA->GetCameraMode() != eCM_EyeView )
			{
				if( 1.0f > pObject->GetEngineObject()->GetAlpha() )
				{
					RemoveShadow( pObject );
				}
				else
				{
					AddShadow( pObject );
				}
			}
			else
#endif

			// 080305 ggomgrak --- Visible�� üũ�ϸ� �ø��� ������Ʈ�� �ݿ��� �ȵǴ� ���װ� �����Ƿ� �ּ�ó����.
			//if( pObject->GetEngineObject()->IsVisible() )
			{
				AddShadow( pObject );
			}
			//else
			//{
			//	RemoveShadow( pObject );
			//}

			return;
		}

	}

	RemoveShadow( pObject );
}

void CObjectManager::ShowChatBalloon( CObject* pObject, char* pStrChat, DWORD dwColor, DWORD dwAliveTime )
{
	if( m_dwOverInfoOption & eOO_CHAT_NOBALLOON )
		return;

	if( !WINDOWMGR->IsOpendAllWindows() )
		return;

	if( pObject )
		pObject->ShowChatBalloon( TRUE, pStrChat, dwColor, dwAliveTime );
}

void CObjectManager::HideChatBalloonAll()
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		pObject->ShowChatBalloon( FALSE, NULL );
	}
}

void CObjectManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol) 
	{
	case MP_BOSS_REST_START_NOTIFY:
		{
			MSG_DWORD* pmsg = (MSG_DWORD *)pMsg;
			CMonster* const monsterObject = (CMonster*)GetObject(
				pmsg->dwData);

			if(0 == monsterObject)
			{
				break;
			}
			else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
			{
				break;
			}

			OBJECTSTATEMGR->StartObjectState(
				monsterObject,
				eObjectState_Rest);

			DWORD effectNumber = 0;

			switch(monsterObject->GetMonsterKind())
			{
			case eBOSSKIND_TARINTUS:
				{
					effectNumber = FindEffectNum(
						"Boss_Tarintus_Recover.beff");
					break;
				}
			case eBOSSKIND_ARACH:
				{
					effectNumber = FindEffectNum(
						"Boss_Arach_Recovery.beff");
					break;
				}
			case eBOSSKIND_LEOSTEIN:
				{
					effectNumber = FindEffectNum(
						"BOSS_LeoStein_Recover.beff");
					break;
				}
			case eBOSSKIND_DRAGONIAN:
				{
					effectNumber = 9506;
					break;
				}
			}

			mRestHandleContainer[monsterObject->GetMonsterKind()] = EFFECTMGR->StartEffectProcess(
				effectNumber,
				monsterObject,
				NULL,
				0,
				monsterObject->GetID());
		}	
		break;
	case MP_BOSS_RECALL_NOTIFY:
		{
			MSG_DWORD * pmsg = (MSG_DWORD *)pMsg;
			CMonster* const monsterObject = (CMonster*)GetObject(
				pmsg->dwData);

			if(0 == monsterObject)
			{
				break;
			}
			else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
			{
				break;
			}

			OBJECTSTATEMGR->StartObjectState(
				monsterObject,
				eObjectState_Rest);

			int effectNumber = 0;

			switch(monsterObject->GetMonsterKind())
			{
			case eBOSSKIND_TARINTUS:
				{
					effectNumber = FindEffectNum(
						"BOSS_Tarintus_Summon.beff");
					break;
				}
			case eBOSSKIND_KIERRA:
				{
					effectNumber = FindEffectNum(
						"BOSS_Kierra_Summon.beff");
					break;
				}
			case eBOSSKIND_ARACH:
				{
					effectNumber = FindEffectNum(
						"BOSS_Arach_Summon.beff");
					break;
				}
			case eBOSSKIND_LEOSTEIN:
				{
					effectNumber = FindEffectNum(
						"BOSS_LeoStein_Summon.beff");
					break;
				}
			case eBOSSKIND_DRAGONIAN:
				{
					effectNumber = 9508;
					break;
				}
			case eBOSSKIND_REBORN:
				{
					effectNumber = FindEffectNum(
						"BOSS_Arach_Summon.beff");
					break;
				}
			}

			mRestHandleContainer[monsterObject->GetMonsterKind()] = EFFECTMGR->StartEffectProcess(
				effectNumber,
				monsterObject,
				NULL,
				0,
				monsterObject->GetID());
		}
		break;

		case MP_BOSS_LIFE_NOTIFY:
			{
				MSG_DWORD2 * pmsg = (MSG_DWORD2 *)pMsg;

				if(CObject* const object = GetObject(pmsg->dwData2))
				{
					object->SetLife(
						pmsg->dwData1);
				}
			}
		break;
		case MP_BOSS_STAND_END_NOTIFY:
			{
				MSG_DWORD * pmsg = (MSG_DWORD *)pMsg;
				CMonster* const monsterObject = (CMonster*)GetObject(
					pmsg->dwData);

				if(0 == monsterObject)
				{
					break;
				}
				else if(FALSE == (eObjectKind_Monster & monsterObject->GetObjectKind()))
				{
					break;
				}
				else if(mRestHandleContainer.end() == mRestHandleContainer.find(monsterObject->GetMonsterKind()))
				{
					break;
				}

				HEFFPROC effectProcedure = mRestHandleContainer[monsterObject->GetMonsterKind()];
				mRestHandleContainer.erase(
					monsterObject->GetMonsterKind());

				EFFECTMGR->ForcedEndEffect(
					effectProcedure);
			}
			break;

		// �ʵ庸�� - 05.12 �̿���
		// �ʵ庸�� �� ���� ����� ȸ��
		case MP_FIELD_LIFE_NOTIFY:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				CMonster* pMonster = (CMonster*)GetObject(pmsg->dwData3);

				if(!pMonster)
					return;

				if( pMonster->GetMonsterKind() != pmsg->dwData2 )
					return;

				pMonster->SetLife( pmsg->dwData1 );
			}
			break;
		break;
	}
}

void CObjectManager::AllPlayerPKOff()
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		if( pObject->GetObjectKind() != eObjectKind_Player ) continue;
		CPlayer* pPlayer = (CPlayer*)pObject;
		if( pPlayer->IsPKMode() == FALSE ) continue;

		pPlayer->SetPKMode( FALSE );

		if( HERO )
		if( pPlayer != HERO )
		{
			HERO->ClearTarget( pPlayer );
		}
	
		pPlayer->RemoveObjectEffect( PK_EFFECT_ID );
	}
	//ĳ���� �������� �ٲٱ�
	ApplyOverInfoOptionToAll();
}


void CObjectManager::SetSelectedObjectID(DWORD dwId) 
{ 
	m_SelectedObjectID = dwId;
	SetSelectedEffect( NULL );
	if(m_SelectedObjectID == 0)
	{		
		if(GAMEIN->GetMonsterGuageDlg())
			GAMEIN->GetMonsterGuageDlg()->SetActive(FALSE);
	}
}

CMapObject* CObjectManager::AddMapObject(BASEOBJECT_INFO* pBaseObjectInfo, BASEMOVE_INFO* pMoveInfo, char* strData, float fScale, float fAngle )
{
	CObject* pPreObj = GetObject( pBaseObjectInfo->dwObjectID );
	if( pPreObj )
	{
		RemoveObject( &pPreObj );
	}

	CMapObject* pMapObject = m_MapObjectPool.Alloc();
	pMapObject->Init( eObjectKind_MapObject, pBaseObjectInfo );

	pMapObject->m_RotateInfo.Angle.SetAngleDeg( fAngle );
	APPEARANCEMGR->SetMapObjectAppearance( pMapObject, strData, fScale );
	MOVEMGR->InitMove( pMapObject, pMoveInfo, fAngle );	
	OBJECTSTATEMGR->InitObjectState( pMapObject );

	m_ObjectTable.Add( pMapObject, pMapObject->GetID() );

	ApplyOverInfoOption( pMapObject );
	ApplyShadowOption( pMapObject );

	return pMapObject;
}

// desc_hseos_����ý���_01
// S ����ý��� �߰� added by hseos 2007.05.07
CSHFarmRenderObj* CObjectManager::AddFarmRenderObj( BASEOBJECT_INFO* pBaseObjectInfo, BASEMOVE_INFO* pMoveInfo, CSHFarmRenderObj::stFarmRenderObjInfo* pFarmRenderObjInfo)
{
	CObject* pPreObj = GetObject(pBaseObjectInfo->dwObjectID);
	ASSERT(pPreObj == NULL);
	if(pPreObj)
	{
		RemoveObject(&pPreObj);
	}

	CSHFarmRenderObj* pFarmRenderObj = m_csFarmRenderObjPool.Alloc();

	pFarmRenderObj->Init(eObjectKind_FarmObj, pBaseObjectInfo, pFarmRenderObjInfo);
	pFarmRenderObj->InitObjectBalloon(ObjectBalloon_Name);
	pFarmRenderObj->m_MoveInfo.CurPosition.x = 0;
	pFarmRenderObj->m_MoveInfo.CurPosition.y = 0;
	pFarmRenderObj->m_MoveInfo.CurPosition.z = 0;

	APPEARANCEMGR->InitAppearance(pFarmRenderObj);

	pMoveInfo->SetMaxTargetPosIdx(0);
	MOVEMGR->InitMove(pFarmRenderObj, pMoveInfo);
	OBJECTSTATEMGR->InitObjectState(pFarmRenderObj);

	m_ObjectTable.Add(pFarmRenderObj, pFarmRenderObj->GetID());

	ApplyOverInfoOption(pFarmRenderObj);
	ApplyShadowOption(pFarmRenderObj);

	return pFarmRenderObj;
}
// E ����ý��� �߰� added by hseos 2007.05.07

CPet* CObjectManager::AddPet(DWORD MasterIdx, BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,PET_OBJECT_INFO* pPetObjectInfo, ITEMBASE* pItemInfo)
{
	CObject* pPreObj = GetObject( pBaseObjectInfo->dwObjectID );
	ASSERT( pPreObj == NULL );
	if( pPreObj )
	{
		RemoveObject( &pPreObj );
	}

	CPet* pPet = m_PetPool.Alloc();

	pPet->Init( eObjectKind_Pet, pBaseObjectInfo );
	pPet->InitPet( pPetObjectInfo );
	pPet->InitObjectBalloon( ObjectBalloon_Name | ObjectBalloon_Chat );
	pPet->SetWearedItem( pItemInfo );
	pPet->SetMaster( MasterIdx );
	
	APPEARANCEMGR->InitAppearance( pPet );

	MOVEMGR->InitMove( pPet,pMoveInfo );

	OBJECTSTATEMGR->InitObjectState( pPet );

	m_ObjectTable.Add(pPet,pPet->GetID());

	ApplyOverInfoOption( pPet );
	ApplyShadowOption( pPet );

	return pPet;
}

CHeroPet* CObjectManager::AddHeroPet(BASEOBJECT_INFO* pBaseObjectInfo,BASEMOVE_INFO* pMoveInfo,PET_OBJECT_INFO* pPetObjectInfo, ITEMBASE* pItemInfo)
{
	// 091026 LUJ, �޸𸮸� ������ �� ����ϵ��� �Ѵ�
	SAFE_DELETE(mpHeroPet);
	mpHeroPet = new CHeroPet;
	mpHeroPetBackup = NULL;

	mpHeroPet->Init( eObjectKind_Pet, pBaseObjectInfo );
	mpHeroPet->InitPet( pPetObjectInfo );
	mpHeroPet->InitObjectBalloon( ObjectBalloon_Name | ObjectBalloon_Chat );
	mpHeroPet->SetWearedItem( pItemInfo );
	mpHeroPet->SetMaster( HEROID );
	
	APPEARANCEMGR->InitAppearance( mpHeroPet );

	MOVEMGR->InitMove( mpHeroPet, pMoveInfo );

	OBJECTSTATEMGR->InitObjectState( mpHeroPet );

	m_ObjectTable.Add( mpHeroPet, mpHeroPet->GetID() );

	ApplyOverInfoOption( mpHeroPet );
	ApplyShadowOption( mpHeroPet );

	return mpHeroPet;
}

void CObjectManager::RemoveHeroPet()
{
	m_ObjectTable.Remove( mpHeroPet->GetID() );

	APPEARANCEMGR->CancelAlphaProcess( mpHeroPet );
	APPEARANCEMGR->CancelReservation( mpHeroPet );

	mpHeroPetBackup = mpHeroPet;

	delete mpHeroPet;
	mpHeroPet = NULL;
}

// 090422 ShinJS --- Ż�� �߰�
CVehicle* CObjectManager::AddVehicle( BASEOBJECT_INFO* pBaseObjectInfo, BASEMOVE_INFO* pMoveInfo, MONSTER_TOTALINFO* pTotalInfo )
{
	if(CObject* pPreObj = GetObject( pBaseObjectInfo->dwObjectID ))
	{
		RemoveObject( &pPreObj );
	}
	// ���� ������ ������ �̵��� ���, ������ ������Ʈ ������ ���޵Ǵ� ��찡 �ִ�.
	else if(pBaseObjectInfo->ObjectState == eObjectState_Die)
	{
		return 0;
	}

	CVehicle* pVehicle = m_VehiclePool.Alloc();

	pVehicle->Init( eObjectKind_Vehicle, pBaseObjectInfo );
	pVehicle->InitMonster( pTotalInfo );
	pVehicle->InitVehicle();
	pVehicle->InitObjectBalloon( ObjectBalloon_Name );

	APPEARANCEMGR->InitAppearance( pVehicle );
	MOVEMGR->InitMove(pVehicle,pMoveInfo);

	OBJECTSTATEMGR->InitObjectState( pVehicle );

	m_ObjectTable.Add( pVehicle, pVehicle->GetID() );

	ApplyOverInfoOption( pVehicle );
	ApplyShadowOption( pVehicle );

	return pVehicle;
}

// 090422 ShinJS --- Ż�� ����
void CObjectManager::RemoveVehicle( DWORD dwVehicleId )
{
	CVehicle* pVehicle = (CVehicle*)GetObject( dwVehicleId );
	if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )		return;

	APPEARANCEMGR->CancelAlphaProcess( pVehicle );
	APPEARANCEMGR->CancelReservation( pVehicle );

	pVehicle->Release();
	pVehicle->GetEngineObject()->Release();

	AddGarbageObject( pVehicle );

	ApplyOverInfoOption( pVehicle );
	ApplyShadowOption( pVehicle );
}

void CObjectManager::HideNpc(WORD UniqueIdx, BOOL bOn)
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		if( ( pObject->GetObjectKind() == eObjectKind_Npc ) )
		{
			if( ((CNpc*)pObject)->GetNpcUniqueIdx() == UniqueIdx )
			{

				if(bOn)
				{
#ifdef _GMTOOL_
					if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM  && CAMERA->GetCameraMode() != eCM_EyeView )
						pObject->GetEngineObject()->SetAlpha( 1.0f );
					else
#endif
						pObject->GetEngineObject()->Show();
					
					AddShadow(pObject);
				}
				else
				{
#ifdef _GMTOOL_
					if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM  && CAMERA->GetCameraMode() != eCM_EyeView )
						pObject->GetEngineObject()->SetAlpha( 0.3f );
					else
#endif
						pObject->GetEngineObject()->HideWithScheduling();

					if( pObject->GetID() == GetSelectedObjectID() )
						SetSelectedObject( NULL );

					RemoveShadow(pObject);
				}
			}
		}
	}
}

CObject* CObjectManager::GetObjectByName(const char* name)
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{		
		if( strcmp(pObject->GetObjectName(), name) == 0 )
		{
			return pObject;
		}
	}

	return NULL;
}

// 070911 LYW --- ObjectManager : Add function to return object id.
DWORD CObjectManager::GetObjectByChatName(const char* name)
{
	DWORD dwObjectID = 0;
	LPTSTR pNext = 0;
	char tempBuf[4] = {0};

	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{		
		char nameBuf[128] = {0};
		LPTSTR pName = pObject->GetObjectName() ;
		int nTotalLen = strlen(pName) ;
		LPTSTR pCur  = pObject->GetObjectName() ;
		
		for(int count = 0 ; count < nTotalLen ; )
		{
			if(eObjectKind_Npc != pObject->GetObjectKind())
			{
				if( *pCur == '^' )
				{
					pNext = pCur + 1 ;

					if( *pNext == 's' )
					{
						strcat(nameBuf, " ") ;
						pCur += 2 ;
						count += 2 ;

						continue ;
					}
				}
			}
			else
			{
				if( *pCur == ' ' )
				{
					strcat(nameBuf, " ") ;
					++pCur ;
					++count ;

					continue ;
				}
			}

			if( pCur + 1 != CharNext( CharPrev( pCur, pCur + 1 ) ) )
			{
				memset(tempBuf, 0, sizeof(tempBuf)) ;
				memcpy(tempBuf, pCur, sizeof(char)*2) ;
				

				strcat(nameBuf, tempBuf) ;
				pCur += 2 ;
				count += 2 ;
			}
			else
			{
				memcpy(nameBuf, pCur, sizeof(char)) ;
				++pCur ;
				++count ;
			}
		}

		if( strcmp(nameBuf, name) == 0 )
		{
			dwObjectID = pObject->GetID() ;

			break ;
		}
	}

	return dwObjectID ;
}

// 070911 LYW --- ObjectManager : Add function to return monster id.
DWORD CObjectManager::GetNearMonsterByName(const char* name)								// HERO�� ���� ���� ������ �ִ� ���� ���̵� ��ȯ�ϴ� �Լ�.
{
	VECTOR3 vHeroPos;																		// HERO�� ��ġ�� ���� ���͸� �����Ѵ�.
	HERO->GetPosition(&vHeroPos) ;															// HERO�� ��ġ�� �޴´�.

	DWORD dwObjectID = 0 ;																	// ��ȯ�� ���� ���̵� ���� ������ �����ϰ� 0���� �����Ѵ�.

	float fNearDistance = 2000.0f ;															// ���� ���� �Ÿ� ���� ���� ������ �����ϰ� �Ÿ��� 20m�� �����Ѵ�.
	float fDistance = 0.0f ;
	char* pName = NULL ;
	char* pCur  = NULL ;
	char* pNext = NULL ;

	int nTotalLen = 0 ;
	char nameBuf[128] ;
	char tempBuf[4] ;

	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{		
		if( pObject->GetObjectKind() <= 2 ) continue ;

		fDistance = CalcDistanceXZ(&vHeroPos, &pObject->GetBaseMoveInfo()->CurPosition) ;	// HERO�� ������ �Ÿ��� ���Ѵ�.

		if( fNearDistance < fDistance ) continue ;											// HERO�� ������ �Ÿ��� �ּҰŸ� ���� ũ��, continue ó���� �Ѵ�.

		fNearDistance = fDistance ;															// �ּ� �Ÿ��� HERO�� ������ �Ÿ��� �����Ѵ�.

		dwObjectID = pObject->GetID() ;														// ���� ������Ʈ�� ���̵� �����Ѵ�.

		memset(nameBuf, 0, sizeof(nameBuf)) ;

		pName = pObject->GetObjectName() ;
		nTotalLen = strlen(pName) ;

		pCur  = pObject->GetObjectName() ;

		for(int count = 0 ; count < nTotalLen ; )
		{
			if(eObjectKind_Npc != pObject->GetObjectKind())
			{
				if( *pCur == '^' )
				{
					pNext = pCur + 1 ;

					if( *pNext == 's' )
					{
						strcat(nameBuf, " ") ;
						pCur += 2 ;
						count += 2 ;

						continue ;
					}
				}
			}
			else
			{
				if( *pCur == ' ' )
				{
					strcat(nameBuf, " ") ;
					++pCur ;
					++count ;

					continue ;
				}
			}

			if( pCur + 1 != CharNext( CharPrev( pCur, pCur + 1 ) ) )
			{
				memset(tempBuf, 0, sizeof(tempBuf)) ;
				memcpy(tempBuf, pCur, sizeof(char)*2) ;
				

				strcat(nameBuf, tempBuf) ;
				pCur += 2 ;
				count += 2 ;
			}
			else
			{
				memcpy(nameBuf, pCur, sizeof(char)) ;
				++pCur ;
				++count ;
			}
		}

		if( strcmp(nameBuf, name) == 0 )
		{
			dwObjectID = pObject->GetID() ;

			break ;
		}
	}

	return dwObjectID ;
}


float CObjectManager::GetDistance( CObject* a, CObject* b )
{
	if( a != NULL || b != NULL )
	{
		assert( 0 );
		return 0;
	}
	
	const VECTOR3 locationVector = a->GetCurPosition() - b->GetCurPosition();

	return sqrt( pow( locationVector.x, 2.0f ) + pow( locationVector.y, 2.0f ) + pow( locationVector.z, 2.0f ) );
}

void CObjectManager::SetHide( DWORD id, WORD level )
{
	CObject* pObject = m_ObjectTable.GetData( id );

	if( !pObject )
	{
		return;
	}

	if( pObject->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	CPlayer* pPlayer = ( CPlayer* )pObject;

	//Alemuri stealth visible for Party Members--------------------------------
	BOOL isPartyMember = FALSE;
	CParty HeroParty = PARTYMGR->GetParty();
	for (int i = 0; i < HeroParty.GetMemberNum(); i++)
	{
		if ( HeroParty.GetMemberID(i) == id)
			isPartyMember = TRUE;
	}
	//-------------------------------------------------------------------------

	pPlayer->GetCharacterTotalInfo()->HideLevel = level;

	WORD DetectLevel = HERO->GetCharacterTotalInfo()->DetectLevel;

	if( level )
	{
		if( !m_HideObjectTable.GetData( pObject->GetID() ) )
		{
			m_HideObjectTable.Add( pObject, pObject->GetID() );
		}

		if( level <= DetectLevel  || isPartyMember == TRUE)
		{
			// 090213 ShinJS --- ĳ���� ����� �ɼ� ���������� ���� ��쿡�� ���������� �Ѵ�.
			if( !OBJECTMGR->IsOverInfoOption( eOO_HIDE_PLAYER ) )
			{
				pPlayer->GetEngineObject()->Show();

				pPlayer->GetEngineObject()->SetAlpha( 0.3f );

				pPlayer->GetCharacterTotalInfo()->bVisible = FALSE;

				pPlayer->HideEffect( FALSE );
			}
		}
		else
		{
#ifdef _GMTOOL_
			if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM  && CAMERA->GetCameraMode() != eCM_EyeView )
				pPlayer->GetEngineObject()->SetAlpha( 0.3f );
			else
#endif
			{
				if( pPlayer == HERO )
				{
					pPlayer->GetEngineObject()->SetAlpha( 0.3f );
				}
				else
				{
					pPlayer->GetEngineObject()->HideWithScheduling();
					pPlayer->HideEffect( TRUE );
				}
			}

			pPlayer->GetCharacterTotalInfo()->bVisible = FALSE;

			if( pPlayer->GetID() == GetSelectedObjectID() )
			{
				SetSelectedObject( NULL );	
			}
		}
	}
	else
	{
		m_HideObjectTable.Remove( pObject->GetID() );

		pPlayer->GetEngineObject()->SetAlpha( 1.0f );
		pPlayer->GetEngineObject()->Show();

		pPlayer->GetCharacterTotalInfo()->bVisible = TRUE;		
		pPlayer->HideEffect( FALSE );
	}

	ApplyShadowOption(pPlayer);		
	ApplyOverInfoOption(pPlayer);
}

void CObjectManager::SetDetect( DWORD id, WORD level )
{
	if( id != HEROID )
	{
		return;
	}

	HERO->GetCharacterTotalInfo()->DetectLevel = level;

	m_HideObjectTable.SetPositionHead();

	while(CObject* const pObject = m_HideObjectTable.GetData())
	{
		CPlayer* pPlayer = ( CPlayer* )pObject;

		SetHide( pPlayer->GetID(), pPlayer->GetCharacterTotalInfo()->HideLevel );
	}
}

// 090227 ShinJS --- �ش�ID�� NPC�� �����Ѵ�
void CObjectManager::RemoveNPC( DWORD dwNpcId )
{
	CNpc* pNpc = (CNpc*)GetObject( dwNpcId );
	if( !pNpc )		return;
	if( pNpc->GetObjectKind() != eObjectKind_Npc )	return;

	APPEARANCEMGR->CancelAlphaProcess( pNpc );
	APPEARANCEMGR->CancelReservation( pNpc );

	pNpc->GetEngineObject()->Release();

	AddGarbageObject( pNpc );
}

BOOL CObjectManager::StartVehicleInstall( WORD wMonsterKind, CItem* pUseItem )
{
	const BASE_MONSTER_LIST* const pMonsterInfo = GAMERESRCMNGR->GetMonsterListInfo( wMonsterKind );

	if( 0 == pMonsterInfo )
	{
		return FALSE;
	}

	MOVEMGR->HeroMoveStop();

	// 100319 ShinJS --- ���� ��ġ���� �������� ����� �����Ѵ�.
	CVehicle* pVehicle = (CVehicle*)GetObject( VEHICLE_INSTALL_ID );
	if( pVehicle )
	{
		const ICONBASE iconBase = pVehicle->GetUsedItem();
		CItem* pItem = ITEMMGR->GetItem( iconBase.dwDBIdx );
		pItem->SetLock( FALSE );

		RemoveVehicle( VEHICLE_INSTALL_ID );
	}

	// 100707 ShinJS �̵����� ��� �ڵ� ��ġ
	if( pMonsterInfo->RunMove > 0 )
	{
		pUseItem->SetLock( TRUE );
		GAMEIN->GetInventoryDialog()->Set_QuickSelectedItem( pUseItem );

		if( VEHICLEMGR->InstallVehicleToNearPosition( wMonsterKind ) == FALSE )
		{
			// ��ġ ����
			pUseItem->SetLock( FALSE );
			return FALSE;
		}
		return TRUE;
	}


	BASEOBJECT_INFO binfo;												// �⺻ ������Ʈ ������ ���� ����ü�� �����Ѵ�.
	ZeroMemory( &binfo, sizeof(binfo) );
	binfo.dwObjectID = VEHICLE_INSTALL_ID;								// �⺻ ������Ʈ ���̵� �����Ѵ�.
	SafeStrCpy( binfo.ObjectName, pMonsterInfo->Name, sizeof( binfo.ObjectName ) / sizeof( *( binfo.ObjectName ) ) );
	BASEMOVE_INFO minfo;												// �⺻ �̵� ������ ���� ����ü�� �����Ѵ�.
	ZeroMemory( &minfo, sizeof(minfo) );
	HERO->GetPosition( &minfo.CurPosition );
	MONSTER_TOTALINFO moninfo;
	ZeroMemory( &moninfo, sizeof(moninfo) );
	moninfo.MonsterKind = wMonsterKind;

	pVehicle = AddVehicle(&binfo, &minfo, &moninfo);

	if( 0 == pVehicle )
	{
		return FALSE;
	}

	pVehicle->SetInstall( FALSE );
	pVehicle->GetEngineObject()->SetAlpha( 0.3f );

	// 100319 ShinJS --- ��ġ�� ������ ���� ����
	ICONBASE iconBase;
	iconBase.wIconIdx = pUseItem->GetItemIdx();
	iconBase.Position = pUseItem->GetPosition();
	iconBase.dwDBIdx = pUseItem->GetDBIdx();
	pVehicle->SetUsedItem( iconBase );

	pUseItem->SetLock( TRUE );
	GAMEIN->GetInventoryDialog()->Set_QuickSelectedItem( pUseItem );

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1818 ) );
	return TRUE;
}

// 091211 pdy ���ͽõ� ���� Ÿ�ټ����� �ȵǴ� ���� ����
CNpc* CObjectManager::GetNpcByUniqueIdx(WORD UniqueIdx)
{
	m_ObjectTable.SetPositionHead();

	for(CObject* pObject = m_ObjectTable.GetData();
		0 < pObject;
		pObject = m_ObjectTable.GetData())
	{
		if( ( pObject->GetObjectKind() == eObjectKind_Npc ) )
		{
			if( ((CNpc*)pObject)->GetNpcUniqueIdx() == UniqueIdx )
			{
				return (CNpc*)pObject;
			}
		}
	}
	return NULL; 
}