#include "stdafx.h"
#include "DungeonMgr.h"
#include "MHMap.h"
#include "MHFile.h"
#include "ObjectManager.h"
#include "GameResourceStruct.h"
#include "GameResourceManager.h"
#include "./FadeDlg.h"
#include "./GameIn.h"
#include "ChatManager.h"
#include "TileManager.h"
#include "..\MHCamera.h"
#include "MoveManager.h"

GLOBALTON(CDungeonMgr)

CDungeonMgr::CDungeonMgr(void)
{
	m_CurDungeon.Clear();
}

CDungeonMgr::~CDungeonMgr(void)
{
	m_WarpList.SetPositionHead();

	for(stWarpInfo* pWarp = m_WarpList.GetData();
		0 < pWarp;
		pWarp = m_WarpList.GetData())
	{
		SAFE_DELETE(
			pWarp);
	}

	m_SwitchNpcList.SetPositionHead();

	for(stSwitchNpcInfo* pSwitchNpc = m_SwitchNpcList.GetData();
		0 < pSwitchNpc;
		pSwitchNpc = m_SwitchNpcList.GetData())
	{
		SAFE_DELETE(
			pSwitchNpc);
	}
}

void CDungeonMgr::Init()
{
	WORD num = MAP->GetMapNum();
	if(IsDungeonMap(num))
	{
		m_WarpList.Initialize(40);
		m_SwitchNpcList.Initialize(30);

		LoadDungeonInfo();
	}
}

void CDungeonMgr::Process()
{
	if(! IsDungeonMap(MAP->GetMapNum()))
		return;

	float fDistance  ;
	VECTOR3 heroPos;
	VECTOR3 warpPos;
	stWarpState* pWarpState;
	stWarpInfo* pWarp;

	for(WORD i=0; i<MAX_DUNGEON_WARP; i++)
	{
		pWarpState = &m_CurDungeon.m_WarpState[i];

		if(! pWarpState)
			break;

		if(FALSE == pWarpState->bActive ||
			0 == pWarpState->dwObjectIndex)
			continue;

		pWarp = m_WarpList.GetData(pWarpState->dwIndex);
		if(! pWarp)
			continue;

		warpPos.x = pWarp->wCurPosX;
		warpPos.y = 0.0f;
		warpPos.z = pWarp->wCurPosZ;

		HERO->GetPosition(&heroPos);
		fDistance = CalcDistanceXZ(&heroPos, &warpPos ) ;

		if(fDistance <= (float)pWarp->wRadius)
		{
			CFadeDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetFadeDlg() ;

			if( pDlg )
			{
				// 091215 ONS �޼����� �������� �������� ���� �����Ѵ�.
				static DWORD dwLastChangeTime = 0;
				if( gCurTime - dwLastChangeTime < 3000 )	
					return;	
				else dwLastChangeTime = gCurTime;

				VECTOR3 destPos;
				destPos.x = pWarp->wDstPosX;
				destPos.y = 0.0f;
				destPos.z = pWarp->wDstPosZ;

				pDlg->FadeOut() ;

				// 091119 ONS ����:����ΰ� �̵��� �����ε����� ����ϱ� ���� �������� ����/����
				MSG_DUNGEON_FADE_MOVE msg ;
				ZeroMemory(&msg, sizeof(MSG_DUNGEON_FADE_MOVE));
				msg.Category = MP_DUNGEON ; 
  				msg.Protocol = MP_DUNGEON_FADEMOVE_SYN ;
  				msg.dwObjectID = HEROID ;
  				msg.dwMoverID = HEROID ;
				msg.dwWarpIndex = pWarp->dwIndex;
				msg.cpos.Compress(&destPos) ;

  				NETWORK->Send( &msg, sizeof( MSG_DUNGEON_FADE_MOVE ) ) ;
			}
			
			return;
		}
	}
}

void CDungeonMgr::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_DUNGEON_ENTRANCE_ACK:
		{
			m_CurDungeon.Clear();
			MSG_DUNGEON_INFO* pmsg = (MSG_DUNGEON_INFO*)pMsg;

			m_CurDungeon.dwIndex = pmsg->dwIndex;
			m_CurDungeon.dwPartyIndex = pmsg->dwPartyIndex;
			m_CurDungeon.dwChannelID = pmsg->dwChannelID;
			m_CurDungeon.dwPoint = pmsg->dwPoint;
			m_CurDungeon.dwJoinPlayerNum = pmsg->dwJoinPlayerNum;
			m_CurDungeon.difficulty = pmsg->difficulty;
			memcpy(m_CurDungeon.m_WarpState, pmsg->warpState, sizeof(m_CurDungeon.m_WarpState));
			memcpy(m_CurDungeon.m_SwitchNpcState, pmsg->SwitchNpcState, sizeof(m_CurDungeon.m_SwitchNpcState));
			m_CurDungeon.dwStartTime = gCurTime;

			for(WORD i=0; i<MAX_DUNGEON_WARP; i++)
			{
				if(0 == m_CurDungeon.m_WarpState[i].dwIndex)
					break;

				SetWarp(m_CurDungeon.m_WarpState[i].dwIndex, m_CurDungeon.m_WarpState[i].bActive);
			}

			for(WORD i=0; i<MAX_DUNGEON_SWITCH; i++)
			{
				if(0 == m_CurDungeon.m_SwitchNpcState[i].wIndex)
					break;

				SetSwitchNpc(m_CurDungeon.m_SwitchNpcState[i].wIndex, m_CurDungeon.m_SwitchNpcState[i].bActive);
			}

		}
		break;

	case MP_DUNGEON_SETWARP_ACK:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			if(m_CurDungeon.dwChannelID != pmsg->dwData1)
				break;

			stWarpInfo* pWarp = m_WarpList.GetData(pmsg->dwData2);
			if(pWarp)
			{
				SetWarp(pmsg->dwData2, (BOOL)pmsg->dwData3);
			}
		}
		break;

	case MP_DUNGEON_SETSWITCH_ACK:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			if(m_CurDungeon.dwChannelID != pmsg->dwData1)
				break;

			stSwitchNpcInfo* pSwitchNpc = m_SwitchNpcList.GetData(pmsg->dwData2);
			if(pSwitchNpc)
			{
				SetSwitchNpc((WORD)pmsg->dwData2, (BOOL)pmsg->dwData3);
			}
		}
		break;
	case MP_DUNGEON_ENTRANCE_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			switch(pmsg->dwData)
			{
			case eDungeonERR_NoParty:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1981) );
				break;

			case eDungeonERR_NotEnoughLevel:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1982) );
				break;

			case eDungeonERR_OverLevel:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1983) );
				break;

			case eDungeonERR_EntranceDelay:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1984) );
				break;

			case eDungeonERR_OverCount:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1210) );
				break;

			case eDungeonERR_DungeonMapOff:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(1615) );
				break;

			case eDungeonERR_WrongDiffculty:
				CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(2263) );
				break;
			}
		}
		break;

	case MP_DUNGEON_ZONEBLOCK:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			ZoneBlock((float)pmsg->dwData1, (float)pmsg->dwData2, (float)pmsg->dwData3);
		}
		break;

	case MP_DUNGEON_ZONEFREE:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			ZoneFree((float)pmsg->dwData1, (float)pmsg->dwData2, (float)pmsg->dwData3);
		}
		break;

	case MP_DUNGEON_EDGEBLOCK:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			EdgeBlock((float)pmsg->dwData1, (float)pmsg->dwData2, (float)pmsg->dwData3);
		}
		break;

	case MP_DUNGEON_EDGEFREE:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			EdgeFree((float)pmsg->dwData1, (float)pmsg->dwData2, (float)pmsg->dwData3);
		}
		break;
	// 091119 ONS �δ� ���̵幫�� ��� ó��
	case MP_DUNGEON_FADEMOVE_ACK:
		{
			MSG_DUNGEON_FADE_MOVE* pmsg = (MSG_DUNGEON_FADE_MOVE*)pMsg;
			MOVEMGR->Move_FadeInOut( pMsg );
			AfterWarpAction(pmsg->dwWarpIndex);
		}
		break;
	default:
		break;
	}
}

void CDungeonMgr::LoadDungeonInfo()
{
	char buf[MAX_PATH] = {0,};
	sprintf(buf, "%s%d", "system/resource/DungeonInfo_", MAP->GetMapNum());
	strcat(buf, ".bin");
	
	CMHFile file;
	if(! file.Init( buf, "rb" ))
		return;

	char StrBuf[256] = {0,};

	while(1)
	{
		if(file.IsEOF())
			break;

		file.GetString(StrBuf);

		if( StrBuf[0] != '#' )
		{
			file.GetLine(StrBuf,256);
			continue;
		}
		else if( strcmp( StrBuf , "#WARP_LIST" ) == 0 )
		{
			m_WarpList.RemoveAll();

			while(1)
			{
				if(file.IsEOF())
					break;

				file.GetString(StrBuf);
				if( StrBuf[0] != '#' )
				{
					file.GetLine(StrBuf,256);
					continue;
				}
				else if( strcmp( StrBuf , "#WARP" ) == 0 )
				{
					stWarpInfo* pWarp = new stWarpInfo;
					if(pWarp)
					{
						pWarp->dwIndex = file.GetDword();
						pWarp->wMapNum = file.GetWord();
						pWarp->wNpcJob = file.GetWord();
						strcpy(pWarp->name, file.GetString());
						pWarp->wRadius = file.GetWord();
						pWarp->wCurPosX = file.GetWord();
						pWarp->wCurPosZ = file.GetWord();
						pWarp->wDstPosX = file.GetWord();
						pWarp->wDstPosZ = file.GetWord();
						pWarp->bActive = FALSE;

						m_WarpList.Add(pWarp, pWarp->dwIndex);
					}
				}
				else if( strcmp( StrBuf , "#WARP_LIST_END" ) == 0 )
				{
					break;
				}
			}
		}
		else if( strcmp( StrBuf , "#SWITCH_LIST" ) == 0 )
		{
			m_SwitchNpcList.RemoveAll();

			while(1)
			{
				if(file.IsEOF())
					break;

				file.GetString(StrBuf);
				if( StrBuf[0] != '#' )
				{
					file.GetLine(StrBuf,256);
					continue;
				}
				else if( strcmp( StrBuf , "#SWITCH" ) == 0 )
				{
					stSwitchNpcInfo* pSwitchNpc = new stSwitchNpcInfo;
					if(pSwitchNpc)
					{
						pSwitchNpc->wIndex = file.GetWord();
						pSwitchNpc->wMapNum = file.GetWord();
						pSwitchNpc->wNpcJob = file.GetWord();
						strcpy(pSwitchNpc->name, file.GetString());
						pSwitchNpc->wRadius = file.GetWord();
						pSwitchNpc->wPosX = file.GetWord();
						pSwitchNpc->wPosZ = file.GetWord();
						pSwitchNpc->bActive = FALSE;

						m_SwitchNpcList.Add(pSwitchNpc, pSwitchNpc->wIndex);
					}
				}
				else if( strcmp( StrBuf , "#NPC_LIST_END" ) == 0 )
				{
					break;
				}
			}
		}
	}
}

void CDungeonMgr::SetWarp(DWORD dwIndex, BOOL bActive)
{
	stWarpInfo* pWarp = m_WarpList.GetData(dwIndex);
	if(! pWarp)
		return;

	stWarpState* pWarpState = m_CurDungeon.GetWarp(dwIndex);
	if(! pWarpState)
		return;

	if(pWarpState->dwObjectIndex)
	{
		CObject* pObject = OBJECTMGR->GetObject(pWarpState->dwObjectIndex);
		if(! pObject || 
			eObjectKind_Npc!=pObject->GetObjectKind())
			return;

		pWarpState->bActive = bActive;
		if(pWarpState->bActive)
		{
			// Show
			//OBJECTMGR->HideNpc(pWarpState->dwObjectIndex, TRUE);
			pObject->GetEngineObject()->Show();
			OBJECTMGR->AddShadow(pObject);	
			pObject->ShowObjectName( TRUE ) ;
		}
		else
		{
			// Hide
			//OBJECTMGR->HideNpc(pWarpState->dwObjectIndex, FALSE);
			pObject->GetEngineObject()->HideWithScheduling();
			OBJECTMGR->RemoveShadow(pObject);
			pObject->ShowObjectName( FALSE ) ;

			if( pObject->GetID() == OBJECTMGR->GetSelectedObjectID() )
				OBJECTMGR->SetSelectedObject( NULL );
		}
	}
	else
	{
		static int number = 0;

		if(bActive)
		{
			// ����
			NPC_TOTALINFO tinfo;															// npc ��Ż ������ ���� ����ü�� �����Ѵ�.
			BASEOBJECT_INFO binfo;															// �⺻ ������Ʈ ������ ���� ����ü�� �����Ѵ�.
			BASEMOVE_INFO minfo;															// �⺻ �̵� ������ ���� ����ü�� �����Ѵ�.

			binfo.dwObjectID = STATICNPC_ID_START + 2000 + number++;
			binfo.ObjectState = eObjectState_None;
			tinfo.MapNum = MAP->GetMapNum();
			minfo.bMoving = FALSE;
			minfo.KyungGongIdx = 0;
			minfo.MoveMode = eMoveMode_Run;

			tinfo.NpcKind = pWarp->wNpcJob;												// ��Ż ������ npc ������ �����Ѵ�.
			// 080218 LYW --- MHMap : ������Ʈ �̸��� ���� Ȯ��.
			SafeStrCpy( binfo.ObjectName, pWarp->name, 32+1 );							// �⺻ ������ npc �̸��� �����Ѵ�.
			tinfo.NpcUniqueIdx = 0;//file.GetWord();										// ��Ż ������ npc ����ũ �ε����� �����Ѵ�.

			NPC_LIST* pInfo = GAMERESRCMNGR->GetNpcInfo(tinfo.NpcKind);					// npc ������ �����Ͽ� npc ����Ʈ ������ �޴´�.

			if(pInfo)																	// npc ����Ʈ ������ ��ȿ�ϸ�,
			{
				tinfo.NpcJob = pInfo->JobKind;											// ��Ż ������ npc ������ �����Ѵ�.
			}
			else																		// npc ����Ʈ ������ ��ȿ���� ������,
			{
				tinfo.NpcJob = 0;														// ������ 0���� �����Ѵ�.
			}
			
			//jop�� 0�̸� �̸��� ������.
			if( tinfo.NpcJob == 0 )														// ������ 0�̸�,
			{
				binfo.ObjectName[0] = 0;												// �̸��� �����.
			}

			minfo.CurPosition.x = pWarp->wCurPosX;										// �̵������� ���Ϸ� ���� ���� ��ġ�� ��ġ�� �����Ѵ�.
			minfo.CurPosition.y = 0;
			minfo.CurPosition.z = pWarp->wCurPosZ;
			
			CNpc* pNpc = OBJECTMGR->AddNpc(&binfo,&minfo,&tinfo);

			//����.....
			float fDir = 0.0f;//file.GetFloat();												// ������ �о���δ�.
			pNpc->SetAngle( DEGTORAD( fDir ) );											// ������ �����Ѵ�.

			pWarpState->dwObjectIndex = pNpc->GetID();
		}
		else
		{
			// ���� :: ���� ������ �����϶��???
			int a;
			a = 10;
		}
	}

	pWarpState->bActive = bActive;
}

void CDungeonMgr::SetSwitchNpc(WORD wIndex, BOOL bActive)
{
	stSwitchNpcInfo* pSwitchNpcInfo = m_SwitchNpcList.GetData(wIndex);
	if(! pSwitchNpcInfo)
		return;

	stSwitchNpcState* pSwitchNpcState = m_CurDungeon.GetSwitchNpc(wIndex);
	if(! pSwitchNpcState)
		return;

	if(pSwitchNpcState->dwObjectIndex)
	{
		CNpc* pObject = (CNpc*)OBJECTMGR->GetObject(pSwitchNpcState->dwObjectIndex);
		if(! pObject || 
			eObjectKind_Npc!=pObject->GetObjectKind())
			return;

		pSwitchNpcState->bActive = bActive;
		if(pSwitchNpcState->bActive)
		{
			// Show
			pObject->GetEngineObject()->Show();
			OBJECTMGR->AddShadow(pObject);	
			pObject->ShowObjectName( TRUE ) ;
		}
		else
		{
			// Hide
			pObject->GetEngineObject()->HideWithScheduling();
			OBJECTMGR->RemoveShadow(pObject);
			pObject->ShowObjectName( FALSE ) ;

			if( pObject->GetID() == OBJECTMGR->GetSelectedObjectID() )
				OBJECTMGR->SetSelectedObject( NULL );
		}
	}
}

BOOL CDungeonMgr::IsDungeonMap(MAPTYPE map)
{
	/*switch(map)
	{
	//case Temple_of_Greed:
	//case Broken_Shipwrecked:
	//case The_Death_tower:
	//case The_Spirit_Forest:
	//case The_Cristal_Empire:
	//case TreeOfKenai:
	//case TheCryingTavern:
	//case TheUndergroundPassage:
	//case UnderGroundPrison:
		//return TRUE;
	}*/

	return FALSE;
}

void CDungeonMgr::ZoneBlock(float posx, float posz, float range)
{
	WORD num = MAP->GetMapNum();
	if(! IsDungeonMap(num))
		return;

	CTileManager* pTileManager = MAP->GetTileManager();
	
	VECTOR3 vStgPos;
	float fx, fz;
	float fAdd = -50.0f;

	for(fx=posx-range; fx<=posx+range+fAdd; fx+=50.0f)
	{
		for(fz=posz-range; fz<=posz+range+fAdd; fz+=50.0f)
		{
			vStgPos.x = fx;
			vStgPos.y = 0.0f;
			vStgPos.z = fz;

			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
	}
}

void CDungeonMgr::ZoneFree(float posx, float posz, float range)
{
	WORD num = MAP->GetMapNum();
	if(! IsDungeonMap(num))
		return;

	CTileManager* pTileManager = MAP->GetTileManager();
	
	VECTOR3 vStgPos;
	float fx, fz;
	float fAdd = -50.0f;

	for(fx=posx-range; fx<=posx+range+fAdd; fx+=50.0f)
	{
		for(fz=posz-range; fz<=posz+range+fAdd; fz+=50.0f)
		{
			vStgPos.x = fx;
			vStgPos.y = 0.0f;
			vStgPos.z = fz;

			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		}
	}
}

void CDungeonMgr::EdgeBlock(float posx, float posz, float range)
{
	WORD num = MAP->GetMapNum();
	if(! IsDungeonMap(num))
		return;

	CTileManager* pTileManager = MAP->GetTileManager();
	
	float range2 = range + 50.0f;
	VECTOR3 vStgPos;
	float fx, fz = posz;
	float fAdd = -50.0f;

	for( fx = posx - range ; fx <= posx + range + fAdd ; fx += 50.0f )//
	{
		vStgPos.x = fx;
		vStgPos.z = fz - range;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.z = fz + range + fAdd;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}
	
	fx = posx;
	for( fz = posz - range ; fz <= posz + range + fAdd; fz += 50.0f )
	{
		vStgPos.x = fx - range;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.x = fx + range + fAdd;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}

	// �̰� ���ϸ� ���ۺ��� ����.
	fz = posz;
	for( fx = posx - range2 ; fx <= posx + range2 + fAdd ; fx += 50.0f )//
	{
		vStgPos.x = fx;
		vStgPos.z = fz - range2;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.z = fz + range2 + fAdd;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}
	
	fx = posx;
	for( fz = posz - range2 ; fz <= posz + range2 + fAdd ; fz += 50.0f )
	{
		vStgPos.x = fx - range2;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.x = fx + range2 + fAdd;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->AddTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}
}

void CDungeonMgr::EdgeFree(float posx, float posz, float range)
{
	WORD num = MAP->GetMapNum();
	if(! IsDungeonMap(num))
		return;

	CTileManager* pTileManager = MAP->GetTileManager();

	float range2 = range + 50.0f;
	VECTOR3 vStgPos;
	float fx, fz = posz;
	float fAdd = -50.0f;

	for( fx = posx - range ; fx <= posx + range + fAdd ; fx += 50.0f )
	{
		vStgPos.x = fx;
		vStgPos.z = fz - range;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.z = fz + range + fAdd;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}
	
	fx = posx;
	for( fz = posz - range ; fz <= posz + range + fAdd; fz += 50.0f )
	{
		vStgPos.x = fx - range;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.x = fx + range + fAdd;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}

	// �̰� ���ϸ� ���ۺ��� ����.
	fz = posz;
	for( fx = posx - range2 ; fx <= posx + range2 + fAdd ; fx += 50.0f )//
	{
		vStgPos.x = fx;
		vStgPos.z = fz - range2;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.z = fz + range2 + fAdd;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}
	
	fx = posx;
	for( fz = posz - range2 ; fz <= posz + range2 + fAdd ; fz += 50.0f )
	{
		vStgPos.x = fx - range2;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
		vStgPos.x = fx + range2 + fAdd;
		vStgPos.z = fz;
		if( vStgPos.x > 0 && vStgPos.x < 51200 && vStgPos.z > 0 && vStgPos.z < 51200 )
			pTileManager->RemoveTileAttrByAreaData( NULL, &vStgPos, SKILLAREA_ATTR_BLOCK );
	}
}

// 091119 ONS �δ� ���̵幫���� ó���Լ�
void CDungeonMgr::AfterWarpAction( DWORD dwWarpIndex )
{ 
	// Ž���ǻ�� <�����ǹ�>�̵��� ī�޶� ���� Attach/Detatch 
	if( MAP->GetMapNum() == GraniteZone )
	{
		switch(dwWarpIndex)
		{
		case 1011:
			{
				// ���͸� �Ҵ�.
				CAMERA->AttachCameraFilter( 1 ) ;
			}
			break;
		case 1012:
		case 1027:
			{
				// ���͸� ����.
				CAMERA->DetachCameraFilter( 1 ) ;
			}
			break;
		}			
	}
}

void CDungeonMgr::ClearCurDungeon()
{
	m_CurDungeon.Clear();
}

void CDungeonMgr::AddEntranceNpc( DWORD dwDungeonKey, DWORD dwNpcJob )
{
	m_mapEntranceNpcJob.insert( std::make_pair( dwNpcJob, dwDungeonKey ) );
}

BOOL CDungeonMgr::IsDungeonEntranceNpc( const DWORD dwNpcJob ) const
{
	return ( m_mapEntranceNpcJob.find( dwNpcJob ) != m_mapEntranceNpcJob.end() );
}