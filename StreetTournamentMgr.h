#pragma once

#define STREETTOURNAMENTMGR CStreetTournamentMgr::GetInstance()

class CPlayer ;

class CStreetTournamentMgr
{

protected:
	WORD m_dwState;
	WORD m_dwStage;
	DWORD last_updatetime;
	BOOL m_aplyname;

public:
	CStreetTournamentMgr();
	~CStreetTournamentMgr();

	//static CStreetTournamentMgr* GetInstance() { static CStreetTournamentMgr STREETTOURNAMENTMGR; return &STREETTOURNAMENTMGR; }
	GETINSTANCE( CStreetTournamentMgr );

	void SetState( WORD State );
	DWORD GetState() const { return m_dwState; }
	BOOL isEnemy(CPlayer* Operator, CPlayer* Target);
	BOOL isStreetTournament();
	void OnActiveNumber();
	void Proses();
	void SetStage(WORD sStage)	{	m_dwStage = sStage;	}
	WORD GetStage() const { return m_dwStage; }
	
};