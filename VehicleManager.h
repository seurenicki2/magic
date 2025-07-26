// -------------------------------------
// 090422 ShinJS --- Ż�� Manager Class
// -------------------------------------
#pragma once

#define VEHICLEMGR	CVehicleManager::GetInstance()

class CVehicle;
class CPlayer;

class CVehicleManager
{
	DWORD mRidingVehicle;
	BOOL m_bCheckTime;
public:
	GETINSTANCE( CVehicleManager );

	CVehicleManager();
	virtual ~CVehicleManager();
	void NetworkMsgParse( BYTE Protocol, LPVOID);
	void SetRidingVehicle(DWORD dwVehicleID) { mRidingVehicle = dwVehicleID; }
	DWORD GetRidingVehicle() const { return mRidingVehicle; }
	void Summon( DWORD dwVehicleID, DWORD dwPlayerID, POSTYPE dwItemPos, BOOL summonByPlayer ) const;
	void UnSummon( DWORD dwPlayerID, DWORD dwVehicleID ) const;
	void RequestGetOn( DWORD dwAskPlayerID, DWORD dwVehicleID, MONEYTYPE, DWORD tick) const;
	void GetOn( DWORD dwPlayerID, DWORD dwVehicleID, DWORD dwSeatPos, DWORD dwMasterID, MONEYTYPE dwTollMoney, const char* szVehicleName, const char* szPlayerName ) const;	// ž�� ó��
	void GetOff( DWORD dwPlayerID, DWORD dwVehicleID, DWORD dwSeatPos ) const;						// ž�� ���� ó��

	void Follow( void* pMsg );						// ���󰡱� ó��

	void SetCheckTime( BOOL bCheckTime ) { m_bCheckTime= bCheckTime; }
	BOOL IsCheckTime() const { return m_bCheckTime; }

	// 091022 ShinJS --- Ż�� ��ġ
	void InstallVehicle( CObject* pObject );
	BOOL InstallVehicleToNearPosition( const WORD wMonsterKind );

	// 100211 ShinJS --- ������ �Ǵ�
	BOOL IsMaster( DWORD dwPlayerID );
};