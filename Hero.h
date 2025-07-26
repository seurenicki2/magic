#pragma once

#include "Player.h"
#include "CAction.h"
//inspect
#include "Item.h"

class CSHFamily;
class CWayPointManager;
class CSound;
class CKeyboard;
//wasd 2
//typedef std::deque<CAction*> CActionDeq;

enum eKeyMoveDir //wasd movement
{
	MOVE_STOP		= 0,

	MOVE_UP			= (1 <<  0),
	MOVE_RIGHT		= (1 <<  1),
	MOVE_DOWN		= (1 <<  2),
	MOVE_LEFT		= (1 <<  3),

	MOVE_UP_LEFT	= (1 <<  4),
	MOVE_UP_RIGHT	= (1 <<  5),
	MOVE_DOWN_LEFT	= (1 <<  6),
	MOVE_DOWN_RIGHT	= (1 <<  7),
};
class CHero : public CPlayer  
{
	// �⺻ ����
	HERO_TOTALINFO 	m_HeroTotalInfo;
	EXPTYPE	m_maxExpPoint;
	PlayerStat	mCharStat;
	PlayerStat	mItemBaseStat;
	PlayerStat	mItemOptionStat;
	PlayerStat	mItemStat;

	// 080313 LUJ, ��Ʈ ������ ���� �и�
	PlayerStat	mSetItemStat;

	DWORD mWeaponEnchantLevel;
	DWORD mPhysicDefenseEnchantLevel;
	DWORD mMagicDefenseEnchantLevel;
	// �׼�
	CAction	m_MovingAction;
	CAction	m_NextAction;
	CAction	m_SkillStartAction;
	// ��ȸ�ൿ
	DWORD m_dwLastSocietyAction;

	DWORD		mAccuracy;
	DWORD		mAvoid;
	DWORD		mBlock;
	DWORD		mPhysicAttackMax;
	DWORD		mPhysicAttackMin;
	DWORD		mMagicAttack;
	DWORD		mPhysicDefense;
	DWORD		mMagicDefense;
	DWORD		mCriticalRate;
	DWORD		mCriticalDamage;
	DWORD		mLifeRecover;
	DWORD		mManaRecover;
	DWORD		mMaxLife;
	DWORD		mMaxMana;
	DWORD		m_totalpvpscore;
	DWORD		m_pvpkill;
	DWORD		m_pvpkilled;

	BYTE		m_StorageNum;
	/// �ڵ�����	
	BOOL				m_bIsAutoAttackMode;
	BOOL				m_bIsAutoAttacking;
	CActionTarget		m_AutoAttackTarget;

	/// �⺻����
	DWORD				m_CurComboNum;
	DWORD				mSkillDelay;
	DWORD				mSkillCancelDelay;
	DWORD				mCurrentSkillObjectIndex;

	AbnormalStatus mAbnormalStatus;
	Status				mBuffStatus;
	Status				mRateBuffStatus;
	Status				mPassiveStatus;
	Status				mRatePassiveStatus;
	CSHFamily* m_pcsFamily;
	stMONSTERMETER m_stMonstermeterInfo;
	BOOL m_bNoDeffenceFromArmor;
	BYTE m_byExtendedInvenCount;
	CSound* mMoveSound;
	DWORD m_dwVipPoint;
	//aziz Reborn 24 Sep
	DWORD m_dwRebornData;
	BOOL m_DisableWASD;

	//Alemuri Get Item Drop Rates---
	DWORD m_ServerDropRates;
	int m_ServerDropRatesMultiplier;
	//------------------------------
public:
	CHero();
	virtual ~CHero();
	void				InitHero( HERO_TOTALINFO*);
	void				GetHeroTotalInfo(HERO_TOTALINFO*);
	HERO_TOTALINFO*		GetHeroTotalInfo() { return &m_HeroTotalInfo; }

	/// ����ġ
	EXPTYPE				GetExpPoint() { return m_HeroTotalInfo.ExpPoint; }
	EXPTYPE				GetMaxExpPoint() { return m_maxExpPoint; }
	void				SetExpPoint( EXPTYPE dwPoint, BYTE type=1 );
	void				SetMaxExpPoint( EXPTYPE dwPoint );
	virtual void		SetLevel( LEVELTYPE level );

	void				SetSkillPoint( DWORD point );
	void				SetHeroLevelupPoint( LEVELTYPE dwPoint ) { m_HeroTotalInfo.LevelUpPoint = dwPoint; }
	DWORD				GetHeroLevelUpPoint() { return m_HeroTotalInfo.LevelUpPoint; }
	/// ���� �˸�
	void				HeroStateNotify( BYTE State );

	void				SetWeaponEnchantLevel( DWORD level )		{ mWeaponEnchantLevel = level; }
	void				SetPhysicDefenseEnchantLevel( DWORD level )	{ mPhysicDefenseEnchantLevel = level; }
	void				SetMagicDefenseEnchantLevel( DWORD level )	{ mMagicDefenseEnchantLevel = level; }

	const DWORD			GetWeaponEnchantLevel()	const				{ return mWeaponEnchantLevel; }
	const DWORD			GetPhysicDefenseEnchantLevel() const		{ return mPhysicDefenseEnchantLevel; }
	const DWORD			GetMagicDefenseEnchantLevel() const			{ return mMagicDefenseEnchantLevel; }
	/// ���� ��� ����ü
	PlayerStat&	GetCharStats() { return mCharStat; }
	PlayerStat&	GetItemBaseStats() { return mItemBaseStat; }
	PlayerStat&	GetItemOptionStats() { return mItemOptionStat; }
	PlayerStat&	GetItemStats() { return mItemStat; }

	// 080313 LUJ, ��Ʈ ������ ���� ��ȯ
	PlayerStat& GetSetItemStats()	{ return mSetItemStat; }

	/// �⺻ ����
	void				SetStrength( DWORD val );
	void				SetDexterity( DWORD val );
	void				SetVitality( DWORD val );
	void				SetWisdom( DWORD val );
	void				SetIntelligence( DWORD val );

	DWORD				GetVitality()				const;
	DWORD				GetWisdom()					const;
	DWORD				GetStrength()				const;
	DWORD				GetDexterity()				const;
	DWORD				GetIntelligence()			const;

	/// ����� / ����
	virtual void		SetMaxLife( DWORD maxlife );
	virtual void		SetLife( DWORD life, BYTE type = 1 );
	void				ChangeLife( int changeval );

	virtual void		SetMaxMana( DWORD val );
	virtual void		SetMana( DWORD val, BYTE type = 1 );
	virtual DWORD		GetMana() { return m_HeroTotalInfo.Mana; }
	virtual DWORD		DoGetMaxMana();
	BYTE				GetManaPercent();
	void				ChangeMana(int changeval);

	DWORD				GetMaxLifeWithItem();
	DWORD				GetMaxManaWithItem();
	virtual DWORD		DoGetAttackRate();
	virtual DWORD		DoGetDefenseRate();
	//Alemuri Mobstats calc------------------------------
	virtual float		DoGetEvasionRateFloat();
	virtual DWORD		DoGetDefenseRateMobStats();
	virtual DWORD		DoGetMagDefenseRateMobStats();
	virtual DWORD		DoGetShieldDefense();
	//---------------------------------------------------
	virtual DWORD		DoGetMagAttackRate();
	virtual DWORD		DoGetMagDefenseRate();
	virtual DWORD		DoGetAccuracyRate();
	virtual DWORD		DoGetEvasionRate();
	virtual DWORD		DoGetCriticalRate();		
	virtual DWORD		DoGetMagicCriticalRate();		
	virtual DWORD		DoGetMoveSpeedRate();		
	virtual DWORD		DoGetCastingSpeedRate();		
	virtual DWORD		DoGetAttackSpeedRate();	
	virtual void		SetBadFame(FAMETYPE);
	void				SetPVPScoreMain(DWORD);
	void				SetPVPScoreALL(WORD, WORD);
	DWORD				GetPVPScoretotal();
	DWORD				GetPVPScorekilled();
	DWORD				GetPVPScorekill();
	DWORD				GetAccuracy();
	DWORD				GetAvoid();
	DWORD				GetBlock();
	BYTE				GetJop(BYTE idx) const { return m_CharacterInfo.Job[0]; }
	DWORD				GetTotalBlock();
	DWORD				GetPhysicAttackMax();
	DWORD				GetPhysicAttackMin();
	DWORD				GetMagicAttack();
	DWORD				GetPhysicDefense();
	DWORD				GetMagicDefense();
	DWORD				GetCriticalRate();
	DWORD				GetCriticalDamage();
	DWORD				GetLifeRecover();
	DWORD				GetManaRecover();
	/// ��Ƽ
	DWORD				GetPartyIdx() { return m_HeroTotalInfo.PartyID; }
	void				SetPartyIdx( DWORD PartyId ) { m_HeroTotalInfo.PartyID = PartyId;	}
	virtual void		ClearGuildInfo();
	void				SetGuildName( char* Name );
	char*				GetGuildName();
	void				SetGuildEntryDate( char* day );
	char*				GetGuildEntryDate();
	void				SetFamilyName( char* Name );
	char*				GetFamilyName();
	void				SetFamilyEntryDate( char* day );
	char*				GetFamilyEntryDate();

	/// ��ȸ �ൿ
	virtual BOOL		StartSocietyAct( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE );
	virtual BOOL		EndSocietyAct();
	void				StartSocietyActForQuest( int nStartMotion, int nIngMotion = -1, int nEndMotion = -1, BOOL bHideWeapon = FALSE );
	BOOL				CanSocietyAction( DWORD curTime );

	/// �⺻ �ൿ
	void				Die( CObject* pAttacker, BOOL bFatalDamage, BOOL bCritical, BOOL bDecisive );
	// 080910 LUJ, ������ ���� ó���� RESULTINFO ����ü�� �޾� ó���ϵ��� ��
	void				Damage( CObject* pAttacker, BYTE DamageKind, DWORD Damage, const RESULTINFO&);
	void				Revive( VECTOR3* pRevivePos );
	void				Heal( CObject* pHealer,BYTE HealKind,DWORD HealVal );

	void				SetMovingAction( CAction* pAction ) { m_MovingAction.CopyFrom( pAction ); }
	void				SetNextAction( CAction* pAction ) { m_NextAction.CopyFrom( pAction );	}
	CAction*			GetNextAction()	{ return &m_NextAction; }
	void				SetSkillStartAction( CAction* pAction ) { m_SkillStartAction.CopyFrom( pAction ); }

	void				Process();
	virtual void		OnStartObjectState(EObjectState);
	virtual void		OnEndObjectState(EObjectState);
	void				ApplyInterface();
	void				SetMoney( MONEYTYPE Money );
	MONEYTYPE			GetMoney() { return m_HeroTotalInfo.Money; }

	/// â��
	BYTE				GetStorageNum()	{ return m_StorageNum; }
	void				SetStorageNum( BYTE num ) { m_StorageNum = num; }

	friend class		CAttackManager;
	/// �ڵ�����
	BOOL				IsAutoAttacking() { return m_bIsAutoAttacking; }
	CActionTarget*		GetAutoAttackTarget() { return &m_AutoAttackTarget; }
	void				EnableAutoAttack( CActionTarget* pTarget )
	{	
		if( m_bIsAutoAttackMode && pTarget->GetTargetKind() == eActionTargetKind_Object )
		{
			m_bIsAutoAttacking = TRUE;	
			m_AutoAttackTarget.CopyFrom( pTarget ); 
		}
	}
	void				DisableAutoAttack() 
	{ 
		m_bIsAutoAttacking = FALSE; 
		m_AutoAttackTarget.CopyFrom( NULL ); 
	}
	void				ToggleAutoAttackMode() { m_bIsAutoAttackMode = !m_bIsAutoAttackMode; }

	DWORD				GetCurComboNum() { return m_CurComboNum < MAX_COMBO_NUM ? m_CurComboNum : 0; }
	void				SetCurComboNum( DWORD num )	{ m_CurComboNum = num; }


	/// Ÿ�� ����
	void				ClearTarget( CObject* pObject );
	void				OnHitTarget( CObject* pMainTarget );
	DWORD				GetSkillDelay() const { return mSkillDelay; }
	void				SetSkillDelay(DWORD skillDelay) { mSkillDelay = skillDelay; }
	void				StartSkillProgressBar( DWORD time );

	Status*				GetBuffStatus() { return &mBuffStatus; }
	Status*				GetRateBuffStatus() { return &mRateBuffStatus; }
	Status*				GetPassiveStatus() { return &mPassiveStatus; }
	Status*				GetRatePassiveStatus() { return &mRatePassiveStatus; }

	AbnormalStatus*	GetAbnormalStatus() { return &mAbnormalStatus; }

	//inspect
	CItem *	m_OtherPlayerEquip[eWearedItem_Max];
	// 090204 LUJ, �нú� ���¿��� ���Ǻ� �����ϴ� ��ų�� üũ�Ѵ�
public:
	// 090204 LUJ, Ư�� ��ų�� �߰��Ѵ�
	void AddSpecialSkill( cBuffSkillInfo* pInfo );
	// 090204 LUJ, Ư�� ��ų�� �����Ѵ�
	void RemoveSpecialSkill( cBuffSkillInfo* pInfo );
private:
	// 090204 LUJ, Ư�� ��ų�� ���μ��� Ÿ�ӿ� üũ�Ѵ�
	void ProcessSpecialSkill();
private:
	struct SpecialSkillData
	{
		cBuffSkillInfo*	mBuffSkillInfo;
		BOOL			mIsTurnOn;
	};
	typedef std::list< SpecialSkillData >	SpecialSkillList;
	SpecialSkillList						mSpecialSkillList;

public:
	void AddMatch();
	void RemoveMatch();

	// �׸��� ���� ��ü �÷��̾� ���� ��Ʈ ����. ������� ��Ī ������ �ٲ���� �� ȣ��Ǿ�� �Ѵ�.
	void CheckAllHeart();

private:
	// ��Ʈ ��Ī�� �÷��̾� ��
	int m_matchedQuantity;

public:
	void SetFamily(CSHFamily*);
	CSHFamily* GetFamily() { return m_pcsFamily; }
	void SetMonstermeterPlaytime(DWORD nTime, DWORD nTimeTotal)		{ m_stMonstermeterInfo.nPlayTime = nTime; m_stMonstermeterInfo.nPlayTimeTotal = nTimeTotal; }
	void SetMonstermeterKillMonNum(DWORD nNum, DWORD nNumTotal)		{ m_stMonstermeterInfo.nKillMonsterNum = nNum; m_stMonstermeterInfo.nKillMonsterNumTotal = nNumTotal; }
	void ProcMonstermeterPlayTime();

	// �̵��ӵ� ���� ��ȯ
public:
	virtual float DoGetMoveSpeed();
	DWORD GetSkillCancelDelay() const { return mSkillCancelDelay; }
	void SetSkillCancelDelay(DWORD delay) { mSkillCancelDelay = delay; }
	void SetCurrentSkill( const DWORD skillIndex ) { mCurrentSkillObjectIndex = skillIndex; }
	const DWORD GetCurrentSkill() const { return mCurrentSkillObjectIndex; }

//---KES ��� ���� ���� ���� 071128
	BOOL	IsNoDeffenceFromArmor()		{ return m_bNoDeffenceFromArmor; }
//----------------------------------

	// 071210 LYW --- Hero : HERO�� Ȯ�� �κ��丮 ���� ���� �� ��ȯ�ϴ� �Լ� �߰�.
	void Set_HeroExtendedInvenCount(BYTE count) ;
	BYTE Get_HeroExtendedInvenCount() { return m_byExtendedInvenCount; }

	void PassiveSkillCheckForWeareItem();

//---KES ���󰡱�
protected:
	DWORD	m_dwFollowPlayerIdx;
public:
	void SetFollowPlayer( DWORD PlayerIdx )		{ m_dwFollowPlayerIdx = PlayerIdx; }
	DWORD GetFollowPlayer()						{ return m_dwFollowPlayerIdx; }
//---------------

	// 090526 ShinJS --- �̸����� ��Ƽ�ʴ� Error �����ʰ��� �۾��ð� ����, ����ð����� ��õ� ���� �Ǵ�
protected:
	DWORD	m_dwInvitePartyByNameLastTime;				// �۾��ð� ���� 
	DWORD	m_dwInvitePartyByNameLimitTime;				// ���ѽð� ����
	BOOL				m_bIsSlotwait;
	DWORD				m_lastspinslot;
	DWORD				m_lastspinget;
	MONEYTYPE			m_Moneyspinget;
	MONEYTYPE			m_Moneyspinjackpot;
	WORD				m_WinSlot1;
	WORD				m_WinSlot2;
	WORD				m_WinSlot3;
	WORD				m_WinSlot4;
	WORD				m_Wincode;
	//Alemuri Party War------------------
	BOOL isPartyWar;
	std::string ally1;
	std::string ally2;
	std::string ally3;
	std::string ally4;
	std::string ally5;
	std::string ally6;
	std::string ally7;

	std::string enemy1;
	std::string enemy2;
	std::string enemy3;
	std::string enemy4;
	std::string enemy5;
	std::string enemy6;
	std::string enemy7;
	//-----------------------------------

public:
	//Alemuri Party War---------------------------------------------------------------------------
	void	SetPartyWar( BOOL isWar )	{ isPartyWar = isWar; }
	BOOL	GetPartyWar()	{ return isPartyWar; }
	void	SetAlly1(std::string allyName) {ally1 = allyName;}
	void	SetAlly2(std::string allyName) {ally2 = allyName;}
	void	SetAlly3(std::string allyName) {ally3 = allyName;}
	void	SetAlly4(std::string allyName) {ally4 = allyName;}
	void	SetAlly5(std::string allyName) {ally5 = allyName;}
	void	SetAlly6(std::string allyName) {ally6 = allyName;}
	void	SetAlly7(std::string allyName) {ally7 = allyName;}
	
	void	SetEnemy1(std::string enemyName) {enemy1 = enemyName;}
	void	SetEnemy2(std::string enemyName) {enemy2 = enemyName;}
	void	SetEnemy3(std::string enemyName) {enemy3 = enemyName;}
	void	SetEnemy4(std::string enemyName) {enemy4 = enemyName;}
	void	SetEnemy5(std::string enemyName) {enemy5 = enemyName;}
	void	SetEnemy6(std::string enemyName) {enemy6 = enemyName;}
	void	SetEnemy7(std::string enemyName) {enemy7 = enemyName;}

	std::string	GetAlly1() {return ally1;}
	std::string	GetAlly2() {return ally2;}
	std::string	GetAlly3() {return ally3;}
	std::string	GetAlly4() {return ally4;}
	std::string	GetAlly5() {return ally5;}
	std::string	GetAlly6() {return ally6;}
	std::string	GetAlly7() {return ally7;}

	std::string	GetEnemy1() {return enemy1;}
	std::string	GetEnemy2() {return enemy2;}
	std::string	GetEnemy3() {return enemy3;}
	std::string	GetEnemy4() {return enemy4;}
	std::string	GetEnemy5() {return enemy5;}
	std::string	GetEnemy6() {return enemy6;}
	std::string	GetEnemy7() {return enemy7;}
	//------------------------------------------------------------------------------------------

	//Alemuri Item Drop Ratesinfo to client-----------------------------------------------------------------
	void	SetServerDropRatesInfo( DWORD dropRatesFromServer )	{ m_ServerDropRates = dropRatesFromServer; }
	DWORD	GetServerDropRatesInfo()			{ return m_ServerDropRates; }
	void	SetServerDropRatesInfoMultiplier( DWORD multiplier )	{ m_ServerDropRatesMultiplier = multiplier; }
	DWORD	GetServerDropRatesInfoMultiplier()			{ return m_ServerDropRatesMultiplier; }
	//------------------------------------------------------------------------------------------------------

	//Alemuri Delete Item Custom------------------------------
	void DeleteItemCustom(POSTYPE position, DWORD itemIdx, int durability);
	//--------------------------------------------------------

	void	SetInvitePartyByNameLastTime( DWORD dwTime )	{ m_dwInvitePartyByNameLastTime = dwTime; }
	void	SetInvitePartyByNameLimitTime( DWORD dwTime )	{ m_dwInvitePartyByNameLimitTime = dwTime; }
	DWORD	GetInvitePartyByNameLastTime() const			{ return m_dwInvitePartyByNameLastTime; }
	DWORD	GetInvitePartyByNameLimitTime() const			{ return m_dwInvitePartyByNameLimitTime; }
	BOOL IsNoEquip(eArmorType, eWeaponType, eWeaponAnimationType);
	BOOL IsEnable(const BUFF_SKILL_INFO&);
	virtual float GetBonusRange() const;
	virtual void SetObjectBattleState(eObjectBattleState eBattleState);

	// 090907 ShinJS --- 5�� ��ã�� Source ����
protected:	
	CWayPointManager*	m_WayPointManager;																				// ��ã��� WayPointManager;

public:
	void ClearWayPoint();																								// WayPoint �ʱ�ȭ
    BOOL Move_UsePath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove=FALSE );	// ��ǥ ���� ��ã��
	BOOL Move_Simple( VECTOR3* pTargetPos );																			// ���� �̵����ɽ� ��ǥ �������� �̵�ó��
	BOOL NextMove();																									// ��ã��� ����� ���� WayPoint�� �̵�
//wasd movement//Beyond SEA2
/// Ű���� �Է� �̵�
private:
	float m_KeyMove_DistOneStep;
	float m_KeyMoveMount_DistOneStep;
public://Beyond SEA2
	void Move(int nKeyMoveDir);
	BOOL Move_WASDPath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove=FALSE );
	void	DisableWASD(int val);
	void Move_KeyboardInput2();
	void Stop_SendtoServer();
	
	void	SetSlotStart( BOOL stats )	{ m_bIsSlotwait = stats; }
	void	SetGetmoneyspinslot( MONEYTYPE moneyspinget, WORD slot1, WORD slot2, WORD slot3, WORD slot4, MONEYTYPE totaljackpot, WORD wincode);
	void	StopGetmoneyspinslot();
	void	DoGetmoneyspinslot();
	//void	SetSlotStat( DWORD lsgett )	{ m_lastspinget = lsgett; }
	void	SetMapjackpot( MONEYTYPE nJackpotbygm )	{ m_Moneyspinjackpot = nJackpotbygm; }
	MONEYTYPE GetMapJackpot() { return m_Moneyspinjackpot; }
	stMONSTERMETER* GetMonstermeterInfo() {return &m_stMonstermeterInfo;}
};