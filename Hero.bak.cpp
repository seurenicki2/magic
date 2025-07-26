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

	// 080313 LUJ, ��Ʈ ������ �ɷ� �ʱ�ȭ
	ZeroMemory( &mSetItemStat, sizeof( PlayerStat ) );

	m_dwLastSocietyAction = gCurTime;

	GAMEIN->GetQuickDlg()->SetActiveSheet( 0, 0 );

	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	// desc_hseos_���͹���01
	// S ���͹��� �߰� added by hseos 2007.05.23 2008.01.11:CPlayer ���� �ű�
	ZeroMemory(&m_stMonstermeterInfo, sizeof(m_stMonstermeterInfo));
	mCurrentSkillObjectIndex = 0;


//---KES ��� ���� ���� ���� 071128
	m_bNoDeffenceFromArmor = FALSE;
	mSkillDelay = 0;
	mSkillCancelDelay = 0;
	m_dwFollowPlayerIdx = 0;
//---------------

	// 090526 ShinJS --- �̸����� ��Ƽ�ʴ�� �۾��ð� �ʱ�ȭ
	m_dwInvitePartyByNameLastTime = 0;
	m_dwInvitePartyByNameLimitTime = 0;


	// 100226 ShinJS --- ��æƮ ���� ���� �߰�
	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;
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
			// 091026 ShinJS --- �׼� ����� ��ã������ �ʱ�ȭ
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
			if(GetCurComboNum() < MAX_COMBO_NUM || GetCurComboNum() == SKILL_COMBO_NUM)	// �ڵ������� �޺� 2������	12/3�� ȸ�� ��� 3���� 2�� �ٲ�
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

	// 090204 LUJ, Ư�� ��ų�� �ϳ��� �˻��Ͽ� Ȱ��/��Ȱ����Ų��
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
		if(gCurTime - GetStateStartTime() > 10000)	//���ڸ� ���̱⿣ �����κ��� �ɸ���.
		{
			OBJECTSTATEMGR->EndObjectState(this,eObjectState_SkillUsing);
		}
	}

	ProcMonstermeterPlayTime();
	OBJECTMGR->ApplyShadowOption(
		this);

	// 090909 ShinJS --- ��ã�� ����� ���� ó��
	switch( m_WayPointManager->GetWMStatus() )
	{
	// �������� ��
	case eWMStatus_Success:
		{
			// �� ��������Ʈ�� ��ȿȭ�ϰ�, ���ο� �����͸� �־�� �Ѵ�
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);

			// ���� ��������Ʈ�� �̵�
			NextMove();

			// 091204 --- ��ǥ ��ǥ ����(�����Ͽ� ��ã�⸦ �Ѱ�� Stop�޼����� ������ ��ǥ��ǥ�� ��������. �ٽ� �����Ҽ� �ֵ��� �Ѵ�.)
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
	// ������ ���
	case eWMStatus_Fail:
		{
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);
			ClearWayPoint();

			// 091208 ShinJS --- ��ã�� ���н� ������ �̵� �ִ��������� �̵��Ѵ�.
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			VECTOR3 startPos;
			GetPosition( &startPos );

			// ������ �̵����� �ִ������� ���Ѵ�.
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
}

// desc_hseos_���͹���01
// S ���͹��� �߰� added by hseos 2007.05.23 2008.01.11:CPlayer ���� �ű�
void CHero::ProcMonstermeterPlayTime()
{
	if (gCurTime > m_stMonstermeterInfo.nPlayTimeTick)
	{
		m_stMonstermeterInfo.nPlayTimeTick = gCurTime + 1000;
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
// E ���͹��� �߰� added by hseos 2007.05.23 2008.01.11:CPlayer ���� �ű�

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
	// 080616 LUJ, �ִ� HP�� ���� �ʰ� ǥ�õǵ��� �Ѵ�
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

//---KES divide 0 ����
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

	float value = (float)GetExpPoint()/(float)m_maxExpPoint;

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( value, 0 );
	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent( value*100 );

	// �������̽� : ��ġ ǥ��
	char szValue[50];
	// 070124 LYW --- Modified this line.
	//sprintf( szValue, "%d / %d", GetExpPoint(), dwPoint );
	sprintf( szValue, "%d/%d", GetExpPoint(), dwPoint );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if( ps )	ps->SetStaticText( szValue );
}

void CHero::SetExpPoint(EXPTYPE dwPoint, BYTE type)
{
	m_HeroTotalInfo.ExpPoint = dwPoint;

	// �������̽� ������ ����
	float value = (float)dwPoint/(float)m_maxExpPoint;
	// 080530 LYW --- Hero : Modified a data type DWORD to EXPTYPE.
	DWORDEX valueTime = (type == 0)? 0 : (2000/m_maxExpPoint)*dwPoint;
	//DWORD valueTime = 0 ;

	// 080604 LYW --- Hero : ����ġ ������ ó�� ����.
	// ����ġ �������� time�� ���� �ʴ°� �´´ٰ� �Ǵ�.
	// type�� 0���� �����Ѵ�.
	type = 1 ;

	// 080626 LYW --- Hero : �ִ� ���� ���� ��ġ�� ���� ���� ���� Define ������ ������.
	//if(GetLevel() == 99 || type == 0 )
	if(GetLevel() == MAX_CHARACTER_LEVEL_NUM || type == 0 )
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( 1, 0 );
	else
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( value, (DWORD)valueTime );

	// �������̽� : ĳ���� â
	CObjectGuagen* ps = (CObjectGuagen *)GAMEIN->GetCharacterDialog()->GetWindowForID(CI_EXPPOINTGUAGE);
	if( ps )
	{
		if(type == 0)
			ps->SetValue( value, 0 );
		else
			ps->SetValue( value, (DWORD)valueTime );
	}	

	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent(value*100);
	// �������̽� : ��ġ ǥ��

	// 080603 LYW --- Hero : ����ġ ó�� (__int32)���� (__int64)�� ���� ó��.
	//char szValue[50];
	//if(GetLevel() >= 99)
	//	sprintf( szValue, "[ %s ]", CHATMGR->GetChatMsg(179) );	//"[ �� ]"
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
	sprintf( szValue, "%.2f%%", value*100 );

#ifdef _CHEATENABLE_	
	if(CHEATMGR->IsCheatEnable())
	{
		char buf1[66];
		EXPTYPE exp = GetMaxExpPoint() ;
 		sprintf(buf1, " %0.f/%0.f", (float)dwPoint, (float)exp);
		strcat(szValue, buf1);
	}
#endif

	cStatic* ps2 = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if( ps2 )	ps2->SetStaticText( szValue );
}

void CHero::SetLevel(LEVELTYPE level)
{
	// �������̽� â ǥ�� ��ġ ����
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

	// �������̽� ������ ����
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

// 100223 ShinJS --- ���� �������� ���Ͽ� ���������� ��ȭ������ �޾� ����
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

	// �������̽� : ��ġ ǥ��
	char szValue[50];
	sprintf( szValue, "%d/%d", val, GetMaxMana() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );

	// ��ġ����
	m_HeroTotalInfo.Mana = val;
}
void CHero::SetStrength(DWORD val)
{
	m_HeroTotalInfo.Str = val;

	//SW060906 ���۾����� ����
	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// �������̽� â ǥ�� ��ġ ����
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


	// �������̽� â ǥ�� ��ġ ����
	GAMEIN->GetCharacterDialog()->SetDexterity();
	// 070411 LYW --- Hero : Modified function SetDexterity.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->PrintEvasionRate();
	GAMEIN->GetCharacterDialog()->PrintCriticalRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetVitality(DWORD val)
{
	m_HeroTotalInfo.Vit = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// �������̽� â ǥ�� ��ġ ����
	GAMEIN->GetCharacterDialog()->SetVitality();
	// 070411 LYW --- Hero : Modified function SetVitality.
	//GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->PrintDefenseRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetWisdom(DWORD val)
{
	m_HeroTotalInfo.Wis = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// �������̽� â ǥ�� ��ġ ����
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

	// �������̽� â ǥ�� ��ġ ����
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

BOOL CHero::StartSocietyAct( int nStartMotion, int nIngMotion, int nEndMotion, BOOL bHideWeapon )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Immortal )
	{
		if( GetState() == eObjectState_Society )
		{
			if( m_ObjectState.State_End_Motion != -1 )	//�ɱ� ������ ���
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
//			if(SKILLMGR->GetSkillObject(TEMP_SKILLOBJECT_ID) != NULL)	// �������� ������ �ȿ°��
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
// �ൿ �Լ���..
void CHero::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
	//	ASSERT(GetLife() == 0);

	WINDOWMGR->CloseAllMsgBox();

	GAMEIN->GetDealDialog()->SetDisable(FALSE);
	GAMEIN->GetDealDialog()->SetActive(FALSE);	// ����â �ݱ�

	GAMEIN->GetNpcScriptDialog()->SetDisable(FALSE);	//NPCâ�� �ݱ�
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);

	// ���α׷��� â �ݱ�
	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );

		if( dialog->IsActive() )
		{
			dialog->Cancel();
		}
	}

	SetMovingAction(NULL);
	DisableAutoAttack();					// ������� ����
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
	// 090907 ShinJS --- ���� ��� WayPoint�� �����Ѵ�
	ClearWayPoint();
}

// 080910 LUJ, ������ ���� ó���� RESULTINFO ����ü�� �޾� ó���ϵ��� ��
void CHero::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	//////////////////////////////////////////////////////////////////////////
	// ī�޶� �ڷ� �и�
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

		// �̵� �߿� ���ݹ��� ������ �Ĵٺ��� ���ƾ��Ѵ�. �׷��� ������ ����Ŀ�� �߻��Ѵ�
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
	// ���ΰ��� ������ �Դ� ����Ÿ�ٰ� ������ ���ݾ� ����Ʈ��	
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
		// desc_hseos_ASSERT����_01
		// S ASSERT���� �߰� added by hseos 2007.06.02
		// ..�Ǽ����� ������ ���� ������ �ƴϹǷ� �߰�
	case eWeaponType_None	:								break;
		// E ASSERT���� �߰� added by hseos 2007.06.02
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

	CAMERA->SetCharState( eCS_Normal, 0 );	//�Ͼ�� ������ ����.

	CPlayer::Revive(pRevivePos);
}

DWORD CHero::DoGetAttackRate() 
{
	// ���� ���ݷ�
	float	fWeaponAttack	= 0;
	{
		const PlayerStat::Value& value = GetItemBaseStats().mPhysicAttack;
		fWeaponAttack += value.mPlus * ( 1.0f + value.mPercent );
	}

	// ĳ���� ����
	WORD	wLevel			= GetLevel() ;
	float	AEnchant		= float( GetWeaponEnchantLevel() );

	// ��
	float	fStrengthAll	= float( GetStrength() );

	// ���� ��
	float	fRealStrengthAll	= float( m_HeroTotalInfo.Str );
	float	fPercent		= GetRatePassiveStatus()->PhysicAttack
							+ GetRateBuffStatus()->PhysicAttack
							+ (GetItemOptionStats().mPhysicAttack.mPercent * 100.f)
							+ (GetSetItemStats().mPhysicAttack.mPercent * 100.f);
	
	// +�ɼ�
	float	fPlus			= GetPassiveStatus()->PhysicAttack
							+ GetBuffStatus()->PhysicAttack
							+ GetItemOptionStats().mPhysicAttack.mPlus
							+ GetSetItemStats().mPhysicAttack.mPlus;

	// ���� ���ݷ�			= ( ( ���� ���ݷ� + ���� ) * ( 1 + ( �� * 0.001 ) ) + ( ������ - 30 ) ) * ( 1 + ( ���� ��æƮ ���� ^ 2 / 400 ) )
	float	fMaxAttack		= ( ( ( fWeaponAttack + wLevel ) * ( 1 + ( fStrengthAll * 0.001f ) ) + ( fRealStrengthAll - 30 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( max( 0, Round( fMaxAttack/1.5f, 1 ) ) );
}

DWORD CHero::DoGetDefenseRate() 
{
	// �� ����
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

	//---KES ��� ���� ���� ���� 071128
	m_bNoDeffenceFromArmor = FALSE;
	//-----------------------------------

	if( m_CharacterInfo.Job[0] == 2 )	//�α�
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES ��� ���� ���� ���� 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
	}
	else if( m_CharacterInfo.Job[0] == 3 )	//������
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES ��� ���� ���� ���� 071128
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
				//---KES ��� ���� ���� ���� 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
	}

	// ĳ���� ����
	WORD	wLevel				= GetLevel() ;
	float	DEnchant			= float( GetPhysicDefenseEnchantLevel() );
	float	fRealVitalityAll	= float( m_HeroTotalInfo.Vit );
	// �Ѱǰ�
	float	fVitality			=	(float)GetVitality();

	// %�ɼ�
	float	fPercent		= GetRatePassiveStatus()->PhysicDefense
							+ GetRateBuffStatus()->PhysicDefense
							+ (GetItemOptionStats().mPhysicDefense.mPercent * 100.f)
							+ (GetSetItemStats().mPhysicDefense.mPercent * 100.f)
							+ GetRatePassiveStatus()->Shield
							+ GetRateBuffStatus()->Shield;
	// +�ɼ�
	float	fPlus			= GetPassiveStatus()->PhysicDefense
							+ GetBuffStatus()->PhysicDefense
							+ GetItemOptionStats().mPhysicDefense.mPlus
							+ GetSetItemStats().mPhysicDefense.mPlus
							+ GetPassiveStatus()->Shield
							+ GetBuffStatus()->Shield;

	// ���� ����			= ( ( ��� ���� + ���� )   * ( 1 + ( �ǰ� / 3000 ) )         + ( (�����ǰ� - 40) / 5 ) ) * ( 1 + ( �� ��æƮ ���� ���� / 200 ) )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1.0f + ( fVitality / 3000 ) ) + ( ( fRealVitalityAll - 40 ) / 5 ) ) * ( 1 + DEnchant / 200 ) ) 
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( max( 0, Round( fTotalDefense, 1 ) ) );
}

DWORD CHero::DoGetMagAttackRate() 
{
	// ���� ���ݷ�
	float	fMagicAttack		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicAttack;

		fMagicAttack = value.mPlus * ( 1.0f + value.mPercent );
	}

	// ĳ���� ����
	WORD	wLevel			= GetLevel() ;
	float	AEnchant		= float( GetWeaponEnchantLevel() );

	// ����
	float	fIntelligenceAll	= float( GetIntelligence() );

	// ���� ����
	float	fRealIntelligenceAll	= float( m_HeroTotalInfo.Int );
	float	fPercent		= GetRatePassiveStatus()->MagicAttack
							+ GetRateBuffStatus()->MagicAttack
							+ (GetItemOptionStats().mMagicAttack.mPercent * 100.f)
							+ (GetSetItemStats().mMagicAttack.mPercent * 100.f);
	
	// +�ɼ�
	float	fPlus			= GetPassiveStatus()->MagicAttack
							+ GetBuffStatus()->MagicAttack
							+ GetItemOptionStats().mMagicAttack.mPlus
							+ GetSetItemStats().mMagicAttack.mPlus;

	// ���� ���ݷ�			= ( ( ���� ���ݷ� + ���� ) * ( 1 + (���� * 0.001) ) + ( �������� - 40 ) ) * ( 1 + ( ���� ��æƮ ���� ^ 2 / 400 ) )
	float	fMaxAttack		= ( ( ( fMagicAttack + wLevel ) * ( 1 + ( fIntelligenceAll * 0.001f ) ) + ( fRealIntelligenceAll - 40 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( max( 0, Round( fMaxAttack/2, 1 ) ) );
}

DWORD CHero::DoGetMagDefenseRate() 
{
	// �� ����
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	// ĳ���� ����
	WORD	wLevel			= GetLevel() ;
	float	DEnchant		= float( GetMagicDefenseEnchantLevel() );

	// ����
	float	fWisdomAll		= float( GetWisdom() );
	float	fRealWisdomAll	= float( m_HeroTotalInfo.Wis);

	// %�ɼ�
	float	fPercent		= GetRatePassiveStatus()->MagicDefense
							+ GetRateBuffStatus()->MagicDefense
							+ (GetItemOptionStats().mMagicDefense.mPercent * 100.f) 
							+ (GetSetItemStats().mMagicDefense.mPercent * 100.f);
	
	// +�ɼ�
	float	fPlus			= GetPassiveStatus()->MagicDefense
							+ GetBuffStatus()->MagicDefense
							+ GetItemOptionStats().mMagicDefense.mPlus
							+ GetSetItemStats().mMagicDefense.mPlus;

	// ���� ����			= ( ( ��� ���� + ���� ) * ( 1 + ( ���� / 3000 ) ) + ( (�������� - 30) / 5 ) ) * ( 1 + ( �� ��æƮ ���� ���� / 200 )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1 + ( fWisdomAll / 3000 ) ) + ( fRealWisdomAll / 5 ) ) * ( 1 + ( DEnchant / 200 ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;
	
	return DWORD( max( 0, Round( fTotalDefense, 1 ) ) );
}

DWORD CHero::DoGetAccuracyRate() 
{
	// ����
	DWORD wLevel		= GetLevel() ;

	// �⺻ ����
	RaceType race			= (RaceType)GetRace();
	BYTE byClass			=  m_CharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// ��ø
	float fDexterityAll	= float( GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// �������
	float fClassOrderVal = 0 ;

	switch( m_CharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 14.f ; break ;
	case 2 : fClassOrderVal = 18.f ; break ;
	case 3 : fClassOrderVal = 11.f ; break ;
	case 4 : fClassOrderVal = 15.f ; break ;
	}

	// %�ɼ�
	float	fPercent		= GetRatePassiveStatus()->Accuracy
							+ GetRateBuffStatus()->Accuracy
							+ (GetItemOptionStats().mAccuracy.mPercent * 100.f)
							+ (GetSetItemStats().mAccuracy.mPercent * 100.f);
	
	// +�ɼ�
	float	fPlus			= GetPassiveStatus()->Accuracy
							+ GetBuffStatus()->Accuracy
							+ GetItemOptionStats().mAccuracy.mPlus
							+ GetSetItemStats().mAccuracy.mPlus;

	// ����					= ( ( ( ��ø - ���� ��ø - 5 ) / ( ����* 5 ) ) * 0.6f + (  ( ���� ��ø - 30 ) / 1000 ) + ( ���� * ���߻�� / 15000 ) * 100 )
	float	fTotalAccuracy	= ( ( fDexterityRate * 0.6f + ( ( fBaseDexterity - 30.f ) / 1000.f ) + ( wLevel * fClassOrderVal / 15000.f ) ) * 100.f )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( floor( 0.5f + fTotalAccuracy ) );
}

DWORD CHero::DoGetEvasionRate() 
{
	// ����
	DWORD wLevel		= GetLevel() ;

	// �⺻ ����
	RaceType race			= (RaceType)GetRace();
	BYTE byClass			=  m_CharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// ��ø
	float fDexterityAll	= float( GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// �������
	float fClassOrderVal = 0 ;

	switch( m_CharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 14.f ; break ;
	case 2 : fClassOrderVal = 18.f ; break ;
	case 3 : fClassOrderVal = 11.f ; break ;
	case 4 : fClassOrderVal = 15.f ; break ;
	}

	// %�ɼ�
	float	fPercent		= GetRatePassiveStatus()->Avoid
							+ GetRateBuffStatus()->Avoid
							+ (GetItemOptionStats().mEvade.mPercent * 100.f)
							+ (GetSetItemStats().mEvade.mPercent * 100.f);
	
	// +�ɼ�
	float	fPlus			= GetPassiveStatus()->Avoid
							+ GetBuffStatus()->Avoid
							+ GetItemOptionStats().mEvade.mPlus
							+ GetSetItemStats().mEvade.mPlus;

	// ȸ��					= ( ( ( ��ø - ���� ��ø ) / (  ( ���� - 1 ) * 5 ) ) * 0.6f + (  ( ���� ��ø - 30 ) / 1000 ) + ( ���� * ���߻�� / 15000 ) * 100 )
	float	fTotalEvasion	= ( ( fDexterityRate * 0.6f + ( ( fBaseDexterity - 30.f ) / 1000.f ) + ( wLevel * fClassOrderVal / 15000.f ) ) * 100.f )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( floor( 0.5f + fTotalEvasion ) );
}

DWORD CHero::DoGetCriticalRate() 
{
	// ����
	DWORD wLevel		= GetLevel() ;

	// �⺻ ����
	RaceType race			= (RaceType)GetRace();
	BYTE byClass			=  m_CharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );

	// ��ø
	float fDexterityAll	= float( GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// �������
	float fClassOrderVal = 0 ;

	switch( m_CharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 0.14f ; break ;
	case 2 : fClassOrderVal = 0.18f ; break ;
	case 3 : fClassOrderVal = 0.11f ; break ;
	case 4 : fClassOrderVal = 0.15f ; break ;
	}

	// %�ɼ�
	float	fPlus		= GetPassiveStatus()->CriticalRate
						+ GetBuffStatus()->CriticalRate
						+ GetItemOptionStats().mCriticalRate.mPlus
						+ GetSetItemStats().mCriticalRate.mPlus;

	float	fPercent	= GetRatePassiveStatus()->CriticalRate
						+ GetRateBuffStatus()->CriticalRate
						+ (GetItemOptionStats().mCriticalRate.mPercent * 100.f)
						+ (GetSetItemStats().mCriticalRate.mPercent * 100.f);
	
	DWORD dwCritical	= DWORD( ( 45.f * fDexterityRate ) + ( ( fBaseDexterity - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f ) );
	
	return dwCritical;
}

DWORD CHero::DoGetMagicCriticalRate()	
{
	// ����
	DWORD wLevel		= GetLevel() ;

	// �⺻ ����
	RaceType race				= (RaceType)GetRace();
	BYTE byClass				=  m_CharacterInfo.Job[0];
	const PlayerStat stat		= GAMERESRCMNGR->GetBaseStatus( race, byClass );

	// ����
	float	fIntelligenceAll	= float( GetIntelligence() );
	float fBaseIntelligence		= stat.mIntelligence.mPlus;
	float fIntelligenceRate		= ( wLevel > 1 ? max(fIntelligenceAll - fBaseIntelligence, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// ����
	float	fWisdomAll			= float( GetWisdom() );
	float fBaseWisdomAll		= stat.mWisdom.mPlus;
	float fWisdomAllRate		= ( wLevel > 1 ? max(fWisdomAll - fBaseWisdomAll, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	float	fPlus				= GetPassiveStatus()->MagicCriticalRate
								+ GetBuffStatus()->MagicCriticalRate
								+ GetItemOptionStats().mMagicCriticalRate.mPlus
								+ GetSetItemStats().mMagicCriticalRate.mPlus;

	// %�ɼ�
	float	fPercent			= GetRatePassiveStatus()->MagicCriticalRate
								+ GetRateBuffStatus()->MagicCriticalRate
								+ (GetItemOptionStats().mMagicCriticalRate.mPercent * 100.f)
								+ (GetSetItemStats().mMagicCriticalRate.mPercent * 100.f);
	
	DWORD	dwCritical			= DWORD( ( 10.f * fIntelligenceRate ) + ( 20.f * fWisdomAllRate ) + ( ( fBaseIntelligence - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f ) );
	
	return dwCritical;
}

DWORD CHero::DoGetMoveSpeedRate()	
{
	// 091229 ONS ĳ���� ����â�� ��µǴ� �̵��ӵ� ǥ�ù�� ����
	float	fMoveSpeed = DoGetMoveSpeed() / 4.5f - 100.0f;
	
	return max( 0, DWORD( floor( 0.5f + fMoveSpeed ) ) );
}

DWORD CHero::DoGetCastingSpeedRate()
{
	float	fPercent			= GetRatePassiveStatus()->MagicSkillSpeedRate
								+ GetRateBuffStatus()->MagicSkillSpeedRate;

	// 100209 ONS ĳ���� �ӵ� ǥ�ù�� ����.
	return DWORD( floor( 0.5f + fPercent ) );
}

DWORD CHero::DoGetAttackSpeedRate()		
{
	float	fPercent			= GetRatePassiveStatus()->PhysicSkillSpeedRate
								+ GetRateBuffStatus()->PhysicSkillSpeedRate;
	
	return DWORD( floor( 0.5f + fPercent ) );
}

/* �����ؾ��� ���°��� �������� �˸���. */
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
	if( dwElapsedTime > 2000 ) //2��
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

	// 090204 LUJ, ȿ������ ���� �����̳� �� ���� ��ų�� �˻��� �� �� �ڷ� ������
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

	// 090204 LUJ, ���� �˻縦 ���� �߰��Ѵ�
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
	
	// ��ü ����
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

	// 081203 LUJ, �ݿø���Ų��
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

	// 081203 LUJ, �ݿø���Ų��
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

	// 081203 LUJ, �ݿø���Ų��
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

	// 081203 LUJ, �ݿø���Ų��
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

	// 081203 LUJ, �ݿø���Ų��
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
// 071210 LYW --- Hero : HERO�� Ȯ�� �κ��丮 ���� ���� �� ��ȯ�ϴ� �Լ� �߰�.
void CHero::Set_HeroExtendedInvenCount(BYTE count)
{
	// ����� Ȯ�� �κ� ���� �� ���� ���ѵǾ� �ְ�, ���� ������ ���� �� ���� ���� ���� �ʰ� ����Ѵ�.
	if( count > 2 ) return ;	

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

// 090204 LUJ, Ư�� ��ų�� �߰��Ѵ�.
void CHero::AddSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	// 090204 LUJ, ó������ �׻� �����Ѵ�. ���μ��� �� �˻��ϸ鼭 ���ǿ� ���� ���� ��� �����Ѵ�
	buffSkillInfo->AddBuffStatus();

	SpecialSkillData specialSkillData = { 0 };
	specialSkillData.mBuffSkillInfo	= buffSkillInfo;
	specialSkillData.mIsTurnOn		= TRUE;
	mSpecialSkillList.push_back( specialSkillData );
}

// 090204 LUJ, Ư�� ��ų�� �����Ѵ�
void CHero::RemoveSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	SpecialSkillList::iterator it = mSpecialSkillList.end();

	// 090204 LUJ, �����̳� �߿� ������ ��ų�� ã�´�
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

	// 090204 LUJ, ���ǿ� ���� ������ ������ �� �����Ƿ� �˻� �� ����ؾ� �Ѵ�
	if( specialSkillData.mIsTurnOn )
	{
		buffSkillInfo->RemoveBuffStatus();
	}

	mSpecialSkillList.erase( it );
}

void CHero::ClearWayPoint()
{
	m_WayPointManager->ClearAllWayPoint();

	// Big/Mini Map�� MovePoint ����
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
	// 50cm Ÿ���� �߾ӿ� ���� �Ѵ�.
	pDestPos->x = (int(pDestPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	pDestPos->y = 0;
	pDestPos->z = (int(pDestPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;

	// �����尡 Ȱ��ȭ�� ������ �������� �ʴ´�
	if( InterlockedCompareExchange(&g_PathThreadActivated, 0, 0) == 1 )
	{
		return FALSE;
	}

	VECTOR3 t_StartPos;
	GetPosition( &t_StartPos );

	/// 0. Ÿ���� �̵��Ұ������� ���
	if (MAP->CollisionCheck_OneTile( pDestPos ) == FALSE)
	{
		// �̵��Ұ����������� �̵� ���� �Ǵ�
		if( !bIsCollisionPosMove )
			return FALSE;

		// ������ �̵����� �ִ������� ���Ѵ�.
		if( MAP->CollisionLine( &t_StartPos, pDestPos, pDestPos, MAP->GetMapNum(), this ) == FALSE )
			return FALSE;
	}

	/// ���� ó��
	ClearWayPoint();

	if( MOVEMGR->IsMoving( this ) )
		MOVEMGR->CalcPositionEx( this, gCurTime );

	/// 1. �ѹ��� ���� �Ÿ��� �� �� �ִٸ� �������� �̵�
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
		// ����
		if( MOVEMGR->IsMoving( this ) )
			MOVEMGR->HeroMoveStop();

		/// 2. ��ã��� �̵�
		// ��ã�� �����带 �����Ѵ�
		// �����尡 ���ϵ� �� ��������Ʈ�� �߰��ȴ�
		// 091204 ShinJS --- 3��� ���
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


	// 091208 ShinJS --- �������̸鼭 ž�� ���� ��� Ż�͵� �̵���Ų��.
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
void CHero::Move(int nKeyMoveDir)
{
	// ĳ���Ͱ� ���ư� ���溤�� - ī�޶�� ĳ���ͻ��� ����
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
	// 50cm Ÿ���� �߾ӿ� ���� �Ѵ�.
	//pDestPos->x = (int(pDestPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	//pDestPos->y = 0;
	//pDestPos->z = (int(pDestPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;

	// �����尡 Ȱ��ȭ�� ������ �������� �ʴ´�
	if( InterlockedCompareExchange(&g_PathThreadActivated, 0, 0) == 1 )
	{
		return FALSE;
	}

	VECTOR3 t_StartPos;
	GetPosition( &t_StartPos );

	/// 0. Ÿ���� �̵��Ұ������� ���
	if (MAP->CollisionCheck_OneTile( pDestPos ) == FALSE)
	{
		// �̵��Ұ����������� �̵� ���� �Ǵ�
		if( !bIsCollisionPosMove )
			return FALSE;

		// ������ �̵����� �ִ������� ���Ѵ�.
		if( MAP->CollisionLine( &t_StartPos, pDestPos, pDestPos, MAP->GetMapNum(), this ) == FALSE )
			return FALSE;
	}

	/// ���� ó��
	ClearWayPoint();

	if( MOVEMGR->IsMoving( this ) )
		MOVEMGR->CalcPositionEx( this, gCurTime );

	/// 1. �ѹ��� ���� �Ÿ��� �� �� �ִٸ� �������� �̵�
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
		// ����
		if( MOVEMGR->IsMoving( this ) )
			MOVEMGR->HeroMoveStop();

		/// 2. ��ã��� �̵�
		// ��ã�� �����带 �����Ѵ�
		// �����尡 ���ϵ� �� ��������Ʈ�� �߰��ȴ�
		// 091204 ShinJS --- 3��� ���
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
	int nKeyMoveDir = MACROMGR->GetKeyMoveDir();
	CChaptchaDlg* ChaptchaDlg = GAMEIN->GetChaptchaDlg();
	if( MACROMGR->IsKeyChanged() )
	{
		MACROMGR->ClearKeyChanged();

		// ���α׷��� â�� �� ������ â�� ����Ѵ�.
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
