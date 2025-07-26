// SiegeWarfareMgr.h: interface for the CSiegeWarfareMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)
#define AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ��� ���� ���� ���� - �ٸ� Player�� ������ �� ������ �ȵǴ� ���� ����.
class CPlayer ;
class cSkillEffect ;

enum eSiegeWarfare_Type
{
	eSiegeWarfare_Type_Nera,
	eSiegeWarfare_Type_Max,
};

enum eSiegeWarfare_State
{
	eSiegeWarfare_State_Before,
	eSiegeWarfare_State_Start,
	eSiegeWarfare_State_First,
	eSiegeWarfare_State_End,
};

enum eSiegeWarfare_Map_Type
{
	eNeraCastle_Village,					// ���� �� ����
	eNeraCastle_UnderGround,				// ���� ���� ���� ��
	eNeraCastle_Zebin,						// ���� ��������
	eNeraCastle_Lusen,						// ���� �缾����
	eSiegeWarfare_Map_Max,
};

enum eSiegeWarfare_WaterSeed
{
	eSiegeWarfare_WaterSeed_Using,
	eSiegeWarfare_WaterSeed_Using_Complete,
	eSiegeWarfare_WaterSeed_Cancel,
};

struct SiegeWarfareKindMapInfo
{
	DWORD MapNum;
	WORD WarfareType;
	WORD MapType;
	float PosX;
	float PosZ;
};

enum eSiegeWarfare_Notify
{
	eSiegeWarfare_Notify_Befor_30m,
	eSiegeWarfare_Notify_Befor_5m,
	eSiegeWarfare_Notify_Begin,
	eSiegeWarfare_Notify_End,
	eSiegeWarfare_Notify_End_Before_10m,
	eSiegeWarfare_Notify_Max,
};

enum eSiegeWarfare_Info_Kind
{
//	eSiegeWarfare_Info_Schedule,
	eSiegeWarfare_Info_Damage_Ratio,
	eSiegeWarfare_Info_Heal_Ratio,
	eSiegeWarfare_Info_Critical_Rate,
	eSiegeWarfare_Info_WarterSeed_GuildLevel,
	eSiegeWarfare_Info_WarterSeed_Time,
	// 081017 LYW --- SiegeWarfareMgr : ������ ����ϴ� �Ÿ� ���� �з� �߰�.
	eSiegeWarfare_Info_Craving_Distance,
	eSiegeWarfare_Info_VilageInfo,
	eSiegeWarfare_Info_Max,
};

// 081009 LYW --- SiegeWarfareMgr : ���� �ҽ� ����.
enum eSiegeWarfare_WarterSeed_Error
{
	eSiegeWarfare_WarterSeed_Error_NotState = 1,
	eSiegeWarfare_WarterSeed_Error_UsingPlayer,
	eSiegeWarfare_WarterSeed_Error_NotGuildJoin,
	eSiegeWarfare_WarterSeed_Error_CastleGuld,
	eSiegeWarfare_WarterSeed_Error_GuildLevel,
	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ��� �Ÿ�üũ ���� �з� �߰�.
	eSiegeWarfare_WarterSeed_Error_SoFarDistance,
	eSiegeWarfare_WarterSeed_Error_Max,
};

// 081006 LYW --- SiegeWarfareMgr : ���� ���������� ����� ���̵� ����Ʈ ����ü �߰�.
struct st_GUILDMAPMOVEINFO
{
	MAPTYPE mapNum ;

	float fXpos ;
	float fZpos ;
} ;

// 081015 LYW --- SiegeWarfareMgr : ���ͽõ� ���� ���� �з�.
enum eStep_CravingDirection
{
	eStep_Start = 0,
	eStep_Continue,
	eStep_End,
} ;

#define SIEGEWARFAREMGR CSiegeWarfareMgr::GetInstance()

#define WATERSEED_EFFECTNUM						301

class CSiegeWarfareMgr 
{
	CSiegeWarfareMgr();
	~CSiegeWarfareMgr();

	DWORD					m_dwMapType;	// �������� Ÿ��
	WORD					m_wState;		// �� ����

	DWORD					m_dwWaterSeedUsingGuildLevelLimit;				// ���ͽõ带 ����ϱ� ���� ��� ����
	DWORD					m_dwWaterSeedUsingSecond;						// ���ͽõ带 �ÿ��ϴ� �ð�(��)

	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ带 ����� �� �ִ� �Ÿ��� ���� ������ �߰��Ѵ�.
	WORD					m_wCravingDistance ;

	SYSTEMTIME				m_stReviveTime;

	BOOL					m_bRevive;

	WORD					m_wReviveMessageCount;
	DWORD					m_dwStartTime;

	std::list<SiegeWarfareKindMapInfo>	m_SiegeWarfareKindMapList;					// ������ ���� �� ����Ʈ
	std::list<DWORD>					m_ForbiddenSkillList;						// ���Ұ� ��ų ����Ʈ
	std::list<DWORD>					m_ForbiddenItemList;						// ���Ұ� ������ ����Ʈ
	std::list<DWORD>					m_ForbiddenSkillListTournament;						// ���Ұ� ��ų ����Ʈ
	std::list<DWORD>					m_ForbiddenItemListTournament;
	// 080829 LYW --- SiegeWarfareMgr : ���� ��ų ����� ���� ���� ����.
	DWORD					m_dwBuffSkillPay ;

	// 081019 LYW --- SiegeWarfareMgr : �칰npc ��ġ�� ���� map �����̳� �߰�.
	typedef std::map<DWORD, VECTOR3>	M_WELLPOS ;
	M_WELLPOS							m_MWellPos ;

	// 091211 pdy ���ͽõ� ���� Ÿ�ټ����� �ȵǴ� ���� ����
	cSkillEffect*						m_WaterSeedEffect;

public:
	MAKESINGLETON( CSiegeWarfareMgr );

	void					Process();

	// ���� ������ �����´�.
	void					LoadInfo();
	// ������ �ʱ�ȭ �Ѵ�.
	void					Init();

	void					SetReviveTime();

	void					SetRevive(BOOL bRevive)		{ m_bRevive = bRevive;	}

	// ��� ������ ��ų �ΰ�?
	BOOL					IsFobiddenSkill(DWORD SkillIdx);
	// ��� ������ ������ �ΰ�?
	BOOL					IsFobiddenItem(DWORD ItemIdx);
	BOOL					IsFobiddenSkillTournament(DWORD SkillIdx);
	BOOL					IsFobiddenItemTournament(DWORD ItemIdx);
	// ���������ΰ�?
	BOOL					IsSiegeWarfareZone(DWORD MapNum);
	BOOL					IsSiegeWarfareZone();
	// ������ ������ �ִ� ���ΰ�?
	BOOL					IsSiegeWarfareKindMap(DWORD MapNum);

	BOOL					IsRevive();

	bool					UseCandleStand();

	// ���� ���� ���� ��������
	WORD					GetState()	{ return m_wState; }

	// ��Ʈ��ũ �޽��� �ļ�
	void					NetworkMsgParse(BYTE Protocol,void* pMsg );
	// ������ ��Ȳ�� ��û�Ѵ�.
	void					CM_RequestState();
	// ������ ��Ȳ ��û�� �޴´�.
	void					MMC_SiegeWarfareState(WORD State);

	// 080829 LYW --- SiegeWarfareMgr : ���ͽõ��� ����� �Ϸ�Ǿ����� ������ ��û�ϴ� �Լ�.
	void					IsEndUsing_WaterSeed() ;

	// 080829 LYW --- SiegeWarfareMgr : ���� ��ų ����� ����/��ȯ�ϴ� �Լ� �߰�.
	void					Set_BuffSkillPay(DWORD dwPay) { m_dwBuffSkillPay = dwPay ; }
	DWORD					Get_BuffSkillPay() { return m_dwBuffSkillPay ; }

	// 080904 LYW --- SiegeWarfareMgr : ���� ���� �����ʿ��� �������� �����ϴ� ������ üũ�Ѵ�.
	BOOL					Is_CastleMap() ;

	// 080907 LYW --- SiegeWarfareMgr : ���ͽõ� ��� �ð� ��ȯ �Լ� �߰�.
	DWORD					Get_WaterSeedUsingSecond() { return m_dwWaterSeedUsingSecond ; }

	// 081009 LYW --- SiegeWarfareMgr : �����ҽ� ����.
	DWORD					Get_WaterSeedLimitGuildLevel() { return m_dwWaterSeedUsingGuildLevelLimit; }

	// 081006 LYW --- SiegeWarfareMgr : �� �� ��� ���������� ����� ���̵� ����Ʈ ������ ���� ����Ʈ �߰�.
	typedef std::list< st_GUILDMAPMOVEINFO >	L_GUILDMAPMOVEINFO ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Rushen ;
	L_GUILDMAPMOVEINFO		m_List_MapMove_Zevyn ;
    
	// 081006 LYW --- SiegeWarfareMgr : ���������� ����� ���̵� ���� �ε�/��ȯ �Լ� �߰�.
	void Load_GuildMapMoveInfo() ;
	void Get_GuildMapMoveInfo_Rushen(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map) ;
	void Get_GuildMapMoveInfo_Zevyn(st_GUILDMAPMOVEINFO* pInfo, MAPTYPE map) ;

	L_GUILDMAPMOVEINFO& Get_MapMoveList_Rushen()	{ return  m_List_MapMove_Rushen ; }
	L_GUILDMAPMOVEINFO& Get_MapMoveList_Zevyn()		{ return  m_List_MapMove_Zevyn ; }

	// 081009 LYW --- SiegeWarfareMgr : ���ͽõ� ��� ��� ���� ������ ��ȯ�ϴ� �Լ� �߰�.
	DWORD GetGuildLevelLimit_UseWaterSeed()			{ return m_dwWaterSeedUsingGuildLevelLimit ; }
	DWORD GetUseWaterSeedSecond()					{ return m_dwWaterSeedUsingSecond ; }

	// 081015 LYW --- SiegeWarfareMgr : ���ͽõ� ���� ������ �ϴ� �Լ� �߰�.
	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ���� ���� ���� - �ٸ� Player�� �����ϴ°� �Ⱥ��̴� ���� ����.
	//void Start_CravingDirection(BYTE byStep) ;
	void Start_CravingDirection(CPlayer* pPlayer, BYTE byStep) ;

	// 081017 LYW --- SiegeWarfareMgr : ���ͽõ� ������ ����ϴ� ��ġ�� ��ȯ�ϴ� �Լ� �߰�.
	WORD Get_CravingDistance()						{ return m_wCravingDistance ; }

	// 081019 LYW --- SiegeWarfareMgr : ���� �� Ÿ���� ��ȯ�ϴ� �Լ� �߰�.
	DWORD Get_SiegeMapType()						{ return m_dwMapType ; }

	// 081019 LYW --- SiegeWarfareMgr : �칰 ��ġ �ε� �Լ� �߰�.
	void Load_WellPos() ;

	// 081019 LYW --- SiegeWarfareMgr : �칰 ��ġ ��ȯ �Լ� �߰�.
	VECTOR3* Get_WellPos(DWORD dwUniqueIdx) ;
};

#endif // !defined(AFX_SIEGEWARFAREMGR_H__0620B52A_B018_4B66_9EF9_6A9D6703763D__INCLUDED_)


/*
L_GUILDMAPMOVEINFO& info = Get_MapMoveList_Rushen();
*/