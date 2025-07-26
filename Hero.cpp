#include "stdafx.h"
#include "Hero.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "ItemManager.h"
#include "CharacterCalcManager.h"
#include "PartyManager.h"
#include "cSkillTreeManager.h"
#include "ObjectGuagen.h"
#include "./Audio/MHAudioManager.h"
#include "StreetStallManager.h"
#include "CheatMsgParser.h"
#include "interface/cWindowManager.h"
#include "ChatManager.h"
#include "WindowIdEnum.h"
#include "ReviveDialog.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "DealDialog.h"
#include "ProgressDialog.h"
#include "mhMap.h"
#include "BattleSystem_Client.h"
#include "NpcScriptDialog.h"
#include "CommonCalcFunc.h"
#include "AppearanceManager.h"
#include "ShoutDialog.h"
#include "GuageDialog.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "QuickDlg.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "../[cc]skill/client/info/buffskillinfo.h"
#include "../[cc]skill/Client/Object/SkillObject.h"
#include "NpcImageDlg.h"
#include "cskilltrainingdlg.h"
#include "cSkillTreeDlg.h"
#include "ChatManager.h"
#include "ObjectBalloon.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "MHTimeManager.h"
#include "CWayPointManager.h"
#include "BigMapDlg.h"
#include "MiniMapDlg.h"
#include "TileManager.h"
#include "MovePoint.h"
#include "WorldMapDlg.h"
#include "MonsterGuageDlg.h"
#include "BattleGuageDlg.h"
#include "..\hseos\Family\SHFamily.h"

#include "GameResourceManager.h"
#include "input/UserInput.h"//wasd movement
#include "ChattingDlg.h"
#include "maingame.h" 
#include "ShoutDialog.h"
#include "Gamein.h"
#include "./ChatRoomCreateDlg.h"
#include "./ChatRoomDlg.h"
#include ".\cchangenamedialog.h"
#include "GuildNoteDlg.h"
#include "MiniNoteDialog.h"
#include "NoteDialog.h"
#include "UnionNoteDlg.h"
//wasd 2
#include "macromanager.h"
#include "Object.h"
#include "MainSystemDlg.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "SlotDialog.h"
#include "ChaptchaDlg.h"
VECTOR3 gHeroPos;

extern volatile LONG g_PathThreadActivated;

CHero::CHero() :
m_pcsFamily(new CSHFamily),
m_WayPointManager(new CWayPointManager)
//wasd movement//Beyond SEA2
,m_KeyMove_DistOneStep(1)//100//200
{
	//m_NextAction.Clear();
	m_CurComboNum = 0;
	m_maxExpPoint = 0;
	m_StorageNum = 0;	
	m_matchedQuantity = 0;
	m_byExtendedInvenCount = 0;
	m_DisableWASD = 0;
	//inspect
	for (int i = 0; i<eWearedItem_Max; i++)
	{
		m_OtherPlayerEquip[i] = NULL;
	}
}

CHero::~CHero()
{
	ITEMMGR->RemoveCoolTime( GetID() );

	Release();
	SAFE_DELETE(m_pcsFamily);
	SAFE_DELETE(m_WayPointManager);
}

void CHero::InitHero(HERO_TOTALINFO * pHeroTotalInfo)
{
	mMoveSound = 0;
	m_MovingAction.Clear();
	m_NextAction.Clear();
	m_SkillStartAction.Clear();
	m_bIsAutoAttackMode = TRUE;
	m_bIsAutoAttacking = FALSE;
	memcpy(&m_HeroTotalInfo, pHeroTotalInfo, sizeof(HERO_TOTALINFO));
	
	m_maxExpPoint = GAMERESRCMNGR->GetMaxExpPoint(GetLevel());
	
	ZeroMemory( &mItemBaseStat, sizeof( PlayerStat ) );
	ZeroMemory( &mItemOptionStat, sizeof( PlayerStat ) );

	// 080313 LUJ, ¼¼Æ® ¾ÆÀÌÅÛ ´É·Â ÃÊ±âÈ­
	ZeroMemory( &mSetItemStat, sizeof( PlayerStat ) );

	m_dwLastSocietyAction = gCurTime;

	GAMEIN->GetQuickDlg()->SetActiveSheet( 0, 0 );

	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	// desc_hseos_¸ó½ºÅÍ¹ÌÅÍ01
	// S ¸ó½ºÅÍ¹ÌÅÍ Ãß°¡ added by hseos 2007.05.23 2008.01.11:CPlayer ¿¡¼­ ¿Å±è
	ZeroMemory(&m_stMonstermeterInfo, sizeof(m_stMonstermeterInfo));
	mCurrentSkillObjectIndex = 0;


//---KES Àåºñ ¹æ¾î·Â Àû¿ë ¿©ºÎ 071128
	m_bNoDeffenceFromArmor = FALSE;
	mSkillDelay = 0;
	mSkillCancelDelay = 0;
	m_dwFollowPlayerIdx = 0;
//---------------

	// 090526 ShinJS --- ÀÌ¸§À¸·Î ÆÄÆ¼ÃÊ´ë½Ã ÀÛ¾÷½Ã°¢ ÃÊ±âÈ­
	m_dwInvitePartyByNameLastTime = 0;
	m_dwInvitePartyByNameLimitTime = 0;


	// 100226 ShinJS --- ÀÎÃ¦Æ® ·¹º§ Á¤º¸ Ãß°¡
	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;
	m_totalpvpscore = 0;
	m_lastspinslot = 0;
	m_bIsSlotwait = FALSE;
	m_lastspinget = 0;
	//fixdebug
	m_Moneyspinjackpot = 0;
	m_Moneyspinget = 0;
	m_WinSlot1 = 0;
	m_WinSlot2 = 0;
	m_WinSlot3 = 0;
	m_WinSlot4 = 0;

	//Alemuri Party War----
	isPartyWar = FALSE;

	ally1 = "";
	ally2 = "";
	ally3 = "";
	ally4 = "";
	ally5 = "";
	ally6 = "";
	ally7 = "";
	enemy1 = "";
	enemy2 = "";
	enemy3 = "";
	enemy4 = "";
	enemy5 = "";
	enemy6 = "";
	enemy7 = "";
	//---------------------

	//Alemuri Item Drop Rates Info to client-----
	m_ServerDropRates = 0;
	m_ServerDropRatesMultiplier = 1;
	//-------------------------------------------
}

void CHero::Process()
{
	CPlayer::Process();
	CAMERA->Process();
	Move_KeyboardInput2();//wasd 2
	if(m_MovingAction.HasAction())
	{
		if(m_MovingAction.CheckTargetDistance(&GetCurPosition()) == TRUE)
		{
			// 091026 ShinJS --- ¾×¼Ç ½ÇÇà½Ã ±æÃ£±âÁ¤º¸ ÃÊ±âÈ­
			ClearWayPoint();

			MOVEMGR->HeroMoveStop();

			m_MovingAction.ExcuteAction(this);
			m_MovingAction.Clear();
		}
		else
		{
			if( m_MovingAction.GetActionKind() == eActionKind_Skill )
			{
				static DWORD dwSkillPersuitTime	= 0;

				if(gCurTime > dwSkillPersuitTime)
				{
					m_MovingAction.ExcuteAction(this);

					const int SKILL_PERSUIT_TICK = 1000;
					dwSkillPersuitTime = gCurTime + SKILL_PERSUIT_TICK;
				}
			}
			else if( MOVEMGR->IsMoving(this) == FALSE )
			{
				m_MovingAction.Clear();
			}
		}
	}
	else if(m_bIsAutoAttacking)
	{
		if(m_NextAction.HasAction() == FALSE)
		{
			if(GetCurComboNum() < MAX_COMBO_NUM || GetCurComboNum() == SKILL_COMBO_NUM)	// ÀÚµ¿°ø°ÝÀº ÄÞº¸ 2±îÁö¸¸	12/3ÀÏ È¸ÀÇ °á°ú 3¿¡¼­ 2·Î ¹Ù²ñ
			{
				if(SKILLMGR->OnSkillCommand(this,&m_AutoAttackTarget,FALSE) == FALSE)
					DisableAutoAttack();
			}
		}
		else
		{
			m_NextAction.ExcuteAction(this);
		}
	}

	// 090204 LUJ, Æ¯¼ö ½ºÅ³À» ÇÏ³ª¾¿ °Ë»çÇÏ¿© È°¼º/ºñÈ°¼º½ÃÅ²´Ù
	ProcessSpecialSkill();
	GetPosition(&gHeroPos);
	MAP->SetShadowPivotPos(&gHeroPos);
	AUDIOMGR->SetListenerPosition(
		&gHeroPos,
		GetAngle());

	if(IsDied())
	{
		if(eObjectState_Die != GetState())
		{
			OBJECTSTATEMGR->EndObjectState(
				this,
				GetState());
			OBJECTSTATEMGR->StartObjectState(
				this,
				eObjectState_Die);

			ChangeMotion(
				CHARACTER_MOTION[eCharacterMotion_Die1][GetWeaponAniType()],
				0);
			ChangeBaseMotion(
				CHARACTER_MOTION[eCharacterMotion_Died1][GetWeaponAniType()]);
		}

		PKMGR->SetPKStartTimeReset();
	}

	if(GetState() == eObjectState_SkillUsing)
	{
		if(gCurTime - GetStateStartTime() > 10000)	//¼ýÀÚ¸¦ ÁÙÀÌ±â¿£ Áø¹ýºÎºÐÀÌ °É¸°´Ù.
		{
			OBJECTSTATEMGR->EndObjectState(this,eObjectState_SkillUsing);
		}
	}

	ProcMonstermeterPlayTime();
	OBJECTMGR->ApplyShadowOption(
		this);

	// 090909 ShinJS --- ±æÃ£±â °á°ú¿¡ µû¸¥ Ã³¸®
	switch( m_WayPointManager->GetWMStatus() )
	{
	// ¼º°øÇßÀ» ¶§
	case eWMStatus_Success:
		{
			// Çö ¿þÀÌÆ÷ÀÎÆ®¸¦ ¹«È¿È­ÇÏ°í, »õ·Î¿î µ¥ÀÌÅÍ¸¦ ³Ö¾î¾ß ÇÑ´Ù
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);

			// ´ÙÀ½ ¿þÀÌÆ÷ÀÎÆ®·Î ÀÌµ¿
			NextMove();

			// 091204 --- ¸ñÇ¥ ÁÂÇ¥ ¼³Á¤(¿¬¼ÓÇÏ¿© ±æÃ£±â¸¦ ÇÑ°æ¿ì Stop¸Þ¼¼Áö¸¦ ¹ÞÀ¸¸é ¸ñÇ¥ÁÂÇ¥°¡ Áö¿öÁø´Ù. ´Ù½Ã ¼³Á¤ÇÒ¼ö ÀÖµµ·Ï ÇÑ´Ù.)
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
			CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
			CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
			CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
			if( pBigMapDlg )		pBigMapDlg->SetMovePoint( &vecDest );
			if( pWorldToBigMapDlg )	pWorldToBigMapDlg->SetMovePoint( &vecDest );
			if( pMiniMapDlg )		pMiniMapDlg->SetMovePoint( &vecDest );

			MOVEPOINT->SetPoint( &vecDest );
		}
		break;
	// ½ÇÆÐÇÑ °æ¿ì
	case eWMStatus_Fail:
		{
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);
			ClearWayPoint();

			// 091208 ShinJS --- ±æÃ£±â ½ÇÆÐ½Ã Á÷¼±»ó ÀÌµ¿ ÃÖ´ëÁöÁ¡À¸·Î ÀÌµ¿ÇÑ´Ù.
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			VECTOR3 startPos;
			GetPosition( &startPos );

			// Á÷¼±»ó ÀÌµ¿°¡´É ÃÖ´ëÁöÁ¡À» ±¸ÇÑ´Ù.
			if( MAP->CollisionLine( &startPos, &vecDest, &vecDest, MAP->GetMapNum(), this ) == FALSE )
				break;

			if( MOVEMGR->IsMoving( this ) )
				MOVEMGR->CalcPositionEx( this, gCurTime );

			if ( MAP->CollisionCheck_OneLine_New( &startPos, &vecDest ) == TRUE)
			{
				Move_Simple( &vecDest );
				MOVEPOINT->SetPoint( &vecDest );
			}
		}
		break;
	}

	//Alemuri Dragonya Delete Costume-----------------------------------------------------------------------------------
	const ITEMBASE*	itemBaseCostumeHairs = ITEMMGR->GetItemInfoAbsIn(this, TP_WEAR_START + eWearedItem_Costume_Head );
	const ITEMBASE*	itemBaseCostumeGloves = ITEMMGR->GetItemInfoAbsIn(this, TP_WEAR_START + eWearedItem_Costume_Glove );
	const ITEMBASE*	itemBaseCostumeDress = ITEMMGR->GetItemInfoAbsIn(this, TP_WEAR_START + eWearedItem_Costume_Dress );
	const ITEMBASE*	itemBaseCostumeShoes = ITEMMGR->GetItemInfoAbsIn(this, TP_WEAR_START + eWearedItem_Costume_Shoes );
	const ITEMBASE*	itemBaseCostumeWings = ITEMMGR->GetItemInfoAbsIn(this, TP_WEAR_START + eWearedItem_Wing );
	DWORD wearCostumeHairs = GetWearedItemIdx( eWearedItem_Costume_Head );
	DWORD wearCostumeGloves = GetWearedItemIdx( eWearedItem_Costume_Glove );
	DWORD wearCostumeDress = GetWearedItemIdx( eWearedItem_Costume_Dress );
	DWORD wearCostumeShoes = GetWearedItemIdx( eWearedItem_Costume_Shoes );
	DWORD wearCostumeWings = GetWearedItemIdx( eWearedItem_Wing );
	if (wearCostumeHairs == Dragonya_Soul_Hairs && GetAliasType() != 1)
	{
		if (itemBaseCostumeHairs)
			DeleteItemCustom(itemBaseCostumeHairs->Position, itemBaseCostumeHairs->wIconIdx, itemBaseCostumeHairs->Durability);
	}
	if (wearCostumeGloves == Dragonya_Soul_Gloves && GetAliasType() != 1)
	{
		if (itemBaseCostumeGloves)
			DeleteItemCustom(itemBaseCostumeGloves->Position, itemBaseCostumeGloves->wIconIdx, itemBaseCostumeGloves->Durability);
	}
	if (wearCostumeDress == Dragonya_Soul_Body && GetAliasType() != 1)
	{
		if (itemBaseCostumeDress)
			DeleteItemCustom(itemBaseCostumeDress->Position, itemBaseCostumeDress->wIconIdx, itemBaseCostumeDress->Durability);
	}
	if (wearCostumeShoes == Dragonya_Soul_Shoes && GetAliasType() != 1)
	{
		if (itemBaseCostumeShoes)
			DeleteItemCustom(itemBaseCostumeShoes->Position, itemBaseCostumeShoes->wIconIdx, itemBaseCostumeShoes->Durability);
	}
	if (wearCostumeWings == Dragonya_Soul_Wings && GetAliasType() != 1)
	{
		if (itemBaseCostumeWings)
			DeleteItemCustom(itemBaseCostumeWings->Position, itemBaseCostumeWings->wIconIdx, itemBaseCostumeWings->Durability);
	}
	//------------------------------------------------------------------------------------------------------------------

}

// desc_hseos_¸ó½ºÅÍ¹ÌÅÍ01
// S ¸ó½ºÅÍ¹ÌÅÍ Ãß°¡ added by hseos 2007.05.23 2008.01.11:CPlayer ¿¡¼­ ¿Å±è
void CHero::ProcMonstermeterPlayTime()
{
	if (gCurTime - m_stMonstermeterInfo.nPlayTimeTick > 60000)
	{
		m_stMonstermeterInfo.nPlayTimeTick = gCurTime;
		++m_stMonstermeterInfo.nPlayTime;
		++m_stMonstermeterInfo.nPlayTimeTotal;
	}
	if (gCurTime > m_lastspinslot && m_bIsSlotwait == TRUE )
	{
		m_lastspinslot = gCurTime + 150;

		CSlotDialog* pHandler = (CSlotDialog*)GAMEIN->GetSlotDlg();
		if( pHandler->IsActive() )
		{
			pHandler->Prosses();
		}
	} 
	
	if (m_bIsSlotwait == TRUE && (gCurTime > m_lastspinget && m_lastspinget != 0))
	{
		m_bIsSlotwait = FALSE;
		m_lastspinslot = 0;
		m_lastspinget = 0;

		DoGetmoneyspinslot();
	}
}
// E ¸ó½ºÅÍ¹ÌÅÍ Ãß°¡ added by hseos 2007.05.23 2008.01.11:CPlayer ¿¡¼­ ¿Å±è

void CHero::GetHeroTotalInfo(HERO_TOTALINFO * info)
{
	ASSERT(info);
	memcpy(info, &m_HeroTotalInfo, sizeof(HERO_TOTALINFO));
}


void CHero::ApplyInterface()
{
#ifndef _TESTCLIENT_
	SetMoney(m_HeroTotalInfo.Money);
	SetLevel(m_CharacterInfo.Level);
	SetMaxLife(m_CharacterInfo.MaxLife);
	SetLife(m_CharacterInfo.Life, 0);
	SetMaxMana(m_HeroTotalInfo.MaxMana);
	SetMana(m_HeroTotalInfo.Mana, 0);
	SetMaxExpPoint(m_maxExpPoint);

	SetExpPoint(m_HeroTotalInfo.ExpPoint);

	SetStrength(m_HeroTotalInfo.Str);
	SetDexterity(m_HeroTotalInfo.Dex);
	SetVitality(m_HeroTotalInfo.Vit);
	SetWisdom(m_HeroTotalInfo.Wis);
	SetIntelligence(m_HeroTotalInfo.Int);

	SetPartyIdx(m_HeroTotalInfo.PartyID);
	SetGuildMemberRank(m_CharacterInfo.PositionInMunpa);
	SetGuildIdx(m_CharacterInfo.MunpaID);
	SetGuildName(m_CharacterInfo.GuildName);
	SetFamilyName(m_CharacterInfo.FamilyName);
	GAMEIN->GetCharacterDialog()->RefreshGuildInfo();
	GAMEIN->GetCharacterDialog()->RefreshFamilyInfo();
	SetBadFame(m_CharacterInfo.BadFame);
	// 070117 LYW --- Add function to setting character face.
	GAMEIN->GetGuageDlg()->MakeHeroFace() ;

	if(cStatic* const nameStatic = (cStatic*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGENAME))
	{
		nameStatic->SetStaticText(
			m_BaseObjectInfo.ObjectName);
	}
#endif
}

void CHero::SetMaxLife(DWORD maxlife)
{
	CPlayer::SetMaxLife(maxlife);

	DWORD newMaxLife = GetMaxLife();
	if(newMaxLife == 0)
	{
		newMaxLife = 1;
	}
	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFE))->SetValue(
		(float)m_CharacterInfo.Life/(float)newMaxLife, 0 );

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", GetLife(), newMaxLife );

	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFETEXT);
	if( ps )	ps->SetStaticText( szValue );	
}

void CHero::Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal)
{
	CHATMGR->AddMsg(
		CTC_GETMONEY,
		CHATMGR->GetChatMsg(778),
		pHealer ? pHealer->GetObjectName() : GetObjectName(),
		HealVal);
	EFFECTMGR->AddHealNumber(
		HealVal,
		pHealer,
		this,
		HealKind);
	ChangeLife(
		HealVal);
}
void CHero::ChangeLife(int changeval)
{
	// 070419 LYW --- Hero : modified function ChangeLife.
	// 080616 LUJ, ÃÖ´ë HP¸¦ ³ÑÁö ¾Ê°Ô Ç¥½ÃµÇµµ·Ï ÇÑ´Ù
	DWORD newlife = min( GetMaxLife(), GetLife() + changeval );

	SetLife(newlife);
}

void CHero::SetLife(DWORD life, BYTE type)
{
	CPlayer::SetLife(life);

	if( m_BaseObjectInfo.ObjectBattleState == eObjectBattleState_Battle )
	{
		GAMEIN->GetBattleGuageDlg()->HeroBattleGuageUpdate();
	}

	if(GetState() == eObjectState_Die)
	{
		life = 0;
		GAMEIN->GetBattleGuageDlg()->HeroDie();
	}

//---KES divide 0 ¹æÁö
	DWORD MaxLife = GetMaxLife();
	if(MaxLife == 0)
	{
		MaxLife = 1;
	}

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFE))->SetValue(
		(float)life/(float)MaxLife, 
		1000);

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", life, GetMaxLife() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFETEXT);
	if( ps )	ps->SetStaticText( szValue );
}

void CHero::SetMoney(MONEYTYPE Money)
{
	m_HeroTotalInfo.Money = Money;
	if(GAMEIN->GetInventoryDialog())
		GAMEIN->GetInventoryDialog()->SetMoney(Money);

	GAMEIN->GetSkillTrainingDlg()->SetGold();
}

void CHero::SetMaxExpPoint(EXPTYPE dwPoint)
{
	m_maxExpPoint = dwPoint;

	float value = (float)GetExpPoint() / (float)m_maxExpPoint;

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue(value, 0);
	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent( value*100 );

	// ÀÎÅÍÆäÀÌ½º : ¼öÄ¡ Ç¥½Ã
	char szValue[50];
	// 070124 LYW --- Modified this line.
	//sprintf( szValue, "%d / %d", GetExpPoint(), dwPoint );
	sprintf(szValue, "%d/%d", GetExpPoint(), dwPoint);
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if (ps)	ps->SetStaticText(szValue);
}

void CHero::SetExpPoint(EXPTYPE dwPoint, BYTE type)
{
	m_HeroTotalInfo.ExpPoint = dwPoint;

	// ÀÎÅÍÆäÀÌ½º °ÔÀÌÁö º¯°æ
	float value = (float)dwPoint / (float)m_maxExpPoint;
	// 080530 LYW --- Hero : Modified a data type DWORD to EXPTYPE.
	DWORDEX valueTime = (type == 0) ? 0 : (2000 / m_maxExpPoint)*dwPoint;
	//DWORD valueTime = 0 ;

	// 080604 LYW --- Hero : °æÇèÄ¡ °ÔÀÌÁö Ã³¸® ¼öÁ¤.
	// °æÇèÄ¡ °ÔÀÌÁö´Â timeÀ» ¾²Áö ¾Ê´Â°Ô ¸Â´Â´Ù°í ÆÇ´Ü.
	// typeÀ» 0À¸·Î ¼¼ÆÃÇÑ´Ù.
	type = 1;

	if (GetLevel() == MAX_CHARACTER_LEVEL_NUM || type == 0)
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue(1, 0);
	else
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue(value, (DWORD)valueTime);

	// ÀÎÅÍÆäÀÌ½º : Ä³¸¯ÅÍ Ã¢
	CObjectGuagen* ps = (CObjectGuagen *)GAMEIN->GetCharacterDialog()->GetWindowForID(CI_EXPPOINTGUAGE);
	if (ps)
	{
		if (type == 0)
			ps->SetValue(value, 0);
		else
			ps->SetValue(value, (DWORD)valueTime);
	}

	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent(value*100);
	// ÀÎÅÍÆäÀÌ½º : ¼öÄ¡ Ç¥½Ã

	// 080603 LYW --- Hero : °æÇèÄ¡ Ã³¸® (__int32)¿¡¼­ (__int64)·Î º¯°æ Ã³¸®.
	//char szValue[50];
	//if(GetLevel() >= 99)
	//	sprintf( szValue, "[ %s ]", CHATMGR->GetChatMsg(179) );	//"[ èÇ ]"
	//else
	//	sprintf( szValue, "%.2f%%", value*100 );

	//	#ifdef _CHEATENABLE_	
	//	if(CHEATMGR->IsCheatEnable())
	//	{
	//		char buf1[66];
	//		wsprintf(buf1, " %d/%d", dwPoint, GetMaxExpPoint());
	//		strcat(szValue, buf1);
	//	}
	//	#endif

	char szValue[50];
	sprintf(szValue, "%.2f%%", value * 100);

#ifdef _CHEATENABLE_	
	if (CHEATMGR->IsCheatEnable())
	{
		char buf1[66];
		EXPTYPE exp = GetMaxExpPoint();
		sprintf(buf1, " %0.f/%0.f", (float)dwPoint, (float)exp);
		strcat(szValue, buf1);
	}
#endif

	cStatic* ps2 = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if (ps2)	ps2->SetStaticText(szValue);
}

void CHero::SetLevel(LEVELTYPE level)
{
	// ÀÎÅÍÆäÀÌ½º Ã¢ Ç¥½Ã ¼öÄ¡ º¯°æ
	GAMEIN->GetCharacterDialog()->SetLevel(level);

	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELEVEL);
	char tempStr[ 128 ] = {0, } ;
	sprintf( tempStr, "%d", level ) ;
	if( ps ) ps->SetStaticText( tempStr ) ;

	CPlayer::SetLevel(level);
	ITEMMGR->RefreshAllItem();
}

void CHero::SetSkillPoint(DWORD point)
{
	m_HeroTotalInfo.SkillPoint += point;
	GAMEIN->GetSkillTrainingDlg()->SetSkillPoint();
	GAMEIN->GetSkillTreeDlg()->SetSkillPoint();
}

void CHero::SetMaxMana(DWORD val)
{
	m_HeroTotalInfo.MaxMana = val;

	// ÀÎÅÍÆäÀÌ½º °ÔÀÌÁö º¯°æ
	DWORD neMaxMana = GetMaxMana();
	if(neMaxMana == 0)
	{
		neMaxMana = 1;
	}
	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANA))->SetValue(
		(float)m_HeroTotalInfo.Mana/(float)neMaxMana, 0 );

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", GetMana(), neMaxMana );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );
}

// 100223 ShinJS --- ¸¶³ª µ¥¹ÌÁö·Î ÀÎÇÏ¿© ¸¶³ªÁ¤º¸¸¦ º¯È­·®À¸·Î ¹Þ¾Æ °»½Å
void CHero::ChangeMana(int changeval)
{
	DWORD newMana = min( GetMaxMana(), GetMana() + changeval );
	SetMana( newMana );
}

void CHero::SetMana(DWORD val, BYTE type)
{
	CPlayer::SetMana(val);

	if(m_HeroTotalInfo.MaxMana != 0)
	{
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANA))->SetValue(
			(float)val/(float)GetMaxMana(), 
			1000);
	}

	// ÀÎÅÍÆäÀÌ½º : ¼öÄ¡ Ç¥½Ã
	char szValue[50];
	sprintf( szValue, "%d/%d", val, GetMaxMana() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );

	// ¼öÄ¡º¯°æ
	m_HeroTotalInfo.Mana = val;
}
void CHero::SetStrength(DWORD val)
{
	m_HeroTotalInfo.Str = val;

	//SW060906 ÆêÀÛ¾÷À¸·Î º¯°æ
	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// ÀÎÅÍÆäÀÌ½º Ã¢ Ç¥½Ã ¼öÄ¡ º¯°æ
	GAMEIN->GetCharacterDialog()->SetStrength();
	// 070411 LYW --- Hero : Modified function SetStrength.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetDexterity(DWORD val)
{
	m_HeroTotalInfo.Dex = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);


	// ÀÎÅÍÆäÀÌ½º Ã¢ Ç¥½Ã ¼öÄ¡ º¯°æ
	GAMEIN->GetCharacterDialog()->SetDexterity();
	// 070411 LYW --- Hero : Modified function SetDexterity.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->PrintEvasionRate();
	GAMEIN->GetCharacterDialog()->PrintCriticalRate();
	GAMEIN->GetCharacterDialog()->PrintBlockRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetVitality(DWORD val)
{
	m_HeroTotalInfo.Vit = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// ÀÎÅÍÆäÀÌ½º Ã¢ Ç¥½Ã ¼öÄ¡ º¯°æ
	GAMEIN->GetCharacterDialog()->SetVitality();
	// 070411 LYW --- Hero : Modified function SetVitality.
	//GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->PrintDefenseRate();
	GAMEIN->GetCharacterDialog()->PrintBlockRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetWisdom(DWORD val)
{
	m_HeroTotalInfo.Wis = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// ÀÎÅÍÆäÀÌ½º Ã¢ Ç¥½Ã ¼öÄ¡ º¯°æ
	GAMEIN->GetCharacterDialog()->SetWisdom();
	GAMEIN->GetCharacterDialog()->UpdateData();
	GAMEIN->GetCharacterDialog()->PrintMagDefenseRate() ;
	GAMEIN->GetCharacterDialog()->PrintMagicCriticalRate() ;
	GAMEIN->GetCharacterDialog()->UpdateData();
}

// 070411 LYW --- Hero : Add function to setting intelligence to hero.
void CHero::SetIntelligence( DWORD val )
{
	m_HeroTotalInfo.Int = val;

	CHARCALCMGR->CalcCharStats(this);

	// ÀÎÅÍÆäÀÌ½º Ã¢ Ç¥½Ã ¼öÄ¡ º¯°æ
	GAMEIN->GetCharacterDialog()->SetIntelligence();
	GAMEIN->GetCharacterDialog()->UpdateData();
	GAMEIN->GetCharacterDialog()->PrintMagAttackRate() ;
	GAMEIN->GetCharacterDialog()->PrintMagicCriticalRate() ;
	GAMEIN->GetCharacterDialog()->UpdateData();
}

void CHero::SetBadFame( FAMETYPE val )
{
	m_CharacterInfo.BadFame = val;
	GAMEIN->GetCharacterDialog()->SetBadFame( val );	
}

void CHero::SetPVPScoreMain(DWORD val)
{
	m_totalpvpscore = val;
	if (MAP->GetMapNum() == 10)
	{
		GAMEIN->GetChallengeZoneClearNo1Dlg()->SetActive(TRUE);
		GAMEIN->GetChallengeZoneClearNo1Dlg()->SetActivePVP(m_totalpvpscore, m_pvpkill, m_pvpkilled);
	}
}

void CHero::SetPVPScoreALL(WORD val1, WORD val2)
{
	m_pvpkill += val1;
	m_pvpkilled += val2;
	m_totalpvpscore += val1;
	m_totalpvpscore -= val2;
	if (MAP->GetMapNum() == 10)
	{
		if (!GAMEIN->GetChallengeZoneClearNo1Dlg())
		{
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetActive(TRUE);
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetActivePVP(m_totalpvpscore, m_pvpkill, m_pvpkilled);
		} else {
			GAMEIN->GetChallengeZoneClearNo1Dlg()->SetActivePVP(m_totalpvpscore, m_pvpkill, m_pvpkilled);
		}
	}

}

DWORD CHero::GetPVPScoretotal()
{
	return m_totalpvpscore;
}

DWORD CHero::GetPVPScorekilled()
{
	return m_pvpkilled;
}

DWORD CHero::GetPVPScorekill()
{
	return m_pvpkill;
}

BOOL CHero::StartSocietyAct( int nStartMotion, int nIngMotion, int nEndMotion, BOOL bHideWeapon )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Immortal )
	{
		if( GetState() == eObjectState_Society )
		{
			if( m_ObjectState.State_End_Motion != -1 )	//¾É±â µ¿ÀÛÀÇ °æ¿ì
			{
				if( m_ObjectState.bEndState == TRUE )
					return FALSE;
				if( gCurTime - GetStateStartTime() < GetEngineObject()->GetAnimationTime(m_ObjectState.State_Start_Motion ) )
					return FALSE;

				OBJECTSTATEMGR->EndObjectState( this, GetState(), GetEngineObject()->GetAnimationTime( m_ObjectState.State_End_Motion ) );
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	m_ObjectState.State_Start_Motion	= (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion		= (short int)nIngMotion;
	m_ObjectState.State_End_Motion		= (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;


	if(m_ObjectState.State_Start_Motion != -1)
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );

		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}


	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );

	m_BaseObjectInfo.ObjectState	= eObjectState_Society;
	m_ObjectState.State_Start_Time	= gCurTime;
	m_ObjectState.bEndState			= FALSE;

	if( m_ObjectState.State_Ing_Motion == -1 )
		OBJECTSTATEMGR->EndObjectState( this, eObjectState_Society, GetEngineObject()->GetAnimationTime( m_ObjectState.State_Start_Motion ) );

	return TRUE;
}

BOOL CHero::EndSocietyAct()
{
	if( m_ObjectState.State_Ing_Motion != -1 )
	{
		return FALSE;
	}
	return TRUE;
}

void CHero::StartSocietyActForQuest( int nStartMotion, int nIngMotion /* = -1 */, int nEndMotion /* = -1 */, BOOL bHideWeapon /* = FALSE  */ )
{
	m_ObjectState.State_Start_Motion = (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion = (short int)nIngMotion;
	m_ObjectState.State_End_Motion = (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;

	if( m_ObjectState.State_Start_Motion != -1 )
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );
		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}

	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );
}

#ifdef _GMTOOL_
const char* GetStateName( BYTE state )
{
	static char StateName[eObjectState_MAX][64] = {
	"eObjectState_None",						
	"eObjectState_Enter",						
	"eObjectState_Move",						
	"eObjectState_Ungijosik",					
	"eObjectState_Rest",						
	"eObjectState_Deal",						
	"eObjectState_Exchange",					
	"eObjectState_StreetStall_Owner",			
	"eObjectState_StreetStall_Guest",			
	"eObjectState_PrivateWarehouse",			
	"eObjectState_Munpa",						
	"eObjectState_SkillStart",				
	"eObjectState_SkillSyn",					
	"eObjectState_SkillBinding",				
	"eObjectState_SkillUsing",				
	"eObjectState_SkillDelay",				
	"eObjectState_TiedUp_CanMove",			
	"eObjectState_TiedUp_CanSkill",
	"eObjectState_TiedUp",
	"eObjectState_Die",						
	"eObjectState_BattleReady",
	"eObjectState_Exit",						
	"eObjectState_Immortal",					
	"eObjectState_Society",					
	"eObjectState_ItemUse",
	"eObjectState_TournamentReady",
	"eObjectState_TournamentProcess",
	"eObjectState_TournamentEnd",
	"eObjectState_TournamentDead",
	"eObjectState_Engrave",
	"eObjectState_RestDamage",
	"eObjectState_Connect"
	};

	return StateName[state];
}
#endif

void CHero::OnStartObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Move:
		{
			if(IsGetOnVehicle())
			{
				break;
			}

			SNDIDX walkSound = 1254;
			SNDIDX runSound = 1255;

			if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(GetWearedItemIdx(eWearedItem_Wing)))
			{
				if(ePartType_Wing == itemInfo->Part3DType)
				{
					walkSound = 1342;
					runSound = 1343;
				}
			}

			mMoveSound = AUDIOMGR->Play(
				eMoveMode_Run == m_MoveInfo.MoveMode ? runSound : walkSound,
				GetID());
			break;
		}
	}

	CPlayer::OnStartObjectState(State);
}

void CHero::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Enter:
		{
		}
		break;
	case eObjectState_Deal:
		{
			HERO->HeroStateNotify(eObjectState_Deal);
			GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );
			//GAMEIN->GetNpcImageDlg()->SetActive( FALSE );
		}
		break;

	case eObjectState_SkillUsing:
	case eObjectState_SkillBinding:
		SKILLMGR->OnComboTurningPoint(this);
		break;

	case eObjectState_SkillSyn:
		{
#ifndef _TESTCLIENT_
//			if(SKILLMGR->GetSkillObject(TEMP_SKILLOBJECT_ID) != NULL)	// ¼­¹ö¿¡¼­ ÀÀ´äÀÌ ¾È¿Â°æ¿ì
//				OBJECTSTATEMGR->StartObjectState(this,eObjectState_SkillUsing);
//			else				
				SKILLMGR->OnComboTurningPoint(this);
#else
			SKILLMGR->OnComboTurningPoint(this);
#endif
		}
		break;

	case eObjectState_Move:
		{
			AUDIOMGR->Stop(
				mMoveSound);
		}
		break;
	case eObjectState_SkillStart:
		{
			if(m_SkillStartAction.GetActionKind() != eActionKind_Skill_RealExecute)
				break;

			m_SkillStartAction.ExcuteAction(this);
			m_SkillStartAction.Clear();
		}
		break;
	}
	CPlayer::OnEndObjectState(State);
}

//////////////////////////////////////////////////////////////////////////
// Çàµ¿ ÇÔ¼öµé..
void CHero::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{

	//Alemuri PK notification to GM in case hosting an Event that requires to know the killer
#ifdef _GMTOOL_
	if (pAttacker)
	{
		if ( MAP->GetMapNum() != 10) //if Map is not Battle Arena
		{
			DWORD life = this->m_CharacterInfo.Life;

			if (pAttacker->GetObjectKind() == eObjectKind_Player && HERO->IsPKMode())
			{
				CHATMGR->AddMsg(CTC_SYSMSG, "You have been killed by %s",  pAttacker->GetObjectName());
			}
		}
	}
#endif
	//-----------------------------------------------------------------------------------------

	//	ASSERT(GetLife() == 0);

	WINDOWMGR->CloseAllMsgBox();

	GAMEIN->GetDealDialog()->SetDisable(FALSE);
	GAMEIN->GetDealDialog()->SetActive(FALSE);	// »óÁ¡Ã¢ ´Ý±â

	GAMEIN->GetNpcScriptDialog()->SetDisable(FALSE);	//NPCÃ¢µµ ´Ý±â
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);

	// ÇÁ·Î±×·¹½º Ã¢ ´Ý±â
	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );

		if( dialog->IsActive() )
		{
			dialog->Cancel();
		}
	}

	SetMovingAction(NULL);
	DisableAutoAttack();					// ¿ÀÅä°ø°Ý ²ô±â
	SetLife(0);

	const eWeaponType weapon = GetWeaponEquipType();

	DWORD time = 0;
	if( bFatalDamage )
	{
		time = HERO->GetEngineObject()->GetAnimationTime( CHARACTER_MOTION[ eCharacterMotion_Died2 ][ weapon ] );
	}
	else
	{
		time = HERO->GetEngineObject()->GetAnimationTime( CHARACTER_MOTION[ eCharacterMotion_Died1 ][ weapon ] );
	}
	CAMERA->SetCharState( eCS_Die, time );

	CPlayer::Die(pAttacker,bFatalDamage,bCritical,bDecisive);
	GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
	// 090907 ShinJS --- Á×Àº °æ¿ì WayPoint¸¦ Á¦°ÅÇÑ´Ù
	ClearWayPoint();
}

// 080910 LUJ, µ¥¹ÌÁö »óÅÂ Ã³¸®¸¦ RESULTINFO ±¸Á¶Ã¼¸¦ ¹Þ¾Æ Ã³¸®ÇÏµµ·Ï ÇÔ
void CHero::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	//////////////////////////////////////////////////////////////////////////
	// Ä«¸Þ¶ó µÚ·Î ¹Ð¸²
	VECTOR3 push;
	MATRIX4 my;
	SetVector3(&push,0,0,30);
	SetRotationYMatrix(&my,-m_RotateInfo.Angle.ToRad());
	TransformVector3_VPTR2(&push,&push,&my,1);
	CAMERA->SetPushedVibration(&push,200);
	
	if( pAttacker )
	{
		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			PKMGR->SetPKStartTimeReset();
		}

		// ÀÌµ¿ Áß¿¡ °ø°Ý¹ÞÀº ¹æÇâÀ» ÃÄ´Ùº¸Áö ¸»¾Æ¾ßÇÑ´Ù. ±×·¸Áö ¾ÊÀ¸¸é ¹®¿öÄ¿°¡ ¹ß»ýÇÑ´Ù
		if(0 == OBJECTMGR->GetSelectedObject() &&
			FALSE == GetBaseMoveInfo()->bMoving)
		{
			VECTOR3 position = pAttacker->GetCurPosition() - GetCurPosition();
			GetEngineObject()->SetDirection(
				&position);
			OBJECTMGR->SetSelectedObject(
				pAttacker);
		}
	}
	
	CPlayer::Damage(pAttacker,DamageKind,Damage,result);
}

void CHero::OnHitTarget(CObject* pMainTarget)
{
	//////////////////////////////////////////////////////////////////////////
	// ÁÖÀÎ°ø°ú µ¥¹ÌÁö ÀÔ´Â ¸ÞÀÎÅ¸°Ù°ú °¡±î¿ì¸é Á¶±Ý¾¿ ¶³¾îÆ®¸²	
	VECTOR3 targetpos = pMainTarget->GetCurPosition();
	VECTOR3 Curpos = HERO->GetCurPosition();

	float WeaponMinDistance = 100;
	switch(GetWeaponEquipType())
	{
	case eWeaponType_Sword	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Mace	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Axe	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Dagger	:	WeaponMinDistance = 100;	break;
	case eWeaponType_Staff	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Bow	:	WeaponMinDistance = 150;	break;	
	case eWeaponType_Gun	:	WeaponMinDistance = 100;	break;	
		// desc_hseos_ASSERT¼öÁ¤_01
		// S ASSERT¼öÁ¤ Ãß°¡ added by hseos 2007.06.02
		// ..¸Ç¼ÕÀ¸·Î °ø°ÝÀÏ ¶§µµ ¿À·ù´Â ¾Æ´Ï¹Ç·Î Ãß°¡
	case eWeaponType_None	:								break;
		// E ASSERT¼öÁ¤ Ãß°¡ added by hseos 2007.06.02
	default: ASSERT(0);
	}

	if(CalcDistanceXZ(&targetpos,&Curpos) < WeaponMinDistance)
	{
		VECTOR3 pos;	pos.x = 0;	pos.y = 0;	pos.z = 10.f;
		float angle = HERO->GetAngle();
		TransToRelatedCoordinate(&pos,&pos,angle);
		pos = pos + Curpos;
		MOVEMGR->HeroEffectMove(HERO,&pos,0,0);
	}
}


void CHero::ClearTarget(CObject* pObject)
{
	if(m_AutoAttackTarget.GetTargetID() == pObject->GetID())
		DisableAutoAttack();
	if(m_NextAction.GetTargetID() == pObject->GetID())
	{
		m_NextAction.Clear();
	}
}


void CHero::Revive(VECTOR3* pRevivePos)
{
	DisableAutoAttack();

	CAMERA->SetCharState( eCS_Normal, 0 );	//ÀÏ¾î³ª´Â µ¿ÀÛÀÌ ¾ø´Ù.

	CPlayer::Revive(pRevivePos);
}

DWORD CHero::DoGetAttackRate()
{
	// ¹«±â °ø°Ý·Â
	float	fWeaponAttack	= 0;
	{
		const PlayerStat::Value& value = GetItemBaseStats().mPhysicAttack;
		fWeaponAttack += value.mPlus * (1.0f + value.mPercent);
	}

	// Ä³¸¯ÅÍ ·¹º§
	WORD	wLevel			= GetLevel() ;
	float	AEnchant		= float( GetWeaponEnchantLevel() );

	// Èû
	float	fStrengthAll	= float( GetStrength() );
	// ¼ø¼ö Èû
	float	fRealStrengthAll	= float( m_HeroTotalInfo.Str );
	float	fPercent		= GetRatePassiveStatus()->PhysicAttack
							+ GetRateBuffStatus()->PhysicAttack
							+(GetItemOptionStats().mPhysicAttack.mPercent * 100.f)
							+ (GetSetItemStats().mPhysicAttack.mPercent * 100.f);
	//
	// +¿É¼Ç
	float	fPlus			= GetPassiveStatus()->PhysicAttack
							+ GetBuffStatus()->PhysicAttack
							+ GetItemOptionStats().mPhysicAttack.mPlus * 3 //Alemuri HC Orichalcum x3 increase
							+ GetSetItemStats().mPhysicAttack.mPlus;

	// ¹°¸® °ø°Ý·Â			= ( ( ¹«±â °ø°Ý·Â + ·¹º§ ) * ( 1 + ( Èû * 0.001 ) ) + ( ¼ø¼öÈû - 30 ) ) * ( 1 + ( ¹«±â ÀÎÃ¦Æ® ·¹º§ ^ 2 / 400 ) )
	float	fMaxAttack		= ( ( ( fWeaponAttack + wLevel ) * ( 1 + ( fStrengthAll * 0.001f ) ) + ( fRealStrengthAll - 30 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	fMaxAttack *= 3.8f; //Alemuri Scale Attack Up
	return DWORD( max( 0, Round( fMaxAttack, 1 ) ) );
}

DWORD CHero::DoGetDefenseRate()
{
	// ¹æ¾î±¸ ¹æ¾î·Â
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mPhysicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	DWORD Armor = GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	//---KES Àåºñ ¹æ¾î·Â Àû¿ë ¿©ºÎ 071128
	m_bNoDeffenceFromArmor = FALSE;
	//-----------------------------------

	if( m_CharacterInfo.Job[0] == 2 )	//·Î±×
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES Àåºñ ¹æ¾î·Â Àû¿ë ¿©ºÎ 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
	}
	else if( m_CharacterInfo.Job[0] == 3 )	//¸ÞÀÌÁö
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES Àåºñ ¹æ¾î·Â Àû¿ë ¿©ºÎ 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( GetPassiveStatus()->LightArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES Àåºñ ¹æ¾î·Â Àû¿ë ¿©ºÎ 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
	}

	// Ä³¸¯ÅÍ ·¹º§
	WORD	wLevel				= GetLevel() ;
	float	DEnchant			= float( GetPhysicDefenseEnchantLevel() );
	float	fRealVitalityAll	= float( m_HeroTotalInfo.Vit );
	// ÃÑ°Ç°­
	float	fVitality			=	(float)GetVitality();
	// %¿É¼Ç
	float	fPercent		= GetRatePassiveStatus()->PhysicDefense
							+ GetRateBuffStatus()->PhysicDefense
							+ (GetItemOptionStats().mPhysicDefense.mPercent * 100.f)
							+ (GetSetItemStats().mPhysicDefense.mPercent * 100.f)
							+ GetRatePassiveStatus()->Shield
							+ GetRateBuffStatus()->Shield;
	// +¿É¼Ç
	float	fPlus			= GetPassiveStatus()->PhysicDefense
							+ GetBuffStatus()->PhysicDefense
							+ GetItemOptionStats().mPhysicDefense.mPlus
							+ GetSetItemStats().mPhysicDefense.mPlus
							+ GetPassiveStatus()->Shield
							+ GetBuffStatus()->Shield;

	// ¹°¸® ¹æ¾î·Â			= ( ( Àåºñ ¹æ¾î·Â + ·¹º§ )   * ( 1 + ( °Ç°­ / 3000 ) )         + ( (¼ø¼ö°Ç°­ - 40) / 5 ) ) * ( 1 + ( ¹æ¾î±¸ ÀÎÃ¦Æ® ·¹º§ ÃÑÇÕ / 200 ) )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1.0f + ( fVitality / 500 ) ) + ( ( fRealVitalityAll - 40 ) / 5 ) ) * ( 1 + DEnchant / 200 ) ) 
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	fTotalDefense *= 1.8f; // 1.8f; //Alemuri Scale Defense Up
	return DWORD( max( 0, Round( fTotalDefense, 1 ) ) );

}

DWORD CHero::DoGetMagAttackRate()
{
	// ¹«±â °ø°Ý·Â
	float	fMagicAttack		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicAttack;

		fMagicAttack = value.mPlus * ( 1.0f + value.mPercent );
	}

	// Ä³¸¯ÅÍ ·¹º§
	WORD	wLevel			= GetLevel() ;
	float	AEnchant		= float( GetWeaponEnchantLevel() );

	// Áö´É
	float	fIntelligenceAll	= float( GetIntelligence() );

	// ¼ø¼ö Áö´É
	float	fRealIntelligenceAll	= float( m_HeroTotalInfo.Int );
	float	fPercent		= GetRatePassiveStatus()->MagicAttack
							+ GetRateBuffStatus()->MagicAttack
							+ (GetItemOptionStats().mMagicAttack.mPercent * 100.f)
							+ (GetSetItemStats().mMagicAttack.mPercent * 100.f);
	
	// +¿É¼Ç
	float	fPlus			= GetPassiveStatus()->MagicAttack
							+ GetBuffStatus()->MagicAttack
							+ GetItemOptionStats().mMagicAttack.mPlus * 3 //Alemuri HC Moonstone x3 increase
							+ GetSetItemStats().mMagicAttack.mPlus;

	// ¸¶¹ý °ø°Ý·Â			= ( ( ¹«±â °ø°Ý·Â + ·¹º§ ) * ( 1 + (Áö´É * 0.001) ) + ( ¼ø¼öÁö´É - 40 ) ) * ( 1 + ( ¹«±â ÀÎÃ¦Æ® ·¹º§ ^ 2 / 400 ) )
	float	fMaxAttack		= ( ( ( fMagicAttack + wLevel ) * ( 1 + ( fIntelligenceAll * 0.001f ) ) + ( fRealIntelligenceAll - 40 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	fMaxAttack *= 3.8f; //Alemuri Scale Mattack Up
	return DWORD( max( 0, Round( fMaxAttack, 1 ) ) );
}


DWORD CHero::DoGetMagDefenseRate()
{
	// ¹æ¾î±¸ ¹æ¾î·Â
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	// Ä³¸¯ÅÍ ·¹º§
	WORD	wLevel			= GetLevel() ;
	float	DEnchant		= float( GetMagicDefenseEnchantLevel() );

	// Á¤½Å
	float	fWisdomAll = float(GetWisdom()* 5.0f);
	float	fRealWisdomAll	= float( m_HeroTotalInfo.Wis);

	// %¿É¼Ç
	float	fPercent		= GetRatePassiveStatus()->MagicDefense
							+ GetRateBuffStatus()->MagicDefense
							+ (GetItemOptionStats().mMagicDefense.mPercent * 100.f) 
							+ (GetSetItemStats().mMagicDefense.mPercent * 100.f);
	
	// +¿É¼Ç
	float	fPlus			= GetPassiveStatus()->MagicDefense
							+ GetBuffStatus()->MagicDefense
							+ GetItemOptionStats().mMagicDefense.mPlus
							+ GetSetItemStats().mMagicDefense.mPlus;

	// ¸¶¹ý ¹æ¾î·Â			= ( ( Àåºñ ¹æ¾î·Â + ·¹º§ ) * ( 1 + ( Á¤½Å / 3000 ) ) + ( (¼ø¼öÁ¤½Å - 30) / 5 ) ) * ( 1 + ( ¹æ¾î±¸ ÀÎÃ¦Æ® ·¹º§ ÃÑÇÕ / 200 )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1 + ( fWisdomAll / 3000 ) ) + ( fRealWisdomAll / 5 ) ) * ( 1 + ( DEnchant / 200 ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;
	
	fTotalDefense *= 3.0f; //Alemuri Scale Mdef Up
	return DWORD( max( 0, Round( fTotalDefense, 1 ) ) );
}

DWORD CHero::DoGetAccuracyRate()
{

	BYTE byJobOrder			= m_CharacterInfo.Job[0] ;
	DWORD wLevel			= GetLevel() ;

	// 080313 LUJ, ÆÛ¼¾Æ® ´É·ÂÄ¡ Ãß°¡
	// 080408 LUJ, ÆÛ¼¾Æ® ´É·ÂÀÌ ¿Ã¹Ù·Î °è»êµÇÁö ¾Ê´Â ¹®Á¦ ¼öÁ¤
	// 080701 LYW, ¹öÇÁ·Î ´Ã¾î³ª´Â ½ºÅÈ ¼öÄ¡µµ Ãß°¡ÇÑ´Ù.
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus ) * ( 1.0f + mItemStat.mDexterity.mPercent );
	//float wStrengthAll	= ( mCharStat.mStrength.mPlus + mItemStat.mStrength.mPlus ) * ( 1.0f + mItemStat.mStrength.mPercent );
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemBaseStat.mDexterity.mPlus + GetBuffStatus()->Dex) * ( 1.0f + mItemBaseStat.mDexterity.mPercent );
	//float wStrengthAll	= ( mCharStat.mStrength.mPlus + mItemBaseStat.mStrength.mPlus + GetBuffStatus()->Str) * ( 1.0f + mItemBaseStat.mStrength.mPercent );
	float wDexterityAll = float( GetDexterity() );
	float wStrengthAll = float( GetStrength() );
	// ¼¼Æ® ´É·Â Àû¿ëÇÑ ¼öÄ¡¸¦ ¾ò¾î³½´Ù
	//{
	//	const PlayerStat::Value& value = GetSetItemStats().mDexterity;

	//	wDexterityAll =	
	//		DWORD( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) )  +
	//		GetPassiveStatus()->Dex+
	//		GetBuffStatus()->Dex;
	//}
	wDexterityAll *= 0.2f;
	wStrengthAll *= 0.2f;

	DWORD wClassOrderVal	= 0 ;
	//float wPassiveSkill		= GetPassiveStatus()->Accuracy;
	float	wPassiveSkill	= GetRatePassiveStatus()->Accuracy
								+ GetRateBuffStatus()->Accuracy
								+ (GetItemOptionStats().mAccuracy.mPercent * 100.f)
								+ (GetSetItemStats().mAccuracy.mPercent * 100.f)

								+ GetPassiveStatus()->Accuracy
								//+ GetBuffStatus()->Accuracy
								+ GetItemOptionStats().mAccuracy.mPlus;
								//+ GetSetItemStats().mAccuracy.mPlus;
	//------------------------------------------------------------------------------------------------

	float wBuffSkillVal		= GetBuffStatus()->Accuracy;

	switch( byJobOrder )
	{
	case 1 : wClassOrderVal = 20 ; break ;
	case 2 : wClassOrderVal = 25 ; break ;
	case 3 : wClassOrderVal = 15 ; break ;
	}

	float fValA				= ((float)wDexterityAll / 2) ;
	float fValC				= ((float)wStrengthAll / 4) ;
	float fValB				= (float)(fValA + wLevel + wClassOrderVal + fValC);

	float wAccuracy			= GetFloatRound(fValB) ;

	float dwTotalAccuracy	= wAccuracy + wPassiveSkill + wBuffSkillVal;

	// 071002 ¿õÁÖ, ¼¼Æ® ¾ÆÀÌÅÛÀ¸·Î ºÎ¿©µÈ ´É·ÂÄ¡¸¦ Ãß°¡ÇÑ´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mAccuracy;

		dwTotalAccuracy = max( 0, ( dwTotalAccuracy + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	// 080313 LUJ, ¾ÆÀÌÅÛ ½ºÅÈÀ» ÀÏ¹Ý°ú ¼¼Æ®·Î ºÐ¸®
	{
		const PlayerStat::Value& value = mItemBaseStat.mAccuracy;

		dwTotalAccuracy = ( dwTotalAccuracy + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	return DWORD( dwTotalAccuracy );

}

DWORD CHero::DoGetEvasionRate()
{
	BYTE byJobOrder			= m_CharacterInfo.Job[0] ;
	DWORD wLevel			= GetLevel() ;

	// 080313 LUJ, ÆÛ¼¾Æ® ´É·ÂÄ¡ Ãß°¡
	// 080408 LUJ, ÆÛ¼¾Æ® ´É·ÂÀÌ ¿Ã¹Ù·Î °è»êµÇÁö ¾Ê´Â ¹®Á¦ ¼öÁ¤
	// 080701 LYW, ¹öÇÁ·Î ´Ã¾î³ª´Â ½ºÅÈ ¼öÄ¡µµ Ãß°¡ÇÑ´Ù.
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus ) * ( 1.0f + mItemStat.mDexterity.mPercent );
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemBaseStat.mDexterity.mPlus + GetBuffStatus()->Dex) * ( 1.0f + mItemBaseStat.mDexterity.mPercent );
	float wDexterityAll = float( GetDexterity() );
	// ¼¼Æ® ´É·Â Àû¿ëÇÑ ¼öÄ¡¸¦ ¾ò¾î³½´Ù
	//{
	//	const PlayerStat::Value& value = GetSetItemStats().mDexterity;

	//	wDexterityAll =	
	//		DWORD( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) )  +
	//		GetPassiveStatus()->Dex+
	//		GetBuffStatus()->Dex;
	//}
	wDexterityAll *= 0.2f;

	DWORD wClassOrderVal	= 0 ;
	//float wPassiveSkill		= GetPassiveStatus()->Avoid ;
	float	wPassiveSkill	= GetRatePassiveStatus()->Avoid
								+ GetRateBuffStatus()->Avoid
								+ (GetItemOptionStats().mEvade.mPercent * 100.f)
								+ (GetSetItemStats().mEvade.mPercent * 100.f)

								+ GetPassiveStatus()->Avoid
								//+ GetBuffStatus()->Avoid
								+ GetItemOptionStats().mEvade.mPlus;
								//+ GetSetItemStats().mEvade.mPlus;
	//------------------------------------------------------------------------------------------------

	float wBuffSkillVal		= GetBuffStatus()->Avoid ;

	switch( byJobOrder )
	{
	case 1 : wClassOrderVal = 20 ; break ;
	case 2 : wClassOrderVal = 25 ; break ;
	case 3 : wClassOrderVal = 15 ; break ;
	}

	float fValA				= ((float)wDexterityAll / 2) ;
	float fValB				= (float)(fValA + wLevel + wClassOrderVal);

	float wEvasion			= GetFloatRound(fValB) ;

	float dwTotalEvasion	= wEvasion + wPassiveSkill + wBuffSkillVal;

	// 071002 ¿õÁÖ, ¼¼Æ® ¾ÆÀÌÅÛÀ¸·Î ºÎ¿©µÈ ´É·ÂÄ¡¸¦ Ãß°¡ÇÑ´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mEvade;

		dwTotalEvasion = max( 0, ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	// 080313 LUJ, ¾ÆÀÌÅÛ ½ºÅÈÀ» ÀÏ¹Ý°ú ¼¼Æ®·Î ºÐ¸®
	{
		const PlayerStat::Value& value = mItemBaseStat.mEvade;

		dwTotalEvasion = ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	return DWORD( dwTotalEvasion );
}

DWORD CHero::DoGetCriticalRate()
{
	DWORD wLevel			= GetLevel() ;

	// 080313 LUJ, ÆÛ¼¾Æ® ´É·ÂÄ¡ Ãß°¡
	// 080408 LUJ, ÆÛ¼¾Æ® ´É·ÂÀÌ ¿Ã¹Ù·Î °è»êµÇÁö ¾Ê´Â ¹®Á¦ ¼öÁ¤
	// 080701 LYW, ¹öÇÁ·Î ´Ã¾î³ª´Â ½ºÅÈ ¼öÄ¡µµ Ãß°¡ÇÑ´Ù.
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus ) * ( 1.0f + mItemStat.mDexterity.mPercent );
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemBaseStat.mDexterity.mPlus + GetBuffStatus()->Dex) * ( 1.0f + mItemBaseStat.mDexterity.mPercent );
	float wDexterityAll = float( GetDexterity() );
	// ¼¼Æ® ´É·Â Àû¿ëÇÑ ¼öÄ¡¸¦ ¾ò¾î³½´Ù
	//{
	//	const PlayerStat::Value& value = GetSetItemStats().mDexterity;

	//	wDexterityAll =	
	//		DWORD( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) )  +
	//		GetPassiveStatus()->Dex+
	//		GetBuffStatus()->Dex;
	//}
	wDexterityAll *= 0.2f;
	//float wPassiveSkill		= GetPassiveStatus()->CriticalRate ;
	float	wPassiveSkill	= GetRatePassiveStatus()->CriticalRate
								+ GetRateBuffStatus()->CriticalRate
								+ (GetItemOptionStats().mCriticalRate.mPercent * 100.f)
								+ (GetSetItemStats().mCriticalRate.mPercent * 100.f)

								+ GetPassiveStatus()->CriticalRate
								//+ GetBuffStatus()->CriticalRate
								+ GetItemOptionStats().mCriticalRate.mPlus;
								//+ GetSetItemStats().CriticalRate.mPlus;
	//------------------------------------------------------------------------------------------------

	float wBuffSkillVal		= GetBuffStatus()->CriticalRate ;

	float fValA				= (((float)wDexterityAll - 15) * 7) ;
	float fValB				= (float)(125 + wLevel + fValA + wPassiveSkill + wBuffSkillVal);

	float dwCritical		=  fValB; //GetFloatRound(fValB) ; //Bug crazy value

	// 071002 ¿õÁÖ, ¼¼Æ® ¾ÆÀÌÅÛÀ¸·Î ºÎ¿©µÈ ´É·ÂÄ¡¸¦ Ãß°¡ÇÑ´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mCriticalRate;

		dwCritical = max( 0, ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	// 080313 LUJ, ¾ÆÀÌÅÛ ½ºÅÈÀ» ÀÏ¹Ý°ú ¼¼Æ®·Î ºÐ¸®
	{
		const PlayerStat::Value& value = mItemBaseStat.mCriticalRate;

		dwCritical = ( dwCritical + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	return DWORD( dwCritical );
}

DWORD CHero::DoGetMagicCriticalRate()	
{
	// ·¹º§
	DWORD wLevel		= GetLevel() ;

	// ±âº» ½ºÅÈ
	RaceType race				= (RaceType)GetRace();
	BYTE byClass				=  m_CharacterInfo.Job[0];
	const PlayerStat stat		= GAMERESRCMNGR->GetBaseStatus( race, byClass );

	// Áö´É
	float	fIntelligenceAll	= float( GetIntelligence() );
	float fBaseIntelligence		= stat.mIntelligence.mPlus;
	//float fIntelligenceRate		= ( wLevel > 1 ? max(fIntelligenceAll - fBaseIntelligence, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );
	//float fIntelligenceRate		= ( wLevel > 1 ? max(fIntelligenceAll - fBaseIntelligence, 0) / ( ( 100 ) * 5.f ) : 0 );
	float fIntelligenceRate		= (max(fIntelligenceAll - fBaseIntelligence, 0) / ( ( 100 ) * 5.f ));

	// Á¤½Å
	float	fWisdomAll			= float( GetWisdom() );
	float fBaseWisdomAll		= stat.mWisdom.mPlus;
	//float fWisdomAllRate		= ( wLevel > 1 ? max(fWisdomAll - fBaseWisdomAll, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );
	//float fWisdomAllRate		= ( wLevel > 1 ? max(fWisdomAll - fBaseWisdomAll, 0) / ( ( 50 ) * 5.f ) : 0 );
	float fWisdomAllRate		= ( max(fWisdomAll - fBaseWisdomAll, 0) / ( ( 50 ) * 5.f ));

	float	fPlus				= GetPassiveStatus()->MagicCriticalRate
								+ GetBuffStatus()->MagicCriticalRate
								+ GetItemOptionStats().mMagicCriticalRate.mPlus
								+ GetSetItemStats().mMagicCriticalRate.mPlus;

	// %¿É¼Ç
	float	fPercent			= GetRatePassiveStatus()->MagicCriticalRate
								+ GetRateBuffStatus()->MagicCriticalRate
								+ (GetItemOptionStats().mMagicCriticalRate.mPercent * 100.f)
								+ (GetSetItemStats().mMagicCriticalRate.mPercent * 100.f);
	
	DWORD	dwCritical			= DWORD( ( 10.f * fIntelligenceRate ) + ( 20.f * fWisdomAllRate ) + ( ( fBaseIntelligence - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f ) );
	
	return dwCritical;
}

DWORD CHero::DoGetMoveSpeedRate()	
{
	// 091229 ONS Ä³¸¯ÅÍ Á¤º¸Ã¢¿¡ Ãâ·ÂµÇ´Â ÀÌµ¿¼Óµµ Ç¥½Ã¹æ¹ý º¯°æ
	float	fMoveSpeed = DoGetMoveSpeed() / 4.5f - 100.0f;
	
	return max( 0, DWORD( floor( 0.5f + fMoveSpeed ) ) );
}

DWORD CHero::DoGetCastingSpeedRate()
{
	float	fPercent			= GetRatePassiveStatus()->MagicSkillSpeedRate
								+ GetRateBuffStatus()->MagicSkillSpeedRate;

	// 100209 ONS Ä³½ºÆÃ ¼Óµµ Ç¥½Ã¹æ¹ý º¯°æ.
	return DWORD( floor( 0.5f + fPercent ) );
}

DWORD CHero::DoGetAttackSpeedRate()		
{
	float	fPercent			= GetRatePassiveStatus()->PhysicSkillSpeedRate
								+ GetRateBuffStatus()->PhysicSkillSpeedRate;
	
	return DWORD( floor( 0.5f + fPercent ) );
}

/* Á¾·áÇØ¾ßÇÒ »óÅÂ°ªÀ» ¼­¹ö¿¡µµ ¾Ë¸°´Ù. */
void CHero::HeroStateNotify(BYTE EndState)
{
	MSG_BYTE msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_STATE_NOTIFY;
	msg.dwObjectID = GetID();
	msg.bData = EndState;

	NETWORK->Send(&msg, sizeof(msg));
}

BYTE CHero::GetManaPercent()
{
	return (BYTE)((GetMana()*100) / GetMaxMana());
}

DWORD CHero::DoGetMaxMana()
{
	return m_HeroTotalInfo.MaxMana;
}

void CHero::ClearGuildInfo()
{
	CPlayer::ClearGuildInfo();
}

char* CHero::GetGuildEntryDate()
{
	return m_HeroTotalInfo.MunpaCanEntryDate;
}

void CHero::SetGuildEntryDate(char* day)
{
	SafeStrCpy(m_HeroTotalInfo.MunpaCanEntryDate, day, 11);
}

char* CHero::GetFamilyEntryDate()
{
	return m_HeroTotalInfo.FamilyCanEntryDate;
}

void CHero::SetFamilyEntryDate(char* day)
{
	SafeStrCpy(m_HeroTotalInfo.FamilyCanEntryDate, day, 11);
}

void CHero::SetGuildName(char* Name)	
{ 
	SafeStrCpy(m_CharacterInfo.GuildName, Name, MAX_GUILD_NAME+1);	
}

char* CHero::GetGuildName()			
{ 
	return m_CharacterInfo.GuildName;
}

void CHero::SetFamilyName(char* Name)	
{ 
	SafeStrCpy(
		m_CharacterInfo.FamilyName,
		Name,
		_countof(m_CharacterInfo.FamilyName));
}

char* CHero::GetFamilyName()			
{ 
	return m_CharacterInfo.FamilyName;
}

BOOL CHero::CanSocietyAction( DWORD curTime )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Society &&
		GetState() != eObjectState_Immortal )
		return FALSE;

	DWORD dwElapsedTime = gCurTime - m_dwLastSocietyAction;
	if( dwElapsedTime > 2000 ) //2ÃÊ
	{
		m_dwLastSocietyAction = gCurTime;
		return TRUE;
	}

	return FALSE;
}
void CHero::ProcessSpecialSkill()
{
	if( mSpecialSkillList.empty() )
	{
		return;
	}

	// 090204 LUJ, È¿À²¼ºÀ» À§ÇØ ÄÁÅ×ÀÌ³Ê ¸Ç ¾ÕÀÇ ½ºÅ³À» °Ë»çÇÑ ÈÄ ¸Ç µÚ·Î µ¹¸°´Ù
	SpecialSkillData specialSkillData = mSpecialSkillList.front();
	mSpecialSkillList.pop_front();

	cBuffSkillInfo* const buffSkillInfo = specialSkillData.mBuffSkillInfo;
	const BOOL validCondition = IsEnable(*(buffSkillInfo->GetSkillInfo()));
	const BOOL isTurnOn	= (! specialSkillData.mIsTurnOn && validCondition);
	const BOOL isTurnOff = (specialSkillData.mIsTurnOn && ! validCondition);

	if( isTurnOn )
	{
		buffSkillInfo->AddBuffStatus();
		specialSkillData.mIsTurnOn = TRUE;
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}
	else if( isTurnOff )
	{
		buffSkillInfo->RemoveBuffStatus();
		specialSkillData.mIsTurnOn = FALSE;
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}

	// 090204 LUJ, ´ÙÀ½ °Ë»ç¸¦ À§ÇØ Ãß°¡ÇÑ´Ù
	mSpecialSkillList.push_back( specialSkillData );
}

DWORD CHero::GetAccuracy()
{
	return mAccuracy;
}

DWORD CHero::GetAvoid()
{
	return mAvoid;
}

DWORD CHero::GetBlock()
{
	return mBlock;
}
DWORD CHero::GetTotalBlock()
{
	/*const float rate = mRateBuffStatus.Block + mRatePassiveStatus.Block;*/
	float		bonus = 0;

	switch (GetJop(0))
	{
		// 080910 LUJ, ÆÄÀÌÅÍ
	case 1:
	{
			  bonus = 15.f;
			  break;
	}
		// 080910 LUJ, ·Î±×
	case 2:
	{
			  bonus = 10.f;
			  break;
	}
		// 080910 LUJ, ¸ÞÀÌÁö
	case 3:
	{
			  bonus = 5.f;
			  break;
	}
		// 100218 ShinJS --- ¸¶Á·
	case 4:
	{
			  bonus = 9.f;
			  break;
	}
	}

	//float blockValue = (float(GetDexterity() / 27.f) + bonus);
	float blockValue = bonus;
	return (DWORD)blockValue;
	
	//return ( float( GetDexterity() / 27.f ) + rate + bonus );
}
DWORD CHero::GetPhysicAttackMax()
{
	return mPhysicAttackMax;
}

DWORD CHero::GetPhysicAttackMin()
{
	return mPhysicAttackMin;
}

DWORD CHero::GetMagicAttack()
{
	return mMagicAttack;
}

DWORD CHero::GetPhysicDefense()
{
	return mPhysicDefense;
}

DWORD CHero::GetMagicDefense()
{
	return mMagicDefense;
}

DWORD CHero::GetCriticalRate()
{
	return mCriticalRate;
}

DWORD CHero::GetCriticalDamage()
{
	return mCriticalDamage;
}

DWORD CHero::GetLifeRecover()
{
	return mLifeRecover;
}

DWORD CHero::GetManaRecover()
{
	return mManaRecover;
}

DWORD	CHero::GetMaxLifeWithItem()
{
	return mMaxLife;
}

DWORD	CHero::GetMaxManaWithItem()
{
	return mMaxMana;
}


void CHero::AddMatch()
{
	++m_matchedQuantity;

	SetHeart( TRUE );
}


void CHero::RemoveMatch()
{
	if( 1 > --m_matchedQuantity )
	{
		SetHeart( FALSE );

		m_matchedQuantity = 0;
	}
}


void CHero::CheckAllHeart()
{
	m_matchedQuantity = 0;
	
	// ÀüÃ¼ Àç°è»ê
	{
		CYHHashTable<CObject>& objectTable = OBJECTMGR->GetObjectTable();
		YHTPOSITION position = objectTable.GetPositionHead();

		for(CObject* object = objectTable.GetDataPos( &position );
            0 < object;
			object = objectTable.GetDataPos( &position ))
		{
			if( eObjectKind_Player != object->GetObjectKind() )
			{
				continue;
			}

			CPlayer* player = ( CPlayer* )object;

			if( player->CheckHeart( this ) )
			{
				player->SetHeart( TRUE );

				AddMatch();
			}
			else
			{
				player->SetHeart( FALSE );

				RemoveMatch();
			}
		}
	}
}

DWORD CHero::GetVitality() const 
{ 
	const float rate =
		mRatePassiveStatus.Vit +
		mRateBuffStatus.Vit +
		mItemBaseStat.mVitality.mPercent +
		mItemOptionStat.mVitality.mPercent +
		mSetItemStat.mVitality.mPercent * 100.f;
	const float plus =
		mPassiveStatus.Vit +
		mBuffStatus.Vit +
		mItemBaseStat.mVitality.mPlus +
		mItemOptionStat.mVitality.mPlus +
		mSetItemStat.mVitality.mPlus;
	float Result = mCharStat.mVitality.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, ¹Ý¿Ã¸²½ÃÅ²´Ù
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetWisdom() const
{ 
	const float rate =
		mRatePassiveStatus.Wis +
		mRateBuffStatus.Wis +
		mItemBaseStat.mWisdom.mPercent +
		mItemOptionStat.mWisdom.mPercent +
		mSetItemStat.mWisdom.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Wis +
		mBuffStatus.Wis + 
		mItemBaseStat.mWisdom.mPlus + 
		mItemOptionStat.mWisdom.mPlus +
		mSetItemStat.mWisdom.mPlus;
	float Result = mCharStat.mWisdom.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, ¹Ý¿Ã¸²½ÃÅ²´Ù
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetStrength() const
{
	const float	rate =
		mRatePassiveStatus.Str +
		mRateBuffStatus.Str +
		mItemBaseStat.mStrength.mPercent +
		mItemOptionStat.mStrength.mPercent +
		mSetItemStat.mStrength.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Str +
		mBuffStatus.Str +
		mItemBaseStat.mStrength.mPlus +
		mItemOptionStat.mStrength.mPlus +
		mSetItemStat.mStrength.mPlus;
	float Result = mCharStat.mStrength.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, ¹Ý¿Ã¸²½ÃÅ²´Ù
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetDexterity() const 
{ 
	const float rate =
		mRatePassiveStatus.Dex +
		mRateBuffStatus.Dex +
		mItemBaseStat.mDexterity.mPercent +
		mItemOptionStat.mDexterity.mPercent +
		mSetItemStat.mDexterity.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Dex +
		mBuffStatus.Dex +
		mItemBaseStat.mDexterity.mPlus +
		mItemOptionStat.mDexterity.mPlus +
		mSetItemStat.mDexterity.mPlus;
	float Result = mCharStat.mDexterity.mPlus * (1.f + rate / 100.f) + plus;
	
	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, ¹Ý¿Ã¸²½ÃÅ²´Ù
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetIntelligence() const
{
	const float	rate =
		mRatePassiveStatus.Int +
		mRateBuffStatus.Int +
		mItemBaseStat.mIntelligence.mPercent +
		mItemOptionStat.mIntelligence.mPercent +
		mSetItemStat.mIntelligence.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Int +
		mBuffStatus.Int +
		mItemBaseStat.mIntelligence.mPlus +
		mItemOptionStat.mIntelligence.mPlus +
		mSetItemStat.mIntelligence.mPlus;
	float Result = mCharStat.mIntelligence.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, ¹Ý¿Ã¸²½ÃÅ²´Ù
	return DWORD( Round( Result, 1 ) );
}

float CHero::DoGetMoveSpeed()
{
	float Speed = CPlayer::DoGetMoveSpeed();
	
	if( GetAbnormalStatus()->IsMoveStop )
	{
		Speed = 0;
	}

	return Speed;
}

void CHero::StartSkillProgressBar( DWORD time )
{
	GetObjectBalloon()->GetProgressBar()->CLI_Start(CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_TYPE, CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_WIDTH, time);
}
// 071210 LYW --- Hero : HEROÀÇ È®Àå ÀÎº¥Åä¸® ¼ö¸¦ ¼³Á¤ ¹× ¹ÝÈ¯ÇÏ´Â ÇÔ¼ö Ãß°¡.
void CHero::Set_HeroExtendedInvenCount(BYTE count)
{
	// ÇöÀç´Â È®Àå ÀÎº¥ ¼ö°¡ µÎ °³·Î Á¦ÇÑµÇ¾î ÀÖ°í, µû·Î ¾µÀÏÀÌ ¾øÀ» °Í °°¾Æ Á¤ÀÇ ÇÏÁö ¾Ê°í »ç¿ëÇÑ´Ù.
	if (count > 4) return; //6tab

	m_byExtendedInvenCount = count ;
}

void CHero::PassiveSkillCheckForWeareItem()
{
	mPassiveStatus = Status();
	mRatePassiveStatus = Status();

	SKILLTREEMGR->SetPositionHead();

	for(SKILL_BASE* pSkillBase = SKILLTREEMGR->GetSkillBase();
		0 < pSkillBase;
		pSkillBase = SKILLTREEMGR->GetSkillBase())
	{
		const LEVELTYPE skillLevel = min(
			LEVELTYPE(SKILLMGR->GetSkillSize(pSkillBase->wSkillIdx)),
			pSkillBase->Level);
		cActiveSkillInfo* const pSkill = ( cActiveSkillInfo* )SKILLMGR->GetSkillInfo(
			pSkillBase->wSkillIdx - 1 + skillLevel);

		if( pSkill->GetKind() != SKILLKIND_PASSIVE )
			continue;

		for( DWORD cnt = 0; cnt < MAX_BUFF_COUNT; cnt++ )			
		{
			const DWORD index = pSkill->GetSkillInfo()->Buff[ cnt ];
			cBuffSkillInfo* pSkillInfo = ( cBuffSkillInfo* )SKILLMGR->GetSkillInfo( index );

			if(0 == pSkillInfo)
			{
				break;
			}
			else if(FALSE == IsEnable(*(pSkillInfo->GetSkillInfo())))
			{
				continue;
			}

			pSkillInfo->AddPassiveStatus();
		}	
	}

}

// 090204 LUJ, Æ¯¼ö ½ºÅ³À» Ãß°¡ÇÑ´Ù.
void CHero::AddSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	// 090204 LUJ, Ã³À½¿¡´Â Ç×»ó Àû¿ëÇÑ´Ù. ÇÁ·Î¼¼½º ¶§ °Ë»çÇÏ¸é¼­ Á¶°Ç¿¡ ¸ÂÁö ¾ÊÀ» °æ¿ì ÇØÁ¦ÇÑ´Ù
	buffSkillInfo->AddBuffStatus();

	SpecialSkillData specialSkillData = { 0 };
	specialSkillData.mBuffSkillInfo	= buffSkillInfo;
	specialSkillData.mIsTurnOn		= TRUE;
	mSpecialSkillList.push_back( specialSkillData );
}

// 090204 LUJ, Æ¯¼ö ½ºÅ³À» Á¦°ÅÇÑ´Ù
void CHero::RemoveSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	SpecialSkillList::iterator it = mSpecialSkillList.end();

	// 090204 LUJ, ÄÁÅ×ÀÌ³Ê Áß¿¡ µ¿ÀÏÇÑ ½ºÅ³À» Ã£´Â´Ù
	for(	;
			mSpecialSkillList.end() != it;
			++it )
	{
		const SpecialSkillData& specialSkillData = *it;

		if( specialSkillData.mBuffSkillInfo == buffSkillInfo )
		{
			break;
		}
	}

	if( mSpecialSkillList.end() == it )
	{
		return;
	}

	const SpecialSkillData& specialSkillData = *it;

	// 090204 LUJ, Á¶°Ç¿¡ µû¶ó ¹ÌÀû¿ë »óÅÂÀÏ ¼ö ÀÖÀ¸¹Ç·Î °Ë»ç ÈÄ Ãë¼ÒÇØ¾ß ÇÑ´Ù
	if( specialSkillData.mIsTurnOn )
	{
		buffSkillInfo->RemoveBuffStatus();
	}

	mSpecialSkillList.erase( it );
}

void CHero::ClearWayPoint()
{
	m_WayPointManager->ClearAllWayPoint();

	// Big/Mini MapÀÇ MovePoint Á¦°Å
	CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
	CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
	CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
	CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
	if( pBigMapDlg )		pBigMapDlg->ClearMovePoint();
	if( pWorldToBigMapDlg )	pWorldToBigMapDlg->ClearMovePoint();
	if( pMiniMapDlg )		pMiniMapDlg->ClearMovePoint();
}

BOOL CHero::Move_UsePath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove )
{
	CChaptchaDlg* ChaptchaDlg = GAMEIN->GetChaptchaDlg();
	if( ChaptchaDlg->IsActive() )
	{
		MOVEMGR->MoveStop(this,0);
		return TRUE;
	}
	// 50cm Å¸ÀÏÀÇ Áß¾Ó¿¡ ¼­°Ô ÇÑ´Ù.
	pDestPos->x = (int(pDestPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	pDestPos->y = 0;
	pDestPos->z = (int(pDestPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;

	// ½º·¹µå°¡ È°¼ºÈ­µÈ µ¿¾ÈÀº ÁøÀÔÇÏÁö ¾Ê´Â´Ù
	if( InterlockedCompareExchange(&g_PathThreadActivated, 0, 0) == 1 )
	{
		return FALSE;
	}

	VECTOR3 t_StartPos;
	GetPosition( &t_StartPos );

	/// 0. Å¸°ÙÀÌ ÀÌµ¿ºÒ°¡Áö¿ªÀÏ °æ¿ì
	if (MAP->CollisionCheck_OneTile( pDestPos ) == FALSE)
	{
		// ÀÌµ¿ºÒ°¡Áö¿ª¿¡¼­ÀÇ ÀÌµ¿ ¿©ºÎ ÆÇ´Ü
		if( !bIsCollisionPosMove )
			return FALSE;

		// Á÷¼±»ó ÀÌµ¿°¡´É ÃÖ´ëÁöÁ¡À» ±¸ÇÑ´Ù.
		if( MAP->CollisionLine( &t_StartPos, pDestPos, pDestPos, MAP->GetMapNum(), this ) == FALSE )
			return FALSE;
	}

	/// »çÀü Ã³¸®
	ClearWayPoint();

	if( MOVEMGR->IsMoving( this ) )
		MOVEMGR->CalcPositionEx( this, gCurTime );

	/// 1. ÇÑ¹ø¿¡ Á÷¼± °Å¸®·Î °¥ ¼ö ÀÖ´Ù¸é Á÷¼±À¸·Î ÀÌµ¿
	HTR_S(eRT_OneLineCheck);
	BOOL t_CollisionCheck_Oneline = MAP->CollisionCheck_OneLine_New( &t_StartPos, pDestPos );
	HTR_E(eRT_OneLineCheck);
	if ( t_CollisionCheck_Oneline == TRUE)
	{
		Move_Simple( pDestPos );

		if( bMousePointDisplay )
			MOVEPOINT->SetPoint( pDestPos );
	}
	else
	{
		// Á¤Áö
		if( MOVEMGR->IsMoving( this ) )
			MOVEMGR->HeroMoveStop();

		/// 2. ±æÃ£±â·Î ÀÌµ¿
		// ±æÃ£±â ¾²·¹µå¸¦ »ý¼ºÇÑ´Ù
		// ¾²·¹µå°¡ ¸®ÅÏµÉ ¶§ ¿þÀÌÆ÷ÀÎÆ®°¡ Ãß°¡µÈ´Ù
		// 091204 ShinJS --- 3¹è¼Ó »ç¿ë
		if(FALSE == MAP->PathFind(
			&t_StartPos,
			pDestPos,
			m_WayPointManager,
			FALSE))
		{
			return FALSE;
		}
	}

	m_WayPointManager->SetFlagDestination( *pDestPos );

	return TRUE;
}

BOOL CHero::Move_Simple( VECTOR3* pTargetPos )
{
	CChaptchaDlg* ChaptchaDlg = GAMEIN->GetChaptchaDlg();
	if( ChaptchaDlg->IsActive() )
	{
		MOVEMGR->MoveStop(this,0);
		return TRUE;
	}
	m_MoveInfo.InitTargetPosition();
	m_MoveInfo.SetTargetPosition( 0, *pTargetPos );
	m_MoveInfo.SetMaxTargetPosIdx( 1 );

	MOVE_ONETARGETPOS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_ONETARGET;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = HEROID;
	msg.SetStartPos( &m_MoveInfo.CurPosition );
	msg.SetTargetPos( m_MoveInfo.GetTargetPosition(0) );

	NETWORK->Send(&msg,msg.GetSize());

	MOVEMGR->StartMoveEx( this, NULL, gCurTime );

	MACROMGR->CheckToggleState( this );


	// 091208 ShinJS --- ¼ÒÀ¯ÁÖÀÌ¸é¼­ Å¾½Â ÁßÀÎ °æ¿ì Å»°Íµµ ÀÌµ¿½ÃÅ²´Ù.
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
	if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle )
	{
		if( pVehicle->GetOwnerIndex() == GetID() &&
			IsGetOnVehicle() )
		{
			pVehicle->StartMove( pTargetPos );
		}
	}
	return TRUE;
}

BOOL CHero::NextMove()
{
	VECTOR3 t_WayPoint;
	if ( m_WayPointManager->PopWayPoint(&t_WayPoint) == TRUE )
	{
		if( !Move_Simple( &t_WayPoint ) )
		{
			ClearWayPoint();
			return FALSE;
		}

		return TRUE;
	}

	ClearWayPoint();
	return FALSE;
}

void CHero::SetGetmoneyspinslot(MONEYTYPE moneyspinget, WORD slot1, WORD slot2, WORD slot3, WORD slot4, MONEYTYPE totaljackpot, WORD wincode)
{
	m_lastspinget	= gCurTime + 100;
	m_Moneyspinget	= moneyspinget;
	m_WinSlot1		= slot1;
	m_WinSlot2		= slot2;
	m_WinSlot3		= slot3;
	m_WinSlot4		= slot4;
	m_Moneyspinjackpot = totaljackpot;
	m_Wincode		= wincode;
}

void CHero::StopGetmoneyspinslot()
{
	
	CSlotDialog* pHandler = (CSlotDialog*)GAMEIN->GetSlotDlg();
	if( pHandler->IsActive() )
	{
		pHandler->SetResultSpin(0, 1, 2, 3, 4, m_Moneyspinjackpot, 0);
	}

	m_lastspinget	= 0;
	m_Moneyspinget	= 0;
	m_WinSlot1		= 0;
	m_WinSlot2		= 0;
	m_WinSlot3		= 0;
	m_WinSlot4		= 0;
	m_Wincode		= 0;
	m_bIsSlotwait	= FALSE;
}

void CHero::DoGetmoneyspinslot()
{
	CSlotDialog* pHandler = (CSlotDialog*)GAMEIN->GetSlotDlg();
	if( pHandler->IsActive() )
	{
		pHandler->SetResultSpin(m_Moneyspinget, m_WinSlot1, m_WinSlot2, m_WinSlot3, m_WinSlot4, m_Moneyspinjackpot, m_Wincode);
	}
	m_lastspinget	= 0;
	m_Moneyspinget	= 0;
	m_WinSlot1		= 0;
	m_WinSlot2		= 0;
	m_WinSlot3		= 0;
	m_WinSlot4		= 0;
	m_Wincode		= 0;
	m_bIsSlotwait	= FALSE;
}

float CHero::GetBonusRange() const
{
	const float staticValue = mBuffStatus.Range + mPassiveStatus.Range;
	const float dynamicValue = mRateBuffStatus.Range + mRatePassiveStatus.Range;

	return staticValue * (1.0f + dynamicValue);
}

BOOL CHero::IsNoEquip(eArmorType armorType, eWeaponType weaponType, eWeaponAnimationType weaponAnimationType)
{
	switch(armorType)
	{
	case eArmorType_Robe:
	case eArmorType_Leather:
	case eArmorType_Metal:
		{
			if(GetArmorType(eWearedItem_Dress) != armorType)
			{
				return TRUE;
			}

			break;
		}
	case eArmorType_Shield:
		{
			if(GetArmorType(eWearedItem_Shield) != armorType)
			{
				return TRUE;
			}

			break;
		}
	}

	if(eWeaponType_None != weaponType)
	{
		if(GetWeaponEquipType() != weaponType)
		{
			return TRUE;
		}
	}

	switch( weaponAnimationType )
	{
	case eWeaponAnimationType_None:
		break;
	case eWeaponAnimationType_OneHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_OneHand:
			case eWeaponAnimationType_Dagger:
			case eWeaponAnimationType_TwoBlade:
				break;
			default:
				return TRUE;
			}
		}
		break;

	case eWeaponAnimationType_TwoHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_TwoHand:
			case eWeaponAnimationType_Staff:
			case eWeaponAnimationType_Bow:
			case eWeaponAnimationType_Gun:
			case eWeaponAnimationType_Spear:
				break;
			default:
				return TRUE;
			}
		}
		break;

	default:
		{
			if( GetWeaponAniType() != weaponAnimationType )
				return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CHero::IsEnable(const BUFF_SKILL_INFO& info)
{
	if(IsNoEquip(
		info.mArmorType,
		info.mWeaponType,
		info.mWeaponAnimationType))
	{
		return FALSE;
	}

	float checkValue = 0;
	const BUFF_SKILL_INFO::Condition& condition = info.mCondition;

	switch( condition.mType )
	{
	case BUFF_SKILL_INFO::Condition::TypeLifePercent:
		{
			checkValue = float(GetLifePercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLife:
		{
			checkValue = float(GetLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeMax:
		{
			checkValue = float(GetMaxLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeRecovery:
		{
			checkValue = float(GetLifeRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaPercent:
		{
			checkValue = float(GetManaPercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMana:
		{
			checkValue = float(GetMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaMax:
		{
			checkValue = float(GetMaxMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaRecovery:
		{
			checkValue = float(GetManaRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeStrength:
		{
			checkValue = float(GetStrength());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeIntelligence:
		{
			checkValue = float(GetIntelligence());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeDexterity:
		{
			checkValue = float(GetDexterity());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeWisdom:
		{
			checkValue = float(GetWisdom());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeVitality:
		{
			checkValue = float(GetVitality());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalAttack:
		{
			checkValue = float(GetPhysicAttackMax());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalAttack:
		{
			checkValue = float(GetMagicAttack());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalDefense:
		{
			checkValue = float(GetPhysicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalDefense:
		{
			checkValue = float(GetMagicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalRate:
		{
			checkValue = float(GetCriticalRate());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalDamage:
		{
			checkValue = float(GetCriticalDamage());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeAccuracy:
		{
			checkValue = float(GetAccuracy());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeEvade:
		{
			checkValue = float(GetAvoid());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMoveSpeed:
		{
			checkValue = float(GetMoveSpeed());
			break;
		}
	}

	BOOL isEnable = TRUE;

	switch(condition.mOperation)
	{
	case BUFF_SKILL_INFO::Condition::OperationMore:
		{
			isEnable = (checkValue > condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationMoreEqual:
		{
			isEnable = (checkValue >= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLess:
		{
			isEnable = (checkValue < condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLessEqual:
		{
			isEnable = (checkValue <= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationEqual:
		{
			isEnable = (checkValue == condition.mValue);
			break;
		}
	}

	return isEnable;
}

void CHero::SetObjectBattleState(eObjectBattleState eBattleState)
{
	CPlayer::SetObjectBattleState( eBattleState );

	cBattleGuageDlg* pBattleGuageDlg	=	GAMEIN->GetBattleGuageDlg();

	if( !pBattleGuageDlg )
		return;
	
	if( eBattleState == eObjectBattleState_Battle )
	{
		pBattleGuageDlg->StartBattleSignal();
	}
	else if( eBattleState == eObjectBattleState_Peace )
	{
		pBattleGuageDlg->EndBattleSignal();
	}
}

void CHero::SetFamily(CSHFamily* family)
{
	m_pcsFamily->Set(
		family);
}
//wasd movement
void CHero::Move(int nKeyMoveDir)
{
	// Ä³¸¯ÅÍ°¡ ³ª¾Æ°¥ Àü¹æº¤ÅÍ - Ä«¸Þ¶ó¿Í Ä³¸¯ÅÍ»çÀÌ º¤ÅÍ
	VECTOR3 HerosForwardVector = CAMERA->GetForwardVector();
	VECTOR3 HerosBackwardVector = HerosForwardVector * -1;
	VECTOR3 HerosRightSideVector;
	VECTOR3 HerosLeftSideVector;

	HerosLeftSideVector.x = HerosForwardVector.z * -1;
	HerosLeftSideVector.y = 0;
	HerosLeftSideVector.z = HerosForwardVector.x;
	HerosRightSideVector.x = HerosForwardVector.z;
	HerosRightSideVector.y = 0;
	HerosRightSideVector.z = HerosForwardVector.x * -1;
	VECTOR3 diLeftBack = CAMERA->GetRightVector();
	VECTOR3 diRightWard = diLeftBack * -1;
	VECTOR3 diLeftWard;
	VECTOR3 diRightBack;

	diLeftWard.x = diLeftBack.z;
	diLeftWard.y = 0;
	diLeftWard.z = diLeftBack.x * -1;
	diRightBack.x = diLeftBack.z * -1;
	diRightBack.y = 0;
	diRightBack.z = diLeftBack.x;

	VECTOR3 stPos;
	VECTOR3 tmpPos;
	DWORD TickByFps = GAMERESRCMNGR->m_GameDesc.FramePerSec;
	float tSpeed = float(GetMoveSpeed());
	float mSpeed = 0;//50
	//if(tSpeed>=RUNSPEED && TickByFps>=60)
	//{
	//	mSpeed = tSpeed/6;//5.0f;//6.5
	//}
	//else if(tSpeed>=RUNSPEED && TickByFps<60)
	//{
	//	mSpeed = tSpeed/3;//2.5f;//3.25
	//}
	//Prepare for the Worst Case
	if(m_MoveInfo.MoveMode == eMoveMode_Run)
	{
		if(tSpeed >= RUNSPEED && TickByFps<60)
			mSpeed = tSpeed / 3.0f;
		if(tSpeed >= RUNSPEED && TickByFps>=60)
			mSpeed = tSpeed / 6.0f;
		if(mSpeed < 160)
			mSpeed = 160;
	}
	else if(m_MoveInfo.MoveMode == eMoveMode_Walk)
	{
		if(tSpeed >= WALKSPEED && TickByFps<60)
			mSpeed = tSpeed / 4.0f;
		if(tSpeed >= WALKSPEED && TickByFps>=60)
			mSpeed = tSpeed / 8.0f;
		if(mSpeed < 100)
			mSpeed = 100;
	}

	float mDistance = mSpeed + m_KeyMove_DistOneStep;
	//float mMDistance = mSpeed + m_KeyMoveMount_DistOneStep;

	if(nKeyMoveDir)
	{
		HERO->GetPosition(&stPos);
		//CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
		//normal direction
		if (nKeyMoveDir == MOVE_LEFT)
		{
			tmpPos = stPos + HerosLeftSideVector * mDistance;			
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		if (nKeyMoveDir == MOVE_RIGHT)
		{
			tmpPos = stPos + HerosRightSideVector * mDistance;
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		if (nKeyMoveDir == MOVE_UP)
		{
			tmpPos = stPos + HerosForwardVector * mDistance;			
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		if (nKeyMoveDir == MOVE_DOWN)
		{
			tmpPos = stPos + HerosBackwardVector * mDistance;			
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		//diagonal
		if (nKeyMoveDir == MOVE_UP_LEFT)
		{
			tmpPos = stPos + diLeftWard * mDistance;			//HerosLeftSideVector
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		if (nKeyMoveDir == MOVE_UP_RIGHT)
		{
			tmpPos = stPos + diRightWard * mDistance;			
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		if (nKeyMoveDir == MOVE_DOWN_LEFT)
		{
			tmpPos = stPos + diLeftBack * mDistance;			
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		if (nKeyMoveDir == MOVE_DOWN_RIGHT)
		{
			tmpPos = stPos + diRightBack * mDistance;			
			MOVEMGR->GoTarget(&tmpPos,FALSE);
		}
		DisableAutoAttack();
	}
}
//wasd movement
BOOL CHero::Move_WASDPath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove )
{
	// 50cm Å¸ÀÏÀÇ Áß¾Ó¿¡ ¼­°Ô ÇÑ´Ù.
	//pDestPos->x = (int(pDestPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	//pDestPos->y = 0;
	//pDestPos->z = (int(pDestPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;

	// ½º·¹µå°¡ È°¼ºÈ­µÈ µ¿¾ÈÀº ÁøÀÔÇÏÁö ¾Ê´Â´Ù
	if( InterlockedCompareExchange(&g_PathThreadActivated, 0, 0) == 1 )
	{
		return FALSE;
	}

	VECTOR3 t_StartPos;
	GetPosition( &t_StartPos );

	/// 0. Å¸°ÙÀÌ ÀÌµ¿ºÒ°¡Áö¿ªÀÏ °æ¿ì
	if (MAP->CollisionCheck_OneTile( pDestPos ) == FALSE)
	{
		// ÀÌµ¿ºÒ°¡Áö¿ª¿¡¼­ÀÇ ÀÌµ¿ ¿©ºÎ ÆÇ´Ü
		if( !bIsCollisionPosMove )
			return FALSE;

		// Á÷¼±»ó ÀÌµ¿°¡´É ÃÖ´ëÁöÁ¡À» ±¸ÇÑ´Ù.
		if( MAP->CollisionLine( &t_StartPos, pDestPos, pDestPos, MAP->GetMapNum(), this ) == FALSE )
			return FALSE;
	}

	/// »çÀü Ã³¸®
	ClearWayPoint();

	if( MOVEMGR->IsMoving( this ) )
		MOVEMGR->CalcPositionEx( this, gCurTime );

	/// 1. ÇÑ¹ø¿¡ Á÷¼± °Å¸®·Î °¥ ¼ö ÀÖ´Ù¸é Á÷¼±À¸·Î ÀÌµ¿
	HTR_S(eRT_OneLineCheck);
	BOOL t_CollisionCheck_Oneline = MAP->CollisionCheck_OneLine_New( &t_StartPos, pDestPos );
	HTR_E(eRT_OneLineCheck);
	if ( t_CollisionCheck_Oneline == TRUE)
	{
		Move_Simple( pDestPos );

		if( bMousePointDisplay )
			MOVEPOINT->SetPoint( pDestPos );
	}
	else
	{
		// Á¤Áö
		if( MOVEMGR->IsMoving( this ) )
			MOVEMGR->HeroMoveStop();

		/// 2. ±æÃ£±â·Î ÀÌµ¿
		// ±æÃ£±â ¾²·¹µå¸¦ »ý¼ºÇÑ´Ù
		// ¾²·¹µå°¡ ¸®ÅÏµÉ ¶§ ¿þÀÌÆ÷ÀÎÆ®°¡ Ãß°¡µÈ´Ù
		// 091204 ShinJS --- 3¹è¼Ó »ç¿ë
		if(FALSE == MAP->PathFind(
			&t_StartPos,
			pDestPos,
			m_WayPointManager,
			FALSE))
		{
			return FALSE;
		}
	}
	
	//m_WayPointManager->SetFlagDestination( *pDestPos );

	return TRUE;
}
void CHero::DisableWASD(int val)
{
	if(val != 0)
	{
		m_DisableWASD = TRUE;
	}
	else
	{
		m_DisableWASD = FALSE;
	}
}
//wasd 2
void CHero::Move_KeyboardInput2()
{
	// ---- wasd custom
	//if( m_DisableWASD ) { return; } //useless
	if( !MACROMGR->get_wasd_opt() ) { return; }
	// ----
	int nKeyMoveDir = MACROMGR->GetKeyMoveDir();
	CChaptchaDlg* ChaptchaDlg = GAMEIN->GetChaptchaDlg();
	if( MACROMGR->IsKeyChanged() )
	{
		MACROMGR->ClearKeyChanged();

		// ÇÁ·Î±×·¹½º Ã¢ÀÌ ¶° ÀÖÀ¸¸é Ã¢À» Ãë¼ÒÇÑ´Ù.
		{
			CProgressDialog* dialog = (CProgressDialog*)WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
			if( dialog->IsActive() )
				dialog->Cancel();
		}	

		if( nKeyMoveDir != KEY_MOVE_STOP && !ChaptchaDlg->IsActive())
		{
			Move(nKeyMoveDir);
		}
		else
		{
			MOVEMGR->MoveStop(this,0);
			Stop_SendtoServer();			
		}
	}
	else if( CMHTimeManager::GetInstance()->CheckTimer500() )
	{
		if( nKeyMoveDir != KEY_MOVE_STOP && !ChaptchaDlg->IsActive())
		{
			Move(nKeyMoveDir);
		}
	}
}

void CHero::Stop_SendtoServer()
{
	if ( GetState() == eObjectState_Die ) return;

	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_STOP;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = HEROID;
	msg.cpos.Compress( &GetPosition() );

	NETWORK->Send(&msg,sizeof(MOVE_POS));
}

void CHero::DeleteItemCustom(POSTYPE position, DWORD itemIdx, int durability)
{
	MSG_ITEM_DISCARD_SYN msg;
	msg.Category = MP_ITEM;
	msg.Protocol = MP_ITEM_DISCARD_SYN;
	msg.dwObjectID = gHeroID;
	msg.TargetPos = position;
	msg.wItemIdx = itemIdx;
	msg.ItemNum = durability;

	NETWORK->Send(&msg,sizeof(msg));
}

//Alemuri Mobstats calc------------------------------------------------------------------------------------------------------
float CHero::DoGetEvasionRateFloat()
{
	BYTE byJobOrder			= m_CharacterInfo.Job[0] ;
	DWORD wLevel			= GetLevel() ;

	// 080313 LUJ, ÆÛ¼¾Æ® ´É·ÂÄ¡ Ãß°¡
	// 080408 LUJ, ÆÛ¼¾Æ® ´É·ÂÀÌ ¿Ã¹Ù·Î °è»êµÇÁö ¾Ê´Â ¹®Á¦ ¼öÁ¤
	// 080701 LYW, ¹öÇÁ·Î ´Ã¾î³ª´Â ½ºÅÈ ¼öÄ¡µµ Ãß°¡ÇÑ´Ù.
	//float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemStat.mDexterity.mPlus ) * ( 1.0f + mItemStat.mDexterity.mPercent );
	float wDexterityAll	= ( mCharStat.mDexterity.mPlus + mItemBaseStat.mDexterity.mPlus + GetBuffStatus()->Dex) * ( 1.0f + mItemBaseStat.mDexterity.mPercent );
	
	// ¼¼Æ® ´É·Â Àû¿ëÇÑ ¼öÄ¡¸¦ ¾ò¾î³½´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mDexterity;

		wDexterityAll =	
			DWORD( max( 0, ( wDexterityAll + value.mPlus ) * ( 1.0f + value.mPercent ) ) )  +
			GetPassiveStatus()->Dex+
			GetBuffStatus()->Dex;
	}
	wDexterityAll *= 0.2f;

	DWORD wClassOrderVal	= 0 ;
	//float wPassiveSkill		= GetPassiveStatus()->Avoid ;
	float	wPassiveSkill	= GetRatePassiveStatus()->Avoid
								+ GetRateBuffStatus()->Avoid
								+ (GetItemOptionStats().mEvade.mPercent * 100.f)
								+ (GetSetItemStats().mEvade.mPercent * 100.f)

								+ GetPassiveStatus()->Avoid
								//+ GetBuffStatus()->Avoid
								+ GetItemOptionStats().mEvade.mPlus;
								//+ GetSetItemStats().mEvade.mPlus;
	//------------------------------------------------------------------------------------------------

	float wBuffSkillVal		= GetBuffStatus()->Avoid ;

	switch( byJobOrder )
	{
	case 1 : wClassOrderVal = 20 ; break ;
	case 2 : wClassOrderVal = 25 ; break ;
	case 3 : wClassOrderVal = 15 ; break ;
	}

	float fValA				= ((float)wDexterityAll / 2) ;
	float fValB				= (float)(fValA + wLevel + wClassOrderVal);

	float wEvasion			= GetFloatRound(fValB) ;

	float dwTotalEvasion	= wEvasion + wPassiveSkill + wBuffSkillVal;

	// 071002 ¿õÁÖ, ¼¼Æ® ¾ÆÀÌÅÛÀ¸·Î ºÎ¿©µÈ ´É·ÂÄ¡¸¦ Ãß°¡ÇÑ´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mEvade;

		dwTotalEvasion = max( 0, ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	// 080313 LUJ, ¾ÆÀÌÅÛ ½ºÅÈÀ» ÀÏ¹Ý°ú ¼¼Æ®·Î ºÐ¸®
	{
		const PlayerStat::Value& value = mItemBaseStat.mEvade;

		dwTotalEvasion = ( dwTotalEvasion + value.mPlus ) * ( 1.0f + value.mPercent );
	}

	return dwTotalEvasion;
}

DWORD CHero::DoGetDefenseRateMobStats()
{
	float wDefenseAll		= 0;

	// 080313 LUJ, ¾ÆÀÌÅÛ ½ºÅÈÀ» ÀÏ¹Ý°ú ¼¼Æ®·Î ºÐ¸®
	{
		const PlayerStat::Value& value = GetItemBaseStats().mPhysicDefense;

		wDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	float wEnchantAll		= 1 ;
	float wLevel				= GetLevel() ;
	//float wPassiveSkill		= GetPassiveStatus()->PhysicDefense;
		//wPassiveSkill	= passiveStatus->PhysicAttack;
	//--------------------------------------------------------------------------------------------
	float wPassiveSkill		= GetRatePassiveStatus()->PhysicDefense
								+ GetRateBuffStatus()->PhysicDefense
								+ (GetItemOptionStats().mPhysicDefense.mPercent * 100.f)
								+ (GetSetItemStats().mPhysicDefense.mPercent * 100.f)

								+ GetPassiveStatus()->PhysicDefense
								//+ GetBuffStatus()->PhysicDefense
								+ GetItemOptionStats().mPhysicDefense.mPlus;
								//+ GetSetItemStats().mPhysicDefense.mPlus;
	//--------------------------------------------------------------------------------------------

	DWORD Armor = GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	/// ·Î±× °è¿­
	if( m_CharacterInfo.Job[0] == 2 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 1 )
				wDefenseAll = 0.f;
		}
	}
	/// ¸ÞÀÌÁö °è¿­
	else if( m_CharacterInfo.Job[0] == 3 )
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 1 )
				wDefenseAll = 0.f;
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( GetPassiveStatus()->LightArmor < 1 )
				wDefenseAll = 0.f;
		}
	}	


	float wDefenseA			= ( wDefenseAll / 2 ) + wEnchantAll + wLevel + wPassiveSkill ;
	float wVitalityAll		= float( GetVitality() );

	float wMasteryLevel		= 0 ;
	wVitalityAll *= 0.2f;

	if( pArmor && pBoots && pGlove )
	if( ( pArmor->ArmorType == pBoots->ArmorType ) && ( pArmor->ArmorType == pGlove->ArmorType ) )
	{
		switch( pArmor->ArmorType )
		{
		case eArmorType_Robe:		wMasteryLevel = GetPassiveStatus()->Robe;			break;
		case eArmorType_Leather:	wMasteryLevel = GetPassiveStatus()->LightArmor;	break;
		case eArmorType_Metal:		wMasteryLevel = GetPassiveStatus()->HeavyArmor;	break;
		}
	}

	float wDefenseBuffVal	= GetBuffStatus()->PhysicDefense;

	//float wDefenseB			= 500 + (((wVitalityAll - 20) * 5) + (wMasteryLevel * 30) + (wDefenseBuffVal * 10)) ;
	float wDefenseB				= 1000 + (((wVitalityAll -10) * 20) + (wMasteryLevel * 25) + (wDefenseBuffVal * 10)) ;

	float dwTotalDefense	= ((wDefenseA * wDefenseB) / 1000)  ; //1000 330

	// 070705 ¿õÁÖ, ¼¼Æ® ¾ÆÀÌÅÛÀ¸·Î ºÎ¿©µÈ ´É·ÂÄ¡¸¦ Ãß°¡ÇÑ´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mPhysicDefense;

		dwTotalDefense = max( 0, ( dwTotalDefense + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}
	dwTotalDefense *= 0.8f;
	return dwTotalDefense;	
}

DWORD CHero::DoGetMagDefenseRateMobStats()
{
	float wMagDefenseAll		 = 0;
	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicDefense;
		wMagDefenseAll		 = value.mPlus * (1.0f + value.mPercent);
	}

	float wEnchantAll		= 1 ;
	float wLevel			= GetLevel() ;
	//float wPassiveSkill		= pPlayer->GetPassiveStatus()->MagicDefense;
	//--------------------------------------------------------------------------------------------
	float	wPassiveSkill	= GetRatePassiveStatus()->MagicDefense
								+ GetRateBuffStatus()->MagicDefense
								+ (GetItemOptionStats().mMagicDefense.mPercent * 100.f)
								+ (GetSetItemStats().mMagicDefense.mPercent * 100.f)

								+ GetPassiveStatus()->MagicDefense
								//+ GetBuffStatus()->MagicDefense
								+ GetItemOptionStats().mMagicDefense.mPlus;
								//+ GetSetItemStats().mMagicDefense.mPlus;
	//--------------------------------------------------------------------------------------------

	float wDefenseA			= ( wMagDefenseAll / 5 ) + wEnchantAll + wLevel + wPassiveSkill ;
	float wWisdomAll		= float( GetWisdom() );

	float wDefenseBuffVal	= GetBuffStatus()->MagicDefense;
	wWisdomAll *= 0.2f;

	float wDefenseB			= 500 + (((wWisdomAll - 20) * 5) + (wDefenseBuffVal * 10)) ;

	float dwTotalDefense	= ((wDefenseA * wDefenseB) / 200)  ;

	// 070705 ¿õÁÖ, ¼¼Æ® ¾ÆÀÌÅÛÀ¸·Î ºÎ¿©µÈ ´É·ÂÄ¡¸¦ Ãß°¡ÇÑ´Ù
	{
		const PlayerStat::Value& value = GetSetItemStats().mMagicDefense;

		dwTotalDefense = max( 0, ( dwTotalDefense + value.mPlus ) * ( 1.0f + value.mPercent ) );
	}

	return dwTotalDefense;
}

DWORD CHero::DoGetShieldDefense()
{
	const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(GetWearedItemIdx(eWearedItem_Shield));

	if (!itemInfo)
		return 0;

	const ITEMBASE* const itemBase = ITEMMGR->GetItemInfoAbsIn(HERO, TP_WEAR_START + eWearedItem_Shield);
	
	if (!itemBase)
		return 0;

	float defenseValue = float(itemInfo->PhysicDefense);
	const ITEM_OPTION& itemOption = ITEMMGR->GetOption(
		*itemBase);
	
	if(const EnchantScript* const script = g_CGameResourceManager.GetEnchantScript(itemOption.mEnchant.mIndex))
	{
		const stEnchantLvBonusRate& pEnchantLvBonusRate = g_CGameResourceManager.GetEnchantLvBonusInfo(
			itemOption.mEnchant.mLevel);

		for(EnchantScript::Ability::const_iterator it = script->mAbility.begin();
			script->mAbility.end() != it;
			++it )
		{
			switch( EnchantScript::eType( *it ) )
			{
			case ReinforceScript::eTypePhysicDefence:
				{
					defenseValue += ( itemOption.mEnchant.mLevel * itemInfo->EnchantDeterm );
					defenseValue += GetBonusEnchantValue( *itemInfo, itemOption, (DWORD)itemInfo->PhysicDefense, pEnchantLvBonusRate );
					break;
				}
			}
		}
	}
	return ( DWORD( defenseValue ) );
}
//--------------------------------------------------------------------------------------------------------------------------------------------

