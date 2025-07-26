// SiegeWarfareMgr.cpp: implementation of the CSiegeWarfareMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiegeWarfareMgr.h"
#include "Guild.h"
#include "GuildManager.h"
#include "Player.h"
#include "MHFile.h"
#include "mhMap.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "../ChatManager.h"

#include "./WindowIDEnum.h"

#include "./cMsgBox.h"

#include "./interface/cWindowManager.h"

#include "./ProgressDialog.h"

#include "./GMNotifyManager.h"

#include "./NpcRecallMgr.h"

#include "./SiegeWarFlagDlg.h"

#include "./GameIn.h"

#include "../[cc]skill/client/Effect/SkillEffect.h"

// 081020 LYW --- SiegeWarfareMgr : ������������ �̴ϸ�/���͹���/�����۸�/���� â�� ��Ȱ��ȭ �ϵ��� �Ѵ�. - �۰���
#include "./MiniMapDlg.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"

CSiegeWarfareMgr::CSiegeWarfareMgr()
{
	m_dwMapType = eSiegeWarfare_Map_Max;
	m_wState = eSiegeWarfare_State_Before;
	m_dwWaterSeedUsingGuildLevelLimit = 0;
	m_dwWaterSeedUsingSecond = 0;
	m_bRevive = FALSE;
	m_wReviveMessageCount = 0;
	m_dwStartTime = 0;

	// 080829 LYW --- SiegeWarfareMgr : ���� ��ų ����� ���� ���� �ʱ�ȭ.
	m_dwBuffSkillPay = 0 ;

	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ带 ����� �� �ִ� �Ÿ��� ���� ������ �ʱ�ȭ �Ѵ�.
	m_wCravingDistance = 0 ;

	m_WaterSeedEffect = new cSkillEffect();
}

CSiegeWarfareMgr::~CSiegeWarfareMgr()
{
	delete m_WaterSeedEffect; 
}

void CSiegeWarfareMgr::Init()
{
	m_dwMapType = eSiegeWarfare_Map_Max;
	m_wState = eSiegeWarfare_State_Before;
	m_dwWaterSeedUsingGuildLevelLimit = 0;
	m_dwWaterSeedUsingSecond = 0;
	m_bRevive = FALSE;
	m_wReviveMessageCount = 0;
	m_dwStartTime = 0;

	// 081006 LYW --- SiegeWarfareMgr : ���������� ����� �� �̵� ���� �ε�.
	Load_GuildMapMoveInfo() ;

	// 081019 LYW --- SiegeWarfareMgr : �칰 ��ġ �ε�.
	Load_WellPos() ;
	m_WaterSeedEffect->Init( 0 , 0 , 0 );
}

void CSiegeWarfareMgr::LoadInfo()
{
	CMHFile fp;
	char szFile[256] = {0,};
	char szLine[256] = {0,};
	sprintf(szFile, "./System/Resource/SiegeWarInfo.bin");
	fp.Init(szFile, "rb");
	int nKind = 0;

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s ������ �������� �ʽ��ϴ�.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while(TRUE)
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}

		
		switch( nKind )
		{
			case eSiegeWarfare_Info_Damage_Ratio:
				break;
			case eSiegeWarfare_Info_Heal_Ratio:
				break;
			case eSiegeWarfare_Info_Critical_Rate:
				break;
			case eSiegeWarfare_Info_WarterSeed_GuildLevel:
				sscanf(szLine, "%d", &m_dwWaterSeedUsingGuildLevelLimit);
				break;
			case eSiegeWarfare_Info_WarterSeed_Time:
				sscanf(szLine, "%d", &m_dwWaterSeedUsingSecond);
				break;

			// 081017 LYW --- SiegeWarfareMgr : ������ ����ϴ� �Ÿ� ���� �޴´�.
			case eSiegeWarfare_Info_Craving_Distance :
			sscanf(szLine, "%u", &m_wCravingDistance) ;
			break ;

			case eSiegeWarfare_Info_VilageInfo:
				break; 
		}

		nKind++;
	}
	sprintf(szFile, "./System/Resource/SiegeWarMapInfo.bin");
	fp.Init(szFile, "rb");

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s ������ �������� �ʽ��ϴ�.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while( TRUE )
	{
		if (fp.IsEOF()) break;
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if (strstr(szLine, "END")) 
		{
			break;
		}

		SiegeWarfareKindMapInfo stMapInfo;

		memset(&stMapInfo, NULL, sizeof(SiegeWarfareKindMapInfo));

		sscanf(szLine, "%hd %hd %hd %f %f", &stMapInfo.WarfareType, &stMapInfo.MapNum, &stMapInfo.MapType, &stMapInfo.PosX, &stMapInfo.PosZ);

		m_SiegeWarfareKindMapList.push_back(stMapInfo);

		if( stMapInfo.MapNum == MAP->GetMapNum() )
		{
			m_dwMapType = stMapInfo.MapType;
		}
	}

	sprintf(szFile, "./System/Resource/SiegeWarfareFobiddebSkillList.bin");
	fp.Init(szFile, "rb");

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s ������ �������� �ʽ��ϴ�.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while( TRUE )
	{
		if ( fp.IsEOF() ) break;
		fp.GetLine(szLine, sizeof(szLine));
		if ( strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if( strstr(szLine, "END") )
		{
			break;
		}

		DWORD skillIdx = 0;
		sscanf(szLine, "%d", &skillIdx);

		m_ForbiddenSkillList.push_back(skillIdx);
	}

	sprintf(szFile, "./System/Resource/SiegeWarfareFobiddebItemList.bin");
	fp.Init(szFile, "rb");

	if(!fp.IsInited())
	{
		char szTmp[256];
		sprintf(szTmp, "%s ������ �������� �ʽ��ϴ�.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while( TRUE )
	{
		if ( fp.IsEOF() ) break;
		fp.GetLine(szLine, sizeof(szLine));
		if ( strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if( strstr(szLine, "END") )
		{
			break;
		}

		DWORD itemIdx = 0;
		sscanf(szLine, "%d", &itemIdx);

		m_ForbiddenItemList.push_back(itemIdx);
	}

	sprintf(szFile, "./System/Resource/TournamentFobiddebSkillList.bin");
	fp.Init(szFile, "rb");

	if(fp.IsInited() == FALSE)
	{
		char szTmp[256];
		sprintf(szTmp, "%s ������ �������� �ʽ��ϴ�.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while( TRUE )
	{
		if ( fp.IsEOF() ) break;
		fp.GetLine(szLine, sizeof(szLine));
		if ( strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if( strstr(szLine, "END") )
		{
			break;
		}

		DWORD skillIdx = 0;
		sscanf(szLine, "%d", &skillIdx);

		m_ForbiddenSkillListTournament.push_back(skillIdx);
	}

	sprintf(szFile, "./System/Resource/TournamentFobiddebItemList.bin");
	fp.Init(szFile, "rb");

	if(fp.IsInited() == FALSE)
	{
		char szTmp[256];
		sprintf(szTmp, "%s ������ �������� �ʽ��ϴ�.", szFile);
		ASSERTMSG(0, szTmp);
		return;
	}

	while( TRUE )
	{
		if ( fp.IsEOF() ) break;
		fp.GetLine(szLine, sizeof(szLine));
		if ( strstr(szLine, "//") ||
			IsEmptyLine(szLine))
		{
			continue;
		}
		else if( strstr(szLine, "END") )
		{
			break;
		}

		DWORD itemIdx = 0;
		sscanf(szLine, "%d", &itemIdx);

		m_ForbiddenItemListTournament.push_back(itemIdx);
	}

	fp.Release();

	//100216 pdy �������� �ٳ���� �׶󼺿� �̴ϸ��� �ȿ����� ���׼���
	CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
	if( pMiniMapDlg )		pMiniMapDlg->SetActive( TRUE );
}

void CSiegeWarfareMgr::Process()
{
	if(IsSiegeWarfareZone(MAP->GetMapNum()) && OBJECTSTATEMGR->GetObjectState(HERO) == eObjectState_Die && m_bRevive )
	{
		if( IsRevive() )
		{
			MSGBASE Msg;
			SetProtocol(&Msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_REVIVE_SYN);
			Msg.dwObjectID = HEROID;

			NETWORK->Send((MSGROOT*)&Msg, sizeof(MSGBASE));

			m_bRevive = FALSE;
		}
	}
}

void CSiegeWarfareMgr::SetReviveTime()
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	m_stReviveTime = st;

	m_stReviveTime.wSecond += 5;

	if( m_stReviveTime.wSecond >= 60 )
	{
		m_stReviveTime.wMinute += 1;
		m_stReviveTime.wSecond -= 60;
	}

	m_wReviveMessageCount = 5;

	m_dwStartTime = gCurTime - 1000;
}

BOOL CSiegeWarfareMgr::IsRevive()
{

	if( gCurTime - m_dwStartTime >= 1000 )
	{
		m_dwStartTime += 1000;
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1638 ), m_wReviveMessageCount) ;
		m_wReviveMessageCount--;
	}

	if( m_wReviveMessageCount <= 0 )
		return TRUE;
/*
	SYSTEMTIME st;
	GetLocalTime( &st );

	BOOL bRevive = FALSE;

	if( m_stReviveTime.wHour > st.wHour  )
	{
		bRevive = TRUE;
	}
	else if( m_stReviveTime.wHour == st.wHour && 
			m_stReviveTime.wMinute > st.wMinute )
	{
		bRevive = TRUE;
	}
	else if(m_stReviveTime.wHour == st.wHour && 
			m_stReviveTime.wMinute >= st.wMinute &&
			m_stReviveTime.wSecond >= st.wSecond )
	{
		bRevive = TRUE;
	}*/

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsFobiddenSkill(DWORD SkillIdx)
{
	std::list<DWORD>::iterator BeginIter = m_ForbiddenSkillList.begin();

	while( BeginIter != m_ForbiddenSkillList.end() )
	{
		if( SkillIdx == *BeginIter )
			return TRUE;

		BeginIter++;
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsFobiddenItem(DWORD ItemIdx)
{
	std::list<DWORD>::iterator BeginIter = m_ForbiddenItemList.begin();

	while( BeginIter != m_ForbiddenItemList.end() )
	{
		if( ItemIdx == *BeginIter )
			return TRUE;

		BeginIter++;
	} 

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsFobiddenSkillTournament(DWORD SkillIdx)
{
	std::list<DWORD>::iterator BeginIter = m_ForbiddenSkillListTournament.begin();

	while( BeginIter != m_ForbiddenSkillListTournament.end() )
	{
		if( SkillIdx == *BeginIter )
			return TRUE;

		BeginIter++;
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsFobiddenItemTournament(DWORD ItemIdx)
{
	std::list<DWORD>::iterator BeginIter = m_ForbiddenItemListTournament.begin();

	while( BeginIter != m_ForbiddenItemListTournament.end() )
	{
		if( ItemIdx == *BeginIter )
			return TRUE;

		BeginIter++;
	} 

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsSiegeWarfareZone(DWORD MapNum)
{
	std::list<SiegeWarfareKindMapInfo>::iterator	itr	=	m_SiegeWarfareKindMapList.begin();

	for( itr; itr != m_SiegeWarfareKindMapList.end(); itr++ )
	{
		SiegeWarfareKindMapInfo MapInfo = *itr;
		
		if( MapInfo.MapNum == MapNum
			&& m_dwMapType > eNeraCastle_Village && m_dwMapType < eSiegeWarfare_Map_Max
			&& m_wState > eSiegeWarfare_State_Before )
			return TRUE;
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsSiegeWarfareZone()
{
	std::list<SiegeWarfareKindMapInfo>::iterator	itr	=	m_SiegeWarfareKindMapList.begin();

	for( itr; itr != m_SiegeWarfareKindMapList.end(); itr++ )
	{
		SiegeWarfareKindMapInfo MapInfo = *itr;
		
		if( MapInfo.MapNum == MAP->GetMapNum()
			&& m_dwMapType > eNeraCastle_Village && m_dwMapType < eSiegeWarfare_Map_Max
			&& m_wState > eSiegeWarfare_State_Before )
			return TRUE;
	}

	return FALSE;
}

BOOL CSiegeWarfareMgr::IsSiegeWarfareKindMap(DWORD MapNum)
{
	std::list<SiegeWarfareKindMapInfo>::iterator	itr	=	m_SiegeWarfareKindMapList.begin();

	for( itr; itr != m_SiegeWarfareKindMapList.end(); itr++ )
	{
		SiegeWarfareKindMapInfo MapInfo = *itr;
		
		if( MapInfo.MapNum == MapNum )
			return TRUE;
	}

	return FALSE;
}

void CSiegeWarfareMgr::NetworkMsgParse(BYTE Protocol, void* pMsg)
{
	switch( Protocol )
	{
	// ������ �߿� �޽����� �˸���.
	case MP_SIEGEWARFARE_NOTIFY:
		{
			MSG_DWORD* msg = (MSG_DWORD*)pMsg;
			
			switch( msg->dwData )
			{
			// ���� 30���� �޽���
			case eSiegeWarfare_Notify_Befor_30m:
				{
					//CHATMGR->AddMsg( CTC_TOGM, "���� ���� 30���� �Դϴ�." );
					NOTIFYMGR->AddMsg( CHATMGR->GetChatMsg( 1696 ), eNTC_EVENT) ;
					CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1696 ) );
				}
				break;

			// ���� 5���� �޽���
			case eSiegeWarfare_Notify_Befor_5m:
				{
					//CHATMGR->AddMsg( CTC_TOGM, "���� ���� 5���� �Դϴ�." );
					NOTIFYMGR->AddMsg( CHATMGR->GetChatMsg( 1697 ), eNTC_EVENT) ;
					CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1697 ) );
				}
				break;

			// ���� ���� �޽���
			case eSiegeWarfare_Notify_Begin:
				{
					//CHATMGR->AddMsg( CTC_TOGM, "�������� ���� �Ǿ����ϴ�.");
					NOTIFYMGR->AddMsg( CHATMGR->GetChatMsg( 1698 ), eNTC_EVENT) ;
					CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1698 ) );
				}
				break;

			// ���� �Ϸ� �޽���
			case eSiegeWarfare_Notify_End:
				{
					//CHATMGR->AddMsg( CTC_TOGM, "�������� �Ϸ� �Ǿ����ϴ�." );
					NOTIFYMGR->AddMsg(CHATMGR->GetChatMsg( 1699 ), eNTC_EVENT) ;
					CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1699 ) );
				}
				break;
			case eSiegeWarfare_Notify_End_Before_10m:
				{
					NOTIFYMGR->AddMsg(CHATMGR->GetChatMsg( 1716 ), eNTC_EVENT) ;
					CHATMGR->AddMsg( CTC_TOGM, CHATMGR->GetChatMsg( 1716 ) );
				}
				break;
			}
		}
		break;
	// ���� �õ� �Ϸ� �Ǿ����� �޴´�.
	case MP_SIEGEWARFARE_WATERSEED_COMPLETE:
		{
			MSG_WATERSEED_COMPLETE* pmsg = NULL ;
			pmsg = (MSG_WATERSEED_COMPLETE*)pMsg ;

			if( !pmsg ) return ;

			// 090720 pdy �븸 ���ÿ�û���� ���� ���� ���ͽõ� ��� ĳ���Ͱ� ������ ���ͽõ� �Ϸ� �޼��� ��� 

			char txt[1024] = {0, } ;

			if( pmsg->dwMapType == eNeraCastle_Lusen )
			{
				sprintf( txt,  CHATMGR->GetChatMsg( 1700 ), pmsg->szGuildName,  CHATMGR->GetChatMsg( 1701 ) ) ;
			}
			else if( pmsg->dwMapType == eNeraCastle_Zebin )
			{
				sprintf( txt, CHATMGR->GetChatMsg( 1700 ), pmsg->szGuildName, CHATMGR->GetChatMsg( 1702 ) ) ;
			}

			NOTIFYMGR->AddMsg(txt, eNTC_EVENT) ;
			CHATMGR->AddMsg( CTC_TOGM, txt );


			// ���ͽõ带 ����ϴ� ������Ʈ ������ �޴´�.
			CPlayer* pPlayer = NULL ;
			pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwObjectID ) ;

			if( !pPlayer )
			{
				//Throw_Error( "Failed to receive object!!", __FUNCTION__ ) ;
				return ;
			}


			//// �����ڰ� HERO���� ���θ� Ȯ���Ѵ�.
			//if( pPlayer->GetID() == HEROID )
			//{
			//	// ���ͽõ� ��� Progress dlg�� Ȱ��ȭ �Ѵ�.
			//	CProgressDialog* dialog = NULL ;
			//	dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG ) ;

			//	if( !dialog )
			//	{
			//		//Throw_Error( "Failed to receive progress dialog!!", __FUNCTION__ ) ;
			//		return ;
			//	}
			//	dialog->Wait( CProgressDialog::eActionUseWaterSeed ) ;
			//}

			Start_CravingDirection(pPlayer, eStep_End) ;

			// 081104 LYW --- SiegeWarfareMgr : ���ͽõ� ��ô ���� �� Player�� ���¸� �����Ѵ�.
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
		}
		break;
	// �������� ��Ȳ�� �޾� �´�.
	case MP_SIEGEWARFARE_STATE_ACK:
		{
			MSG_WORD2* msg = (MSG_WORD2*)pMsg;
			MMC_SiegeWarfareState(msg->wData2);
		}
		break;
	//���� �õ� ��� ���� �޽���
	case MP_SIEGEWARFARE_USE_WATERSEED_ACK:
		{
			// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ���� ó�� ����. �ٸ� Player�� �Ⱥ��̴� ���� ����.
			// ���� �޽��� ��ȯ.
			MSG_DWORD* pmsg = NULL ;
			pmsg = (MSG_DWORD*)pMsg ;

			if( !pmsg ) return ;


			// ���ͽõ带 ����ϴ� ������Ʈ ������ �޴´�.
			CPlayer* pPlayer = NULL ;
			pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData ) ;

			if( !pPlayer )
			{
				//Throw_Error( "Failed to receive object!!", __FUNCTION__ ) ;
				return ;
			}

			// 091123 ������ ���ͽõ� ����� �÷��� �߰� 
			pPlayer->SetUsingWaterSeedFlag( TRUE ) ;

			// �����ڰ� HERO���� ���θ� Ȯ���Ѵ�.
			if( pPlayer->GetID() == HEROID )
			{
				// ���ͽõ� ��� Progress dlg�� Ȱ��ȭ �Ѵ�.
				CProgressDialog* dialog = NULL ;
				dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG ) ;

				if( !dialog )
				{
					//Throw_Error( "Failed to receive progress dialog!!", __FUNCTION__ ) ;
					return ;
				}
				dialog->Wait( CProgressDialog::eActionUseWaterSeed ) ;
			}

			Start_CravingDirection(pPlayer, eStep_Start) ;
		}
		break;
	// ���� �õ� ��� ���� �޽���
	case MP_SIEGEWARFARE_USE_WATERSEED_NACK:
		{
			// 081009 LYW --- SiegeWarfareMgr : �����ҽ� ����.
			MSG_WORD* msg = (MSG_WORD*)pMsg;

			switch(msg->wData)
			{
			case eSiegeWarfare_WarterSeed_Error_NotState:
				CHATMGR->AddMsg( CTC_CHEAT_1,  CHATMGR->GetChatMsg( 1719 ) ) ;
				break;
			case eSiegeWarfare_WarterSeed_Error_UsingPlayer:
				CHATMGR->AddMsg( CTC_CHEAT_1,  CHATMGR->GetChatMsg( 1726 ) ) ;
				break;
			case eSiegeWarfare_WarterSeed_Error_NotGuildJoin:
				CHATMGR->AddMsg( CTC_CHEAT_1,  CHATMGR->GetChatMsg( 1724 ) ) ;
				break;
			case eSiegeWarfare_WarterSeed_Error_CastleGuld:
				CHATMGR->AddMsg( CTC_CHEAT_1,  CHATMGR->GetChatMsg( 1721 ) ) ;
				break;
			case eSiegeWarfare_WarterSeed_Error_GuildLevel:
				CHATMGR->AddMsg( CTC_CHEAT_1,  CHATMGR->GetChatMsg( 1725 ) ) ;
				break;

			// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ���� �Ÿ��� �ʹ� �ִٴ� �����޽��� �߰�.
			case eSiegeWarfare_WarterSeed_Error_SoFarDistance :
				CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1720 ) ) ;
				break ;
			}

			//MSGBASE* msg = (MSGBASE*)pMsg;

			//CHATMGR->AddMsg( CTC_CHEAT_1,  CHATMGR->GetChatMsg( 1703 ) ) ;

			// ĳ������ Deal ���¸� �����Ѵ�.
			OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Deal ) ;
		}
		break;
	// ���� ��� ���� �޽���
	case MP_SIEGEWARFARE_BUFF_NACK:
		{
			MSG_BYTE* msg = (MSG_BYTE*)pMsg ;

			switch( msg->bData )
			{
#ifdef _GMTOOL_
				case e_REQUEST_BUFFSKILL_FAILED_RECEIVE_NPCINFO :		
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Invalid npc info." ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFDATA :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Invalid buffdata." ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_FAILED_RECEIVE_PLAYERINFO :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Invalid player info." ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_TOO_FAR_DISTANCE :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Far Distance." ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_FAILED_FIND_BUFF :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Invalid buff." ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_FAILED_RECEIVE_BUFFSKILLINFO :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Invalid buffskill info." ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_INVALID_PLAYER :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, "Invalid player." ) ;
					}
					break ;
#endif //_GMTOOL_

				case e_REQUEST_BUFFSKILL_NOTENOUGH_PAY : 
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK,  CHATMGR->GetChatMsg( 1704 ) ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_INVALID_GUILD :				
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, CHATMGR->GetChatMsg( 1705 ) ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_INVALID_SIEGWZONE :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, CHATMGR->GetChatMsg( 1706 ) ) ;
					}
					break ;

				case e_REQUEST_BUFFSKILL_INVALID_TIME :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, CHATMGR->GetChatMsg( 1707 ) ) ;
					}
					break ;

				default :
					{
						WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, CHATMGR->GetChatMsg( 1708 ) ) ;
					}
					break ;
			}
		}
		break;
	// ���� ��� ����
	case MP_SIEGEWARFARE_BUFF_ACK:
		{
			MSG_DWORDEX* pmsg = (MSG_DWORDEX*)pMsg ;
			if( !pmsg ) return ;

			char szMsg[256] = {0, } ;
			if( pmsg->dweData > 0 )
			{
				sprintf(szMsg, CHATMGR->GetChatMsg( 1709 ), pmsg->dweData) ;
			}
			else
			{
				sprintf(szMsg, CHATMGR->GetChatMsg( 1710 ), pmsg->dweData) ;
			}

			WINDOWMGR->MsgBox( MBI_NPCBUFFSKILL, MBT_OK, szMsg ) ;
		}
		break;

	case MP_SIEGEWARFARE_USE_WATERSEED_CANCEL_ACK :
		{
			// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ��� ���� �޽��� ó�� ���� - �ٸ� ����� �Ⱥ��̴� ���� ����.
			// ���� �޽��� ��ȯ.
			MSG_DWORD* pmsg = NULL ;
			pmsg = (MSG_DWORD*)pMsg ;

			if( !pmsg ) return ;


			// ���ͽõ带 ����ϴ� ������Ʈ ������ �޴´�.
			CPlayer* pPlayer = NULL ;
			pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData ) ;

			if( !pPlayer )
			{
				//Throw_Error( "Failed to receive object!!", __FUNCTION__ ) ;
				return ;
			}

			pPlayer->SetUsingWaterSeedFlag( FALSE ) ;


			// �����ڰ� HERO���� ���θ� Ȯ���Ѵ�.
			if( pPlayer->GetID() == HEROID )
			{
				CProgressDialog* dialog = NULL ;
				dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG ) ;
				
				if( dialog )
				{
					dialog->SetActive(FALSE);
				}
			}

			Start_CravingDirection(pPlayer, eStep_End) ;
			OBJECTSTATEMGR->EndObjectState( pPlayer, eObjectState_Deal ) ;
		}
		break ;

		// 080922 LYW --- SiegeWarfareMgr : ���������� ���� ���̵� ó��.
	case MP_SIEGEWARFARE_CHANGEMAP_GUILDCOMMITTEE_NACK :
		{
			// ���� Ÿ�� ����
			// 0 : �� ������ ��尡 �ƴϴ�.
			// 1 : �� ������ �߸� �Ǿ���.

			MSG_BYTE* pmsg = NULL ;
			pmsg = (MSG_BYTE*)pMsg ;

			if( !pmsg ) return ;

			if( pmsg->bData == 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg( 1705 ) ) ;
			}
			else if( pmsg->bData == 1 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg( 1711 ) ) ;
			}
		}
		break ;

		// 080922 LYW --- SiegeWarfareMgr : ���� �̵� ���� ó�� �߰�.
	case MP_SIEGEWARFARE_MOVETODUNGEON_NACK : 
		{
			CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg( 1712 ) ) ;
		}
		break ;

		// 080929 LYW --- SiegeWarfareMgr : ����̿� - ���� ������ ��尡 ����.
	case MP_SIEGEWARFARE_CHECK_FLAGUSE_EMPTYMASTER :
		{
			WINDOWMGR->MsgBox(MBI_SIEGEWARFLAG_EXCEPT, MBT_OK, CHATMGR->GetChatMsg(1715) ) ;
		}
		break ;

		// 080929 LYW --- SiegeWarfareMgr : ����̿� - ����� �� �ִ� ���.
	case MP_SIEGEWARFARE_CHECK_FLAGUSE_ACK :
		{
			WINDOWMGR->MsgBox(MBI_SIEGEWARFLAG, MBT_YESNO, CHATMGR->GetChatMsg(1686)) ;
		}
		break ;

		// 080929 LYW --- SiegeWarfareMgr : ����̿� - ����� �� ���� ���.
	case MP_SIEGEWARFARE_CHECK_FLAGUSE_NACK :
		{
			// ���� �޽��� ��ȯ.
			SEND_GUILD_INVITE* pmsg = NULL ;
			pmsg = (SEND_GUILD_INVITE*)pMsg ;

			if( !pmsg ) return ;


			// ��� ���� ���̾�α׸� �޴´�.
			CSiegeWarFlagDlg* pDlg = NULL ;
			pDlg = GAMEIN->GetSiegeWarFlagDlg() ;

			if( !pDlg ) return ;


			// ��� ������ ����Ѵ�.
			if( pmsg->MasterIdx == 97 )
			{
				// 081009 LYW --- SiegeWarfareMgr : ���� �ҽ� ����.
				pDlg->SetFlagName( CHATMGR->GetChatMsg(1722) ) ;
				//pDlg->SetFlagName( "���(����)" ) ;
			}
			else if( pmsg->MasterIdx == 98 )
			{
				// 081009 LYW --- SiegeWarfareMgr : ���� �ҽ� ����.
				pDlg->SetFlagName( CHATMGR->GetChatMsg(1723) ) ;
				//pDlg->SetFlagName( "���(�罨)" ) ;
			}

			pDlg->SetGuildName( pmsg->GuildName );
			pDlg->SetMasterName( pmsg->MasterName );
			pDlg->SetActive(TRUE);
		}
		break ;

	// 081020 LYW --- SiegeWarfareMgr : ������ A�� B�� ���� ���� ó�� �߰� - �۰���.
	case MP_SIEGEWARFARE_NOTICE_KILLANDKILLER_ACK :
		{
			// ���� �޽��� ��ȯ.
			MSG_DWORD2* pmsg = NULL ;
			pmsg = (MSG_DWORD2*)pMsg ;

			if( !pmsg ) return ;


			// �����ڿ� �����ڸ� Ȯ���Ѵ�.
			CPlayer* pDeadMan = NULL ;
			pDeadMan = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData1) ;

			if( !pDeadMan ) return ;

			CPlayer* pKiller = NULL ;
			pKiller = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwData2) ;

			if( !pKiller ) return ;


			// ���� �̸��� �޾Ƽ� ���� �޽��� ó���� �Ѵ�.
			char noticeMsg[256] = {0, } ;
			sprintf( noticeMsg, CHATMGR->GetChatMsg(1622), pKiller->GetObjectName(), pDeadMan->GetObjectName() ) ;
			CHATMGR->AddMsg( CTC_CHEAT_1, noticeMsg ) ;
		}
		break ;

	// 081217 LYW --- SiegeWarfareMgr : ���� ��� �������� ��� �Ҽ� ���� �����, ĳ���� ���� ȭ������ �̵��ϴ� ó�� �߰�.
	case MP_SIEGEWARFARE_DIEINGUILDDUNGEON_NOTICE :
		{
			MSGBASE msg ;

			msg.Category	= MP_SIEGEWARFARE ;
			msg.Protocol	= MP_SIEGEWARFARE_WARPVILLAGE_SYN ;

			msg.dwObjectID	= HEROID ;

			NETWORK->Send( &msg, sizeof(MSGBASE) ) ;
		}
		break ;
	}
}

void CSiegeWarfareMgr::CM_RequestState()
{
	MSGBASE msg;
	SetProtocol(&msg, MP_SIEGEWARFARE, MP_SIEGEWARFARE_STATE_SYN);
	msg.dwObjectID	= gHeroID ;
	NETWORK->Send((MSGROOT*)&msg, sizeof(MSGBASE));
}

void CSiegeWarfareMgr::MMC_SiegeWarfareState(WORD State)
{
	m_wState = State;

	// 100311 pdy �������� ������ �����·� �ٲ��� (������ ���������� �Ǿ����� )
	if( m_wState == eSiegeWarfare_State_End )
	{
		m_wState = eSiegeWarfare_State_Before ;
	}

	if( SIEGEWARFAREMGR->IsSiegeWarfareZone() )
  	{
		// 081020 LYW --- GameIn : ������������ �̴ϸ�/���͹���/�����۸�/���� â�� ��Ȱ��ȭ �ϵ��� �Ѵ�. - �۰���
		// �̴ϸ� ��Ȱ��ȭ.
		CMiniMapDlg* pMiniMapDlg = NULL ;
        pMiniMapDlg = GAMEIN->GetMiniMapDialog() ;

		if( pMiniMapDlg )
		{
			pMiniMapDlg->SetActive(FALSE) ;
		}

		// ���� ���� â ��Ȱ��ȭ.
		CSHMonstermeterDlg* pMonstermeterDlg = NULL ;
		pMonstermeterDlg = GAMEIN->GetMonstermeterDlg() ;

		if( pMonstermeterDlg )
		{
			pMonstermeterDlg->SetActive(FALSE) ;
		}
  	}
}





//-------------------------------------------------------------------------------------------------
//	NAME		: IsEndUsing_WaterSeed
//	DESC		: ���ͽõ��� ����� �Ϸ�Ǿ����� ������ ��û�ϴ� �Լ�.
//	PROGRAMMER	: Yongs Lee
//	DATE		: August 27, 2008
//-------------------------------------------------------------------------------------------------
void CSiegeWarfareMgr::IsEndUsing_WaterSeed() 
{
	// ���ͽõ��� ����� �Ϸ�Ǿ����� ������ ��û�Ѵ�.
	MSGBASE	msg ;

	msg.Category	= MP_SIEGEWARFARE ;
	msg.Protocol	= MP_SIEGEWARFARE_ISCOMPLETE_USING_WATERSEED_SYN ;
	//msg.Protocol	= MP_SIEGEWARFARE_USE_WATERSEED_SYN ;

	msg.dwObjectID	= HEROID ;

	NETWORK->Send( &msg, sizeof(MSGBASE) ) ;
}

// 080903 KTH -- �д븦 ����Ҽ� �ִ°� �������϶� �̴�.
bool CSiegeWarfareMgr::UseCandleStand()
{
	if( !IsSiegeWarfareZone() && m_wState == eSiegeWarfare_State_Before )
		return FALSE;

	return TRUE;
}

// 080904 LYW --- SiegeWarfareMgr : ���� ���� �����ʿ��� �������� �����ϴ� ������ üũ�Ѵ�.
BOOL CSiegeWarfareMgr::Is_CastleMap()
{
	switch( m_dwMapType )
	{
	case eNeraCastle_UnderGround:				// ���� ���� ���� ��
	case eNeraCastle_Zebin:						// ���� ��������
	case eNeraCastle_Lusen:						// ���� �缾����
		return FALSE;
	}

	return TRUE;
}





// 081006 LYW --- SiegeWarfareMgr : ���������� ����� ���̵� ���� �ε�/��ȯ �Լ� �߰�.
void CSiegeWarfareMgr::Load_GuildMapMoveInfo()
{
	CMHFile file ;

	WORD	mapnum = 0;

	if(!file.Init("./System/Resource/MapMoveScroll_81.bin", "rb")) return ;

	while(!file.IsEOF())
	{
		st_GUILDMAPMOVEINFO info ;
//		SafeStrCpy(info.mapName, file.GetString(), 128 ) ;
		info.mapNum = file.GetWord() ;
		info.fXpos	= file.GetFloat() ;
		info.fZpos	= file.GetFloat() ;

		ASSERT(mapnum > 0 ) ;

		m_List_MapMove_Rushen.push_back( info ) ;
	}

	// 080919 LYW --- cMapMoveDialog : ���� ���̵� ����Ʈ ����.
	if(!file.Init("./System/Resource/MapMoveScroll_83.bin", "rb")) return ;

	while(!file.IsEOF())
	{
		st_GUILDMAPMOVEINFO info ;
//		SafeStrCpy(info.mapName, file.GetString(), 128 ) ;
		info.mapNum = file.GetWord() ;
		info.fXpos	= file.GetFloat() ;
		info.fZpos	= file.GetFloat() ;

		ASSERT(mapnum > 0 ) ;

		m_List_MapMove_Zevyn.push_back( info ) ;
	}

	file.Release() ;
}

void CSiegeWarfareMgr::Get_GuildMapMoveInfo_Rushen(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map)
{
	if( !pInfo ) return ;
	memset( pInfo, 0, sizeof( st_GUILDMAPMOVEINFO ) ) ;

	L_GUILDMAPMOVEINFO::iterator it ;

	for( it = m_List_MapMove_Rushen.begin() ; it != m_List_MapMove_Rushen.end() ; ++it )
	{
		st_GUILDMAPMOVEINFO info = *it ;

		if( info.mapNum != map ) continue ;

		pInfo->mapNum	= info.mapNum ;
		pInfo->fXpos	= info.fXpos ;
		pInfo->fZpos	= info.fZpos ;

		return ;
	}
}

void CSiegeWarfareMgr::Get_GuildMapMoveInfo_Zevyn(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map)
{
	if( !pInfo ) return ;
	memset( pInfo, 0, sizeof( st_GUILDMAPMOVEINFO ) ) ;

	L_GUILDMAPMOVEINFO::iterator it ;

	for( it = m_List_MapMove_Zevyn.begin() ; it != m_List_MapMove_Zevyn.end() ; ++it )
	{
		st_GUILDMAPMOVEINFO info = *it ;

		if( info.mapNum != map ) continue ;

		pInfo->mapNum	= info.mapNum ;
		pInfo->fXpos	= info.fXpos ;
		pInfo->fZpos	= info.fZpos ;

		return ;
	}
}





// 081015 LYW --- SiegeWarfareMgr : ���ͽõ� ���� ������ �ϴ� �Լ� �߰�.
// 081017 lyw --- SiegeWarfareMgr : ���ͽõ� ���� ���� ���� - �ٸ� Player�� �����ϴ°� �Ⱥ��̴� ���� ����.
//void CSiegeWarfareMgr::Start_CravingDirection(BYTE byStep)
void CSiegeWarfareMgr::Start_CravingDirection(CPlayer* pPlayer, BYTE byStep)
{
	// 081017 LYW --- SiegeWarfareMgr : �Լ� ���� ��ȿ Ȯ��.
	if( !pPlayer ) return ;


	// NPC�� ����ũ �ε����� Ȯ���Ѵ�.
	DWORD dwUniqueIdx = 0 ;
	

	// �� Ÿ���� Ȯ���Ѵ�.
	switch(SIEGEWARFAREMGR->Get_SiegeMapType())
	{
	case eNeraCastle_Zebin :	dwUniqueIdx = 131 ;	break ;

	case eNeraCastle_Lusen :	dwUniqueIdx = 132 ; break ;

	default : return ;
	}


	// Npc ��ġ ������ �޴´�.
	VECTOR3* pNpcPos = NULL ;
	pNpcPos = Get_WellPos( dwUniqueIdx ) ;

	if( !pNpcPos ) return ;


	// �Ÿ��� Ȯ���Ѵ�.
	float fDistance = CalcDistanceXZ( &pPlayer->GetCurPosition(), pNpcPos ) ;

	if( fDistance > m_wCravingDistance ) 
	{
		CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg( 1720 ) ) ;
		return ;
	}

	//// pPlayer�� ������ �����Ѵ�.
	VECTOR3 dir = *pNpcPos - pPlayer->GetCurPosition() ;
	dir = dir / fDistance;

	float fAngle = VECTORTORAD(dir);
	pPlayer->SetAngle( fAngle );

	// ���� �ε����� �޴´�.
	BYTE byRacial = RaceType_Human ;
	byRacial = pPlayer->GetRace() ;


	// ������ �޴´�.
	BYTE byGender = GENDER_MALE ;
	byGender = pPlayer->GetGender() ;


	// ������ �������̴� ������ �����Ѵ�.
	//pPlayer->RemoveObjectEffect(SIEGEWARFARE_USING_WATERSEED) ;
	m_WaterSeedEffect->EndEffect();

	// 091211 pdy ���ͽõ� ���� Ÿ�ټ����� �ȵǴ� ���� ����
	CNpc* pTargetNpc = OBJECTMGR->GetNpcByUniqueIdx((WORD)dwUniqueIdx) ;

	if( pTargetNpc == NULL )
		return;

	MAINTARGET MainTarget ;
	MainTarget.SetMainTarget( pTargetNpc->GetID() ) ;

	// �з��� ���� ó���� �Ѵ�.
	switch( byRacial )
	{
	case RaceType_Human :
		{
			switch(byGender)
			{
			case GENDER_MALE :
				{
					switch( byStep )
					{
					case eStep_Start :
						{
							WORD wEffect = (WORD)FindEffectNum("H_M_siege_start_a.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
							
						}
						break ;

					case eStep_Continue :
						{
							WORD wEffect = (WORD)FindEffectNum("H_M_siege_carve_b.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;
		
					case eStep_End :
						{
							WORD wEffect = (WORD)FindEffectNum("H_M_siege_finish_c.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					default : break ;
					}
				}
				break ;

			case GENDER_FEMALE :
				{
					switch( byStep )
					{
					case eStep_Start :
						{
							WORD wEffect = (WORD)FindEffectNum("H_W_siege_start_a.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					case eStep_Continue :
						{
							WORD wEffect = (WORD)FindEffectNum("H_W_siege_carve_b.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;
		
					case eStep_End :
						{
							WORD wEffect = (WORD)FindEffectNum("H_W_siege_finish_c.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;

						}
						break ;

					default : break ;
					}
				}
				break ;

			default : break ;
			}
		}
		break ;

	case RaceType_Elf :
		{
			switch(byGender)
			{
			case GENDER_MALE :
				{
					switch( byStep )
					{
					case eStep_Start :
						{
							WORD wEffect = (WORD)FindEffectNum("E_M_siege_start_a.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					case eStep_Continue :
						{
							WORD wEffect = (WORD)FindEffectNum("E_M_siege_carve_b.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;
		
					case eStep_End :
						{
							WORD wEffect = (WORD)FindEffectNum("E_M_siege_finish_c.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					default : break ;
					}
				}
				break ;

			case GENDER_FEMALE :
				{
					switch( byStep )
					{
					case eStep_Start :
						{
							WORD wEffect = (WORD)FindEffectNum("E_W_siege_start_a.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					case eStep_Continue :
						{
							WORD wEffect = (WORD)FindEffectNum("E_W_siege_carve_b.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;
		
					case eStep_End :
						{
							WORD wEffect = (WORD)FindEffectNum("E_W_siege_finish_c.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					default : break ;
					}
				}
				break ;

			default : break ;
			}
		}
		break ;
	// 090520 ONS �ű����� ���� ó�� �߰�
	case RaceType_Devil :
		{
			switch(byGender)
			{
			case GENDER_MALE :
				{
					switch( byStep )
					{
					case eStep_Start :
						{
							WORD wEffect = (WORD)FindEffectNum("E_M_siege_start_a.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					case eStep_Continue :
						{
							WORD wEffect = (WORD)FindEffectNum("E_M_siege_carve_b.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;
		
					case eStep_End :
						{
							WORD wEffect = (WORD)FindEffectNum("E_M_siege_finish_c.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					default : break ;
					}
				}
				break ;

			case GENDER_FEMALE :
				{
					switch( byStep )
					{
					case eStep_Start :
						{
							WORD wEffect = (WORD)FindEffectNum("E_W_siege_start_a.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					case eStep_Continue :
						{
							WORD wEffect = (WORD)FindEffectNum("E_W_siege_carve_b.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;
		
					case eStep_End :
						{
							WORD wEffect = (WORD)FindEffectNum("E_W_siege_finish_c.beff");
							m_WaterSeedEffect->Init( wEffect , pPlayer , &MainTarget );
							m_WaterSeedEffect->StartEffect( FALSE ) ;
						}
						break ;

					default : break ;
					}
				}
				break ;

			default : break ;
			}
		}
		break ;

	default : break ;
	}
}




// 081019 LYW --- SiegeWarfareMgr : �칰 ��ġ �ε� �Լ� �߰�.
void CSiegeWarfareMgr::Load_WellPos()
{
	CMHFile file;
	file.Init("System/Resource/StaticNpc.bin","rb") ;

	if(file.IsInited() == FALSE) return ;

	DWORD NpcUniqueIdx ;
	char Name[256] = { 0, } ;

	while(1)
	{
		if(file.IsEOF()) break ;

		file.GetDword() ;
		file.GetWord() ;
		file.GetString(Name) ;
		NpcUniqueIdx = file.GetWord() ;

		if( NpcUniqueIdx == 131 || NpcUniqueIdx == 132 )
		{
			VECTOR3 vPos ;
			memset(&vPos, 0, sizeof(VECTOR3)) ;

			vPos.x = static_cast<float>(file.GetDword()) ;
			vPos.z = static_cast<float>(file.GetDword()) ;

			m_MWellPos.insert( std::make_pair( NpcUniqueIdx, vPos ) ) ;
		}
		else
		{
			file.GetDword() ;
			file.GetDword() ;
		}

		file.GetFloat() ;
		file.GetByte();
	}
	
	file.Release() ;
}





// 081019 LYW --- SiegeWarfareMgr : �칰 ��ġ ��ȯ �Լ� �߰�.
VECTOR3* CSiegeWarfareMgr::Get_WellPos(DWORD dwUniqueIdx)
{
	M_WELLPOS::iterator it ;

	for( it = m_MWellPos.begin() ; it != m_MWellPos.end() ; ++it )
	{
		if( it->first != dwUniqueIdx ) continue ;

		return &it->second ;
	}

	return NULL ;
}