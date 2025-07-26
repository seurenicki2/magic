// ----------------------------- 
// 090422 ShinJS --- Ż�� Class
// ----------------------------- 
#pragma once

#include "Object.h"
#include "Monster.h"

class CVehicle : public CMonster
{
	BOOL		m_bInstalled;											// ��ġ ����(������� �� �Ǵ�)
	char		m_MasterName[ MAX_NAME_LENGTH+1 ];						// �������� �̸�
	DWORD		m_dwLastDistCheckTime;									// ���󰡱�� ������ �Ÿ�Ȯ�� �ð�
	DWORD		m_dwRequestGetOnPlayerID;								// ž�� ��û�� �� Player ID ����

	CYHHashTable<DWORD> m_htRiderInfo;									// ž���� ���� ���� ���̺� [ ž����ġ(1~mSeatSize)-Key, CObject ID-Data ]
	// 090316 LUJ, ��ȯ�� ������ ����
	ICONBASE mUsedItemToSummon;
	DWORD		m_dwExecutedEffectHandle;
	int			m_nExecutedEffectNum;
	BOOL		m_bFollow;

public:
	enum{	
		eFollowRandomDist	= 100,										// ���󰡱� ����� �����ϰ� ������ �ִ� �Ÿ� (��ġ = Master��ġ + Ż�� ������+ FollowRandomDist)
		eFollowExecuteDist	= 300,										// ���󰡱� ���డ�� �Ÿ�
		eGetOnLimitTime		= 2000,										// ��ž�� ���ѽð�(2��)
		eRecallExecuteDist	= 3000,										// �����̵� ���డ�� �Ÿ�
		eSummonLimitTime	= 5000,										// ��ȯ/���� ���ѽð�(5��)
	};

	CVehicle();
	virtual ~CVehicle();

	void InitVehicle();
	void Release();

	virtual void Process();
	virtual void SetMotionInState(EObjectState);
	virtual float DoGetMoveSpeed();

	void SetInstall( BOOL bInstall ) { m_bInstalled = bInstall; }											// ��ġ ���� ����	
	void SetMasterName( const char* name ) { SafeStrCpy( m_MasterName, name, MAX_NAME_LENGTH+1 ); }			// ������ �̸� ����
	void SetRequestGetOnPlayerID( DWORD dwPlayerID ) { m_dwRequestGetOnPlayerID = dwPlayerID; }				// ž�� ��û�� �� Player ID ����
	BOOL IsInstalled() const { return m_bInstalled; }
	BOOL HasRider() { return m_htRiderInfo.GetDataNum() > 0; }
	virtual DWORD GetOwnerIndex() const { return GetMonsterTotalInfo().OwnedObjectIndex; }
	const char* GetMasterName() const { return m_MasterName; }
	DWORD GetRequestGetOnPlayerID() const { return m_dwRequestGetOnPlayerID; }								// ž�� ��û�� �� Player ID ���ϱ�

	void SetPosToMousePos();																				// ���콺�� �ش��ϴ� ��ġ�� Ż�� �̵�(��ġ��)
	BOOL IsValidPosForInstall();																			// ��ġ�ϱ� ������ ��ġ���� �Ǵ�

	void GetOn( CPlayer*, DWORD dwSeatPos  );																// ž��
	BOOL GetOff( CPlayer* pPlayer, DWORD dwSeatPos );														// ž������
	// 090316 LUJ, ��� �°��� ������ �Ѵ�
	void GetOffAll();
	BOOL IsGetOn( CPlayer* pPlayer );																		// ž�¿��� Ȯ��
	DWORD CanGetOn( CPlayer* pPlayer );																		// ž�� ���� ���� Ȯ��, ���ɽ� ž�� ��ġ ��ȯ
	BOOL CanGetOff( CPlayer* pPlayer );																		// ���� ���� ���� Ȯ��

	DWORD GetRiderID( DWORD dwSeatPos ) { return DWORD(m_htRiderInfo.GetData( dwSeatPos )); }				// �¼������� ž���� ID ���ϱ�
	DWORD GetEmptySeatPos( DWORD dwPlayerID );																// ����ִ� �¼���ȣ ���ϱ�

	BOOL CanControl( CPlayer* pPlayer );																	// �̵��� �Ҽ� �ִ��� �Ǵ�
	void Move_OneTarget( MOVE_ONETARGETPOS_FROMSERVER* pMsg );												// ��ġ�� �̵�(���ν��� ��� ž���� �̵�)
	void StartMove( VECTOR3* pTargetPos );																	// �̵�����(���ν��� ��� ž���� �̵�)
	void MoveStop( VECTOR3* pVecPos );																		// �̵� ����(���ν��� ��� ž���� �̵�����)

	MONEYTYPE GetTollMoney();																					// Ż���� ž�¿�� ���ϱ�
	// 090316 LUJ, ��ȯ�� �ʿ��� ������ ����
	inline const ICONBASE& GetUsedItem() const { return mUsedItemToSummon; }
	inline void SetUsedItem( const ICONBASE& usedItem ) { mUsedItemToSummon = usedItem; }
	const char* GetBeffFileNameFromMotion(DWORD dwMotionType);											// ��������� ���� Beff�����̸��� ��ȯ
	void ChangeBeff( DWORD dwMotionType );
	void SetFollow( BOOL bFollow ) { m_bFollow = bFollow; }
};
