#include "stdafx.h"
#include "GameIn.h"
//#ifdef _CHEATENABLE_
#include "CheatMsgParser.h"
#include "CharacterDialog.h"
#include "MainGame.h"
#include "WindowIdEnum.h"
#include "./Interface/cWindowManager.h"
#include "BattleSystem_client.h"
#include "TutorialDlg.h"
#include "ConductManager.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "MoveManager.h"
#include "MHTimeManager.h"
#include "QuickManager.h"
#include "PartyManager.h"
#include "ObjectActionManager.h"
#include "GuildManager.h"
#include "StreetStallManager.h"
#include "StorageManager.h"
#include "FriendManager.h"
#include "NoteManager.h"
#include "ChatManager.h"
#include "mhmap.h"
#include "AliasManager.h"
#include "SlotDialog.h"
#include "ReportDialog.h"
#include "ChaptchaDlg.h"

// 070228 LYW --- GameIn : Include header file of facial manager.
#include "FacialManager.h"

//050630SW
#include "cChatTipManager.h"
//-
#include "ExchangeManager.h"
#include "NpcScriptManager.h"
#include "./Audio/MHAudioManager.h"
#include "OptionManager.h"
#include "MHCamera.h"
#include "./Input/UserInput.h"
#include "MouseCursor.h"
#include "MiniMapDlg.h"
#include "NewWeatherDialog.h"
#include "PartyIconManager.h"
#include "mhMap.h"
#include "cMsgBox.h"

#include "MotionManager.h"
#include "objectstatemanager.h"
#include "CharacterCalcManager.h"
#include "PKManager.h"
#include "GMNotifyManager.h"
#include "ReinforceDlg.h"
#include "NpcScriptDialog.h"
//#include "MixManager.h"
#include "StatusIconDlg.h"
#include "PetStatusIconDialog.h"
#include "CharSelect.h"
#include "StreetTournamentMgr.h"

#include "Showdownmanager.h"

#include "UserInfoManager.h"
#include "ExitManager.h"
#include "HackCheck.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif

#include "Quest.h"
#include "QuestManager.h"

#include "MapChange.h"
#include "GuildFieldWar.h"
#include "ItemShopDialog.h"
#include "StorageDialog.h"
#include "GuildTournamentMgr.h"
#include "GuildUnion.h"
#include "GuageDialog.h"
#include "MainSystemDlg.h"
#include "cResourceManager.h"
#include "cScriptManager.h"
#include "Interface/GameNotifyManager.h"
#include "DateMatchingDlg.h"
#include "AutoNoteManager.h"
#include "./ChatRoomMgr.h"
#include "FishingManager.h"
//aziz MallShop 24
#include "VipMgr.h"
//aziz Reborn 24 Sep
#include "UtilityMgr.h"
//aziz Kill Shop 30 Sep
#include "KillMgr.h"
#include "cSkillTreeManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "cSkillTrainingDlg.h"
#include "cSkillTrainingDlgJct.h" //JobTree
#include "FamilyDialog.h"
#include "NpcRecallMgr.h"
#include "DealDialog.h"
#include "PCRoomManager.h"
#include "AppearanceManager.h"

#ifdef _TESTCLIENT_
#include "TSToolManager.h"
#include "AppearanceManager.h"

void InitTestCharacters(void);
void InitTestHero();
void InitTestMonster();
#endif

#include "../hseos/SHMain.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "../hseos/Debug/SHDebug.h"
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmUpgradeDlg.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "TutorialManager.h"
#include "cMapMoveDialog.h"
#include "./Interface/cNumberPadDialog.h"
#include "WeatherManager.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHDateManager.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "PetManager.h"
#include "SiegeWarfareMgr.h"
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "LimitDungeonMgr.h"
#include "cCookMgr.h"
#include "VehicleManager.h"
#include "cHousingMgr.h"
#include "Trigger/Manager.h"
#include "DungeonMgr.h"
#include "UiSettingManager.h"
#include "ChannelDialog.h"
#include "ConsignmentMgr.h"
#include "ConsignmentDlg.h"
#include "cHousingWebDlg.h"

#include "MonsterGuageDlg.h"
#include "AntiBotManager.h"
#include "MarryManager.h"
#include "InventoryExDialog.h"
//inspect
#include "otherCharacterEquip.h"
#ifdef _NPROTECT_
#include "NProtectManager.h"
#endif

//Alemuri Server Time Update---
#include "ServerTimeDialog.h"
//-----------------------------

//Alemuri MobStats-------------
#include "BattleGuageDlg.h"
#include <iomanip>
#include <string>  
#include <iostream> 
#include <sstream>   
//-----------------------------

//Alemuri get skill lists from mobs---------------------
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
//------------------------------------------------------
#include "../[Client]LUNA/PartyWar/PWScoreInfoDialog.h"
//

DWORD g_TempHeroID;
extern HWND _g_hWnd;
extern VECTOR3 gHeroPos;

GLOBALTON(CGameIn)


CGameIn::CGameIn()
{
	m_bGameInAcked	= FALSE;
	m_bInitForGame	= FALSE;
	m_GameInInitKind = eGameInInitKind_Login;

	m_pInventoryDlg		= NULL;
	m_pExchangeDlg		= NULL;
	m_pMixDlg			= NULL;
	m_pDealDlg			= NULL;
	m_pCharDlg			= NULL;
	m_pChattingDlg		= NULL;
	m_pOSChattingDlg	= NULL;
	m_pMainSystemDlg	= NULL;
	m_pDebugDlg	= NULL;
	m_pStallKindSelectDlg	= NULL;
	m_pStreetStallDlg	= NULL;
	m_pNpcScriptDlg		= NULL;
	m_pChatOptionDlg	= NULL;
	m_pOptionDlg		= NULL;
	m_pExitDlg			= NULL;
	m_pMonstermeterDlg	= NULL;
	m_pFarmBuyDlg		= NULL;
	m_pFarmUpgradeDlg	= NULL;
	m_pFarmManageDlg	= NULL;
	m_pDateZoneListDlg			= NULL;
	m_pChallengeZoneListDlg		= NULL;
	m_pChallengeZoneClearNo1Dlg	= NULL;
	m_MoneyDlg			= NULL;
	m_pQuickDlg			= NULL;
	m_pGuageDlg			= NULL;

	m_pMiniMapDlg		= NULL;
	m_pBigMapDlg		= NULL;

	m_pStorageDialog		= NULL;
	m_pNoteDialog		= NULL;
	m_pFriendDialog		= NULL;
	m_pMiniNoteDialogWrite	= NULL;
	m_pMiniNoteDialogRead = NULL;
	m_pMiniFriendDialog	= NULL;
	m_pReviveDialog		= NULL;
	m_pWantNpcDialog	= NULL;

	m_pMonsterGuageDlg	= NULL;
	m_pDissolveDig	= NULL;
	m_pItemShopDialog	= NULL;
	m_MovePoint = 0;
	m_MoveMap	= 0;
	m_DestMoveMap = 0;
	m_GuildIdx	= 0;
	m_SiegeWarMapNum	= 0;

	m_pShoutDlg			= NULL;
	m_pFishingDlg		= NULL;
	m_pFishingGaugeDlg	= NULL;
	m_pFishingPointDlg	= NULL;
	m_pDateMatchingDlg	 = NULL;
	m_pIdentificationDlg = NULL;
	m_pFavorIconDlg		= NULL;
	m_pMatchMyInfoDlg	= NULL;
	m_pMatchPartnerInfoDlg	= NULL;
	m_pMatchRecordDlg	= NULL ;
	ZeroMemory(
		&GameinServerTime,
		sizeof(GameinServerTime));
	m_Clientdate = 0;
	m_Clienttime = 0;

	m_pGameNotifyDlg = NULL;
	m_TutorialDlg = NULL;
	m_HelpDlg = NULL;
	m_TutorialBtnDlg = NULL;
	m_MapMoveDlg = NULL;
	m_pChangeNameDlg = NULL;
	m_pNumberPadDlg = NULL;
	
	m_pAutoNoteDlg = NULL;
	m_pAutoAnswerDlg = NULL;
	m_pChatRoomMainDlg = NULL;
	m_pChatRoomCreateDlg = NULL;
	m_pChatRoomDlg = NULL;
	m_pChatRoomGuestListDlg	= NULL;
	m_pChatRoomOptionDlg = NULL ;
	m_pChatRoomJoinDlg = NULL ;
	m_pCookDlg = NULL;
	m_pSiegeFlagDlg = NULL;
	m_pFadeDlg = NULL;
	m_pBattleGuageDlg	=	NULL;
	m_bAfterRender = TRUE;
	m_pNewWeatherDlg = NULL;
	m_pSlotDlg			= NULL;
	m_pReportDlg			= NULL;
	m_pChaptchaDlg = NULL;
	//inspect
	m_pCharacterEquipInfo = NULL;

	//Alemuri Server Time Update
	m_pServerTimeDlg = NULL;
	//---------------------------

	//Alemuri Party War----------
	m_pCurShowImage = NULL;
	m_WinnerShowImage = NULL;
	m_battleState = 3;
	m_WinnerTeam = 0;
	partyWarResetAll();
	nTeam1Score = 0;
	nTeam2Score = 0;
	//---------------------------
	
	//Alemuri Search Drop Items----------------------
	m_tabIndex = 0;
	//-----------------------------------------------
}

CGameIn::~CGameIn()
{

}

#ifdef _TESTCLIENT_
void InitForTestClient(int GameInInitKind)
{
	if( GameInInitKind != eGameInInitKind_MapChange)
	{
		MAINGAME->LoadPack( "data/3dData/character.gpk" );
		MAINGAME->LoadPack( "data/3dData/galaxychar.gpk" );
		MAINGAME->LoadPack( "data/3dData/map.gpk" );
		MAINGAME->LoadPack( "data/interface/2dImage/image.gpk" );
		MAINGAME->LoadPack( "data/3dData/effect.gpk" );
		MAINGAME->LoadPack( "data/3dData/monster.gpk" );
		MAINGAME->LoadPack( "data/3dData/npc.gpk" );
		MAINGAME->LoadPack( "data/interface/2dImage/npcImage.gpk" );

		while(TRUE == MAINGAME->IsLoading());

		GAMERESRCMNGR->LoadTestClientInfo();
		APPEARANCEMGR->Init();
		MAP->InitMap(GAMERESRCMNGR->m_TestClientInfo.Map);
	}

	GAMEIN->m_bGameInAcked = TRUE;

	//LOGFILE("OPTIONMGR->ApplySettings()");
	OPTIONMGR->ApplySettings();
}
#endif

BOOL CGameIn::InitForGame(int GameInInitKind)
{
	m_bInitForGame = TRUE;

	WINDOWMGR->CreateGameIn();
	WINDOWMGR->AfterInit();
	USERINFOMGR->LoadUserInfo(eUIK_INTERFACE);
	CHATMGR->SetChattingDlg( GAMEIN->GetChattingDlg() ) ;
	CHATMGR->RestoreChatList();	
	ITEMMGR->ClearCoolTimeInfo();

	// 080411 LYW --- GameIn : Ã¤ÆÃ¹æ¿¡¼­ Ã¤ÆÃÁßÀÌ ¾ú´Ù¸é, Ã¤ÆÃ ³»¿ëÀ» º¹±¸ÇÑ´Ù.
	CHATROOMMGR->RestoreChat() ;

	QUESTMGR->InitializeQuest();
	PKMGR->Init();
	GUILDUNION->Clear();
	OPTIONMGR->InitForGameIn();
	HOUSINGMGR->Init();

	return TRUE;
}

BOOL CGameIn::Init(void* pInitParam)
{
	int GameInInitKind = *(int*)pInitParam;
	m_GameInInitKind = GameInInitKind;
	m_bGameInAcked = FALSE;

#ifdef _TESTCLIENT_
	InitForTestClient(GameInInitKind);
#endif

	InitForGame(GameInInitKind);
	APPEARANCEMGR->EnbaleReservation();

	if(MAP->GetMapNum() != 0)
	{
		MAP->LoadStaticNpc(MAP->GetMapNum());

		MAP->LoadNpcForQuestDialog() ;
	}


	SHOWDOWNMGR->Init();
	EXCHANGEMGR->Init();
	STREETSTALLMGR->Init();
	FRIENDMGR->Init();
	PARTYMGR->Init();
	GUILDMGR->Init();
	WEATHERMGR->Init();
	FISHINGMGR->Init();
	COOKMGR->Init();
	SKILLTREEMGR->Init();
	CONDUCTMGR->RegistConduct() ;
	CSHMain::Init();

	//Alemuri Party War----------
	GAMEIN->setCurrentRenderState(3);
	//---------------------------

#ifdef _TESTCLIENT_
	InitTestHero();
	InitTestMonster();
	InitTestCharacters();

	if( TSTOOLMGR->IsShowing() )
		TSTOOLMGR->ShowTSDialog( TRUE );
#endif

#ifdef _GMTOOL_
	if( GMTOOLMGR->IsShowing() )
		GMTOOLMGR->ShowGMDialog( TRUE );
#endif
	
	NETWORK->SetCurState(this);

	switch(GameInInitKind)
	{
	case eGameInInitKind_Login:
	case eGameInInitKind_MapChange:
		{
			MSG_DWORD msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_GAMEIN_SYN;
			msg.dwData	 = gChannelNum;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	case eGameInInitKind_MovePos:
		{
			MSG_DWORD2 msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_GAMEINPOS_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.dwData1 = gChannelNum;
			msg.dwData2 = m_MovePoint;

			NETWORK->Send( &msg, sizeof(msg) );			
		}
		break;
	case eGameInInitKind_GTEnter:
		{
			MSG_DWORD3 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_BATTLEJOIN_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.dwData1 = m_GuildIdx;
			msg.dwData2 = 0;
			msg.dwData3 = m_MoveMap;
			NETWORK->Send( &msg, sizeof(msg) );
			
			m_MoveMap = 0;
		}
		break;
	case eGameInInitKind_GTLeave:
		{
			MSGBASE msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_LEAVE_SYN;
			msg.dwObjectID = g_TempHeroID;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	case eGameInInitKind_GTObserverEnter:
		{
			MSG_DWORD3 msg;
			msg.Category = MP_GTOURNAMENT;
			msg.Protocol = MP_GTOURNAMENT_OBSERVERJOIN_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.dwData1 = m_GuildIdx;
			msg.dwData2 = m_BattleIdx;
			msg.dwData3 = m_MoveMap;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	case eGameInInitKind_DungeonObserverEnter:
		{
			MSG_WORD3 msg;
			msg.Category = MP_CHEAT;
			msg.Protocol = MP_CHEAT_DUNGEON_OBSERVERJOIN_SYN;
			msg.dwObjectID = g_TempHeroID;
			msg.wData1 = WORD(m_DestMoveMap);
			msg.wData2 = WORD(m_BattleIdx);
			msg.wData3 = WORD(m_MoveMap);
			
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	}
	ITEMMGR->itDebug();
#ifdef _TESTCLIENT_
	g_UserInput.SetInputFocus( TRUE );
	USERINFOMGR->SetMapChage( FALSE );
#endif

//Alemuri Preload Monster Data for each Map--------------------------------------------------------------------------
	if (preloadMonsterInMap.size() <= 0)
	{
		std::vector<DWORD> emptyNumberZero;
		emptyNumberZero.push_back(0);
		preloadMonsterInMap.push_back(emptyNumberZero); //fill the index 0 with an empty vector since Map 0 doesn't exist
		std::string mapName = "";
		for (int nMap = 1; nMap <= 150; nMap++)
		{
			sPRELOAD_INFO* pPreLoadInfo = new sPRELOAD_INFO;
			pPreLoadInfo->MapNum = nMap;
	
			GAMERESRCMNGR->LoadPreMonsterData(
			pPreLoadInfo,
			MAPTYPE(pPreLoadInfo->MapNum));

			if (pPreLoadInfo)
			{
				BASE_MONSTER_LIST* pBMonInfo = NULL;
				DIRECTORYMGR->SetLoadMode(eLM_Monster);

				for(int i=0; i<pPreLoadInfo->Count[ePreLoad_Monster]; i++)
				{
					pBMonInfo = GAMERESRCMNGR->GetMonsterListInfo(pPreLoadInfo->Kind[ePreLoad_Monster][i]);
	
					preloadMonsterInMapList.push_back(pBMonInfo->MonsterKind);
				}
				preloadMonsterInMap.push_back(preloadMonsterInMapList); //Fill the Vector of the map nMap
				preloadMonsterInMapList.clear(); //Clean after every Map Vector Completed
			}
		}
	}
//---------------------------------------------------------------------------------------------------------------

	return TRUE;
}

void CGameIn::ReleaseForGame()
{
	if( !m_bInitForGame ) return;

	while(FALSE == m_pBackupMsgQueue.empty())
	{
		MSGBASE* pMsg = (MSGBASE*)m_pBackupMsgQueue.front();
		m_pBackupMsgQueue.pop();
		if(pMsg)
			SAFE_DELETE(pMsg);
	}

	UISETTINGMGR->UpdateSettingInfo();

	WINDOWMGR->DestroyWindowAll();
	
	// ---- weather custom : fix crash when goto select character
#ifdef _FORMOON_
	m_pCWeatherDialog = NULL;
#else
	m_pNewWeatherDlg = NULL;
#endif

	WEATHERMGR->EffectOff();

	m_bInitForGame = FALSE;

	m_pInventoryDlg			= NULL;
	m_pExchangeDlg			= NULL;
	m_pMixDlg				= NULL;
	m_pDealDlg				= NULL;
	m_pCharDlg				= NULL;
	m_pChattingDlg			= NULL;
	m_pOSChattingDlg		= NULL;
	m_pMainSystemDlg		= NULL;
	m_pDebugDlg				= NULL;
// LYJ 051017 ±¸ÀÔ³ëÁ¡»ó Ãß°¡
	m_pStallKindSelectDlg	= NULL;
	m_pStreetStallDlg		= NULL;
	m_pNpcScriptDlg			= NULL;
	m_pChatOptionDlg		= NULL;
	m_pOptionDlg			= NULL;
	m_pExitDlg				= NULL;
	m_MoneyDlg				= NULL;
	m_pQuickDlg				= NULL;
	m_pGuageDlg				= NULL;

	m_pMiniMapDlg			= NULL;
	m_pBigMapDlg			= NULL;

	m_pStorageDialog			= NULL;
	m_pNoteDialog			= NULL;
	m_pFriendDialog			= NULL;
	m_pMiniNoteDialogWrite	= NULL;
	m_pMiniNoteDialogRead	= NULL;
	m_pMiniFriendDialog		= NULL;
	m_pReviveDialog			= NULL;
	m_pWantNpcDialog		= NULL;

	m_pMonsterGuageDlg		= NULL;

	m_pReinforceDlg			= NULL;

	m_pDissolveDig			= NULL;
	m_pShoutDlg				= NULL;
// 22-May-2012 Death
	m_pNewWeatherDlg		= NULL;
	m_pSlotDlg				= NULL;
	//inspect
	m_pCharacterEquipInfo = NULL;

	//Alemuri Server Time Update
	m_pServerTimeDlg = NULL;
	//---------------------------
}

void CGameIn::Release(CGameState* pNextGameState)
{
	APPEARANCEMGR->DisableReservation();
	BATTLESYSTEM->ReleaseBattle();
	STATUSICONDLG->Release();

	CURSOR->SetActive( FALSE );
	CHATMGR->SetChattingDlg( NULL );

	AUDIOMGR->StopAll();
	QUICKMGR->Release();
	ITEMMGR->Release();

	SKILLTREEMGR->Release();
	PKMGR->Release();
	USERINFOMGR->SaveUserInfo( eUIK_USERSTATE | eUIK_INTERFACE | eUIK_USERQUEST );
	EFFECTMGR->RemoveAllEffect();
	OBJECTMGR->RemoveAllObject();
	TRIGGERMGR->RestoreTile();
	MAP->Release();

	GUILDWARMGR->Release();
	PICONMGR->DeleteAllPartyIcon();

	ReleaseForGame();

	HACKCHECK->Release();

#ifdef _GMTOOL_
	GMTOOLMGR->ShowGMDialog( FALSE );
#elif _TESTCLIENT_
	TSTOOLMGR->ShowTSDialog( FALSE );
#endif

	SKILLMGR->ReleaseAllSkillObject();
	CAMERA->Release() ;

	g_pExecutive->UnloadAllPreLoadedGXObject(0);

	g_TempHeroID = gHeroID;
	gHeroID = 0;
}

extern WORD mapnumforMSGBOX = 0;
void CGameIn::Process()
{
	if(!m_bGameInAcked)
		return;

	CHATTIPMGR->Process();
	
	GetMiniMapDialog()->Process();

	EXITMGR->Process();
	ITEMMGR->Process();
	SKILLMGR->Process();
	FISHINGMGR->Process();
	GTMGR->Process();
	SIEGEWARFAREMGR->Process();

	//---KES TEST ³Ê¹« Áö¼ÓÀûÀ¸·Î µ¹°í ÀÖ´Â °ÍÀº ¾Æ´Ò±î?
	// 070121 LYW --- Call function to check potal position.
	if( MOVEMGR->IsMoving(HERO) && !HERO->IsPKMode() && !PKMGR->IsPKLooted() && !HERO->IsDied() && MAP->GetMapNum()!=PVP)//Beyond SEA2 warning
	{
		// 100211 ShinJS --- Å»°Í¿¡ Å¾½ÂÁßÀÎ °æ¿ì ¼ÒÀ¯ÁÖ¸¸ Æ÷Å» ÀÌµ¿°¡´É
		const BOOL bInvalidVehicleCheck = (HERO->IsGetOnVehicle() && !VEHICLEMGR->IsMaster( HEROID ));
        if( !bInvalidVehicleCheck )
		{
			WORD curMapNum = HERO->GetCharacterTotalInfo()->CurMapNum;
			WORD mapNum = GAMERESRCMNGR->CheckMapChangeForMapID(&gHeroPos, curMapNum);
			mapnumforMSGBOX = GAMERESRCMNGR->CheckMapChange(&gHeroPos, curMapNum);
			//if( mapNum != 65535 )
			//	//WINDOWMGR->MsgBox(
			//	//MBI_MAPMOVE_AREYOUSURE,
			//	//MBT_YESNO,
			//	//CHATMGR->GetChatMsg(480),
			//	//GetMapName(mapNum));
			//	g_UserInput.GetHeroMove()->ChangeMap(mapnumforMSGBOX); //Alemuri Change Map without MsgBox
			//	//g_UserInput.GetHeroMove()->ChangeMap( mapNum ); //this was old map change in original server
			if (mapNum != 65535) //johan message box saat lewat portal  map
				WINDOWMGR->MsgBox(
				MBI_MAPMOVE_AREYOUSURE,
				MBT_YESNO,
				CHATMGR->GetChatMsg(480),
				GetMapName(mapNum));
			//g_UserInput.GetHeroMove()->ChangeMap( mapNum );
		}
	}

	CSHMain::MainLoop();
	LIMITDUNGEONMGR->Process();
	HACKCHECK->Process();
	HOUSINGMGR->Process();
	DungeonMGR->Process();
#ifdef _NPROTECT_
	NPROTECTMGR->Check();
#endif
	WEATHERMGR->Process();
	ANTIBOTMGR->Process();
	QUESTMGR->Process();
	//sort test
	CInventoryExDialog* pDlg = GetInventoryDialog();
	pDlg->SortWait();
}

void CGameIn::BeforeRender()
{
	MAP->Process(gCurTime);
}

void CGameIn::AfterRender()
{
	if(m_bGameInAcked && m_bAfterRender)
	{
		EFFECTMGR->Process(gCurTime);

		BATTLESYSTEM->Render();
		WINDOWMGR->Render();
		NOTIFYMGR->Render();
		GAMENOTIFYMGR->Render();
		renderPartyWar();

		if(MAP->GetMapNum() == StreetTournament)
		{
			STREETTOURNAMENTMGR->Proses();
		}
#if defined(_GMTOOL_) || defined(_TESTCLIENT_)
		CSHMain::Render();
		MHTIMEMGR->DrawStatus();
		MAP->RenderTileData_Debug();
		WEATHERMGR->RenderDebugState();
#endif
	}
}

// description by hseos S
// ¼­¹ö->Å¬¶óÀÌ¾ðÆ® Åë½Å parsing À» À§ÇÑ µî·Ï ºÎºÐ
// description by hseos E
void CGameIn::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	if(NULL == HERO &&
		Category != MP_ITEM &&
		Category != MP_TUTORIAL &&
		Protocol != MP_USERCONN_GAMEIN_ACK)
	{
		AddBackupMsg(pMsg, dwMsgSize);
		return;
	}

	switch(Category)
	{
	case MP_MOVE:			MOVEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_SKILL:			SKILLMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_CHAR:			CHARCALCMGR->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_USERCONN:		UserConn_NetworkMsgParse( Protocol, pMsg ) ;			break ;
	case MP_HACKCHECK:		HACKCHECK->NetworkMsgParse( Protocol, pMsg );			break;
    case MP_PARTY:			PARTYMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_QUICK:			QUICKMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_ITEM:			ITEMMGR->NetworkMsgParse(Protocol,pMsg);				break;
	case MP_SKILLTREE:		SKILLTREEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_CHAT:			CHATMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_BATTLE:			BATTLESYSTEM->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_EXCHANGE:		EXCHANGEMGR->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_STREETSTALL:	STREETSTALLMGR->NetworkMsgParse(Protocol, pMsg);		break;
	case MP_STORAGE:			STORAGEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_FRIEND:			FRIENDMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_NOTE:			NOTEMGR->NetworkMsgParse(Protocol, pMsg);				break;
	// 080107 KTH -- 
	case MP_BOSSMONSTER:	OBJECTMGR->NetworkMsgParse(Protocol, pMsg);				break;
	// 090422 ShinJS --- Å»°Í MsgParseÃß°¡
	case MP_VEHICLE:		VEHICLEMGR->NetworkMsgParse(Protocol, pMsg);	break;
	case MP_QUEST:			
		{
			QUESTMGR->NetworkMsgParse( Protocol, pMsg );
		}
		break;
	case MP_PK:				PKMGR->NetworkMsgParse(Protocol, pMsg);					break;
	case MP_CHEAT:			CHEATMGR->NetworkMsgParse(Category,Protocol,(MSGBASE*)pMsg);	break;	
	case MP_GUILD:			GUILDMGR->NetworkMsgParse(Protocol, pMsg);				break;
	case MP_GUILD_UNION:	GUILDUNION->NetworkMsgParse(Protocol, pMsg);			break;
	case MP_NPC:			NPC_NetworkMsgParse( Protocol, pMsg ) ;					break ;
	case MP_SIGNAL:			SIGNAL_NetworkMsgParse( Protocol, pMsg ) ;				break ;
	case MP_GUILD_WAR:		GUILDWARMGR->NetworkMsgParse( Protocol, pMsg );		break;
	case MP_GTOURNAMENT:	GTMGR->NetworkMsgParse( Protocol, pMsg);			break;
	case MP_FACIAL :		FACIALMGR->NetworkMsgParse( Protocol, pMsg );		break;
	case MP_EMOTION :		CONDUCTMGR->NetworkMsgParse( Protocol, pMsg ); break;
	case MP_FARM :				g_csFarmManager.CLI_ParseAnswerFromSrv(pMsg); break;
	case MP_RESIDENTREGIST :	g_csResidentRegistManager.CLI_ParseAnswerFromSrv(pMsg);	break;
	case MP_FAMILY :			g_csFamilyManager.CLI_ParseAnswerFromSrv(pMsg);	break;
	case MP_DATE:				g_csDateManager.CLI_ParseAnswerFromSrv(pMsg); break;
	case MP_TUTORIAL :			TUTORIALMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_AUTONOTE:			AUTONOTEMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_CHATROOM :			CHATROOMMGR->NetworkMsgParser(pMsg); break;
	case MP_FISHING:			FISHINGMGR->NetworkMsgParse( Protocol, pMsg); break;
	//aziz MallShop 24
	case MP_VIP:				VIPMGR->NetworkMsgParse( Protocol, pMsg); break;
	//aziz Reborn 24 Sep
	case MP_REBORN:				UTILITYMGR->NetworkMsgParse( Protocol, pMsg); break;
	//aziz Kill Shop 30 Sep
	case MP_KILL:	KILLMGR->NetworkMsgParse( Protocol, pMsg); break;
	//aziz Reborn Point 13 Oct
	case MP_UTILITY:			UTILITYMGR->NetworkMsgParse( Protocol, pMsg); break;

	case MP_PET:				PETMGR->NetworkMsgParse( Protocol, pMsg); break;
	case MP_SIEGEWARFARE :		SIEGEWARFAREMGR->NetworkMsgParse( Protocol, pMsg); break;
  	case MP_RECALLNPC :			NPCRECALLMGR->NetworkMsgParser( Protocol, pMsg); break;
	case MP_LIMITDUNGEON :		LIMITDUNGEONMGR->NetworkMsgParser( Protocol, pMsg); break;
	case MP_COOK:				COOKMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_HOUSE:				HOUSINGMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_TRIGGER:			TRIGGERMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_DUNGEON:			DungeonMGR->NetworkMsgParse(Protocol, pMsg); break;
	case MP_PCROOM:				PCROOMMGR->NetworkMsgParse( Protocol, pMsg ); break;
	case MP_CONSIGNMENT:		CONSIGNMENTMGR->NetworkMsgParse( Protocol, pMsg ); break;
	// [ 14-May-2555 ]Add by Death For new Weather
	case MP_WEATHER:		    WEATHERMGR->NetworkMsgParse( Protocol, pMsg ); break;
	case MP_MARRY:				MARRYMGR->NetworkMsgParse( Protocol, pMsg ); break;
		//aziz add
	case MP_ALIAS:				ALIASMGR->NetworkMsgParse( Protocol, pMsg ); break;
	}
}

void CGameIn::OnDisconnect()
{
	m_bDisconnected = TRUE;
	if( cMsgBox::IsInit() )	//¡ÆOAOA¢´AoCE¢¯a? confirm
	{
		// 070122 LYW --- Modified this line.
		cMsgBox* pMsgBox;
		pMsgBox = WINDOWMGR->MsgBox( MBI_SERVERDISCONNECT, MBT_OK, CHATMGR->GetChatMsg( 212 ) );
		//cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_FAILED_NPC_CHANGE_MAP, MBT_OK, CHATMGR->GetChatMsg( 212 ) );
	}
	else
	{
		MessageBox( NULL, "Disconnected To Server.\nThe Game is Closing.", 0, 0 );
		//MAINGAME->SetGameState(eGAMESTATE_END);
		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
	}
}

void CGameIn::SetLoginTime(SYSTEMTIME Time)
{
	GameinServerTime = Time;
}

SYSTEMTIME	CGameIn::GetLoginTime()
{
	return GameinServerTime;
}

//Ä³¸¯ÅÍ Å¬¶óÀÌ¾ðÆ® ·Î±×ÀÎ ½Ã°£
void	CGameIn::GetClientLoginTime(DWORD& date, DWORD& time)
{
	date = m_Clientdate;
	time = m_Clienttime;
}

void	CGameIn::SetClientLoginTime(DWORD date, DWORD time)
{
	m_Clientdate = date;
	m_Clienttime = time;
}


void CGameIn::UserConn_NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_USERCONN_OBJECT_REMOVE:				UserConn_Object_Remove( pMsg ) ;							break;
	case MP_USERCONN_MONSTER_DIE:				UserConn_Monster_Die( pMsg ) ;								break;
	case MP_USERCONN_NPC_DIE:					UserConn_Monster_Die( pMsg ) ;								break;
	case MP_USERCONN_CHARACTER_DIE:				UserConn_Character_Die( pMsg ) ;							break;	
	// 091110 ONS Æê ¾Ö´Ï¸ÞÀÌ¼Ç Ãß°¡ : Á×¾úÀ»¶§ ¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸® Ãß°¡
	case MP_USERCONN_PET_DIE:					UserConn_Pet_Die( pMsg ) ;									break;	
	// 070417 LYW --- GameIn : Add network message down exp and level.
	case MP_USERCONN_CHARACTER_APPLYEXP_NOTICE :	UserConn_ApplyExp( pMsg ) ;								break;
	case MP_USERCONN_CHARACTER_DOWNEXP_NOTICE :		UserConn_DownExp( pMsg ) ;								break;
	case MP_USERCONN_CHARACTER_DOWNLEVEL_NOTICE :	UserConn_DownLevel( pMsg ) ;							break;
	case MP_USERCONN_READY_TO_REVIVE:			UserConn_Ready_To_Revive() ;								break;
	case MP_USERCONN_CHARACTER_REVIVE:			UserConn_Character_Revive( pMsg ) ;							break;		
	case MP_USERCONN_CHARACTER_REVIVE_NACK:		UserConn_Character_Revive_Nack( pMsg ) ;					break;
	case MP_USERCONN_CHARACTER_REVIVE_FLAG_ACK:	UserConn_Revive_Flag(pMsg); break;
	case MP_USERCONN_NPC_ADD:					UserConn_Npc_Add( pMsg ) ;									break;
	case MP_USERCONN_GRIDINIT:					UserConn_GridInit() ;										break;
	case MP_USERCONN_GAMEIN_ACK:				UserConn_GameIn_Ack( pMsg ) ;								break;
	case MP_USERCONN_GAMEIN_NACK:				__asm int 3;												break;
	case MP_USERCONN_CHARACTER_ADD:				UserConn_Character_Add( pMsg ) ;							break;
	case MP_USERCONN_MONSTER_ADD:				UserConn_Monster_Add( pMsg ) ;								break;	
	case MP_USERCONN_BOSSMONSTER_ADD:			UserConn_BossMonster_Add( pMsg ) ;							break;	
	case MP_USERCONN_CHANGEMAP_ACK:				UserConn_ChangeMap_Ack( pMsg ) ;							break;
	// 070918 LYW --- GameIn : Add process to change map through the npc.
	case MP_USERCONN_NPC_CHANGEMAP_ACK :		UserConn_Npc_ChangeMap_Ack( pMsg ) ;						break;
	case MP_USERCONN_NPC_CHANGEMAP_NACK :		UserConn_Npc_ChangeMap_Nack() ;									break;
	case MP_USERCONN_CHEAT_CHANGEMAP_ACK:		UserConn_Cheat_ChangeMap_Ack( pMsg ) ;						break;					
	case MP_USERCONN_CHANGEMAP_NACK:			UserConn_ChangeMap_Nack() ;									break;
	// 090512 ONS Å¸Á¾Á·ÀÇ ½Å±ÔÁ¾Á· ½ÃÀÛ¸ÊÀ¸·ÎÀÇ ÀÌµ¿ Á¦ÇÑ
	case MP_USERCONN_DEVILMARKET_CHANGEMAP_NACK:UserConn_DevilMarket_ChangeMap_Nack() ;						break;
	case MP_USERCONN_CHARACTERLIST_ACK:			UserConn_CharacterList_Ack( pMsg ) ;						break;
	case MP_USERCONN_BACKTOCHARSEL_ACK:			MAINGAME->SetGameState( eGAMESTATE_CHARSELECT );			break;
	case MP_USERCONN_BACKTOCHARSEL_NACK:		UserConn_BackToCharSel_Nack() ;								break;
	case MP_USERCONN_MAPDESC:					UserConn_MapDesc( pMsg ) ;									break;
	case MP_USERCONN_SETVISIBLE:				UserConn_SetVisible( pMsg ) ;								break;
	case MP_USERCONN_EVENTITEM_USE:				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(396) );	break;
	case MP_USERCONN_EVENTITEM_USE2:			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(396) );	break;
		//sort test
	case MP_USERCONN_SORT_ACK:					UserConn_SortItem(pMsg);									break;
	case MP_USERCONN_READY_TO_REVIVE_BY_GFW:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GD_REVIVALDLG );
			ASSERT( dialog );
			dialog->SetActive( TRUE );
			break;
		}
	case MP_USERCONN_PET_ADD: UserConn_Pet_Add( pMsg ); break;
	case MP_USERCONN_HEROPET_REMOVE: UserConn_HeroPet_Remove( pMsg ); break;
	// 090316 LUJ, Å»°Í
	case MP_USERCONN_VEHICLE_ADD: UserConn_Vehicle_Add( pMsg ); break;
	case MP_USERCONN_VEHICLE_REMOVE: UserConn_Vehicle_Remove( pMsg ); break;
	case MP_USERCONN_MONSTER_TAME_ACK:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			const DWORD ownerObjectIndex = message->dwObjectID;
			const DWORD tamedObjectIndex = message->dwData;

			UserConn_Monster_Tame(
				ownerObjectIndex,
				tamedObjectIndex);
			break;
		}
	case MP_USERCONN_GETSERVERTIME_ACK:
		{
			stTime64t* pmsg = (stTime64t*)pMsg;
			MHTIMEMGR->SetServerTime( pmsg->time64t );
#ifdef _GMTOOL_
			struct tm curTimeWhen = *_localtime64( &pmsg->time64t );
			CHATMGR->AddMsg( CTC_SYSMSG, "server time: %d-%d-%d %02d:%02d:%02d", 
				curTimeWhen.tm_year + 1900,
				curTimeWhen.tm_mon + 1,
				curTimeWhen.tm_mday,
				curTimeWhen.tm_hour,
				curTimeWhen.tm_min,
				curTimeWhen.tm_sec );
#endif
		}
		break;

		// 100507 ShinJS --- Ã¤³ÎÁ¤º¸ ¼ö½Å
	case MP_USERCONN_CHANNELINFO_ACK:
		{
			MSG_CHANNEL_INFO * pmsg = (MSG_CHANNEL_INFO*)pMsg;

			m_pMiniMapDlg->SetChanelComboBox( pmsg );

			if( pmsg->Count <= 1 )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2223 ) );		// ¡°ÇØ´ç ¸ÊÀº Ã¤³ÎÀÌ ÇÏ³ª¸¸ Á¸Àç ÇÏ¹Ç·Î º¯°æÀÌ ºÒ°¡´É ÇÕ´Ï´Ù."
				break;
			}
		}
		break;
	case MP_USERCONN_CHANGE_CHANNEL_IN_GAME_NACK:
		{
			if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CHA_CHANNELDLG))
			{
				dialog->SetActive(
					FALSE);
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(2223));
			}
		}
		break;
	case MP_USERCONN_BILLING_ALERT_ACK:
		{
			const MSG_INT2* const message = (MSG_INT2*)pMsg;
			const int result = message->nData1;
			const int remainedSecond = message->nData2;

			UserConn_Alert(
				result,
				remainedSecond);
		}
		break;
	case MP_USERCONN_BILLING_STOP_ACK:
		{
			if(cWindow* const window = GAMEIN->GetMiniMapDialog()->GetWindowForID(MNM_PCROOM_MALL_BTN))
			{
				window->SetActive(
					FALSE);
			}

			if(cWindow* const window = GAMEIN->GetMiniMapDialog()->GetWindowForID(MNM_PCROOM_PROVIDE_ITEM_BTN))
			{
				window->SetActive(
					FALSE);
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1347));
		}
		break;
	case MP_USERCONN_OPTION_ACK:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			CPlayer* player = (CPlayer*)OBJECTMGR->GetObject(
				message->dwObjectID);

			if(0 == player)
			{
				break;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				break;
			}

			player->GetCharacterTotalInfo()->HideFlag = message->dwData;
			APPEARANCEMGR->AddCharacterPartChange(
				player->GetID());
			}
		break;
		case MP_PVP_CEK_LOGIN:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			const int pvpscoremain		= message->dwData;
			CHero* const hero = OBJECTMGR->GetHero();
			hero->SetPVPScoreMain(pvpscoremain);
			if (MAP->GetMapNum() == 10)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, "Welcome to Battle Royal Map" );
				CHATMGR->AddMsg( CTC_TIPS, "Your PVP Point %d", message->dwData);
			}
		}
		break;
	case MP_PK_PVPPLUS:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;

			CPlayer* modar = (CPlayer*)OBJECTMGR->GetObject(message->dwData);

			CHero* pemain = OBJECTMGR->GetHero();
		
			CHATMGR->AddMsg( CTC_ATTACK, "Kill %s, your PVP score increased 3 point", modar->GetObjectName());
			pemain->SetPVPScoreALL(1, 0);
		}
		break;
	case MP_PK_PVPMINUS:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;
			
			CPlayer* pembunuh = (CPlayer*)OBJECTMGR->GetObject( message->dwData );
			CHero* pemain = OBJECTMGR->GetHero();
			
			CHATMGR->AddMsg( CTC_ATTACKED, "You has killed by %s", pembunuh->GetObjectName());
			pemain->SetPVPScoreALL(0, 1);
		}
		break;
	case MP_ITEM_ADDMONEY_ICE:
		{
			const MSG_DWORDEX2* const message = (MSG_DWORDEX2*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;

			switch (message->dweData1)
			{
				case 0:
					{
						CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( message->dweData2 ) );
					} break;
				case 1:
					{
						CHATMGR->AddMsg( CTC_BUYITEM, CHATMGR->GetChatMsg( 378 ), AddComma( message->dweData2 ) );
					} break;
				case 2:
					{
						CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( message->dweData2 ) );
					} break;
				case 3:
					{
						CHATMGR->AddMsg( CTC_GETMONEY, CHATMGR->GetChatMsg( 301 ), AddComma( message->dweData2 ) );
					} break;
				case 4:
					{
						CHATMGR->AddMsg( CTC_BUYITEM, "You have use %s gold", AddComma( message->dweData2 ) );
					} break;
				default:
					{
						CHATMGR->AddMsg( CTC_BUYITEM, "You have use %s gold", AddComma( message->dweData2 ) );
					} break;
			}
		}
		break;
	case MP_SLOT_ACK:
		{
			const MSG_SLOT_SYN_ACK* const message = (MSG_SLOT_SYN_ACK*)pMsg;
			CHero* pemain = OBJECTMGR->GetHero();
			pemain->SetGetmoneyspinslot(message->dweData1, message->dweData2, message->dweData3, message->dweData4, message->dweData5, message->dweData6, message->dweData7);
		} 
		break;
	case MP_USERCONN_GMSIGNAL_JACKPOT:
		{
			const MSG_DWORDEX2* const message = (MSG_DWORDEX2*)pMsg;
			CHero* pemain = OBJECTMGR->GetHero();
			pemain->SetMapjackpot(message->dweData1);

			CSlotDialog* pHandler = (CSlotDialog*)GAMEIN->GetSlotDlg();
			if( pHandler->IsActive() )
			{
				pHandler->UpdateJackpot(message->dweData1);
			}

			CHATMGR->AddMsg(CTC_EVENT, 
				"[%s]: Slot Machine Jackpot has been added %s by GM, lets try your luck!", 
			GetMapName(MAP->GetMapNum()), AddComma(message->dweData2));

		} 
		break;
	case MP_SLOT_NACK:
		{
			const MSG_WORD* const message = (MSG_WORD*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;

			switch (message->wData)
			{
			case 0:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, "Unknown error!");
				} break;
			case 1:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, "Not enough money!");
				} break;
			case 2:
				{
					CHATMGR->AddMsg(CTC_SYSMSG, "Your Inventory money is full!");
				} break;
			}
			
			CHero* pemain = OBJECTMGR->GetHero();
			pemain->SetGetmoneyspinslot(0, 0, 0, 0, 0, 0, 0);

			CSlotDialog* pHandler = (CSlotDialog*)GAMEIN->GetSlotDlg();
			if( pHandler->IsActive() )
			{
				pHandler->SetActive(FALSE);
			}
		
		} break;
	case MP_SLOT_OPEN_MALL_ACK:
		{
			const MSG_DWORDEX* const message = (MSG_DWORDEX*)pMsg;

			CHero* pemain = OBJECTMGR->GetHero();
			pemain->SetMapjackpot(message->dweData);

			CSlotDialog* pHandler = (CSlotDialog*)GAMEIN->GetSlotDlg();
			if( !pHandler->IsActive() )
			{
				pHandler->SetActive(TRUE);
			}

		} break;
	case MP_SLOT_OPEN_MALL_NACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;

			CHATMGR->AddMsg(CTC_SYSMSG, "You cant open slot machine at this map, try at Alker Harbor");
			
		} break;
	case MP_BROWSER_ACC:
		{
			const MSG_BRWOSER_ACC* const message = (MSG_BRWOSER_ACC*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;
			
			cHousingWebDlg* pDlg = GAMEIN->GetHousingWebDlg();
			if( pDlg )
			{
				if (pDlg->IsActive())
				{
					pDlg->SetActive(FALSE);
				} else {
					pDlg->SetType( 1 );
					char buff[33] = {0,};
					sprintf(buff, message->acccode);
					pDlg->OpenBrowser( message->idxchar, buff );
				}
			}

		} break;
	case MP_USERCONN_STREETTOURNAMENT_REGIST_ACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;
			
			CPlayer* Playerregist = (CPlayer*)OBJECTMGR->GetObject( message->dwObjectID );
			if(!Playerregist) return;

			Playerregist->SetRegisterStreetTournament(TRUE);

			CHATMGR->AddMsg(CTC_SYSMSG, "You have registered in Street Tournament.");
		} break;
	case MP_USERCONN_STREETTOURNAMENT_REGIST_NACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;

			CHATMGR->AddMsg(CTC_SYSMSG, "you cant register at this time.");
		} break;
	case MP_USERCONN_STREETTOURNAMENT_STATE:
		{
			const MSG_WORD3* const message = (MSG_WORD3*)pMsg;
			if (message->dwObjectID != HERO->GetID())
				return;

			if(message->wData2 == STREETTOURNAMENTMGR->GetState()) return;
			
			char buffstage[32]="";

			switch(message->wData3)
			{
			case 0:
				{
					sprintf(buffstage, "");
				} break;
			case 1:
				{
					sprintf(buffstage, "");
				} break;
			case 2:
				{
					sprintf(buffstage, "");
				} break;
			case 3:
				{
					sprintf(buffstage, "");
				} break;
			case 4:
				{
					sprintf(buffstage, "");
				} break;
			}

			if(message->wData1 == 1)
			{
				switch(message->wData2)
				{
					case 0:
						{

						} break;
					case 1:
						{
							CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s Registration open for 10 Minutes", buffstage);
							CHATMGR->AddMsg(CTC_SYSMSG, "- This is Daily Event, all player level above 105 could be participant");
							CHATMGR->AddMsg(CTC_SYSMSG, "- You can register at NPC Street Tournament Manager, Alker Harbor near Coin NPC");
							CHATMGR->AddMsg(CTC_SYSMSG, "- Registration fee 50.000.000 gold");
							CHATMGR->AddMsg(CTC_SYSMSG, "- Prize Golden Coin, Gold, ST Box, etc");
						} break;
					case 2:
						{
							CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s Registration closed, Tournament started in 30sec.", buffstage);
						} break;
					case 3:
						{
							CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s is started!", buffstage);
						} break;
					case 4:
						{
							CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s end!", buffstage);
							if(MAP->GetMapNum() == StreetTournament)
							{
								CHATMGR->AddMsg(CTC_SYSMSG, "You will aoutomatically transfer to Alker Harbour");
							}
						} break;
				}
			}

			STREETTOURNAMENTMGR->SetStage(message->wData3);
			STREETTOURNAMENTMGR->SetState(message->wData2);

			if(MAP->GetMapNum() == StreetTournament)
			{
				if(message->wData2 == 3)
				{
					if(OBJECTMGR->GetSelectedObjectID() != 0)
					{
						OBJECTMGR->SetSelectedObjectID(0);
					}

					if( HERO->GetState() == eObjectState_Immortal )
					{
						OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Immortal );
					} 
					
					OBJECTMGR->ApplyOverInfoOptionToAll();
					

				} else if (message->wData2 == 4){
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_None);
					OBJECTMGR->ApplyOverInfoOptionToAll();
				}
			}

		} break;
	case MP_USERCONN_STREETTOURNAMENT_NOTIFY:
		{
			const MSG_DWORD3* const message = (MSG_DWORD3*)pMsg;
			//if (message->dwObjectID != HERO->GetID())
			//	return;
			char buffstage[32]="";

			switch(message->dwData3)
			{
			case 0:
				{
					sprintf(buffstage, "");
				} break;
			case 1:
				{
					sprintf(buffstage, "");
				} break;
			case 2:
				{
					sprintf(buffstage, "");
				} break;
			case 3:
				{
					sprintf(buffstage, "");
				} break;
			case 4:
				{
					sprintf(buffstage, "");
				} break;
			}

			switch(message->dwData1)
			{
			case 0:
				{
					CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s is cancelled", buffstage);
					CHATMGR->AddMsg(CTC_SYSMSG, "Due low amount of registered player, Street Tournament has been cancelled.");
					
					if(MAP->GetMapNum() == StreetTournament)
					{
						CHATMGR->AddMsg(CTC_SYSMSG, "You will aoutomatically transfer to Alker Harbour");
					}
				} break;
			case 1:
				{
					if(message->dwData2 == 0)
					{
						CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s Registration closed in 5 Min", buffstage);
					} else {
						CHATMGR->AddMsg(CTC_EVENT, "[Street Tournament] %s Registration closed in 5 Min - %d registered", buffstage, message->dwData2);
					}
					
				} break;
			}

		} break;
	case MP_USERCONN_STREETTOURNAMENT_WINNER:
		{
			const MSG_NAME_DWORD3* const message = (MSG_NAME_DWORD3*)pMsg;
			
			//CPlayer* pemenang = (CPlayer*)OBJECTMGR->GetObject( message->dwData );
			//if(!pemenang) return;
			
			
			char namapemenang[128] = {0,};
			sprintf(namapemenang, "%s Win Street Tournament", message->Name);
			CHATMGR->AddMsg(CTC_EVENT, namapemenang);
			CHATMGR->AddMsg(CTC_EVENT, "Prize in Item Mall Storage");

			if (message->dwData1 == HERO->GetID() && MAP->GetMapNum() == StreetTournament)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "You will aoutomatically transfer to Alker Harbour");
			}
		} break;
	case MP_USERCONN_CAPTCHA_SYN:
		{
			const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
			CPlayer* pemain = (CPlayer*)OBJECTMGR->GetObject( message->dwObjectID );
			if(!pemain) return;

			if(message->dwObjectID != HERO->GetID()) return;

			CChaptchaDlg* pHandler = (CChaptchaDlg*)GAMEIN->GetChaptchaDlg();
			if(pHandler)
			{
				pHandler->SetTypeCaptcha(message->dwData2);
				pHandler->SetTypeGame(TRUE);
				pHandler->SetSecNumber(message->dwData1);
			} else {
				pHandler->SetTypeCaptcha(message->dwData2);
				pHandler->SetActive(TRUE);
				pHandler->SetTypeGame(TRUE);
				pHandler->SetSecNumber(message->dwData1);
			}

			CHero* pemainnya = OBJECTMGR->GetHero();
			pemainnya->StopGetmoneyspinslot();

			pemain->SetVeirfyCaptchaState(TRUE);

		} break;
	case MP_USERCONN_CAPTCHA_ACK:
		{
			const MSGBASE* const message = (MSGBASE*)pMsg;

			if (message->dwObjectID != HERO->GetID())
				return;

			CPlayer* pemain = (CPlayer*)OBJECTMGR->GetObject( message->dwObjectID );
			if(!pemain) return;

			pemain->SetVeirfyCaptchaState(FALSE);

		} break;
		//Alemuri Item Drop Rates Info To Client----------------- 
	case MP_USERCONN_GET_ITEM_RATES:
		{
			const MSG_DWORD* const message = (MSG_DWORD*)pMsg;
			HERO->SetServerDropRatesInfo(message->dwData);
		} break;
		//-------------------------------------------------------
		//Alemuri Dragonya immune to Dragonya Soul Costume------------------------------- 
	case MP_USERCONN_DRAGONYA_DODGE:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, "You cannot fight Dragonya with its own power");
		} break;
		//-------------------------------------------------------------------------------
	}
}

void CGameIn::UserConn_Alert(int result, int remainedSecond)
{
	switch(result)
	{
	case 1: // »ç¿ëÀÚÀÇ ³²Àº ½Ã°£
		{
			const int oneMinute = 60;
			const int oneHour = oneMinute * 60;
			const int oneDay = oneHour * 24;

			// ÇÏ·ç ÀÌ»ó ³²¾ÒÀ¸¸é »ó¼¼ÇÏ°Ô Ç¥½ÃÇÏÁö ¾Ê´Â´Ù
			if(const int day = remainedSecond / oneDay)
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1349));
				break;
			}

			TCHAR text[MAX_PATH] = {0};
			TCHAR buffer[MAX_PATH] = {0};

			if(const int hour = remainedSecond / oneHour)
			{
				_stprintf(
					buffer,
					CHATMGR->GetChatMsg(1410),
					hour);
				_tcscat(
					text,
					buffer);

				remainedSecond = remainedSecond - hour * oneHour;
			}

			if(const int minute = remainedSecond / oneMinute)
			{
				_stprintf(
					buffer,
					_T(" %d%s"),
					minute,
					CHATMGR->GetChatMsg(794));
				_tcscat(
					text,
					buffer);

				remainedSecond = remainedSecond - minute * oneMinute;
			}

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1348),
				text);
			break;
		}
	case 0: // ³²Àº ½Ã°£ ¾øÀ½. ÇýÅÃ Á¾·á
	case -1: // È¯ºÒ Á¶Ä¡. ÇýÅÃ Á¾·á
	case -2: // Áßº¹ ·Î±×ÀÎ. ÇýÅÃ Á¾·á
	case -4: // ±âÅ¸ ÇýÅÃ Á¾·á
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg(1347));
			break;
		}
	case -3:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"You're started for reserved speciality on PC room");
			break;
		}
	default:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Not defined alarm is received");
			break;
		}
	}
}

void CGameIn::UserConn_Monster_Tame(DWORD ownerObjectIndex, DWORD tamedObjectIndex) const
{
	CMonster* const monster = (CMonster*)OBJECTMGR->GetObject(
		tamedObjectIndex);

	if(0 == monster)
	{
		return;
	}
	else if(FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
	{
		return;
	}

	monster->SetOwnerIndex(
		ownerObjectIndex);
}

void CGameIn::UserConn_Object_Remove( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwData);
	if(pObject == NULL)
	{
		return;
	}

	// 091026 LUJ, ÄÚµå Á¤¸®
	switch(pObject->GetObjectKind())
	{
	case eObjectKind_Player:
		{
			pObject->OnStartObjectState( eObjectState_Exit );
			break;
		}
	case eObjectKind_Npc:
		{
			const DWORD dwRecallNpcIdx = ( (CNpc*)pObject )->GetNpcTotalInfo()->dwRecallNpcIdx ;

			if(dwRecallNpcIdx)
			{
				NPCRECALLMGR->OnRemoveRecallNpc((CNpc*)pObject);
			}

			break;
		}
	case eObjectKind_Vehicle:
		{
			CVehicle* const vehicle = (CVehicle*)pObject;
			const BOOL isMovableVehicle(0 < vehicle->GetMoveSpeed());

			// 091026 LUJ, È÷¾î·Î°¡ ¼ÒÀ¯ÇÑ ÀÌµ¿Çü Å»°ÍÀº Á¦°Å½ÃÅ°Áö ¾Ê´Â´Ù
			//			±×¸®µå ÀÌµ¿¿¡ µû¶ó ¿ÀºêÁ§Æ®°¡ Ãß°¡/Á¦°ÅµÇ´Âµ¥, ÀÌ°Ô ¹Ý´ë·Î µÇ´Â °æ¿ì°¡ ÀÖ´Ù.
			//			ÀÌ·Î ÀÎÇØ ¿ÀºêÁ§Æ®°¡ Á¦°ÅµÇ°í ÁÂÇ¥¸¦ ¾÷µ¥ÀÌÆ®ÇÏÁö ¾Ê¾Æ Á¦°ÅÇÏÁö ¾Êµµ·Ï Çß´Ù.
			// 091106 ShinJS, Å»°ÍÀÌ Á×Àº»óÅÂÀÎ °æ¿ì(¼ÒÈ¯ÇØÁ¦)´Â Á¦°ÅµÉ¼ö ÀÖµµ·Ï ¼öÁ¤.
			if(gHeroID == vehicle->GetOwnerIndex() &&
				! vehicle->IsDied() &&
				isMovableVehicle)
			{
				return;
			}

			break;
		}
	case eObjectKind_Pet:
		{
			// 091026 LUJ, È÷¾î·Î°¡ ¼ÒÀ¯ÇÑ ÆêÀÎ °æ¿ì Á¦°ÅÇÏÁö ¾Ê°í »óÈ²¿¡ µû¶ó À§Ä¡¸¦ ÀÌµ¿½ÃÅ²´Ù
			//			¼­¹ö¿¡ ÀÖ´ø ÄÚµå¸¦ Å¬¶óÀÌ¾ðÆ®·Î ÀÌµ¿½ÃÄ×´Ù.
			if(HEROPET == pObject)
			{
				CHero* const hero = OBJECTMGR->GetHero();
				VECTOR3 heroPosition = {0};
				hero->GetPosition(&heroPosition);
				VECTOR3 petPosition = {0};
				pObject->GetPosition(&petPosition);

				const float distance = CalcDistance(
					&heroPosition,
					&petPosition);

				if(3000 < distance)
				{
					MOVEMGR->SetHeroPetTarget();
				}

				return;
			}

			break;
		}
	}

	APPEARANCEMGR->CancelAlphaProcess(pObject);
	APPEARANCEMGR->CancelReservation(pObject);
	OBJECTMGR->AddGarbageObject(pObject);
}

void CGameIn::UserConn_Monster_Die( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwData2);
	if(!pObject)
		return;
	// 090316 LUJ, ÀÌ¹Ì Á×Àº °æ¿ì Ã³¸®ÇÒ ÇÊ¿ä ¾ø´Ù.
	else if( pObject->IsDied() )
		return;

	pObject->SetDieFlag();

	if(const BOOL isSuicide = (pmsg->dwObjectID == pmsg->dwData2))
	{
		OBJECTACTIONMGR->Die(
			pObject,
			pObject,
			rand() < RAND_MAX / 2,
			rand() < RAND_MAX / 2,
			rand() < RAND_MAX / 2);
	}
}


void CGameIn::UserConn_Character_Die( void* pMsg )
{
	const MSG_DWORD2* const pmsg = (MSG_DWORD2*)pMsg;
	CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( pmsg->dwData2 );

	if( 0 == player ||
		player->GetObjectKind() != eObjectKind_Player )
	{
		return;
	}

	// 091214 ShinJS --- HERO°¡ Á×Àº °æ¿ì WayPoint¸¦ Á¦°ÅÇÑ´Ù.
	if( HEROID == player->GetID() )
		HERO->ClearWayPoint();

	// 090316 LUJ, »ç¸Á½Ã ÇÏÂ÷ Ã³¸®¸¦ ÇÑ´Ù
	VEHICLEMGR->GetOff( player->GetID(), player->GetVehicleID(), player->GetVehicleSeatPos() );
	player->SetDieFlag();
}

// 091110 ONS Æê ¾Ö´Ï¸ÞÀÌ¼Ç Ãß°¡ : Á×¾úÀ»¶§ ¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸® Ãß°¡
void CGameIn::UserConn_Pet_Die( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwData2);
	if(!pObject)
		return;
	// 090316 LUJ, ÀÌ¹Ì Á×Àº °æ¿ì Ã³¸®ÇÒ ÇÊ¿ä ¾ø´Ù.
	else if( pObject->IsDied() )
		return;

	pObject->SetDieFlag();
}

// 070418 LYW --- GameIn : Add function to notice apply exp.
void CGameIn::UserConn_ApplyExp( void* pMsg )
{
	MSG_DWORDEX2* pmsg = ( MSG_DWORDEX2* )pMsg ;

	HERO->SetExpPoint( pmsg->dweData1 ) ;

	CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(120), (float)pmsg->dweData2 ) ;
}

// 070418 LYW --- GameIn : Add function to notice down exp.
void CGameIn::UserConn_DownExp( void* pMsg )														// °æÇèÄ¡ ÇÏ¶ô ¸Þ½ÃÁö¸¦ Ã³¸®ÇÏ´Â ÇÔ¼ö.
{
	// 080602 LYW --- Player : °æÇèÄ¡ ¼öÄ¡ (__int32) ¿¡¼­ (__int64) »çÀÌÁî·Î º¯°æ Ã³¸®.
	//MSG_DWORD3* pmsg = ( MSG_DWORD3* )pMsg ;														// ¸Þ½ÃÁö¸¦ ¹Þ´Â´Ù.
	MSG_DWORDEX3* pmsg = ( MSG_DWORDEX3* )pMsg ;														// ¸Þ½ÃÁö¸¦ ¹Þ´Â´Ù.

	if(pmsg->dweData3)																				// dwData3°¡ TUREÀÌ¸é,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(848), pmsg->dweData1 ) ;					// Á¦ÀÚ¸® ºÎÈ° Ãß°¡ °æÇèÄ¡ ÇÏ¶ô ¸Þ½ÃÁö¸¦ Ãâ·ÂÇÑ´Ù.
	}
	else																							// dwData3°¡ FALSEÀÌ¸é,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(849), pmsg->dweData1 ) ;					// °æÇèÄ¡ ÇÏ¶ô ¸Þ½ÃÁö¸¦ Ãâ·ÂÇÑ´Ù.
	}

	HERO->SetExpPoint(pmsg->dweData2) ;																// ÇÃ·¹ÀÌ¾îÀÇ °æÇèÄ¡¸¦ ¼¼ÆÃÇÑ´Ù.
}

// 070418 LYW --- GameIn : Add function to notice down level.
void CGameIn::UserConn_DownLevel( void* pMsg )														// ·¹º§ ´Ù¿î ¸Þ½ÃÁö¸¦ Ã³¸®ÇÏ´Â ÇÔ¼ö.
{
	// 080602 LYW --- Player : °æÇèÄ¡ ¼öÄ¡ (__int32) ¿¡¼­ (__int64) »çÀÌÁî·Î º¯°æ Ã³¸®.
	//MSG_DWORD4* pmsg = ( MSG_DWORD4* )pMsg ;														// ¸Þ½ÃÁö¸¦ ¹Þ´Â´Ù.
	MSG_DWORDEX4* pmsg = ( MSG_DWORDEX4* )pMsg ;														// ¸Þ½ÃÁö¸¦ ¹Þ´Â´Ù.

	HERO->SetLevel(static_cast<LEVELTYPE>(pmsg->dweData1)) ;																	// ÇÃ·¹ÀÌ¾îÀÇ ·¹º§À» ¼¼ÆÃÇÑ´Ù.
	HERO->SetExpPoint(pmsg->dweData2) ;																// ÇÃ·¹ÀÌ¾îÀÇ °æÇèÄ¡¸¦ ¼¼ÆÃÇÑ´Ù.

	if(pmsg->dweData4)																				// dwData4°¡ TRUE ÀÌ¸é,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(848), pmsg->dweData3 ) ;					// Á¦ÀÚ¸® ºÎÈ° Ãß°¡ °æÇèÄ¡ ÇÏ¶ô ¸Þ½ÃÁö¸¦ Ãâ·ÂÇÑ´Ù.
	}
	else																							// dwData4°¡ FALSEÀÌ¸é,
	{
		CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(849), pmsg->dweData3 ) ;					// °æÇèÄ¡ ÇÏ¶ô ¸Þ½ÃÁö¸¦ Ãâ·ÂÇÑ´Ù.
	}

	CHATMGR->AddMsg( CTC_GETEXP, CHATMGR->GetChatMsg(657)) ;										// ·¹º§´Ù¿î ¸Þ½ÃÁö¸¦ Ãâ·ÂÇÑ´Ù.

	// 080310 LUJ, ½ºÅ³ Ã¢ÀÇ Á¤º¸¸¦ °»½ÅÇÑ´Ù
	{
		cSkillTrainingDlg* dialog = GAMEIN->GetSkillTrainingDlg();

		if( dialog &&
			dialog->IsActive() )
		{
			dialog->OpenDialog();
			dialog->SetSkillInfo();
		}
	}
}

void CGameIn::UserConn_Ready_To_Revive()
{
	// 080808 KTH -- °ø¼ºÀü Áö¿ªÀÌ¸é ºÎÈ°ÀÌ ¾Æ´Ï¶ó 5ÃÊÈÄ¿¡ ÀÌµ¿ ÇÏµµ·Ï ÇÑ´Ù.
  	if( SIEGEWARFAREMGR->IsSiegeWarfareZone(MAP->GetMapNum() ) )
  	{
  		SIEGEWARFAREMGR->SetRevive(TRUE);
  		SIEGEWARFAREMGR->SetReviveTime();
  		return;
  	}

	cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

	if(reviveDialog)
	{
		reviveDialog->SetActive(TRUE);
	}
}


void CGameIn::UserConn_Character_Revive( void* pMsg )									// Ä³¸¯ÅÍ ºÎÈ°À» Ã³¸®ÇÏ´Â ÇÔ¼ö.
{
	MOVE_POS* pmsg = (MOVE_POS*)pMsg;													// ¸Þ½ÃÁö¸¦ ¹Þ´Â´Ù.
	VECTOR3 pos;																		// À§Ä¡¸¦ ³ªÅ¸³»´Â º¤ÅÍ¸¦ ¼±¾ðÇÑ´Ù.

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);							// ¸Þ½ÃÁöÀÇ ¾ÆÀÌµð·Î ¿ÀºêÁ§Æ®¸¦ ¹Þ´Â´Ù.

	if( pObject )																		// ¿ÀºêÁ§Æ® Á¤º¸°¡ À¯È¿ÇÑÁö Ã¼Å©ÇÑ´Ù.
	{
		pmsg->cpos.Decompress(&pos);													// ºÎÈ° À§Ä¡ Á¤º¸¸¦ ¹Þ´Â´Ù.

		OBJECTACTIONMGR->Revive(pObject,&pos);											// Á¤ÇØÁø À§Ä¡·Î ¿ÀºêÁ§Æ®¸¦ ºÎÈ°½ÃÅ²´Ù.

		if(pObject->GetID() == gHeroID)
		{
			cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

			if(reviveDialog)
			{
				reviveDialog->SetDisable(FALSE);
				reviveDialog->SetActive(FALSE);
			}
		}
	}
}

void CGameIn::UserConn_Revive_Flag(LPVOID packet)
{
	cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

	if(0 == reviveDialog)
	{
		return;
	}

	cWindow* const presentSpotButton = reviveDialog->GetWindowForID(CR_PRESENTSPOT);
	cWindow* const townSpotButton = reviveDialog->GetWindowForID(CR_TOWNSPOT);

	if(0 == presentSpotButton ||
		0 == townSpotButton)
	{
		return;
	}

	const DWORD enableColor = RGB(255, 255, 255);
	const DWORD unableColor = RGB(0, 0, 0);	

	// 100111 LUJ, Á¦ÀÚ¸® ºÎÈ° ¹öÆ°À» ÃÊ±âÈ­ÇÑ´Ù
	presentSpotButton->SetDisable(FALSE);
	presentSpotButton->SetActive(TRUE);
	presentSpotButton->SetImageRGB(enableColor);
	presentSpotButton->SetToolTip("");
	// 100111 LUJ, ¾ÈÀüÁö´ë ºÎÈ° ¹öÆ°À» ÃÊ±âÈ­ÇÑ´Ù
	townSpotButton->SetDisable(FALSE);
	townSpotButton->SetActive(TRUE);
	townSpotButton->SetImageRGB(enableColor);
	townSpotButton->SetToolTip("");

	const MSG_INT* const message = (MSG_INT*)packet;

	switch(message->nData)
	{
	case ReviveFlagHere:
		{
			cImage image;
			SCRIPTMGR->GetImage(
				0,
				&image,
				PFT_HARDPATH);

			townSpotButton->SetImageRGB(unableColor);
			townSpotButton->SetDisable(TRUE);
			townSpotButton->SetToolTip(
				CHATMGR->GetChatMsg(1867),
				RGBA_MAKE(255, 255, 255, 255),
				&image,
				TTCLR_ITEM_CANEQUIP);
			break;
		}
	case ReviveFlagTown:
		{
			cImage image;
			SCRIPTMGR->GetImage(
				0,
				&image,
				PFT_HARDPATH);

			presentSpotButton->SetImageRGB(unableColor);
			presentSpotButton->SetDisable(TRUE);
			presentSpotButton->SetToolTip(
				CHATMGR->GetChatMsg(1867),
				RGBA_MAKE(255, 255, 255, 255),
				&image,
				TTCLR_ITEM_CANEQUIP);
			break;
		}
	}
}

void CGameIn::UserConn_Character_Revive_Nack( void* pMsg )
{
#ifdef _GMTOOL_
	const MSG_BYTE* const message = (MSG_BYTE*)pMsg;

	switch(message->bData)
	{
	case 1:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: you're died already");
			break;
		}
	case 2:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: you're in be looting");
			break;
		}
	case 3:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: your revive flag is invalid");
			break;
		}
	case 4:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				"Error in reviving: you're in exiting");
			break;
		}
	}
#endif

	cDialog* const reviveDialog = WINDOWMGR->GetWindowForID(CR_DIALOG);

	if(reviveDialog)
	{
		reviveDialog->SetDisable(FALSE);
	}
}


void CGameIn::UserConn_Npc_Add( void* pMsg )
{
	SEND_NPC_TOTALINFO* pmsg = (SEND_NPC_TOTALINFO*)pMsg;
			
	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
			yCASE(CAddableInfoList::ShowdownInfo)
			yCASE(CAddableInfoList::StreetStall)
		yENDSWITCH
		
		iter.ShiftToNextData();
	}

	// 100308 pdy ¼­¹ö¿¡¼­ ÆÄ½ÌµÇ¼­¿Â npcÀÌ¸§Àº KeyWordÃ³¸®°¡ ¾ÈµÇ¾îÀÖ¾î Å¬¶ó¿¡¼­ º¯È¯ÇØÁØ´Ù. 
	if( pmsg->TotalInfo.dwRecallNpcIdx != 0 )
	{
		CNpcRecallBase* pRecallBase = NPCRECALLMGR->Get_RecallBase( pmsg->TotalInfo.dwRecallNpcIdx ) ; 
		if( pRecallBase )
		{
			SafeStrCpy( pmsg->BaseObjectInfo.ObjectName , pRecallBase->Get_NpcName() , MAX_NPC_NAME_LENGTH+1 ) ;
		}
	}
	else
	{
		char temp[256] = {0,};
		_parsingKeywordString( pmsg->BaseObjectInfo.ObjectName , temp );
		SafeStrCpy( pmsg->BaseObjectInfo.ObjectName , temp , MAX_NPC_NAME_LENGTH+1 ) ;
	}
	
	CNpc* pNpc = OBJECTMGR->AddNpc(&pmsg->BaseObjectInfo,&moveInfo,&pmsg->TotalInfo);
	MOVEMGR->SetAngle(pNpc,pmsg->Angle,0);

	// Npc Add
	QUESTMGR->SetNpcData( pNpc );

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pNpc;
		//EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pMon,&set,0,pMon->GetID());
		EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pNpc,&set,0,pNpc->GetID());
	}

	// 091104 ONS NPC°¡ Ãß°¡µÉ¶§ ÇØ´ç ÀÌÆåÆ®¸¦ °°ÀÌ Ãß°¡ÇÑ´Ù.
	TRIGGERMGR->RegenEffect(pNpc->GetID());

	// 091208 pdy ¼ÒÈ¯ npc Äù½ºÆ®¸¶Å© µî·ÏÃ³¸® Ãß°¡
	if( pmsg->TotalInfo.dwRecallNpcIdx )
	{
		NPCRECALLMGR->OnAddRecallNpc(pNpc);
	}

}


void CGameIn::UserConn_GridInit()
{
	DeleteFile("DEBUG.txt");
	m_bGameInAcked = TRUE;

	g_UserInput.GetKeyboard()->ClearKeyState();
	g_UserInput.GetKeyboard()->UpdateKeyboardState();
	g_UserInput.SetInputFocus( TRUE );
	USERINFOMGR->LoadUserInfo( eUIK_USERSTATE );
}
//sort test
void CGameIn::UserConn_SortItem(void* pMsg)
{
	SEND_HERO_TOTALINFO * pmsg = (SEND_HERO_TOTALINFO *)pMsg;
	OBJECTMGR->SortItem(pmsg);
}

void CGameIn::UserConn_GameIn_Ack( void* pMsg )
{
	ASSERT(!m_bGameInAcked);

	SEND_HERO_TOTALINFO * pmsg = (SEND_HERO_TOTALINFO *)pMsg;
	ASSERT(!(pmsg->SendMoveInfo.CurPos.wx == 0 && pmsg->SendMoveInfo.CurPos.wz == 0));

	//Ä³¸¯ÅÍ ¼­¹ö ·Î±×ÀÎ½Ã°£, Ä³¸¯ÅÍ Å¬¶óÀÌ¾ðÆ® ·Î±×ÀÎ½Ã°£ ±â·Ï
	GAMEIN->SetLoginTime(pmsg->ServerTime);
	GAMEIN->SetClientLoginTime(MHTIMEMGR->GetMHDate(), MHTIMEMGR->GetMHTime());

	OBJECTMGR->RegisterHero(pmsg);
	OPTIONMGR->UpdateHideFlagFromDB(pmsg->ChrTotalInfo.HideFlag);
	OPTIONMGR->ApplySettings();
	APPEARANCEMGR->InitAppearance(HERO);
	OBJECTSTATEMGR->InitObjectState(HERO);
	GAMEIN->GetMiniMapDialog()->AddHeroIcon(HERO);
	//aziz MallShop in Game Method 1
	GAMEIN->GetCharacterDialog()->RefreshInfo();
//	GAMEIN->GetCharacterDialog()->SetVipInfo();
	//aziz Reborn 24 Sep
//	GAMEIN->GetCharacterDialog()->SetRebornInfo();
	//aziz Kill Shop 30 Sep
//	GAMEIN->GetCharacterDialog()->SetKillInfo();
	//aziz Reborn Point 13 Oct
//	GAMEIN->GetCharacterDialog()->SetRBPointInfo();

	GAMEIN->GetCharacterDialog()->SetPointLeveling(
		TRUE,
		pmsg->HeroTotalInfo.LevelUpPoint);

	EFFECTMGR->StartHeroEffectProcess(eEffect_NewCharacter);
	// 091116 ONS UI À§Ä¡/È°¼ºÈ­»óÅÂ¿¡ º¯°æÀÌ ÀÖÀ¸¸é Àû¿ëÇÑ´Ù.
	UISETTINGMGR->ApplySettingInfo();
	FRIENDMGR->LogInNotify();
	
	VECTOR2 sidpos;
	sidpos.x = 260;
	sidpos.y = 0;
	STATUSICONDLG->Init(HERO,&sidpos,8);

	QUESTMGR->ProcessNpcMark();

	CAMERA->Init(HERO,30,0,1000);
	CAMERA->SetTargetAngleX(0,29.9f,0);
	HERO->InitializeEye();
	HERO->SetStorageNum( static_cast<BYTE>(pmsg->StorageSize) );

	if( &pmsg->ChrTotalInfo.DateMatching.bIsValid )
	{
		GAMEIN->GetDateMatchingDlg()->UpdateMyInfo(&pmsg->ChrTotalInfo.DateMatching) ;
	}

	g_csFarmManager.Init(MAP->GetMapNum());
	g_csDateManager.Init();

	SIEGEWARFAREMGR->LoadInfo();
	SIEGEDUNGEONMGR->LoadInfo();

	// [06-03-2012] Death Add Apply Option for Let server know synv login to game
	OPTIONMGR->ApplySettings();
	OPTIONMGR->SaveGameOption();
	OPTIONMGR->SendOptionMsg();	

	// [16-05-2012] Death Add SYN Weather for New Commer to map
	WEATHERMGR->ShowCurrentWeatherEffect();
	if(GTMAPNUM == MAP->GetMapNum() || MAP->GetMapNum() == PVP)//aziz pvp normal size //battle arena
	{
		HERO->SetCharacterSize(
			1.0f);
	}
	else if(TRUE == SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()))
	{
		if(FALSE == SIEGEWARFAREMGR->Is_CastleMap())
		{
			HERO->SetCharacterSize(
				1.0f);
		}

		MSGBASE message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_SIEGEWARFARE;
		message.Protocol = MP_SIEGEWARFARE_STATE_SYN;
		message.dwObjectID = gHeroID;
		NETWORK->Send(
			&message,
			sizeof(message));
	}
	else if( HOUSINGMGR->IsHousingMap() )
	{
		HERO->SetCharacterSize(
			1.0f);

		MSGBASE msg;
		SetProtocol(&msg, MP_HOUSE, MP_HOUSE_INFO_SYN);
  		msg.dwObjectID = gHeroID;
  		NETWORK->Send( &msg, sizeof( msg ) );
		HOUSINGMGR->StartHouseInfoLoading();
	}
	else if( DungeonMGR->IsDungeonMap(MAP->GetMapNum()))
	{
		DungeonMGR->Init();

		MSGBASE msg;
		SetProtocol(&msg, MP_DUNGEON, MP_DUNGEON_INFO_SYN);
  		msg.dwObjectID = gHeroID;
  		NETWORK->Send( &msg, sizeof( msg ) );
	}

	if( m_GameInInitKind == eGameInInitKind_Login )
	{
		MSGBASE msg;
		msg.Category = MP_ITEM;
		msg.Protocol = MP_ITEM_GIFT_EVENT_START;
		msg.dwObjectID = gHeroID;
		NETWORK->Send( &msg, sizeof( msg ) );
	}

	// 100127 ONS ¸¶Á·ÀÇ °æ¿ì °ÔÀÓ½ÃÀÛ Æ©Åä¸®¾óÀ» Ç¥½ÃÇÏÁö ¾Êµµ·Ï ¼öÁ¤.
	if ( (m_GameInInitKind == eGameInInitKind_Login )
		&& (!TUTORIALMGR->IsCompleteTutorial(e_TT_FirstGameIn)) 
		&& (HERO->GetCharacterTotalInfo()->Race != RaceType_Devil) )
	{
		TUTORIALMGR->Start_Specification_Tutorial(e_TT_FirstGameIn) ;
	}
	// 100611 ·¹º§¾÷ ¹öÆ° È°¼º¿©ºÎ ÆÇ´Ü

	m_pMiniMapDlg->ChangeLevelUpBTNState();

	// Ã¤³Î Á¤º¸ ¿äÃ»
	GAMEIN->GetMiniMapDialog()->RequestChannelInfo();


	ProcessBackupMsg();
}


void CGameIn::UserConn_Character_Add(void* pMsg )
{
	SEND_CHARACTER_TOTALINFO* const pmsg = (SEND_CHARACTER_TOTALINFO*)pMsg;
	BASEMOVE_INFO moveInfo;	
	ZeroMemory(&moveInfo, sizeof(moveInfo));
	moveInfo.SetFrom(&pmsg->MoveInfo);

	for(CAddableInfoIterator iter(&pmsg->AddableInfo);
		CAddableInfoList::MoveInfo == iter.GetInfoKind();
		iter.ShiftToNextData())
	{
		COMPRESSEDTARGETPOS compressedPosition = {0};
		iter.GetInfoData(&compressedPosition);
		moveInfo.SetMaxTargetPosIdx(compressedPosition.PosNum);

		for(BYTE i = 0;i < compressedPosition.PosNum; ++i)
		{
			VECTOR3* const targetPosition = moveInfo.GetTargetPosition(i);
			compressedPosition.pos[i].Decompress(targetPosition);
		}
	}

	moveInfo.bMoving = (0 < moveInfo.GetMaxTargetPosIdx());

	CPlayer* pPlayer = OBJECTMGR->AddPlayerThroughCache(
		&pmsg->BaseObjectInfo,
		&moveInfo,
		&pmsg->TotalInfo,pmsg);

	// 090422 ShinJS --- Å»°Í Á¤º¸°¡ ÀÖ°í Å¾½ÂÁßÀÎ °æ¿ì Å¾½ÂÃ³¸®
	const DWORD dwVehicleID = pmsg->mMountedVehicle.mVehicleIndex;
	const DWORD dwVehicleSeatPos = pmsg->mMountedVehicle.mSeatIndex;

	// Player Å»°Í Á¤º¸ ¼³Á¤
	pPlayer->SetVehicleID( dwVehicleID );
	pPlayer->SetVehicleSeatPos( dwVehicleSeatPos );
	pPlayer->SetRideFurnitureID( pmsg->mRiderInfo.dwFurnitureObjectIndex );
	pPlayer->SetRideFurnitureSeatPos(pmsg->mRiderInfo.wSlot);

	if( HOUSINGMGR->IsHousingMap() )
	{
		pPlayer->SetAngle(
			HOUSINGMGR->GetStartAngle());
		pPlayer->SetCharacterSize(
			1.0f);
	}
	else if(GTMAPNUM == MAP->GetMapNum() || MAP->GetMapNum() == PVP)//aziz normal size pvp //battle arena
	{
		pPlayer->SetCharacterSize(
			1.0f);
	}
	else if(FALSE == SIEGEWARFAREMGR->Is_CastleMap() &&
		TRUE == SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()))
	{
		pPlayer->SetCharacterSize(
			1.0f);
	}
}

void CGameIn::UserConn_Monster_Add(void* pMsg )
{
	SEND_MONSTER_TOTALINFO* pmsg = (SEND_MONSTER_TOTALINFO*)pMsg;
	if(pmsg->TotalInfo.MonsterKind > 10000)
	{
		ASSERT(0);
	}

	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
			yCASE(CAddableInfoList::ShowdownInfo)
			yCASE(CAddableInfoList::StreetStall)
		yENDSWITCH
		
		iter.ShiftToNextData();
	}
	
#ifdef TAIWAN_LOCAL
	//ÀÓ½Ã·Î! ¸ó½ºÅÍÀÌ¸§À» ¼­¹ö¿¡¼­ ¾Èº¸³»ÁÖµµ·Ï ÇÏÀÚ!
	SafeStrCpy( pmsg->BaseObjectInfo.ObjectName,
		GAMERESRCMNGR->GetMonsterListInfo( pmsg->TotalInfo.MonsterKind )->Name,
		MAX_NAME_LENGTH+1 );
#endif
	
	CMonster* pMon = OBJECTMGR->AddMonster(&pmsg->BaseObjectInfo,&moveInfo,&pmsg->TotalInfo);
						
	//////////////////////////////////////////////////////////////////////////

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pMon;
		//EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pMon,&set,0,pMon->GetID());
		EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pMon,&set,0,pMon->GetID());
	}
}


void CGameIn::UserConn_BossMonster_Add(void* pMsg )
{
	SEND_MONSTER_TOTALINFO* pmsg = (SEND_MONSTER_TOTALINFO*)pMsg;
	if(pmsg->TotalInfo.MonsterKind > 10000)
	{
		ASSERT(0);
	}
	
	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
			yCASE(CAddableInfoList::ShowdownInfo)
			yCASE(CAddableInfoList::StreetStall)
		yENDSWITCH
		
		iter.ShiftToNextData();
	}
	
#ifdef TAIWAN_LOCAL
	//ÀÓ½Ã·Î! ¸ó½ºÅÍÀÌ¸§À» ¼­¹ö¿¡¼­ ¾Èº¸³»ÁÖµµ·Ï ÇÏÀÚ!
	SafeStrCpy( pmsg->BaseObjectInfo.ObjectName,
		GAMERESRCMNGR->GetMonsterListInfo( pmsg->TotalInfo.MonsterKind )->Name,
		MAX_NAME_LENGTH+1 );
#endif
	
	CBossMonster* pMon = OBJECTMGR->AddBossMonster(&pmsg->BaseObjectInfo,&moveInfo,&pmsg->TotalInfo);
						
	//////////////////////////////////////////////////////////////////////////

	if(pmsg->bLogin)
	{
		TARGETSET set;
		set.pTarget = pMon;

		switch( pMon->GetMonsterKind() )
		{
		case eBOSSKIND_TARINTUS:
			{
				// 080108 KTH -- µîÀå ¾Ö´Ï¸ÞÀÌ¼ÇÀ» ¼³Á¤ÇØ ÁØ´Ù.
				EFFECTMGR->StartEffectProcess(
					FindEffectNum("BOSS_Tarintus_Regen.beff"), 
					pMon, NULL, 0, pMon->GetID());
				// 080109 KTH -- ¿ÀºêÁ§Æ®¸¦ ¼±ÅÃµÇÁö ¾Êµµ·Ï ¼³Á¤ÇÑ´Ù.
				pMon->GetEngineObject()->DisablePick();
				// 080109 KTH -- µîÀåÇÒ µ¿¾ÈÀÇ ½Ã°£À» Á¤ÇØ ÁØ´Ù.
				pMon->SetOpenningTime(8000);
				break;
			}
			break;
		case eBOSSKIND_LEOSTEIN:
			{
				EFFECTMGR->StartEffectProcess(
					FindEffectNum("BOSS_LeoStein_Regen.beff"), 
					pMon, NULL, 0, pMon->GetID());
				pMon->GetEngineObject()->DisablePick();
				pMon->SetOpenningTime(3000);
			}
			break;
		case eBOSSKIND_DRAGONIAN:
			{
				EFFECTMGR->StartEffectProcess(
					9507, 
					pMon, NULL, 0, pMon->GetID());
				pMon->GetEngineObject()->DisablePick();
				pMon->SetOpenningTime(7000);
			}
			break;
		default:
			EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pMon,&set,0,pMon->GetID());
			break;
		}
	}
}


void CGameIn::UserConn_ChangeMap_Ack(void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	DWORD map = pmsg->dwData;
	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, RESRCMGR->GetMsg( 290 ) );
	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&map,4);
	// 070623 LYW --- GameIn : SaveChatList.
	CHATMGR->SaveChatList() ;

	// 080411 LYW --- GameIn : Ã¤ÆÃ¹æ¿¡¼­ Ã¤ÆÃÁßÀÌ¾ú´Ù¸é, Ã¤ÆÃ³»¿ëÀ» ÀúÀåÇÑ´Ù.
	CHATROOMMGR->SaveChat() ;

	// ÀÎ´ø¸Ê¿¡¼­ ´Ù¸¥°÷À¸·Î ÀÌµ¿ÇÏ¸é ÀÎ´øÁ¤º¸¸¦ Áö¿ìÀÚ.
	MAPTYPE mapNum = MAP->GetMapNum();
	if(DungeonMGR->IsDungeonMap(mapNum))
	{
		DungeonMGR->ClearCurDungeon();
	}
}

// 070918 LYW --- GameIn : Add function to change map through the npc.
void CGameIn::UserConn_Npc_ChangeMap_Ack(void* pMsg)
{
	if( !pMsg ) return ;

	cNpcScriptDialog* pDlg = GAMEIN->GetNpcScriptDialog() ;
	if( !pDlg ) return ;

	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg ;

	DWORD dwMapNum = pmsg->dwData ;

	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, RESRCMGR->GetMsg( 290 ) );

	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&dwMapNum,4);

	pDlg->EndDialog() ;
	pDlg->SetActive(FALSE) ;

	CHATMGR->SaveChatList() ;

	// 080411 LYW --- GameIn : Ã¤ÆÃ¹æ¿¡¼­ Ã¤ÆÃÁßÀÌ¾ú´Ù¸é, Ã¤ÆÃ³»¿ëÀ» ÀúÀåÇÑ´Ù.
	CHATROOMMGR->SaveChat() ;
}


void CGameIn::UserConn_Cheat_ChangeMap_Ack(void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	DWORD map = pmsg->dwData1;

//---KES Ã¤³ÎÀÌµ¿ ¼öÁ¤
	gChannelNum = pmsg->dwData2;
	gCheatMove = TRUE;
//--------------------
    

	// 071208 LYW --- GameIn : ¸Ê ÀÌµ¿ ½ºÅ©·Ñ »ç¿ë½Ã, ¸Ê ÀÌµ¿ ÇÏ¸é¼­ ÀÌÂÊÀ¸·Î µé¾î¿À±â ¶§¹®¿¡, 
	// ¸Þ½ÃÁö ¹øÈ£¸¦ 700 -> 206À¸·Î ¼öÁ¤ÇÑ´Ù.
	WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, CHATMGR->GetChatMsg( 206 ) );
	
	MAPCHANGE->SetGameInInitKind(eGameInInitKind_MapChange);
	MAINGAME->SetGameState(eGAMESTATE_MAPCHANGE,&map,4);
	CHATMGR->SaveChatList();

	// 080411 LYW --- GameIn : Ã¤ÆÃ¹æ¿¡¼­ Ã¤ÆÃÁßÀÌ¾ú´Ù¸é, Ã¤ÆÃ³»¿ëÀ» ÀúÀåÇÑ´Ù.
	CHATROOMMGR->SaveChat() ;
}


void CGameIn::UserConn_ChangeMap_Nack()
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(388) );
	g_UserInput.SetInputFocus(TRUE);
}

// 090512 ONS Å¸Á¾Á·ÀÇ ½Å±ÔÁ¾Á· ½ÃÀÛ¸ÊÀ¸·ÎÀÇ ÀÌµ¿ Á¦ÇÑ - ¸Þ¼¼Áö¹øÈ£ º¯°æ ¿ä¸Á.
void CGameIn::UserConn_DevilMarket_ChangeMap_Nack()
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(388) );
	g_UserInput.SetInputFocus(TRUE);
}

void CGameIn::UserConn_Npc_ChangeMap_Nack()
{
	WINDOWMGR->MsgBox( MBI_FAILED_NPC_CHANGE_MAP, MBT_OK, CHATMGR->GetChatMsg( 56 ) );
	g_UserInput.SetInputFocus(TRUE);

	cNpcScriptDialog* pDlg = GAMEIN->GetNpcScriptDialog() ;

	if( !pDlg ) return ;

	pDlg->EndDialog() ;
	pDlg->SetActive(FALSE) ;
}


void CGameIn::UserConn_CharacterList_Ack(void* pMsg )
{
	SEND_CHARSELECT_INFO* pmsg = (SEND_CHARSELECT_INFO*)pMsg;
	MAPCHANGE->SetGameInInitKind( eGameInInitKind_Login );
	MAINGAME->SetGameState(eGAMESTATE_CHARSELECT, (void *)pmsg, sizeof(SEND_CHARSELECT_INFO));
	//ITEMMGR->DeleteShopItemInfo();
}


void CGameIn::UserConn_BackToCharSel_Nack()
{
	WINDOWMGR->AddListDestroyWindow( WINDOWMGR->GetWindowForID( MBI_NOBTNMSGBOX ) );
	OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Enter );//AO¨öA
}


void CGameIn::UserConn_MapDesc( void* pMsg )
{
	MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;
	MAP->SetVillage( pmsg->wData1 );
	PKMGR->SetPKAllow( pmsg->wData2 );
	gChannelNum = (int)pmsg->wData3;
	//aziz auto PVP when entering PVP map & add operator notice
	if(MAP->GetMapNum()==PVP) //battle arena
	{
		PKMGR->SendMsgPKOn();
		CHATMGR->AddMsg( CTC_OPERATOR , "Welcome to Free Battle Mode" );
	}
	if (g_csDateManager.IsChallengeZone(MAP->GetMapNum()))
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1221 ) );
		return;
	}

#ifdef TAIWAN_LOCAL					
	if( MAP->GetMapNum() == EVENTMAPNUM || MAP->GetMapNum() == Tournament || 
		MAP->GetMapNum() == QuestRoom )
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 394 ), GetMapName( MAP->GetMapNum() ) );
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 384 ), gChannelNum+1, GetMapName( MAP->GetMapNum() ) );
	}
#else
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 384 ), GetMapName( MAP->GetMapNum() ), gChannelNum+1 );
#endif

	if( PKMGR->IsPKAllow() && MAP->GetMapNum()!=PVP )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 383 ) );
	}

	// 06. 03. ¹®ÆÄ°øÁö - ÀÌ¿µÁØ
//	if(HERO->GetGuildIdx() && GUILDMGR->GetGuildNotice())
//		CHATMGR->AddMsg(CTC_GUILD_NOTICE, CHATMGR->GetChatMsg(395), GUILDMGR->GetGuildNotice());
}


void CGameIn::UserConn_SetVisible( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pObj = OBJECTMGR->GetObject(pmsg->dwData1);
	if( !pObj ) // AI¨¬I¨¬¨¢¢¯¢®¨ù¡© NULL¡Æ¨£ AuA¢Ò(E¢çAI¢¯a!)
	{
		ASSERT(0);
		return;
	}
	
	if(pObj->GetObjectKind() != eObjectKind_Player)
	{
		ASSERT(0);
		return;
	}

	CPlayer* ppla = (CPlayer*)pObj;
	if(pmsg->dwData2)
	{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
			ppla->GetEngineObject()->SetAlpha( 1.0f );
		else
#endif						
			ppla->GetEngineObject()->Show();
		
		ppla->GetCharacterTotalInfo()->bVisible = true;
	}
	else
	{
#ifdef _GMTOOL_
		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
			ppla->GetEngineObject()->SetAlpha( 0.3f );
		else
#endif						
			ppla->GetEngineObject()->HideWithScheduling();

		ppla->GetCharacterTotalInfo()->bVisible = false;
	}
}


void CGameIn::NPC_NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_NPC_SPEECH_ACK:
		{
			Npc_Speech_Ack( pMsg ) ;
			// 071021 LYW --- GameIn : Check tutorial condition.
			TUTORIALMGR->Check_NpcTalk(pMsg) ;								// ´ëÈ­¸¦ ½ÃÀÛÇÑ npc¸¦ Ã¼Å©ÇÏ±â À§ÇÑ ÇÔ¼ö.
		}
		break;
	case MP_NPC_SPEECH_NACK:
		{
			Npc_Speech_Nack( pMsg ) ;
		}
		break;
	case MP_NPC_CLOSEBOMUL_ACK:
		{
			Npc_CloseBomul_Ack() ;
		}
		break;
	case MP_NPC_DIE_ACK:
		{
			Npc_Die_Ack( pMsg ) ;
		}
		break;
		
	case MP_NPC_DOJOB_NACK:
		{
			Npc_DoJob_Nack() ;			
		}
		break;

	case MP_NPC_CHECK_HACK_ACK:
		{
			MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
			NPCSCRIPTMGR->StartNpcBusiness(pmsg->dwData3);
		}
		break;
	case MP_NPC_CHECK_HACK_NACK:
		{
			if( HERO->GetState() == eObjectState_Deal )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);

			MSG_DWORD *pmsg = (MSG_DWORD*)pMsg;
			if(pmsg->dwData == 271) // bomul
			{
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(391) );
			}

			// 081009 LYW --- GameIn : °ø¼º ¼Ò½º ¸ÓÁö
			if(pmsg->dwData == 135 || pmsg->dwData == 136)
			{
				GAMEIN->GetNpcScriptDialog()->EndDialog();
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1705));
			}
		}
		break;

	// 090227 ShinJS --- ÀÌµ¿NPC¿¡ ´ëÇÑ Ã³¸®
	case MP_NPC_READYTOMOVE_NOTIFY:
		{
			// ÀÌµ¿NPC ÀÌµ¿ÁØºñ ¸Þ¼¼Áö
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( STATICNPC_ID_START + pmsg->wData );
			// ÇØ´ç NPC ID°¡ Á¸ÀçÇÏ´Â°æ¿ì ÀÌµ¿ÁØºñ ¼³Á¤
			if( pNpc )
				pNpc->SetReadyToMove( TRUE );

			// NPC ÀÌ¸§ ±¸ÇÏ±â
			char* strNPCName = "";
			if( pNpc )
			{
				strNPCName = pNpc->GetObjectName();
			}
			else
			{
				STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetMoveNpcInfo( pmsg->wData );
				if( pStaticNpcInfo )
					strNPCName = pStaticNpcInfo->NpcName;
			}

			CDealDialog* pDealDlg = GAMEIN->GetDealDialog();
			cNpcScriptDialog* pNpcScriptDlg	= GAMEIN->GetNpcScriptDialog();

			// DealDialog°¡ È°¼ºÈ­ µÇ¾î ÀÖ°í µô·¯°¡ ÀÌµ¿NPCÀÎ °æ¿ì ÀÛ¾÷Ã¢À» ´Ý´Â´Ù.
			if( pDealDlg )
			if( pDealDlg->IsActive() && pDealDlg->GetDealerIdx() == pmsg->wData )
			{
				cDialog* pBuyDivideBox	= WINDOWMGR->GetWindowForID( DBOX_BUY );			// ¹°°Ç »ì¶§ °¹¼ö ÀÔ·ÂÃ¢
				cDialog* pSellDivideBox	= WINDOWMGR->GetWindowForID( DBOX_SELL );			// ¹°°Ç ÆÈ¶§ °¹¼ö ÀÔ·ÂÃ¢
				cDialog* pBuyMsgBox		= WINDOWMGR->GetWindowForID( MBI_BUYITEM );			// ¹°°Ç »ì¶§ MSG BOX
				cDialog* pSellMsgBox	= WINDOWMGR->GetWindowForID( MBI_SELLITEM );		// ¹°°Ç ÆÈ¶§ MSG BOX
				

				if( pBuyDivideBox )
				{
					pBuyDivideBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pBuyDivideBox );
				}
				if( pSellDivideBox )
				{
					pSellDivideBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pSellDivideBox );
				}
				if( pBuyMsgBox )
				{
					pBuyMsgBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pBuyMsgBox );
				}
				if( pSellMsgBox )
				{
					pSellMsgBox->SetActive( FALSE );
					WINDOWMGR->AddListDestroyWindow( pSellMsgBox );
				}

				((cDialog*)GAMEIN->GetInventoryDialog())->SetDisable( FALSE );
				((cDialog*)GAMEIN->GetInventoryDialog())->SetActive( FALSE );				// ÀÎº¥Åä¸®¸¦ ´Ý´Â´Ù
				pDealDlg->SetDisable( FALSE );
				pDealDlg->SetActive( FALSE );												// ÆÇ¸ÅÃ¢À» ´Ý´Â´Ù

				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1910), strNPCName );		// ÀÌµ¿ÁØºñÀÛ¾÷À¸·Î ÀÎÇÑ °Å·¡Áß´Ü¸Þ¼¼Áö¸¦ Ãâ·ÂÇÑ´Ù
			}

			// ScriptDialog°¡ È°¼ºÈ­ µÇ¾î ÀÖ°í ÀÌµ¿NPCÀÎ °æ¿ì ´Ý´Â´Ù.
			if( pNpcScriptDlg )
			if( pNpcScriptDlg->IsActive() && pNpcScriptDlg->GetCurNpcIdx() == pmsg->wData)
			{
				pNpcScriptDlg->SetActive( FALSE );											// NPC Script Ã¢À» ´Ý´Â´Ù

				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1910), strNPCName );		// ÀÌµ¿ÁØºñÀÛ¾÷À¸·Î ÀÎÇÑ °Å·¡Áß´Ü¸Þ¼¼Áö¸¦ Ãâ·ÂÇÑ´Ù
			}

		}
		break;

	case MP_NPC_REMOVE_MOVENPC_SYN:
		{
			// ÀÌµ¿NPC Á¦°Å
			MSG_WORD* pmsg = (MSG_WORD*)pMsg;

			// ¼±ÅÃµÇ¾î ÀÖ´Â NPC°¡ Áö¿öÁú ÀÌµ¿NPCÀÎ °æ¿ì
			if( OBJECTMGR->GetSelectedObjectID() == STATICNPC_ID_START + pmsg->wData )
			{
				CObject* pObject = OBJECTMGR->GetSelectedObject();
				if( pObject )
				{
					pObject->OnDeselected();
					pObject->ShowObjectName( FALSE, NAMECOLOR_DEFAULT );		// ¿ÀºêÁ§Æ® ÀÌ¸§À» µðÆúÆ® »ö»óÀ¸·Î ¼¼ÆÃÇÑ´Ù.
				}
				OBJECTMGR->SetSelectedObject( NULL ) ;							// ¿ÀºêÁ§Æ® ¼±ÅÃÀ» ÇØÁ¦ ÇÑ´Ù.
			}

			
			OBJECTMGR->RemoveNPC( STATICNPC_ID_START + pmsg->wData );			// ÀÌµ¿NPC¸¦ Á¦°ÅÇÑ´Ù
		}
		break;

	case MP_NPC_ADD_MOVENPC_SYN:
		{
			// ÀÌµ¿NPC Ãß°¡
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			WORD wMapNum		= LOWORD( pmsg->dwData1 );						// NPC Map ¹øÈ£
			WORD wDir			= HIWORD( pmsg->dwData1 );						// NPC ¹æÇâ
			WORD wNpcIdx		= LOWORD( pmsg->dwData2 );						// NPC Index
			WORD wNpcKindIdx	= HIWORD( pmsg->dwData2 );						// NPC Á¾·ù
			WORD wXPos			= LOWORD( pmsg->dwData3 );						// À§Ä¡
			WORD wZPos			= HIWORD( pmsg->dwData3 );
			BOOL bReadyToMove	= BOOL( pmsg->dwData4 );

			CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( STATICNPC_ID_START + wNpcIdx );
			// ÇØ´ç NPC ID°¡ Á¸ÀçÇÏ´Â°æ¿ì (µð¹ö±×½Ã ÇÑ¹ø¿¡ ¿©·¯°³ÀÇ Ãß°¡µ¥ÀÌÅÍ¸¦ ¹ÞÀº °æ¿ì)
			if( pNpc )
			{
				BASEMOVE_INFO* pminfo = pNpc->GetBaseMoveInfo();
				if( !pminfo )	break;

				pminfo->CurPosition.x = wXPos;										// ÀÌµ¿Á¤º¸¿¡ À§Ä¡¸¦ ¼¼ÆÃÇÑ´Ù.
				pminfo->CurPosition.z = wZPos;

				pNpc->SetAngle( DEGTORAD( float(wDir) ) );							// ¹æÇâÀ» ¼¼ÆÃÇÑ´Ù.
				pNpc->SetReadyToMove( FALSE );										// ÀÌµ¿Àü ÁØºñÀÛ¾÷ ÃÊ±âÈ­

				MOVEMGR->InitMove( pNpc, pminfo );
			}
			else
			{
				// ÀÌµ¿NPC Ãß°¡
				NPC_TOTALINFO tinfo;												// npc ÅäÅ» Á¤º¸¸¦ ´ãÀ» ±¸Á¶Ã¼¸¦ ¼±¾ðÇÑ´Ù.
				BASEOBJECT_INFO binfo;												// ±âº» ¿ÀºêÁ§Æ® Á¤º¸¸¦ ´ãÀ» ±¸Á¶Ã¼¸¦ ¼±¾ðÇÑ´Ù.
				BASEMOVE_INFO minfo;												// ±âº» ÀÌµ¿ Á¤º¸¸¦ ´ãÀ» ±¸Á¶Ã¼¸¦ ¼±¾ðÇÑ´Ù.

				binfo.dwObjectID = STATICNPC_ID_START + wNpcIdx;
				binfo.ObjectState = eObjectState_None;
				tinfo.MapNum = wMapNum;
				minfo.bMoving = FALSE;
				minfo.KyungGongIdx = 0;
				minfo.MoveMode = eMoveMode_Run;
				tinfo.NpcKind = wNpcKindIdx;										// ÅäÅ» Á¤º¸¿¡ npc Á¾·ù¸¦ ¼¼ÆÃÇÑ´Ù.

				// ±âº» Á¤º¸¿¡ npc ÀÌ¸§À» ¼¼ÆÃÇÑ´Ù.
                STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetMoveNpcInfo( wNpcIdx );
				if( pStaticNpcInfo )
					SafeStrCpy( binfo.ObjectName, pStaticNpcInfo->NpcName, MAX_NPC_NAME_LENGTH+1 );
				else
					binfo.ObjectName[0] = 0;

				tinfo.NpcUniqueIdx = wNpcIdx;										// ÅäÅ» Á¤º¸¿¡ npc À¯´ÏÅ© ÀÎµ¦½º¸¦ ¼¼ÆÃÇÑ´Ù.

				NPC_LIST* pInfo = GAMERESRCMNGR->GetNpcInfo( tinfo.NpcKind );		// npc Á¾·ù¸¦ ÂüÁ¶ÇÏ¿© npc ¸®½ºÆ® Á¤º¸¸¦ ¹Þ´Â´Ù.
				if(pInfo)															// npc ¸®½ºÆ® Á¤º¸°¡ À¯È¿ÇÏ¸é,
				{
					tinfo.NpcJob = pInfo->JobKind;									// ÅäÅ» Á¤º¸ÀÇ npc Á÷¾÷À» ¼¼ÆÃÇÑ´Ù.
				}
				else																// npc ¸®½ºÆ® Á¤º¸°¡ À¯È¿ÇÏÁö ¾ÊÀ¸¸é,
				{
					tinfo.NpcJob = 0;												// Á÷¾÷À» 0À¸·Î ¼¼ÆÃÇÑ´Ù.
				}

				//jopÀÌ 0ÀÌ¸é ÀÌ¸§À» Áö¿ìÀÚ.
				if( tinfo.NpcJob == 0 )												// Á÷¾÷ÀÌ 0ÀÌ¸é,
				{
					binfo.ObjectName[0] = 0;										// ÀÌ¸§À» Áö¿î´Ù.
				}

				minfo.CurPosition.x = wXPos;										// ÀÌµ¿Á¤º¸¿¡ À§Ä¡¸¦ ¼¼ÆÃÇÑ´Ù.
				minfo.CurPosition.y = 0;
				minfo.CurPosition.z = wZPos;

				pNpc = OBJECTMGR->AddNpc(&binfo,&minfo,&tinfo);						// NPC Ãß°¡

				pNpc->SetAngle( DEGTORAD( float(wDir) ) );							// ¹æÇâÀ» ¼¼ÆÃÇÑ´Ù.
			}

			pNpc->SetReadyToMove( bReadyToMove );									// ÀÌµ¿ÁØºñ»óÅÂ ¼³Á¤

			TARGETSET set;
			set.pTarget = pNpc;
			EFFECTMGR->StartEffectProcess(eEffect_NewCharacter,pNpc,&set,0,pNpc->GetID());	// Ä³¸¯ÅÍ ÃâÇö ÀÌÆåÆ®¸¦ ½ÇÇàÇÑ´Ù
		}
		break;
	case MP_NPC_PUT_CHAT_ACK:
		{
			MSG_NAME_DWORD2* const message = (MSG_NAME_DWORD2*)pMsg;
			LPCTSTR fileName = message->Name;
			const DWORD textIndex = message->dwData2;

			cTutorialDlg* const dialog = (cTutorialDlg*)WINDOWMGR->GetWindowForID(TUTORIAL_DLG);

			if(0 == dialog)
			{
				break;
			}

			dialog->SetNpcImage(fileName);
			dialog->SetText(
				CHATMGR->GetChatMsg(textIndex));
			dialog->SetInvisibleSecond(5);
			dialog->SetActive(TRUE);
		}
		break;
	}
}


void CGameIn::Npc_Speech_Ack( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

	CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject(pmsg->dwData1);

	if( !pNpc )
	{
		ASSERT(0);
		if( HERO->GetState() == eObjectState_Deal )
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		return;
	}


	//090604 pdy ÇÏ¿ìÂ¡ NPC¸µÅ© ÀÌµ¿ Ãß°¡ ´ëÈ­Ã¢ÀÌ¾Æ´Ñ ÆË¾÷Ã¢ÀÌ ¶ß´Â Æ¯¼ö Á÷¾÷Àº µû·Î Ã³¸®ÇÏ°í ¸®ÅÏ 
	char szBuf[128] = {0,};
	switch( pNpc->GetNpcJob() )
	{
		case HOUSING_RANK1_ROLE:
		case HOUSING_RANK2_ROLE:
		case HOUSING_RANK3_ROLE:
			{
				// 091019 pdy ÇÏ¿ìÂ¡¸Ê¿¡¼­ ·ÎµùÁß¿¡ °¢Á¾ ÇÏ¿ìÂ¡ ±â´É »ç¿ë±ÝÁö Ãß°¡
				if( HOUSINGMGR->IsHousingMap() &&	
					HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
				{
					return;
				}

				//090527 pdy ÇÏ¿ìÂ¡ ÆË¾÷Ã¢ [·©Ä¿Áý ¸µÅ©ÀÔÀå]
				BASEOBJECT_INFO NpcBaseInfo;
				pNpc->GetBaseObjectInfo(&NpcBaseInfo);

				cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1903) , NpcBaseInfo.ObjectName );	//1903	"%s ·©Ä¿ ÁýÀ¸·Î ÀÌµ¿ÇÏ½Ã°Ú½À´Ï±î?"
				
				if( pBox )
					pBox->SetParam( pNpc->GetID() );

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);
			}
			return;
		case HOUSING_PORTAL_ROLE:
			{
				// 091019 pdy ÇÏ¿ìÂ¡¸Ê¿¡¼­ ·ÎµùÁß¿¡ °¢Á¾ ÇÏ¿ìÂ¡ ±â´É »ç¿ë±ÝÁö Ãß°¡
				if( HOUSINGMGR->IsHousingMap() &&	
					HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
				{
					return;
				}

				sprintf(szBuf,CHATMGR->GetChatMsg(1890), pNpc->GetNpcTotalInfo()->szSummonerName );	//1890	"%sÀÇ ÁýÀ¸·Î ÀÌµ¿ ÇÏ½Ã°Ú½À´Ï±î?"

				//090604 pdy ÇÏ¿ìÂ¡ ÆË¾÷Ã¢ [¼ÒÈ¯Æ÷Å»NPC ¸µÅ©ÀÔÀå]
				cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_AREYOUSURE, MBT_YESNO, szBuf );
				if( pBox )
					pBox->SetParam( pNpc->GetID() );

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);
			}
			return;

		case NOTE_ROLE:
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( NOTE_NOTEDLG );
				ASSERT( dialog );

				if( dialog->IsActive() )
				{
					dialog->SetActive( FALSE );
				}
				// ÂÊÁö Ã¢À» ¿­·Á¸é ÀÏ´Ü ¼­¹ö¿¡ ¸ñ·ÏÀ» ¿äÃ»ÇØ¾ßÇÑ´Ù.
				else
				{
					NOTEMGR->NoteListSyn( 1, eMode_NormalNote );
				}

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);

				/*
				// ¿ìÃ¼ÅëÀº Ä³¸¯ÅÍÂÊÀ¸·Î ¹æÇâÀ» µ¹Áö¸»ÀÚ.
				VECTOR3 pos = {0};
				HERO->GetPosition(&pos);
				MOVEMGR->SetLookatPos(pNpc, &pos, 0, gCurTime);
				*/
			}
			return;

		case CONSIGNMENT_ROLE:
			{
				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if( pDlg && !pDlg->IsActive())
				{
					pDlg->SetActive(TRUE);
					pDlg->SetMode(CConsignmentDlg::eConsignment_Mode_Buy);
				}
				else
				{
					pDlg->SetActive(FALSE);
				}

				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);

				VECTOR3 pos = {0};
				HERO->GetPosition(&pos);
				MOVEMGR->SetLookatPos(pNpc, &pos, 0, gCurTime);
			}
			return;
	}


	switch( pNpc->GetNpcKind() )
	{
	case 30:
		{
			cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_SELECTLOGINPOINT, MBT_YESNO, CHATMGR->GetChatMsg(381), pNpc->GetObjectName() );
			if( pBox )
				pBox->SetParam( pNpc->GetID() );
		}
		break;
	default:
		{
			NPCSCRIPTMGR->StartNpcScript( pNpc );
		}
		break;
	}
}


void CGameIn::Npc_Speech_Nack( void* pMsg )
{
	if( HERO->GetState() == eObjectState_Deal )
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);

	MSG_DWORD *pmsg = (MSG_DWORD*)pMsg;
	if(pmsg->dwData == 271) // bomul
	{
		NPCSCRIPTMGR->SetSelectedNpc(NULL);
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(391) );
	}
}


void CGameIn::Npc_CloseBomul_Ack()
{
	if(GetNpcScriptDialog()->IsActive())
	{
		NPCSCRIPTMGR->SetSelectedNpc(NULL);
		GetNpcScriptDialog()->SetActive(FALSE);						
	}
}


void CGameIn::Npc_Die_Ack( void* pMsg )
{
	MSG_DWORD * pmsg = (MSG_DWORD*)pMsg;
	CNpc* pNpc = NPCSCRIPTMGR->GetSelectedNpc();
	if(pNpc)
	{
		if(pNpc->GetID() == pmsg->dwData)
		{
			if(GetNpcScriptDialog()->IsActive())
			{
				NPCSCRIPTMGR->SetSelectedNpc(NULL);
				GetNpcScriptDialog()->SetActive(FALSE);						
			}
		}
	}
}


void CGameIn::Npc_DoJob_Nack()
{
	NPCSCRIPTMGR->SetSelectedNpc(NULL);
	GetNpcScriptDialog()->SetActive(FALSE);						
	if( HERO->GetState() == eObjectState_Deal )
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
}


void CGameIn::SIGNAL_NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_SIGNAL_COMMONUSER:
		{
			Signal_CommonUser( pMsg ) ;
		}
		break;
	case MP_SIGNAL_USER_ADD_ITEM:
		{
			Signal_UserAddItem( pMsg ) ;
		}
		break;
	case MP_SIGNAL_SHOWDOWN_RESULT:	//AO¨öA
		{
			Signal_ShowDown_Result( pMsg ) ;
		}
		break;	
	case MP_SIGNAL_COMMONUSER_EVENT:
		{
			TESTMSG* const data = (TESTMSG*)pMsg;

			CHATMGR->AddMsg(CTC_EVENT, data->Msg);

		} break;
	case MP_SIGNAL_USER_ADD_ITEM_BOX:
		{
			//Signal_UserAddItem( pMsg ) ;
			MSG_USER_ADD_ITEM_BOX* msg = (MSG_USER_ADD_ITEM_BOX*)pMsg;
			char MessageBuff[MAX_PATH] = { 0, };

			ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(msg->dwIconIdx1);
			char	szItemName[MAX_ITEMNAME_LENGTH+1];
			_parsingKeywordString(pItemInfo->ItemName, szItemName);

			ITEM_INFO* pItemInfo2 = ITEMMGR->GetItemInfo(msg->dwIconIdx2);
			char	szItemName2[MAX_ITEMNAME_LENGTH+1];
			_parsingKeywordString(pItemInfo2->ItemName, szItemName2);

			//sprintf(MessageBuff, "[%s]´ÔÀÌ [%s] ¾ÆÀÌÅÛÀ» È¹µæ ÇÏ¿´½À´Ï´Ù.", msg->CharacterName, szItemName);
			wsprintf(MessageBuff, "Grats! [%s] open %s get (%d) %s", msg->CharacterName, szItemName, msg->dwNum, szItemName2);

			CHATMGR->AddMsg( CTC_GETITEM, MessageBuff );
		}
		break;
		//Alemuri Server Time Update--------------------------------------------------------------------
	case MP_SIGNAL_SERVER_TIME_UPDATE:
		{
			MSG_SERVERTIME* pmsg = (MSG_SERVERTIME*)pMsg;
			CServerTimeDlg* dialog = (CServerTimeDlg*)GetServerTimeDlg();
			dialog->SetActive(TRUE);
			dialog->SetTime(pmsg->dwYear, pmsg->dwMonth, pmsg->dwDay, pmsg->dwHour, pmsg->dwMinute);
		}
		break;
		//---------------------------------------------------------------------------------------------
		//Alemuri Item Drop Rates Info To Client-------------------------------------------------------
	case MP_SIGNAL_GET_ITEM_RATES:
		{
			const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
			HERO->SetServerDropRatesInfo(message->dwData1);
			HERO->SetServerDropRatesInfoMultiplier(message->dwData2);
		}
		break;
		//---------------------------------------------------------------------------------------------
	}
}


void CGameIn::Signal_CommonUser( void* pMsg )
{
	TESTMSG* const data = (TESTMSG*)pMsg;
	LPCTSTR keyword = _T("SystemMsg.bin\'");
	// 091019 LUJ, ¹®ÀÚ¿­ÀÌ ÁöÁ¤µÈ ¿¹¾à¾î·Î ½ÃÀÛµÇ¸é ½Ã½ºÅÛ ¸Þ½ÃÁö·Î °£ÁÖÇÑ´Ù
	const BOOL isSystemMessage = (0 == _tcsnicmp(data->Msg, keyword, _tcslen(keyword)));

	if(isSystemMessage)
	{
		LPCTSTR token = data->Msg + _tcslen(keyword);
		const DWORD messageIndex = _ttoi(token ? token : "");
		CHATMGR->AddMsg(
			CTC_OPERATOR,
			CHATMGR->GetChatMsg(messageIndex));
	}
	else
	{
		CHATMGR->AddMsg(
			CTC_OPERATOR,
			data->Msg);
	}
}
// 081114 KTH -- ¾ÆÀÌÅÛ È¹µæÀ» ¾Ë¸²
void CGameIn::Signal_UserAddItem( void* pMsg )
{
	MSG_USER_ADD_ITEM* msg = (MSG_USER_ADD_ITEM*)pMsg;
	char MessageBuff[MAX_PATH] = { 0, };

	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(msg->dwIconIdx);
	char	szItemName[sizeof(pItemInfo->ItemName)];//33
	_parsingKeywordString(pItemInfo->ItemName, szItemName);

	//sprintf(MessageBuff, "[%s]´ÔÀÌ [%s] ¾ÆÀÌÅÛÀ» È¹µæ ÇÏ¿´½À´Ï´Ù.", msg->CharacterName, szItemName);
	//aziz notify Boss Item
	//wsprintf(MessageBuff, CHATMGR->GetChatMsg(1267), msg->CharacterName, szItemName);//szItemName
	//aziz change ctc_operator
	//CHATMGR->AddMsg( CTC_OPERATOR2, CHATMGR->GetChatMsg(1267), msg->CharacterName, szItemName );//MessageBuff
	CHATMGR->AddMsg(CTC_ENCHANT, "%s : Obtainedesu %s", msg->CharacterName, szItemName);
}

void CGameIn::Signal_ShowDown_Result( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CObject* pWinner = OBJECTMGR->GetObject(pmsg->dwData1);
	CObject* pLoser = OBJECTMGR->GetObject(pmsg->dwData2);
	if( pWinner == NULL || pLoser == NULL ) return;
	
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(389),
	pWinner->GetObjectName(), pLoser->GetObjectName(), pWinner->GetObjectName() );
}

void CGameIn::UserConn_Pet_Add( void* pMsg )
{
	MSG_PET_ADD* pmsg = (MSG_PET_ADD*)pMsg;

	BASEMOVE_INFO moveInfo;
	moveInfo.AddedMoveSpeed = 0.f;
	moveInfo.SetFrom(&pmsg->MoveInfo);
	CAddableInfoIterator iter(&pmsg->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
			COMPRESSEDTARGETPOS tpos;
		iter.GetInfoData(&tpos);
		moveInfo.bMoving = TRUE;
		moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
		ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
		moveInfo.SetCurTargetPosIdx(0);
		for(BYTE n=0;n<tpos.PosNum;++n)
			tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
		yENDSWITCH

			iter.ShiftToNextData();
	}

	CPet* pPet = NULL;

	if(pmsg->MasterIdx == gHeroID)
	{
		if( pmsg->IsLogin ||
			0 == HEROPET)
		{
			// 091214 ONS Æê ¸ÊÀÌµ¿½Ã ¼ÒÈ¯°ü·Ã ¸Þ¼¼Áö¸¦ Ãâ·ÂÇÏÁö ¾Êµµ·Ï Ã³¸®.
			if(pmsg->IsSummoned)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1574) );
			}
			pPet = ( CPet* )OBJECTMGR->AddHeroPet( &( pmsg->BaseObjectInfo ), &moveInfo, &( pmsg->PetObjectInfo ), pmsg->ItemInfo );
		}
	}
	else
	{
		pPet = OBJECTMGR->AddPet( pmsg->MasterIdx, &( pmsg->BaseObjectInfo ), &moveInfo, &( pmsg->PetObjectInfo ), pmsg->ItemInfo );
	}

	// 091214 ONS Æê ¸ÊÀÌµ¿½Ã ÀÌÆåÆ®¸¦ Ãâ·ÂÇÏÁö ¾Êµµ·Ï Ã³¸®.
	if( pmsg->IsLogin && pmsg->IsSummoned )
	{
		TARGETSET set;
		set.pTarget = pPet;
		EFFECTMGR->StartEffectProcess(PETMGR->GetSummonEffect(),pPet,&set,0,pPet->GetID());
	}
}

void CGameIn::UserConn_HeroPet_Remove( void* pMsg )
{
	MSG_DWORD_WORD* pmsg = (MSG_DWORD_WORD*)pMsg;

	CObject* pPet = OBJECTMGR->GetObject(pmsg->dwData);
	CObject* pMaster = OBJECTMGR->GetObject(pmsg->dwObjectID);

	if( pPet == NULL )
	{
		return;
	}

	if( pMaster == HERO )
	{
		// 091214 ONS Æê ¸ÊÀÌµ¿½Ã ºÀÀÎ°ü·Ã ¸Þ¼¼Áö¸¦ Ãâ·ÂÇÏÁö ¾Êµµ·Ï Ã³¸®.
		if(pmsg->wData)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1575) );
		}
		OBJECTMGR->RemoveHeroPet();
		return;
	}

	APPEARANCEMGR->CancelAlphaProcess( pPet );
	APPEARANCEMGR->CancelReservation( pPet );
	OBJECTMGR->AddGarbageObject( pPet );
}

void CGameIn::UserConn_Vehicle_Add( void* pMsg )
{
	SEND_VEHICLE_TOTALINFO* const message = ( SEND_VEHICLE_TOTALINFO* )pMsg;
	BASEMOVE_INFO moveInfo;
	ZeroMemory( &moveInfo, sizeof(BASEMOVE_INFO) );
	moveInfo.SetFrom(&message->MoveInfo);
	CAddableInfoIterator iter(&message->AddableInfo);
	BYTE AddInfoKind;
	while((AddInfoKind = iter.GetInfoKind()) != CAddableInfoList::None)
	{
		ySWITCH(AddInfoKind)
			yCASE(CAddableInfoList::MoveInfo)
				COMPRESSEDTARGETPOS tpos;
				iter.GetInfoData(&tpos);
				moveInfo.bMoving = TRUE;
				moveInfo.SetMaxTargetPosIdx(tpos.PosNum);
				ASSERT( tpos.PosNum <= MAX_CHARTARGETPOSBUF_SIZE );
				moveInfo.SetCurTargetPosIdx(0);
				for(BYTE n=0;n<tpos.PosNum;++n)
					tpos.pos[n].Decompress(moveInfo.GetTargetPosition(n));
		yENDSWITCH

		iter.ShiftToNextData();
	}

	CVehicle* pVehicle = OBJECTMGR->AddVehicle( &message->BaseObjectInfo, &moveInfo, &message->TotalInfo );

	if(0 == pVehicle)
	{
		return;
	}

	pVehicle->SetInstall( TRUE );
	pVehicle->SetMasterName( message->MasterName );

	const ITEMBASE* const pItemBase = ITEMMGR->GetItemInfoAbsIn( 0, message->usedItemPosition );
	if( pItemBase != NULL )
	{
		ICONBASE usedItem = { 0 };
		usedItem.wIconIdx = pItemBase->wIconIdx;
		usedItem.Position = message->usedItemPosition;
		usedItem.dwDBIdx = pItemBase->dwDBIdx;

		pVehicle->SetUsedItem( usedItem );
	}

	// 090316 LUJ, Å¾½Â Ã³¸®
	{
		const DWORD seatMaxSize = sizeof( message->mSeat ) / sizeof( *( message->mSeat ) );

		for( DWORD seatIndex = 0; seatIndex < seatMaxSize; ++seatIndex )
		{
			const DWORD playerIndex = message->mSeat[ seatIndex ];
			CPlayer* const player = ( CPlayer* )OBJECTMGR->GetObject( playerIndex );

			if( 0 == player || player->GetObjectKind() != eObjectKind_Player )
			{
				continue;
			}

			pVehicle->GetOn( player, seatIndex + 1 );
		}
	}

	if(message->bLogin)
	{
		TARGETSET set;
		set.pTarget = pVehicle;
		EFFECTMGR->StartEffectProcess(eEffect_MonRegen,pVehicle,&set,0,pVehicle->GetID());
	}
}

void CGameIn::UserConn_Vehicle_Remove( void* pMsg )
{
	const MSG_DWORD2* const message = ( MSG_DWORD2* )pMsg;
	const DWORD playerIndex = message->dwData1;
	const DWORD vehicleIndex = message->dwData2;

	VEHICLEMGR->UnSummon( playerIndex, vehicleIndex );
	UserConn_Monster_Die( pMsg );
}

#ifdef _TESTCLIENT_

void InitTestHero()
{
	DWORD id = 1;
	{	// AOAI¡Æ©ª
		SEND_HERO_TOTALINFO info;
		memset(&info,0,sizeof(info));
		info.BaseObjectInfo.dwObjectID = id++;
		strcpy( info.BaseObjectInfo.ObjectName, "DARK");
		VECTOR3 pos;
		pos.x = GAMERESRCMNGR->m_TestClientInfo.x;
		pos.z = GAMERESRCMNGR->m_TestClientInfo.z;
		info.SendMoveInfo.CurPos.Compress(&pos);
		info.ChrTotalInfo.CurMapNum = GAMERESRCMNGR->m_TestClientInfo.Map;
		info.ChrTotalInfo.CurMapNum = GAMERESRCMNGR->m_TestClientInfo.Map;
		info.ChrTotalInfo.Race = GAMERESRCMNGR->m_TestClientInfo.Race;
		info.ChrTotalInfo.Gender = GAMERESRCMNGR->m_TestClientInfo.Gender;
		info.ChrTotalInfo.JobGrade = 1;
		info.ChrTotalInfo.Job[0] = GAMERESRCMNGR->m_TestClientInfo.Job;
		info.ChrTotalInfo.Life = 1000;
		info.ChrTotalInfo.HairType = GAMERESRCMNGR->m_TestClientInfo.HairType;
		info.ChrTotalInfo.FaceType = GAMERESRCMNGR->m_TestClientInfo.FaceType;
		info.ChrTotalInfo.MaxLife = 1000;
		info.HeroTotalInfo.Mana = 1000;
		info.HeroTotalInfo.MaxMana = 1000;
		// 080626 LYW --- Hero : ÃÖ´ë ·¹º§ Á¦ÇÑ ¼öÄ¡°¡ ¼ýÀÚ ¿´´ø °ÍÀ» Define ¹®À¸·Î ¼öÁ¤ÇÔ.
		//info.ChrTotalInfo.Level = 99;
		info.ChrTotalInfo.Level = MAX_CHARACTER_LEVEL_NUM;
		info.ChrTotalInfo.Height = 1;
		info.ChrTotalInfo.Width = 1;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Weapon] = GAMERESRCMNGR->m_TestClientInfo.WeaponIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Dress] = GAMERESRCMNGR->m_TestClientInfo.DressIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Hat] = GAMERESRCMNGR->m_TestClientInfo.HatIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Shoes] = GAMERESRCMNGR->m_TestClientInfo.ShoesIdx;
		info.ChrTotalInfo.WearedItemIdx[eWearedItem_Shield] = GAMERESRCMNGR->m_TestClientInfo.ShieldIdx;
		info.ChrTotalInfo.bVisible = true;
		BASEOBJECT_INFO obinfo;
		BASEMOVE_INFO moinfo;

		memset( &obinfo, 0, sizeof(obinfo) );		
		memset( &moinfo, 0, sizeof(moinfo) );

		obinfo.dwObjectID = 100;
		obinfo.dwUserID = 5572;
		sprintf( obinfo.ObjectName, "pet-01" );
		obinfo.BattleID = gChannelNum+2;

		moinfo.CurPosition = pos;
		moinfo.CurPosition.x -= 150;
		moinfo.bMoving = true;

		OBJECTMGR->RegisterHero(&info);
		GAMEIN->GetMiniMapDialog()->AddHeroIcon(HERO);

		CObject * pObj = (CObject*)OBJECTMGR->GetHero();
		pObj->SetPosition(&pos);

		CAMERA->Init(pObj,30,0,1000);
		CAMERA->SetTargetAngleX(0,29.9f,0);

		// 070313 LYW --- GameIn : Initialize hero's eye.
		HERO->InitializeEye() ;

		VECTOR2 sidpos;
		sidpos.x = 140;
		sidpos.y = 0;
		STATUSICONDLG->Init(HERO,&sidpos,5);

	//	CPetBase* pPet = OBJECTMGR->AddPet( &obinfo, &moinfo );
	//	HERO->SetPet( pPet );
	}
}


void InitTestMonster()
{
	DWORD id = 10;
	float x,z;
	x = GAMERESRCMNGR->m_TestClientInfo.x + 25;
	z = GAMERESRCMNGR->m_TestClientInfo.z + 500 + 25;


	DWORD temp =0;
	for(int n=0;n<GAMERESRCMNGR->m_TestClientInfo.MonsterNum;++n)
	{
		SEND_MONSTER_TOTALINFO info;
		memset(&info,0,sizeof(info));
		info.BaseObjectInfo.dwObjectID = id++;
		info.BaseObjectInfo.BattleID = gChannelNum+1;
		sprintf(info.BaseObjectInfo.ObjectName,"Monster%02d",temp++);
		VECTOR3 pos;
		pos.x = x;
		pos.z = z;
		info.MoveInfo.CurPos.Compress(&pos);
		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&info.MoveInfo);
		info.TotalInfo.MonsterKind = GAMERESRCMNGR->m_TestClientInfo.MonsterKind;
		g_pMonster = OBJECTMGR->AddMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
		
		x+= 100;
	}

	temp =0;
	for(n=0;n<GAMERESRCMNGR->m_TestClientInfo.BossMonsterNum;++n)
	{
		SEND_MONSTER_TOTALINFO info;
		memset(&info,0,sizeof(info));
		info.BaseObjectInfo.dwObjectID = id++;
		sprintf(info.BaseObjectInfo.ObjectName,"BossMonster%02d",temp++);
		VECTOR3 pos;
		pos.x = x;
		pos.z = z;
		info.MoveInfo.CurPos.Compress(&pos);
		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&info.MoveInfo);
		info.TotalInfo.MonsterKind = GAMERESRCMNGR->m_TestClientInfo.BossMonsterKind;
		g_pBossMonster = OBJECTMGR->AddBossMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
	//	g_pBossMonster->LoadBossMonster(GAMERESRCMNGR->m_TestClientInfo.BossInfofile);
		
		x+= 100;
	}

	x = GAMERESRCMNGR->m_TestClientInfo.x + 25;
	z -= 200;
	int lastnp = 0;
	for(n=0;n<GAMERESRCMNGR->m_TestClientInfo.CharacterNum;++n)
	{
		SEND_CHARACTER_TOTALINFO cinfo;
		memset(&cinfo,0,sizeof(cinfo));
		cinfo.BaseObjectInfo.dwObjectID = id++;
		sprintf(cinfo.BaseObjectInfo.ObjectName,"Character%02d",temp++);
		VECTOR3 pos;
		pos.x = x;
		pos.z = z;
		cinfo.MoveInfo.CurPos.Compress(&pos);
		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&cinfo.MoveInfo);
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.Height = 1;
		cinfo.TotalInfo.Width = 1;
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.Gender = 1;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Dress] = 0;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Weapon] = 0;
		OBJECTMGR->AddPlayer(&cinfo.BaseObjectInfo,&bminfo,&cinfo.TotalInfo);
		
		x+= 100;
		if(n/50 != lastnp)
		{
			lastnp = n/50;
			x = GAMERESRCMNGR->m_TestClientInfo.x + 25;
			z -= 200;
		}
	}
}

//trustpak
void InitTestCharacters(void)
{	
	DWORD id = 1000;
	DWORD temp =0;

	//for(n=0;n<GAMERESRCMNGR->m_TestClientInfo.CharacterNum;++n)
	DWORD n = 0;
	for (n = 0; n < GAMERESRCMNGR->m_TestClientInfo.dwTestCharacterNo;n++)
	{
		SEND_CHARACTER_TOTALINFO cinfo;
		memset(&cinfo,0,sizeof(cinfo));
		cinfo.BaseObjectInfo.dwObjectID = id++;
		sprintf(cinfo.BaseObjectInfo.ObjectName,"Test Character%02d",temp++);
		
		VECTOR3 pos;
		pos.x = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fX;
		pos.y = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fY;
		pos.z = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fZ;
		cinfo.MoveInfo.CurPos.Compress(&pos);

		BASEMOVE_INFO bminfo;
		bminfo.SetFrom(&cinfo.MoveInfo);
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.Height = 1;
		cinfo.TotalInfo.Width = 1;
		cinfo.TotalInfo.bVisible = 1;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Dress] = 23000 + rand()%150;
		cinfo.TotalInfo.WearedItemIdx[eWearedItem_Weapon] = 11000 + rand()%150;		
		cinfo.TotalInfo.Gender = GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].byGender;

		OBJECTMGR->AddPlayer(&cinfo.BaseObjectInfo,&bminfo,&cinfo.TotalInfo);		
		CObject * pObj = OBJECTMGR->GetObject(cinfo.BaseObjectInfo.dwObjectID);
		pObj->SetAngle(GAMERESRCMNGR->m_TestClientInfo.aTestCharacterInfo[n].fAngle);		
	}	
}

#endif

// 080916 LUJ, ÇÕ¼º Ã¢ ¹ÝÈ¯
CComposeDialog* CGameIn::GetComposeDialog()
{
	return ( CComposeDialog* )( WINDOWMGR->GetWindowForID( COMPOSE_DIALOG ) );
}

void CGameIn::AddBackupMsg(void* pMsg, DWORD dwMsgSize)
{
	char* pBackupMsg = new char[dwMsgSize];
	if(pBackupMsg)
	{
		ZeroMemory(pBackupMsg, dwMsgSize);
		memcpy(pBackupMsg, pMsg, dwMsgSize);
		m_pBackupMsgQueue.push(pBackupMsg);
	}
}

void CGameIn::ProcessBackupMsg()
{
#ifdef _GMTOOL_
	FILE* fp = fopen("PacketLog.txt", "wt");
	char buf[256] = {0,};
#endif

	while(FALSE == m_pBackupMsgQueue.empty())
	{
		MSGBASE* pMsg = (MSGBASE*)m_pBackupMsgQueue.front();
		m_pBackupMsgQueue.pop();
		if(pMsg)
		{
#ifdef _GMTOOL_
			if(fp)
			{
				sprintf(buf, "Category:[%02d] , Protocol:[%03d]\n", pMsg->Category, pMsg->Protocol);
				OutputDebugStringA(buf);
				fprintf(fp, buf);
				fflush(fp);
			}
#endif

			NetworkMsgParse(pMsg->Category, pMsg->Protocol, pMsg, 0);
			SAFE_DELETE_ARRAY(pMsg);
		}
	}

#ifdef _GMTOOL_
	if(fp)
		fclose(fp);
#endif
}

//Switch Fly Walk------------------------------------------------------------
void CGameIn::switchFlyWalk( int flyMode )
{
		MSG_DWORD message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_ITEM;
		message.Protocol = MP_ITEM_FLYWALK_ACK;
		message.dwObjectID = gHeroID;
		message.dwData = flyMode;
		heroFlyMode = flyMode;
		if (flyMode == 0)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 5113 ) );
		}
		else if (flyMode == 1)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 5112 ) );
		}
		else if (flyMode == 2)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 5111 ) );
		}
		NETWORK->Send(&message, sizeof(message));
}

//Alemuri Party War----------------------------------------------------------------------------------------------------
void CGameIn::renderPartyWar()
{

	if (m_battleState < 3 && m_battleState > -1)
	{
		DWORD ElapsedTime = gCurTime - partyWarTime;

		switch(m_battleState)
		{
		case eBATTLE_STATE_READY:
			{
				//È­¸éÁß°£¿¡
				DWORD RemainTime = BATTLE_SHOWDOWN_READYTIME > ElapsedTime ? BATTLE_SHOWDOWN_READYTIME - ElapsedTime : 0;
				//sprintf( temp, "Battle Start in %d sec", RemainTime / 1000 );
				m_ImageNumber.SetNumber( RemainTime / 1000 );
				if( m_ImageNumber.IsNumberChanged() )
					m_ImageNumber.SetFadeOut( 500 );

				m_ImageNumber.Render();
			
//				if( m_pCurShowImage )
//					m_pCurShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0xffffffff );
			}
			break;

		case eBATTLE_STATE_FIGHT:
			{
				if( m_pCurShowImage )
					m_pCurShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0xffffffff );

				//È­¸éÁß°£¿¡ ³ª¿Ô´Ù°¡ À§·Î?
				DWORD RemainTime = BATTLE_SHOWDOWN_FIGHTTIME_PARTY > ElapsedTime ? BATTLE_SHOWDOWN_FIGHTTIME_PARTY - ElapsedTime : 0;

				//sprintf( temp, "Battle End in %d sec", RemainTime / 1000 );
				m_ImageNumber.SetNumber( RemainTime / 1000 );
				m_ImageNumber.Render();			

				//Zera Obtener la resolución actual
                const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
                int screenWidth = dispInfo.dwWidth;
                int screenHeight = dispInfo.dwHeight;

               //Zera Escala adaptativa moderada en función de la resolución base 1600x900
              //Zera Solo reduce la mitad de lo que normalmente bajaría
                float baseScale = 1.2f; // Escala base en 1600x900
                float scaleFactor = (screenWidth / 1600.0f);
                float scale = baseScale - ((baseScale - 1.0f) * (1 - scaleFactor) * 0.5f); 

             //Zera Posiciones dinámicas basadas en la resolución
                int team1PosX = (screenWidth * 0.25f); // 25% del ancho de la pantalla
                int team2PosX = (screenWidth * 0.67f); // 75% del ancho de la pantalla

            //Zera Ajuste en el eje Y con un desplazamiento adicional
               int basePosY = (screenHeight * 0.1f); // 10% de la altura de la pantalla
               int yOffset = 50; // Ajuste adicional hacia abajo en píxeles (puedes modificar este valor)
               int posY = basePosY + yOffset; // Aplicar el desplazamiento

           // TEAM 1 SCOREBOARD
              m_ImageNumberTeam1.SetNumber(nTeam1Score);
              m_ImageNumberTeam1.Render();
              m_ImageNumberTeam1.Init(32, 0);
              m_ImageNumberTeam1.SetFillZero(FALSE);
              m_ImageNumberTeam1.SetLimitCipher(3);
              m_ImageNumberTeam1.SetScale(scale, scale);
              m_ImageNumberTeam1.SetPosition(team1PosX, posY);

           // TEAM 2 SCOREBOARD
              m_ImageNumberTeam2.SetNumber(nTeam2Score);
              m_ImageNumberTeam2.Render();
              m_ImageNumberTeam2.Init(32, 0);
              m_ImageNumberTeam2.SetFillZero(FALSE);
              m_ImageNumberTeam2.SetLimitCipher(3);
              m_ImageNumberTeam2.SetScale(scale, scale);
              m_ImageNumberTeam2.SetPosition(team2PosX, posY);
			}
			break;

		case eBATTLE_STATE_RESULT:
			{
				DWORD dwAlpha = 255;
				if( !m_bFadeOut )
				{
					if( ElapsedTime > 5500 )
					{
						m_dwFadeOutStartTime = gCurTime;
						m_bFadeOut = TRUE;
						partyWarResetAll();
					}
				}
				else
				{
					DWORD dwElapsed = gCurTime - m_dwFadeOutStartTime;
					DWORD dwMinusAlpha = 255 * dwElapsed / 1000;
					if( dwMinusAlpha < 255 )
					{
						dwAlpha = 255 - dwMinusAlpha;
					}
					else
					{
						dwAlpha = 0;
					}
				}

				if(m_WinnerTeam)
				{				
					if( m_WinnerShowImage )
						m_WinnerShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0x00ffffff | (dwAlpha << 24) );
				}
				else
				{
					if(ElapsedTime > 2500)
					{
						if( m_WinnerShowImage )
							m_WinnerShowImage->RenderSprite( &m_vTitleScale, NULL, 0.0f, &m_vTitlePos, 0x00ffffff | (dwAlpha << 24) );
					}
				}
			}
			break;
		}
	}
	//CFONT_OBJ->RenderFont(0,temp,strlen(temp),&rect,0xffffffff);
}

void CGameIn::PartyWarScoreBoardUpdater1(int nTeam1)
{

	nTeam1Score = nTeam1;
}

void CGameIn::PartyWarScoreBoardUpdater2(int nTeam2)
{
	nTeam2Score = nTeam2;
}

void CGameIn::setCurrentRenderState(int battlestate)
{
	if (battlestate < 2) //make sure that the image winner loser or draw is not selected during other battle states
		m_WinnerTeam = 0;

	m_battleState = battlestate;
}

void CGameIn::partyWarImagesInit()
{
	m_ImageNumber.Init( 32, 0 );
	m_ImageNumber.SetFillZero( FALSE );
	m_ImageNumber.SetLimitCipher( 3 );
	//m_ImageNumber.SetPosition( 640 - 16, 80 );
	m_ImageNumber.SetScale( 3.0f, 3.0f );
	const DISPLAY_INFO& dispInfo2 = GAMERESRCMNGR->GetResolution();
	m_ImageNumber.SetPosition( (dispInfo2.dwWidth-16)/2, 150 );

	m_dwFadeOutStartTime = 0;
	m_bFadeOut	 = FALSE;

	SCRIPTMGR->GetImage( 59, &m_ImageStart, PFT_HARDPATH );
	SCRIPTMGR->GetImage( 60, &m_ImageWin, PFT_HARDPATH );
	SCRIPTMGR->GetImage( 61, &m_ImageLose, PFT_HARDPATH );
	SCRIPTMGR->GetImage( 62, &m_ImageDraw, PFT_HARDPATH );
	
	const DISPLAY_INFO &dispInfo = GAMERESRCMNGR->GetResolution();
	

	m_pCurShowImage = NULL;
	m_WinnerShowImage = NULL;

	m_vTitleScale.x = 0.0f;
	m_vTitleScale.y = 0.0f;

	m_vTitlePos.x = 0.0;
	m_vTitlePos.y = 0.0;

	partyWarTime = gCurTime;
}
void CGameIn::partyWarStateFight()
{
	m_pCurShowImage = &m_ImageStart;
	m_ImageNumber.SetFillZero( TRUE );
	m_ImageNumber.SetFadeOut( 0 );	//fadeout off
	m_ImageNumber.SetScale( 1.0f, 1.0f );
	const DISPLAY_INFO& dispInfo2 = GAMERESRCMNGR->GetResolution();
	m_ImageNumber.SetPosition( (dispInfo2.dwWidth-32)/2, 150 );

	partyWarTime = gCurTime;
}

void CGameIn::partyWarStateWinner()
{
	if (m_WinnerTeam == 1)
		m_WinnerShowImage = &m_ImageWin;
	else if (m_WinnerTeam == 2)
		m_WinnerShowImage = &m_ImageLose;
	else if (m_WinnerTeam == 3)
		m_WinnerShowImage = &m_ImageDraw;
	else if (m_WinnerTeam == 0)
		m_WinnerShowImage = NULL;

	m_vTitleScale.x = 1.0f;
	m_vTitleScale.y = 1.0f;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	m_vTitlePos.x = (float)(dispInfo.dwWidth - 512)/2;
	m_vTitlePos.y = 100;

	partyWarTime = gCurTime;
}

void CGameIn::partyWarResetAll()
{
	m_WinnerShowImage = NULL;
	m_pCurShowImage = NULL;
	m_battleState = 3;

	m_vTitleScale.x = 0.0f;
	m_vTitleScale.y = 0.0f;

	m_vTitlePos.x = 0.0f;
	m_vTitlePos.y = 0.0f;

	m_dwFadeOutStartTime = 0;
	m_bFadeOut	 = FALSE;

	nTeam1Score = 0;
	nTeam2Score = 0;

	if (HERO)
	{
		HERO->SetPartyWar(FALSE);

		HERO->SetAlly1("");
		HERO->SetAlly2("");
		HERO->SetAlly3("");
		HERO->SetAlly4("");
		HERO->SetAlly5("");
		HERO->SetAlly6("");
		HERO->SetAlly7("");

		HERO->SetEnemy1("");
		HERO->SetEnemy2("");
		HERO->SetEnemy3("");
		HERO->SetEnemy4("");
		HERO->SetEnemy5("");
		HERO->SetEnemy6("");
		HERO->SetEnemy7("");
	}

	OBJECTMGR->ApplyOverInfoOptionToAll();
	partyWarTime = gCurTime;

}
//-------------------------------------------------------------------------------------------------------------------------------------

//Alemuri Search Drops from Mobs---------------------------------------------------------------------------------------------------------------
void CGameIn::SearchDropFromMobs(char* itemToSearch)
{

	ITEM_INFO* info = ITEMMGR->FindItemInfoForName(itemToSearch);
	if (!info)
	{
		return;
	}
	MonsterArrayForDrop.clear();
	MonsterArrayForDropColors.clear();
	
	int count = 0;
	for (int i = 0; i < 2000; i++)
	{
		BASE_MONSTER_LIST* pMonInfo = GAMERESRCMNGR->GetMonsterListInfo(i);

		if (pMonInfo)
		{
			for (int ii = 0; ii < 15; ii++)
			{
				if (count == 45)
				{
					break;
				}
				if (pMonInfo->dropItem[ii].wItemIdx != 0)
				{
					ITEM_INFO* tmpItem = ITEMMGR->GetItemInfo(pMonInfo->dropItem[ii].wItemIdx);
					if (tmpItem) 
					{
						std::string itemName(ITEMMGR->GetItemInfo(pMonInfo->dropItem[ii].wItemIdx)->ItemName);
						if (itemName != "")
						{
							if (itemName == itemToSearch)
							{
								std::string mapName = "";
								for (int nMap = 1; nMap < 150; nMap++)
								{
									int size = preloadMonsterInMap[nMap].size();
									for (int iii = 0; iii < preloadMonsterInMap[nMap].size(); iii++)
									{
										int monKind = preloadMonsterInMap[nMap].at(iii);
										if (pMonInfo->MonsterKind == preloadMonsterInMap[nMap].at(iii))
											mapName = GetMapName(nMap);
									}
								}



								//Alemuri get Monster Location---------------------------------------------------------
								//std::string mapName = "";
								//for (int nMap = 1; nMap < 150; nMap++)
								//{
								//	sPRELOAD_INFO* pPreLoadInfo = new sPRELOAD_INFO;
								//	pPreLoadInfo->MapNum = nMap;

								//	GAMERESRCMNGR->LoadPreMonsterData(
								//	pPreLoadInfo,
								//	MAPTYPE(pPreLoadInfo->MapNum));

								//	if (pPreLoadInfo)
								//	{
								//		BASE_MONSTER_LIST* pBMonInfo = NULL;
								//		DIRECTORYMGR->SetLoadMode(eLM_Monster);

								//		for(int i=0; i<pPreLoadInfo->Count[ePreLoad_Monster]; i++)
								//		{
								//			pBMonInfo = GAMERESRCMNGR->GetMonsterListInfo(pPreLoadInfo->Kind[ePreLoad_Monster][i]);
								//			//if(pBMonInfo)
								//			//	CEngineObject::PreLoadObject(pBMonInfo->ChxName);
								//			if (pMonInfo->MonsterKind == pBMonInfo->MonsterKind)
								//			{
								//				mapName = GetMapName(nMap);
								//				int aa = 0;
								//			}
								//		}
								//	}
								//}
								//--------------------------------------------------------------------------------------

								std::string str1(pMonInfo->Name);
								str1.append("§§TRUE");
								//DWORD color = ITEMMGR->GetItemNameColor(*info);
								MonsterArrayForDrop.push_back(str1);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 255));

								std::string str9 = "  LV";
								str9.append("§§FALSE");
								MonsterArrayForDrop.push_back(str9);
								MonsterArrayForDropColors.push_back(RGB_HALF(0, 255, 0));

								char mobLevel[10];
								sprintf(mobLevel, "%d", pMonInfo->Level);
								std::string str8(mobLevel);
								str8.append("§§FALSE");
								MonsterArrayForDrop.push_back(str8);
								MonsterArrayForDropColors.push_back(RGB_HALF(0, 255, 0));

								if (mapName == "")
								{
									mapName = "Dungeon, Raid Boss or Event";
								}
								
								std::string str10 = "    - ";
								str10.append("§§FALSE");
								MonsterArrayForDrop.push_back(str10);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 0));

								std::string str11 = mapName;
								str11.append("§§FALSE");
								MonsterArrayForDrop.push_back(str11);
								MonsterArrayForDropColors.push_back(RGB_HALF(0, 255, 255));

								std::string str12 = " - ";
								str12.append("§§FALSE");
								MonsterArrayForDrop.push_back(str12);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 0));


								std::string str2 = "   [Qty ";
								str2.append("§§FALSE");
								MonsterArrayForDrop.push_back(str2);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 0));

								char tempDword[9];
								sprintf(tempDword, "%d", pMonInfo->dropItem[ii].byCount);
								std::string str3(tempDword);
								str3.append("§§FALSE");
								MonsterArrayForDrop.push_back(str3);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 0));

								std::string str4 = "]";
								str4.append("§§FALSE");
								MonsterArrayForDrop.push_back(str4);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 0));

								std::string str5 = " [Chance: ";
								str5.append("§§FALSE");
								MonsterArrayForDrop.push_back(str5);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 204));

								DWORD serverDropRates = 0;
								serverDropRates = HERO->GetServerDropRatesInfo();
								const float fIncreaseDropRate = (HERO->GetRateBuffStatus()->IncreaseDropRate + HERO->GetRatePassiveStatus()->IncreaseDropRate);
								serverDropRates += fIncreaseDropRate * serverDropRates / 100;
								int multiplier = HERO->GetServerDropRatesInfoMultiplier();
								serverDropRates *= multiplier;
								float itemPercentage = (float)pMonInfo->dropItem[ii].dwDropPercent;
								itemPercentage = itemPercentage / 1000000 * serverDropRates; //alemuri
								if (itemPercentage > 100)
								{
									itemPercentage = 100;
								}
								char itemPercentagestr[10];
								sprintf(itemPercentagestr, "%.2f", itemPercentage);

								std::string str6(itemPercentagestr);
								str6.append("§§FALSE");
								MonsterArrayForDrop.push_back(str6);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 204));

								std::string str7 = "%] ";
								str7.append("§§FALSE");
								MonsterArrayForDrop.push_back(str7);
								MonsterArrayForDropColors.push_back(RGB_HALF(255, 255, 204));

								count +=1;
							}
						}
					}
				}
			}
		}
	}

	if (count == 0)
		CHATMGR->AddMsg(CTC_SYSMSG, "No monster drops this Item");
	else if (count > 0)
		CHATMGR->AddMsgItemChat(MonsterArrayForDrop, MonsterArrayForDropColors, 0, CTC_GENERALCHAT, "These monsters drop the requested Item");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


//Alemuri Statistic for Monster Name---------------------------------------------------------------------------------------------------------------
void CGameIn::MobStats(char* MobToSearch)
{
	MobStatsArray.clear();
	MobStatsColorArray.clear();

	std::string heroName(HERO->GetObjectName());
	int monsterKind = 0;
	monsterKind = GAMERESRCMNGR->GetMonsterIndexForName(MobToSearch);
	BASE_MONSTER_LIST* pMonInfo = GAMERESRCMNGR->GetMonsterListInfo(monsterKind);
	if (pMonInfo)
	{
		DWORD AddDamageMinP = 0;
		DWORD AddDamageMaxP = 0;
		DWORD AddDamageMinM = 0;
		DWORD AddDamageMaxM = 0;
		int Unit = 0;

		int objectKind = pMonInfo->ObjectKind;
		//Skill List----------------------------------------------
		cActiveSkillInfo** arraySkillList = pMonInfo->SkillInfo;
		int PhysicalSkillsCount = 0;
		int MagicSkillsCount = 0;
		BOOL skillsDetected = FALSE;
		for (int skills = 0; skills < 5; skills++)
		{
			cActiveSkillInfo* skill = arraySkillList[skills];
			if (skill)
			{
				skillsDetected = TRUE;
				Unit = skill->GetSkillInfo()->Unit;
				if (Unit == 1) //Physical Type Skill
				{
					if (skill->GetSkillInfo()->UnitData > AddDamageMaxP)
						AddDamageMaxP = skill->GetSkillInfo()->UnitData;

					if (AddDamageMinP > 0)
					{
						if(skill->GetSkillInfo()->UnitData < AddDamageMinP)
							AddDamageMinP = skill->GetSkillInfo()->UnitData;
					}
					else
					{
						AddDamageMinP = skill->GetSkillInfo()->UnitData;
					}
					PhysicalSkillsCount += 1;
				}
				else if (Unit == 2) //Magical Type Skill
				{
					if (skill->GetSkillInfo()->UnitData > AddDamageMaxM)
						AddDamageMaxM = skill->GetSkillInfo()->UnitData;

					if (AddDamageMinM > 0)
					{
						if (skill->GetSkillInfo()->UnitData < AddDamageMinM)
							AddDamageMinM = skill->GetSkillInfo()->UnitData;
					}
					else
					{
						AddDamageMinM = skill->GetSkillInfo()->UnitData;
					}
					MagicSkillsCount += 1;
				}
			}
		}
		if (AddDamageMinP == 0)
			AddDamageMinP = 350;

		if (AddDamageMaxP == 0)
			AddDamageMaxP = 550;

		if (AddDamageMinM == 0)
			AddDamageMinM = 350;

		if (AddDamageMaxM == 0)
			AddDamageMaxM = 500;
		//--------------------------------------------------------
		//level---------------------------------
		char buff[4];
		sprintf(buff, "%d", pMonInfo->Level);
		std::string level(buff);
		//--------------------------------------
		//Name----------------------------------
		std::string monsterName(pMonInfo->Name);
		//--------------------------------------
		//Aggressive----------------------------
		std::string aggressive = "";
		if (pMonInfo->bForeAttack)
			aggressive = "Yes";
		else if (!pMonInfo->bForeAttack)
			aggressive = "No";
		//--------------------------------------
		//Attack-----------------------------------------------------------------------------------
		int attackPhysicPower = max(pMonInfo->AttackPhysicalMax, pMonInfo->AttackPhysicalMin);
		int attackMagicPower = max(pMonInfo->AttackMagicalMax, pMonInfo->AttackMagicalMin);
		int attack = max(attackPhysicPower, attackMagicPower);
		//-----------------------------------------------------------------------------------------
		//Pattack and Mattack of the Monster-------------------------------------------------------
		DWORD mobPattack = pMonInfo->AttackPhysicalMax;
		DWORD mobMattack = pMonInfo->AttackMagicalMax;
		char buffPattack[10];
		sprintf(buffPattack, "%d", mobPattack);
		std::string mobPattackStr(buffPattack);

		char buffMattack[10];
		sprintf(buffMattack, "%d", mobMattack);
		std::string mobMattackStr(buffMattack);
		//Accuracy---------------------------------------------------------------------------------
		//DWORD accuracyToPrint = pMonInfo->Accuracy + 85;
		DWORD accuracyToPrint = pMonInfo->Accuracy; //This line is Only For Aurora Calculation
		float accuracy = pMonInfo->Accuracy;
		//accuracy += 85.f; //85 get added in attack manager (in Aura Luna this is deactivated)
		char buffAccuracy[5];
		sprintf(buffAccuracy, "%d", accuracyToPrint);
		std::string accuracyStr(buffAccuracy);
		//-----------------------------------------------------------------------------------------
		//Evasion----------------------------------------------------------------------------------
		int evasion = pMonInfo->Avoid;
		char buffEvasion[5];
		sprintf(buffEvasion, "%d", pMonInfo->Avoid);
		std::string evasionStr(buffEvasion);

		//Get Evasion------------------------------------------------------------------------------
		float heroEva = HERO->DoGetEvasionRateFloat();
		float canFullDodgeCalc = (heroEva - accuracy) * 13;
		BOOL canFullDodge = FALSE;
		if (canFullDodgeCalc < 0)
			canFullDodge = TRUE;

		float ACC = 750 - canFullDodgeCalc;
		int randomValue = rand() % 1000;
		//int percentageEvaChances = 0 - (100 - (ACC / 10));
		float percentageEvaChances = 1000 - ACC; //- (ACC / 10);
		percentageEvaChances /= 10;
		//float percentageEvaChances = 100.0f - canFullDodgeCalc;
		//if (percentageEvaChances > 100)
		//	percentageEvaChances = 100.0f;

		if (percentageEvaChances < 0)
			percentageEvaChances = 0.0f;

		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << percentageEvaChances;
		std::string strEvaPercentage = stream.str();

		DWORD EvaPercentageColor;
		if (percentageEvaChances >= 100.0f)
			EvaPercentageColor = RGB_HALF(0, 255, 0);

		if (percentageEvaChances >= 80.0f && percentageEvaChances < 100.0f)
			EvaPercentageColor = RGB_HALF(255, 255, 0);

		if (percentageEvaChances >= 50.0f && percentageEvaChances < 80.0f)
			EvaPercentageColor = RGB_HALF(255, 128, 0);

		if (percentageEvaChances < 50.0f)
			EvaPercentageColor = RGB_HALF(255, 0, 0);

		//-----------------------------------------------------------------------------------------

		//Get Damage Against---------------------------------------------------------------------------------------------------------------
		float AttackMax = pMonInfo->AttackPhysicalMax;
		float AttackMin = pMonInfo->AttackPhysicalMin;
		float AttackMagicMax = pMonInfo->AttackMagicalMax;
		float AttackMagicMin = pMonInfo->AttackMagicalMin;
		float CriticalDamageRate = 0.0f;	
		float CriticalDamagePlus = 0.0f;
		float Defense = (float)HERO->DoGetDefenseRateMobStats();
		float MagicDefense = (float)HERO->DoGetMagDefenseRateMobStats();
		float Weapon = 1;
		float RateAddValue = 0.0f;
		float PlusAddValue = 0.0f;
		float AddDamageMin = AddDamageMinP;
		float AddDamageMax = AddDamageMaxP;
		float damageMinP = 1.f;
		float damageMaxP = 1.f;
		float damageMinM = 1.f;
		float damageMaxM = 1.f;
		float damageMinPCrit = 1.f;
		float damageMaxPCrit = 1.f;
		float damageMinMCrit = 1.f;
		float damageMaxMCrit = 1.f;
		//Min Physical-----------------------------------------------------
		float Shield = HERO->DoGetShieldDefense();
		if( AttackMin > ( Defense + Shield ) )
		{
			damageMinP = AttackMin - ( Defense + Shield );
		}
		const float fAttackerLevel = pMonInfo->Level;
		const float fTargetLevel =  HERO->GetLevel();
		if( fAttackerLevel < fTargetLevel )
		{
			damageMinP = ( damageMinP < 1 ? 1 : damageMinP );
			damageMinP = damageMinP * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f );
		}
		damageMinPCrit = damageMinP;
		damageMinPCrit = ( damageMinPCrit * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;
		//----------------------------------------------------------------------
		//Max Physical-----------------------------------------------------
		if( AttackMin > ( Defense + Shield ) )
		{
			damageMaxP = AttackMax - ( Defense + Shield );
		}
		if( fAttackerLevel < fTargetLevel )
		{
			damageMaxP = ( damageMaxP < 1 ? 1 : damageMaxP );
			damageMaxP = damageMaxP * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f );
		}
		damageMaxPCrit = damageMaxP;
		damageMaxPCrit = ( damageMaxPCrit * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;
		//---------------------------------------------------------------------
		//This is Only For Aura Calculation------------------------------------

		//---------------------------------------------------------------------

		if (damageMinP < 1)
			damageMinP = 1;

		if (damageMaxP < 1)
			damageMaxP = 1;

		if (damageMinPCrit < 1)
			damageMinPCrit = 1;

		if (damageMaxPCrit < 1)
			damageMaxPCrit = 1;

		char buffDamageMin[11];
		sprintf(buffDamageMin, "%d", (DWORD)damageMinP);
		std::string damageMinPStr(buffDamageMin);

		char buffDamageMax[11];
		sprintf(buffDamageMax, "%d", (DWORD)damageMaxP);
		std::string damageMaxPStr(buffDamageMax);

		char buffDamageMinCrit[11];
		sprintf(buffDamageMinCrit, "%d", (DWORD)damageMinPCrit);
		std::string damageMinPCritStr(buffDamageMin);

		char buffDamageMaxCrit[11];
		sprintf(buffDamageMaxCrit, "%d", (DWORD)damageMaxPCrit);
		std::string damageMaxPCritStr(buffDamageMaxCrit);

		DWORD expectedDamageColor = 0;
		DWORD expectedDamageColorCrit = 0;
		if (damageMaxP < 200)
			expectedDamageColor = RGB_HALF(0, 255, 0);

		if (damageMaxP > 200 && damageMaxP < 500)
			expectedDamageColor = RGB_HALF(255, 255, 0);

		if (damageMaxP > 500 && damageMaxP < 1000)
			expectedDamageColor = RGB_HALF(255, 128, 0);

		if (damageMaxP > 1000)
			expectedDamageColor = RGB_HALF(255, 0, 0);

		if (damageMaxP > 10000)
			expectedDamageColor = RGB_HALF(255, 0, 255);

		if (damageMaxP < 200)
			expectedDamageColorCrit = RGB_HALF(0, 255, 0);

		if (damageMaxP > 200 && damageMaxP < 500)
			expectedDamageColorCrit = RGB_HALF(255, 255, 0);

		if (damageMaxP > 500 && damageMaxP < 1000)
			expectedDamageColorCrit = RGB_HALF(255, 128, 0);

		if (damageMaxP > 1000)
			expectedDamageColorCrit = RGB_HALF(255, 0, 0);

		if (damageMaxP > 10000)
			expectedDamageColorCrit = RGB_HALF(255, 0, 255);


		Weapon = pMonInfo->Level * 2;
		AddDamageMin = AddDamageMinM;
		AddDamageMax = AddDamageMaxM;
		//Min Magical-----------------------------------------------------
		int Int = 1; // HERO->GetIntelligence();
		int wisdom 	= 0; // HERO->GetWisdom();
		float FailValue = 1;
		if( fTargetLevel > fAttackerLevel )
		{
			FailValue = (float)(fTargetLevel - fAttackerLevel);
		}
		float castingProtectValue	= HERO->GetBuffStatus()->CastingProtect;
		float castingProtectRate	= HERO->GetRateBuffStatus()->CastingProtect + 1.0f;
		castingProtectValue	+= 	HERO->GetPassiveStatus()->CastingProtect;
		castingProtectRate	+= 	HERO->GetRatePassiveStatus()->CastingProtect;
		float CriticalValue = 1;
		CriticalValue = (float)(fAttackerLevel - fTargetLevel);
		float CriticalRate = HERO->GetMagicCriticalRate();

		float dem = ( AttackMagicMin * 1.75f ) - ( MagicDefense * 4 );
		float x = ( float )( 100 + fAttackerLevel - fTargetLevel );
		float y = 100 + ( ( dem / MagicDefense ) * 25 );
		float z = 1 - ( ( fTargetLevel - fAttackerLevel ) * 0.01 );

		damageMinM = ( AddDamageMin * x * y * z ) / 1000;
		
		//if( random < successRate )
		damageMinMCrit = damageMinM;
		//damageMinM *= ( 2 + ( Int * 0.001f ) ); //Crit
		damageMinM /= ( 2 - ( wisdom * 0.002f ) );
		
		//const float fAttackerLevel = pMonInfo->Level;
		//const float fTargetLevel =  HERO->GetLevel();
		
		//if( fAttackerLevel < fTargetLevel )
		//{
		//	damageMinM = ( damageMinM < 1 ? 1 : damageMinM );
		//	damageMinM = damageMinM * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f );
		//}
		//damageMinMCrit = damageMinM;
		//damageMinMCrit = ( damageMinMCrit * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;
		//--------------------------------------------------------
		//Max Magical-----------------------------------------------------
		dem = ( AttackMagicMax * 1.75f ) - ( MagicDefense * 4 );
		x = ( float )( 100 + fAttackerLevel - fTargetLevel );
		y = 100 + ( ( dem / MagicDefense ) * 25 );
		z = 1 - ( ( fTargetLevel - fAttackerLevel ) * 0.01 );

		damageMaxM = ( AddDamageMax * x * y * z ) / 1000;
		
		//if( random < successRate )
		damageMaxMCrit = damageMaxM;
		damageMaxMCrit *= ( 2 + ( Int * 0.001f ) );
		//damageMaxM /= ( 2 - ( wisdom * 0.002f ) );

		//damageMaxM = ( AttackMagicMax - MagicDefense ) * ( ( 1000.f + AddDamageMax + Weapon ) / 1000.f );
		//damageMaxM = ( damageMaxM * ( 1 +  ( RateAddValue / 100 ) ) ) + PlusAddValue;
		//if( fAttackerLevel < fTargetLevel )
		//{
		//	damageMaxM = ( damageMaxM < 1 ? 1 : damageMaxM );
		//	damageMaxM = damageMaxM * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f );
		//}
		//damageMaxMCrit = damageMaxM;
		//damageMaxMCrit = ( damageMaxMCrit * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;
		//-------------------------------------------------------
		if (damageMinM < 1)
			damageMinM = 1;

		if (damageMaxM < 1)
			damageMaxM = 1;

		if (damageMinMCrit < 1)
			damageMinMCrit = 1;

		if (damageMaxMCrit < 1)
			damageMaxMCrit = 1;

		char buffDamageMinM[11];
		sprintf(buffDamageMinM, "%d", (DWORD)damageMinM);
		std::string damageMinMStr(buffDamageMinM);

		char buffDamageMaxM[11];
		sprintf(buffDamageMaxM, "%d", (DWORD)damageMaxM);
		std::string damageMaxMStr(buffDamageMaxM);

		char buffDamageMinMagicCrit[11];
		sprintf(buffDamageMinMagicCrit, "%d", (DWORD)damageMinMCrit);
		std::string damageMinMCritStr(buffDamageMinMagicCrit);

		char buffDamageMaxMagicCrit[11];
		sprintf(buffDamageMaxMagicCrit, "%d", (DWORD)damageMaxMCrit);
		std::string damageMaxMCritStr(buffDamageMaxMagicCrit);

		DWORD expectedDamageColorM = 0;
		DWORD expectedDamageColorCritM = 0;
		if (damageMaxM < 200)
			expectedDamageColorM = RGB_HALF(0, 255, 0);

		if (damageMaxM > 200 && damageMaxP < 500)
			expectedDamageColorM = RGB_HALF(255, 255, 0);

		if (damageMaxM > 500 && damageMaxP < 1000)
			expectedDamageColorM = RGB_HALF(255, 128, 0);

		if (damageMaxM > 1000)
			expectedDamageColorM = RGB_HALF(255, 0, 0);

		if (damageMaxM > 10000)
			expectedDamageColorM = RGB_HALF(255, 0, 255);

		if (damageMaxM < 200)
			expectedDamageColorCritM = RGB_HALF(0, 255, 0);

		if (damageMaxM > 200 && damageMaxP < 500)
			expectedDamageColorCritM = RGB_HALF(255, 255, 0);

		if (damageMaxM > 500 && damageMaxP < 1000)
			expectedDamageColorCritM = RGB_HALF(255, 128, 0);

		if (damageMaxM > 1000)
			expectedDamageColorCritM = RGB_HALF(255, 0, 0);

		if (damageMaxM > 10000)
			expectedDamageColorCritM = RGB_HALF(255, 0, 255);
		//---------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------------------------------

		//Danger Level-----------------------------------------------------------------------------
		std::string dangerLevel = "";
		if (attack >= 10000)
			dangerLevel = "Insane";
		
		if (attack >= 7000 && attack < 10000)
			dangerLevel = "Very High";

		if (attack >= 5000 && attack < 7000)
			dangerLevel = "High";

		if (attack >= 2000 && attack < 5000)
			dangerLevel = "Medium";

		if (attack >= 1000 && attack < 2000)
			dangerLevel = "Low";

		if (attack < 1000)
			dangerLevel = "Very Low";

		//Danger Color-----------------------------------------------------------------------------
		DWORD dangerColor = 0;
		if (dangerLevel == "Insane")
			dangerColor = RGB_HALF(255, 0, 255);

		if (dangerLevel == "Very High")
			dangerColor = RGB_HALF(255, 0, 0);

		if (dangerLevel == "High")
			dangerColor = RGB_HALF(255, 128, 0);

		if (dangerLevel == "Medium")
			dangerColor = RGB_HALF(255, 255, 0);

		if (dangerLevel == "Low")
			dangerColor = RGB_HALF(0, 255, 0);

		if (dangerLevel == "Very Low")
			dangerColor = RGB_HALF(255, 255, 255);

		//-----------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------

		//Monster Color----------------------------------------------------------------------------
		DWORD MonsterColor	= 0; //GetBattleGuageDlg()->SetTargetLevelSignal( pMonInfo->Level );

		if (HERO->GetLevel() >= pMonInfo->Level || (pMonInfo->Level - HERO->GetLevel()) <= 5)
			MonsterColor = RGB_HALF(0, 255 ,0);
	
		if (pMonInfo->Level > HERO->GetLevel() && (pMonInfo->Level - HERO->GetLevel()) > 5)
			MonsterColor = RGB_HALF(255, 255 ,0);

		if (pMonInfo->Level > HERO->GetLevel() && (pMonInfo->Level - HERO->GetLevel()) > 10)
			MonsterColor = RGB_HALF(255, 128 ,0);

		if (pMonInfo->Level > HERO->GetLevel() && (pMonInfo->Level - HERO->GetLevel()) > 15)
			MonsterColor = RGB_HALF(255, 0 ,0);

		//-----------------------------------------------------------------------------------------

		//Aggressive Color-------------------------------------------------------------------------
		DWORD AggressiveColor = 0;
		if (aggressive == "Yes")
			AggressiveColor = RGB_HALF(255, 0, 0);

		if (aggressive == "No")
			AggressiveColor = RGB_HALF(0, 255, 0);

		//-----------------------------------------------------------------------------------------
		DWORD titlesColors = RGB_HALF(255, 255, 204);

		std::string str1 = "LV " + level + " - ";
		str1.append("§§TRUE");
		MobStatsArray.push_back(str1);
		MobStatsColorArray.push_back(MonsterColor);

		std::string str2 = monsterName + "                                 ";
		str2.append("§§FALSE");
		MobStatsArray.push_back(str2);
		MobStatsColorArray.push_back(MonsterColor);

		std::string str5 = " ";
		str5.append("§§TRUE");
		MobStatsArray.push_back(str5);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str3 = "Aggressive: ";
		str3.append("§§TRUE");
		MobStatsArray.push_back(str3);
		MobStatsColorArray.push_back(titlesColors);

		std::string str4 = aggressive;
		str4.append("§§FALSE");
		MobStatsArray.push_back(str4);
		MobStatsColorArray.push_back(AggressiveColor);

		std::string str6 = "Danger Level: ";
		str6.append("§§TRUE");
		MobStatsArray.push_back(str6);
		MobStatsColorArray.push_back(titlesColors);

		std::string str7 = dangerLevel;
		str7.append("§§FALSE");
		MobStatsArray.push_back(str7);
		MobStatsColorArray.push_back(dangerColor);

		std::string str8 = " ";
		str8.append("§§TRUE");
		MobStatsArray.push_back(str8);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str9 = "Accuracy: ";
		str9.append("§§TRUE");
		MobStatsArray.push_back(str9);
		MobStatsColorArray.push_back(titlesColors);

		std::string str10 = accuracyStr;
		str10.append("§§FALSE");
		MobStatsArray.push_back(str10);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str11 = "Evasion: ";
		str11.append("§§TRUE");
		MobStatsArray.push_back(str11);
		MobStatsColorArray.push_back(titlesColors);

		std::string str12 = evasionStr;
		str12.append("§§FALSE");
		MobStatsArray.push_back(str12);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str31 = "Pattack: ";
		str31.append("§§TRUE");
		MobStatsArray.push_back(str31);
		MobStatsColorArray.push_back(titlesColors);

		std::string str32 = mobPattackStr;
		str32.append("§§FALSE");
		MobStatsArray.push_back(str32);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str33 = "Mattack: ";
		str33.append("§§TRUE");
		MobStatsArray.push_back(str33);
		MobStatsColorArray.push_back(titlesColors);

		std::string str34 = mobMattackStr;
		str34.append("§§FALSE");
		MobStatsArray.push_back(str34);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str13 = " ";
		str13.append("§§TRUE");
		MobStatsArray.push_back(str13);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str14 = heroName + " Evasion Chances: ";
		str14.append("§§TRUE");
		MobStatsArray.push_back(str14);
		MobStatsColorArray.push_back(titlesColors);

		std::string str15 = strEvaPercentage;
		str15.append("§§FALSE");
		MobStatsArray.push_back(str15);
		MobStatsColorArray.push_back(EvaPercentageColor);

		std::string str16 = "%";
		str16.append("§§FALSE");
		MobStatsArray.push_back(str16);
		MobStatsColorArray.push_back(EvaPercentageColor);

		std::string str17 = " ";
		str17.append("§§TRUE");
		MobStatsArray.push_back(str17);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));

		std::string str19 = "Physical Damage Against " + heroName;
		str19.append("§§TRUE");
		MobStatsArray.push_back(str19);
		MobStatsColorArray.push_back(titlesColors);

		std::string str20 = "Normal: ";
		str20.append("§§TRUE");
		MobStatsArray.push_back(str20);
		MobStatsColorArray.push_back(titlesColors);

		if (damageMinPStr == "1" && damageMaxPStr == "1")
		{
			std::string str21 = "No Damage";
			str21.append("§§FALSE");
			MobStatsArray.push_back(str21);
			MobStatsColorArray.push_back(RGB_HALF(255,204,204));
		}
		else
		{
			std::string str21 = damageMinPStr + " - " + damageMaxPStr;
			str21.append("§§FALSE");
			MobStatsArray.push_back(str21);
			MobStatsColorArray.push_back(expectedDamageColor);
		}

		std::string str22 = "Critical: ";
		str22.append("§§TRUE");
		MobStatsArray.push_back(str22);
		MobStatsColorArray.push_back(titlesColors);

		if ((damageMinPCritStr == "1" && damageMaxPCritStr == "1") || (skillsDetected == TRUE && PhysicalSkillsCount == 0))
		{
			std::string str23 = "No Damage";
			str23.append("§§FALSE");
			MobStatsArray.push_back(str23);
			MobStatsColorArray.push_back(RGB_HALF(255,204,204));
		}
		else
		{
			std::string str23 = damageMinPCritStr + " - " + damageMaxPCritStr;
			str23.append("§§FALSE");
			MobStatsArray.push_back(str23);
			MobStatsColorArray.push_back(expectedDamageColorCrit);
		}

		std::string str24 = " ";
		str24.append("§§TRUE");
		MobStatsArray.push_back(str24);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));


		std::string str25 = "Magical Damage Against " + heroName;
		str25.append("§§TRUE");
		MobStatsArray.push_back(str25);
		MobStatsColorArray.push_back(titlesColors);

		std::string str26 = "Normal: ";
		str26.append("§§TRUE");
		MobStatsArray.push_back(str26);
		MobStatsColorArray.push_back(titlesColors);

		if ((damageMinMStr == "1" && damageMaxMStr == "1") || (skillsDetected == TRUE && MagicSkillsCount == 0))
		{
			std::string str23 = "No Damage";
			str23.append("§§FALSE");
			MobStatsArray.push_back(str23);
			MobStatsColorArray.push_back(RGB_HALF(255,204,204));
		}
		else
		{
			std::string str27 = damageMinMStr + " - " + damageMaxMStr;
			str27.append("§§FALSE");
			MobStatsArray.push_back(str27);
			MobStatsColorArray.push_back(expectedDamageColorM);
		}
		//Mob Critical Magic----------------------------------------------------
		//std::string str28 = "Critical: ";
		//str28.append("§§TRUE");
		//MobStatsArray.push_back(str28);
		//MobStatsColorArray.push_back(titlesColors);

		//std::string str29 = damageMinMCritStr + " - " + damageMaxMCritStr;
		//str29.append("§§FALSE");
		//MobStatsArray.push_back(str29);
		//MobStatsColorArray.push_back(expectedDamageColorCritM);
		//----------------------------------------------------------------------

		std::string str30 = " ";
		str30.append("§§TRUE");
		MobStatsArray.push_back(str30);
		MobStatsColorArray.push_back(RGB_HALF(255,255,255));



		std::string str18 = "Map:";
		str18.append("§§TRUE");
		MobStatsArray.push_back(str18);
		MobStatsColorArray.push_back(titlesColors);

		std::string mapName = "";
		for (int nMap = 1; nMap < 150; nMap++)
		{
			int size = preloadMonsterInMap[nMap].size();
			for (int i = 0; i < preloadMonsterInMap[nMap].size(); i++)
			{
				int monKind = preloadMonsterInMap[nMap].at(i);
				if (pMonInfo->MonsterKind == preloadMonsterInMap[nMap].at(i))
				{
					mapName = GetMapName(nMap);
					mapName.append("§§TRUE");
					MobStatsArray.push_back(mapName);
					MobStatsColorArray.push_back(RGB_HALF(0, 255, 255));
				}
			}
		}

		if (mapName == "")
		{
			std::string str = "This monster doesn't spawn in any map,";
			str.append("§§TRUE");
			MobStatsArray.push_back(str);
			MobStatsColorArray.push_back(RGB_HALF(255, 128 ,0));

			std::string str2 = "it might be in a Dungeon, Raid Boss or only Events";
			str2.append("§§TRUE");
			MobStatsArray.push_back(str2);
			MobStatsColorArray.push_back(RGB_HALF(255, 128 ,0));
		}

		CHATMGR->AddMsgItemChat(MobStatsArray, MobStatsColorArray, 0, CTC_GENERALCHAT, "These are the stats for the requested monster");
	}
	else
	{
		CHATMGR->AddMsg(CTC_SYSMSG, "There are no monsters with that name");
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

//Alemuri //whatnpc command----------------------------------------------------------------------------------------------------------------------
void CGameIn::WhatNpc(char* itemToSearch)
{
	ITEMMGR->resetNpcExcludedArray();
	ITEM_INFO* info = ITEMMGR->FindItemInfoForName(itemToSearch);
	if (!info)
	{
		return;
	}
	std::vector<std::string> resultArrayText;
	std::vector<DWORD> resultArrayColors;
	std::string resultStr = "";
	for (int i = 0; i < ITEMMGR->itemsIndexDealer.size(); i++)
	{
		DWORD itemIdxFound = ITEMMGR->GetItemIdxDealer(i);
		if (itemIdxFound == info->ItemIdx)
		{
			DWORD npcIndex = ITEMMGR->GetNpcIdxDealer(i);
			STATIC_NPCINFO* npcInfo = GAMERESRCMNGR->GetStaticNpcInfo(npcIndex);

			std::string npcNameStd = "";
			BOOL excluded = FALSE;
			if (npcInfo)
			{
				std::string npcNameStdTemp(npcInfo->NpcName);
				npcNameStd = npcNameStdTemp;
				for (int ii = 0; ii < ITEMMGR->GetNpcNameExcludedSize(); ii++)
				{
					if	(ITEMMGR->GetNpcNameExcluded(ii) == npcNameStd)
						excluded = TRUE;
				}
			}
			//NPC_LIST* npcInfo = GAMERESRCMNGR->GetNpcInfo(npcIndex); // include PCShop but also NPCs out of game
			if (npcInfo && excluded == FALSE)
			{
			//std::string nameNpcStd = ITEMMGR->GetNpcNameDealer(i); // include PCShop but also NPCs out of game
			//if (nameNpcStd != "") // include PCShop but also NPCs out of game
			//{ // include PCShop but also NPCs out of game
				//char* npcName((char*)nameNpcStd.c_str()); // include PCShop but also NPCs out of game
				char* npcName(npcInfo->NpcName);
				DWORD itemColor = ITEMMGR->GetItemNameColor(*info);
			
				std::string str1(npcName);
				str1.append("§§TRUE");
				resultArrayText.push_back(str1);
				resultArrayColors.push_back(itemColor);
				
				std::string str2(":  ");
				str2.append("§§FALSE");
				resultArrayText.push_back(str2);
				resultArrayColors.push_back(RGB_HALF(255, 255, 0));

				std::string str3(" Price: ");
				str3.append("§§FALSE");
				resultArrayText.push_back(str3);
				resultArrayColors.push_back(RGB_HALF(255, 255, 0)); //yellow

				char buff[15];
				sprintf(buff, "%I64u", info->BuyPrice );
				std::string str4(buff);
				str4.append("§§FALSE");
				resultArrayText.push_back(str4);
				resultArrayColors.push_back(RGB_HALF(255, 255, 255));

				if (info->dwBuyFishPoint > 0)
				{
					std::string str("  Fishing Points: ");
					str.append("§§FALSE");
					resultArrayText.push_back(str);
					resultArrayColors.push_back(RGB_HALF(0, 255, 0));

					char buff[15];
					sprintf(buff, "%d", info->dwBuyFishPoint );
					std::string str1(buff);
					str1.append("§§FALSE");
					resultArrayText.push_back(str1);
					resultArrayColors.push_back(RGB_HALF(255, 255, 255));
				}

				if (info->wPointType > 0)
				{
					std::string str("  Item Required: ");
					str.append("§§FALSE");
					resultArrayText.push_back(str);
					resultArrayColors.push_back(RGB_HALF(0, 255, 0));

					char buff2[15];
					sprintf(buff2, "%d", info->dwPointTypeValue2 );
					std::string str4(buff2);
					str4.append("§§FALSE");
					resultArrayText.push_back(str4);
					resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					std::string str5(" ");
					str5.append("§§FALSE");
					resultArrayText.push_back(str5);
					resultArrayColors.push_back(RGB_HALF(255, 255, 0));

					char buff[15];
					sprintf(buff, "%d", info->dwPointTypeValue1 );
					std::string str1(ITEMMGR->GetItemInfo(info->dwPointTypeValue1)->ItemName);
					str1.append("§§FALSE");
					resultArrayText.push_back(str1);
					resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					std::string str2(" ");
					str2.append("§§FALSE");
					resultArrayText.push_back(str2);
					resultArrayColors.push_back(RGB_HALF(255, 255, 0));

				}

					std::string str5(", ");
					str5.append("§§FALSE");
					resultArrayText.push_back(str5);
					resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					//std::string str13(" [");
					//str13.append("§§FALSE");
					//resultArrayText.push_back(str13);
					//resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					std::string str6(ITEMMGR->GetNpcMapDealer(i)); //str6(GetMapName(npcInfo->MapNum));
					str6.append("§§FALSE");
					resultArrayText.push_back(str6);
					resultArrayColors.push_back(RGB_HALF(0, 255, 255));

					//std::string str11(", ");
					//str11.append("§§FALSE");
					//resultArrayText.push_back(str11);
					//resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					//VECTOR3 coolrinates = npcInfo->vPos;
					//char buffcoordx[15];
					//sprintf(buffcoordx, "%d", (DWORD)coolrinates.x / 100);
					//std::string str8(buffcoordx);
					//str8.append("§§FALSE");
					//resultArrayText.push_back(str8);
					//resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					//std::string str10(", ");
					//str10.append("§§FALSE");
					//resultArrayText.push_back(str10);
					//resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					//char buffcoordy[15];
					//sprintf(buffcoordy, "%d", (DWORD)coolrinates.z / 100);
					//std::string str9(buffcoordy);
					//str9.append("§§FALSE");
					//resultArrayText.push_back(str9);
					//resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					//std::string str12("] ");
					//str12.append("§§FALSE");
					//resultArrayText.push_back(str12);
					//resultArrayColors.push_back(RGB_HALF(255, 255, 255));

					ITEMMGR->addNpcNameExcluded(npcNameStd);
			}
		}
	}
	if (resultArrayText.size() > 0)
	{
		CHATMGR->AddMsgItemChat(resultArrayText, resultArrayColors, 0, CTC_GENERALCHAT, "These are the Npcs that sells the requested Item");
	}
	else
	{
		std::string sentence = "";
		sentence.append(info->ItemName);
		sentence.append(" is not sold in any NPC");
		CHATMGR->AddMsg(CTC_SYSMSG, sentence.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
//Alemuri Search Mob infos for Partial Name----------------------------------------------------------------------
std::string CGameIn::FindMobInfoForPartialName( char* strName )
{
	//BASE_MONSTER_LIST* pInfo = NULL;

	std::string lowCapSearchingName(strName);

	std::for_each(lowCapSearchingName.begin(), lowCapSearchingName.end(), [](char & c){ 
	c = ::tolower(c);
	});
	
	//DWORD searchedItemIdx = searchDropsLowercase
	int searchedMobIdx = 0;
	std::string realMobName = "";
	for (m_tabIndex = m_tabIndex; m_tabIndex < 2000; m_tabIndex++)
	{
		BASE_MONSTER_LIST* pInfo = GAMERESRCMNGR->GetMonsterListInfo(m_tabIndex);
		if (pInfo)
		{
			std::string monName(pInfo->Name);
			std::for_each(monName.begin(), monName.end(), [](char & c){ 
			c = ::tolower(c);
			});
			if( std::strstr( monName.c_str(), lowCapSearchingName.c_str() ) != NULL )
			{
				searchedMobIdx = m_tabIndex;
				std::string tempStr(pInfo->Name);
				realMobName = tempStr;
				//ExcludeSearchDropKey(lowCapItemName);
				m_tabIndex += 1;
				break;
			}
		}
	}

	if (m_tabIndex >= 2000)
		resetTabIndex();

	int testAfterIndex = m_tabIndex;

	//Test after this Tab if there are others, otherwise reset already the Tab Index-----------------
	if (testAfterIndex > 0)
	{
		std::string realMobNameTest = "";
		for (testAfterIndex = testAfterIndex; testAfterIndex < 2000; testAfterIndex++)
		{
			BASE_MONSTER_LIST* pInfo = GAMERESRCMNGR->GetMonsterListInfo(testAfterIndex);
			if (pInfo)
			{
				std::string monNameTest(pInfo->Name);
				std::for_each(monNameTest.begin(), monNameTest.end(), [](char & c){ 
				c = ::tolower(c);
				});
				if( std::strstr( monNameTest.c_str(), lowCapSearchingName.c_str() ) != NULL )
				{
					searchedMobIdx = testAfterIndex;
					std::string tempStr(pInfo->Name);
					realMobNameTest = tempStr;
					//ExcludeSearchDropKey(lowCapItemName);
					testAfterIndex += 1;
					break;
				}
			}
		}		
		if (realMobNameTest == "")
			resetTabIndex();
	}
	//-----------------------------------------------------------------------------------------------
	

	//if (pInfo)
	//{
	//	//OutputDebug(pInfo->ItemName);
	//	std::string str(pInfo->Name);
	//	return str;
	//}

	return realMobName;
}
//--------------------------------------------------------------------------------------------------
